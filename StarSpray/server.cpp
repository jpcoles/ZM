#include <sys/socket.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <ftipsy.hpp>
#include "server.h"
#include "io.h"
#include "tipsyio.h"
#include "math.h"
#include "net.h"

//#define PKDGRAV "./pkdgrav2m_pthread"
#define PKDGRAV "./pkdgrav-exec"

#define SVR_READY 0

typedef struct
{
    union {
        int cmd;
        int result;
    };

    union {
        int next_frame;
    };

} msg_t;

msg_t ack = {{SVR_READY}};

void banner();
int beServer();
int handle_connection(int fd);

int simStatus=0;
int port;
unsigned int nParticles;
unsigned int nFrames;

char *buf = NULL;

const int send_flags = 0;
//const int send_flags = MSG_NOSIGNAL;

void banner()
{
    printf(" STARSPRAY Server\n"
           "==================\n");
}

void help()
{
    fprintf(stderr, "starspray-server <port>\n");
    exit(1);
}

int main(int argc, char **argv)
{
    if (argc < 2) help();

    port = atoi(argv[1]);

    banner();

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        fprintf(stderr, "Failed to create a socket.\n");
        exit(1);
    }

    struct sockaddr_in sock_addr;
    memset(&sock_addr, 0, sizeof(sock_addr));
    
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port   = htons(port);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == -1)
    {
        fprintf(stderr, "Could not bind socket.\n");
        exit(1);
    }

    if (listen(sockfd, 1))
    {
        fprintf(stderr, "Failed to listen :(\n");
        exit(1);
    }

    printf("Listening on port %i\n", port);

    while (true)
    {
        int connectfd = accept(sockfd, NULL, NULL);
        if (connectfd < 0)
        {
            fprintf(stderr, "Failed to accept.\n");
            exit(1);
        }

        printf("Accepted connection\n");

        handle_connection(connectfd);

        printf("Shutting down connection\n");

        shutdown(connectfd, SHUT_RDWR);
        close(connectfd);
    }

    return 0;
}


int handle_connection(int fd)
{
    msg_t msg;
    int current_frame = 0;

    while (true)
    {
        int ret=0;
        if (recv(fd, &msg, sizeof(msg), MSG_WAITALL) == -1) break;;
        if (send(fd, &ack, sizeof(ack), send_flags) == -1) break;
        
        switch (ntohl(msg.cmd))
        {
            case SVR_CMD_START_SIM:
                ret = svr_startSimulation(fd);
                current_frame = 1;
                buf = (char *)realloc(buf, 32 + 36*nParticles);
                break;
            case SVR_CMD_NEXT_FRAME:
                current_frame = ntohl(msg.next_frame);
                ret = svr_sendNextFrame(current_frame, fd);
                break;
            case SVR_CMD_STOP_SIM:
                ret = svr_stopSimulation(fd);
                break;
            default:
                fprintf(stderr, "Unknown command %i!\n", msg.cmd);
                continue;
                break;
        }

        msg.result = htonl(ret);
        if (send(fd, &msg, sizeof(msg), send_flags) == -1) break;
    }

    return 0;
}

int svr_startSimulation(int fd)
{
    int ret=0;

    if (simStatus & SIM_STOPPED) return ret;
    simStatus = SIM_RUNNING;

    //========================================================================
    // Receive the initial conditions from the client
    // Only the positions and velocities are transferred. Particles are
    // assumed to be dark matter.
    //========================================================================
    if ((ret=receiveICs(fd))) return ret;

    //========================================================================
    // Now start up pkdgrav.
    //========================================================================
    fprintf(stderr, "Preparing pkdgrav...");
    if ((ret=system("mkdir -p sim"))) return ret;
    if ((ret=system("rm -f sim/starspray.0* sim/starspray.log .lockfile starspray.pid"))) return ret;
    fprintf(stderr, "Launching pkdgrav...");
    if ((ret=system(PKDGRAV " jonathan.par & echo $! > starspray.pid"))) return ret;
    //if ((ret=system(PKDGRAV " jonathan.par > /dev/null & echo $! > starspray.pid"))) return ret;
    fprintf(stderr, "done.\n");
    fprintf(stderr, "Waiting for startup...");
    if ((ret=system("until [ -e sim/starspray.log ]; do sleep .1; done;"))) return ret;
    if ((ret=system("sleep 1"))) return ret;
    fprintf(stderr, "done.\n");

    return ret;
}

int svr_stopSimulation(int fd)
{
    if (simStatus == SIM_STOPPED) return 0;
    simStatus = SIM_STOPPED;
    return system("kill -9 `cat starspray.pid`");
}

