#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "starspray.h"
#include "simulator.h"
#include "io.h"

#define SSH "/usr/bin/ssh"
#define SERVER "mite.physik.uzh.ch"

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

extern Environment env;

int startSimulator()
{
    int ret=0;

    //========================================================================

    msg_t msg;
    msg.cmd = htonl(SVR_CMD_START_SIM);
    send(env.sock, &msg, sizeof(msg), 0);
    if ((ret=recv(env.sock, &msg, sizeof(msg), MSG_WAITALL) == -1)) return ret;
    if (ntohl(msg.result) != SVR_READY) return 1;

    //========================================================================

    printf("Sending %i particles (IC).\n", env.pList.nParticles);
    printf("Requesting %i frames.\n", env.maxTimesteps);

    int i, pi, pos_vel[6];
    int mts = htonl(env.maxTimesteps);
    send(env.sock, &mts, sizeof(mts), 0);
    int np = htonl(env.pList.nParticles);
    send(env.sock, &np, sizeof(np), 0);
    for (i=0, pi=0; i < env.pList.nParticles; i++, pi+=3)
    {
        pos_vel[0] = htonf(env.pList.pos[pi+0]);
        pos_vel[1] = htonf(env.pList.pos[pi+1]);
        pos_vel[2] = htonf(env.pList.pos[pi+2]);
        pos_vel[3] = htonf(env.pList.vel[pi+0]);
        pos_vel[4] = htonf(env.pList.vel[pi+1]);
        pos_vel[5] = htonf(env.pList.vel[pi+2]);

        send(env.sock, &pos_vel, sizeof(pos_vel), 0);
        //fprintf(stderr, ". %ld", sizeof(pos_vel));
    }
    //fprintf(stderr, "\n");

    if ((ret=recv(env.sock, &msg, sizeof(msg), MSG_WAITALL) == -1)) return ret;

    return 0;
}

int stopSimulator()
{
    int ret=0;
    msg_t msg;
    msg.cmd = htonl(SVR_CMD_STOP_SIM);
    send(env.sock, &msg, sizeof(msg), 0);
    if ((ret=recv(env.sock, &msg, sizeof(msg), MSG_WAITALL) == -1)) return ret;
    if (ntohl(msg.result) != SVR_READY) return 1;

    if ((ret=recv(env.sock, &msg, sizeof(msg), MSG_WAITALL) == -1)) return ret;

    return 0;
}

int pauseSimulator()
{
    return 0;
}

int resumeSimulator()
{
    return 0;
}

int loadNextFrameFromSimulator()
{
    int ret=0;

    //========================================================================

    printf("Requesting Frame %i\n", env.currentTimestep);

    msg_t msg;
    msg.cmd = htonl(SVR_CMD_NEXT_FRAME);
    msg.next_frame = htonl(env.currentTimestep);
    send(env.sock, &msg, sizeof(msg), 0);
    if ((ret=recv(env.sock, &msg, sizeof(msg), MSG_WAITALL) == -1)) return ret;
    if (ntohl(msg.result) != SVR_READY) return 1;

    //========================================================================

    unsigned int nParticles;
    int pos_vel[6];

    if ((ret=recv(env.sock, &nParticles, sizeof(nParticles), MSG_WAITALL) < sizeof(nParticles))) return ret;
    nParticles = ntohl(nParticles);

    printf("Frame %i: Receiving %i particles.\n", env.currentTimestep, nParticles);

    unsigned int i, pi;
    for (i=0, pi=0; i < nParticles; i++, pi+=3)
    {
        if (recv(env.sock, &pos_vel, sizeof(pos_vel), MSG_WAITALL) < sizeof(pos_vel)) { ret = 1; break; }

#if 1
        //env.pList.pos[pi+0] = pos_vel[3];
        //env.pList.pos[pi+1] = pos_vel[4];
        //env.pList.pos[pi+2] = pos_vel[5];
        //env.pList.vel[pi+0] = pos_vel[0];
        //env.pList.vel[pi+1] = pos_vel[1];
        //env.pList.vel[pi+2] = pos_vel[2];

        env.pList.pos[pi+0] = ntohf(pos_vel[0]);
        env.pList.pos[pi+1] = ntohf(pos_vel[1]);
        env.pList.pos[pi+2] = ntohf(pos_vel[2]);
        env.pList.vel[pi+0] = ntohf(pos_vel[3]);
        env.pList.vel[pi+1] = ntohf(pos_vel[4]);
        env.pList.vel[pi+2] = ntohf(pos_vel[5]);
#else
        env.pList.pos[pi+0] = ntohf(pos_vel[0]);
        env.pList.pos[pi+1] = ntohf(pos_vel[1]);
        env.pList.pos[pi+2] = ntohf(pos_vel[2]);
        env.pList.vel[pi+0] = ntohf(pos_vel[3]);
        env.pList.vel[pi+1] = ntohf(pos_vel[4]);
        env.pList.vel[pi+2] = ntohf(pos_vel[5]);
#endif
    }

    if ((ret=recv(env.sock, &msg, sizeof(msg), MSG_WAITALL) == -1)) return ret;

    return ret;
}