int receiveICs(int fd)
{
    int ret=0;
    int err=0;
    ofTipsy out;

    TipsyHeader h;
    TipsyDarkParticle d;
    int pos_vel[6];

    out.open("starspray-ic.std", "standard");
    if (!out.is_open()) return 1;

    if ((ret=recv(fd, &nFrames, sizeof(nFrames), MSG_WAITALL)) == -1) return ret;
    nFrames = ntohl(nFrames);

    if ((ret=recv(fd, &nParticles, sizeof(nParticles), MSG_WAITALL)) == -1) return ret;
    nParticles = ntohl(nParticles);

    printf("%i frames requested.\n", nFrames);
    printf("Receiving %i particles.\n", nParticles);

    h.h_time = 1;
    h.h_nBodies = nParticles;
    h.h_nDims = 3;
    h.h_nDark = h.h_nBodies;
    h.h_nSph  = 
    h.h_nStar = 0;
    
    out << h;

    float mass = 1.0f / h.h_nBodies;
    float soft = 1.0f / sqrtf(h.h_nBodies);
    d.mass    = mass;
    d.eps     = soft;
    d.phi     = 0;
    d.density = 0;

    ret = 0;
    for (unsigned int i=0; i < h.h_nDark; i++)
    {
        if (recv(fd, &pos_vel, sizeof(pos_vel), MSG_WAITALL) == -1) { ret = 1; err = errno; break; }

        d.pos[0] = ntohf(pos_vel[0]);
        d.pos[1] = ntohf(pos_vel[1]);
        d.pos[2] = ntohf(pos_vel[2]);
        d.vel[0] = ntohf(pos_vel[3]);
        d.vel[1] = ntohf(pos_vel[4]);
        d.vel[2] = ntohf(pos_vel[5]);

        out << d;
    }

    fprintf(stderr, "ret != 0 in receiveICs() [1] %i %i\n", ret, err);
    out.close();

    if (ret == 0) ret = write_config_file("jonathan.par");
    fprintf(stderr, "ret != 0 in receiveICs() [2] %i %i\n", ret, err);

    return ret;
}

int svr_sendNextFrame(int current_frame, int fd)
{
    ifTipsy in;
    int ret = 0;
    char filename[256];
    char cmd[1024];

    TipsyHeader h;
    TipsyDarkParticle d;

    snprintf(filename, sizeof(filename), "sim/starspray.%05i", current_frame);

    //printf("Waiting for %s...\n", filename);
    printf("Waiting for %s...\n", filename);

    sprintf(cmd, "until [ -e %s ]; do sleep 1; done;", filename);

    if ((ret=system(cmd))) return ret;

#if 0
    int fp;
    while (true)
    {
        struct stat sb;
        fp = open(filename, O_RDONLY, 0);
        if (fp >= 0)
        {
            if (fstat(fp, &sb) == 0)
            {
                if (sb.st_size == sizeof(h) + (36)*nParticles) // 36 == size of dark particle
                {
                    ssize_t x;
                    //x = read(fp, &buf, sb.st_size);
                    //printf("%ld\n", x);
                    //if (x == sb.st_size) break;
                    //if (read(fp, &buf, sb.st_size) == sb.st_size) break;
                    break;
                }
            }
        }

#if 0
        fp = open(filename, O_RDONLY, 0);
        if (fp >= 0) 
        {
            int pid = fcntl(fp, F_GETOWN, 0);
            printf("%i %i\n", fcntl(fp, F_GETOWN, 0), getpid());
            if (pid == getpid()) break;
            close(fp);
        }
#endif
        close(fp);
        usleep(500);
    }
    close(fp);
#endif

    do {
        in.open(filename, "standard");
        //if (!in.is_open()) { assert(0); return 1; }
    } while (!in.is_open());

    in >> h;

    printf("Sending %i particles from %s.\n", h.h_nDark, filename);

    int pos_vel[6];
    unsigned int np = h.h_nDark;
    np = htonl(np);
    if (send(fd, &(np), sizeof(np), send_flags) == -1) ret = 1;
    for (int i=0, pi=0; !ret && i < h.h_nDark; i++, pi+=3)
    {
        in >> d;
        pos_vel[0] = htonf(d.pos[0]);
        pos_vel[1] = htonf(d.pos[1]);
        pos_vel[2] = htonf(d.pos[2]);
        pos_vel[3] = htonf(d.vel[0]);         
        pos_vel[4] = htonf(d.vel[1]);
        pos_vel[5] = htonf(d.vel[2]);
                     
        if (send(fd, &pos_vel, sizeof(pos_vel), send_flags) == -1) { ret = 1; }
    }

    in.close();

    sprintf(cmd, "rm -f %s", filename);
    system(cmd);

    return ret;
}

int write_config_file(char *filename)
{
    char *config_template = 
        "#\n"
        "# pkdgrav parameter file for StarSpray\n"
        "# Generated on %s\n"
        "#\n"
        "bStandard    = 1\n"
        "achOutName   = sim/starspray\n"
        "achInFile    = starspray-ic.std\n"
        "nSteps	      = %i\n"
        "dExtraStore  = 10.0\n"
        "dDelta       = 0.01\n"
        "#bPeriodic   = 1\n"
        "#dPeriod     = 1e20\n"
        "iMaxRung     = 1    # this forces single stepping\n"
        "#iMaxRung    = 16   # multistepping with a maximum of 16 timestepping rungs\n"
        "\n"
        "#bGravStep   = 1	 # gravity timestepping\n"
        "#dEta        = 0.03 # accuracy of the integration \n"
        "\n"
        "#bEpsAccStep = 1\n"
        "#dEta    	  = 0.25 # accuracy of the integration\n"
        "\n"
        "iLogInterval = 1000\n"
        "iOutInterval = 1\n"
        "bDoDensity   = 0\n"
        "\n"
        "bOverwrite   = 1\n"
        "bParaRead    = 1\n"
        "bParaWrite   = 0\n"
        "\n"
        "dTheta       = 1.2 # accuracy of forces\n"
        "\n"
        "# Do fewer domain decompositions\n"
        "dFracNoDomainRootFind  = 0.1\n"
        "dFracNoDomainDecomp    = 0.01\n"
        "dFracNoDomainDimChoice = 0.999\n\n";

    FILE *fp = fopen(filename, "wt");
    if (fp == NULL) return 1;

    time_t t = time(NULL);
    fprintf(fp, config_template, ctime(&t), nFrames);

    fclose(fp);

    return 0;
}

