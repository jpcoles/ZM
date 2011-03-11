#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include "starspray.h"
#include "simulator.h"
#include "tipsyio.h"
#include "io.h"

#define PKDGRAV "./pkdgrav-exec"

extern Environment env;

int write_config_file(char *filename);

int startSimulator()
{
    int ret=0;

    //if (env.simStatus & SIM_STOPPED) return ret;
    //env.simStatus = SIM_RUNNING;

    saveICs();
    write_config_file("jonathan.par");

    //========================================================================
    // Now start up pkdgrav.
    //========================================================================
    fprintf(stderr, "Preparing pkdgrav...");
    if ((ret=system("mkdir -p sim"))) return ret;
    if ((ret=system("rm -f sim/starspray.0* sim/starspray.log .lockfile starspray.pid"))) return ret;
    fprintf(stderr, "Launching pkdgrav...");
    if ((ret=system(PKDGRAV " -sz 1 jonathan.par > /dev/null & echo $! > starspray.pid"))) return ret;
    //if ((ret=system(PKDGRAV " jonathan.par > /dev/null & echo $! > starspray.pid"))) return ret;
    fprintf(stderr, "done.\n");
    fprintf(stderr, "Waiting for startup...");
    if ((ret=system("until [ -e sim/starspray.log ]; do sleep .1; done;"))) return ret;
    //if ((ret=system("sleep 1"))) return ret;
    fprintf(stderr, "done.\n");

    return ret;
}

int stopSimulator()
{
    //if (env.simStatus == SIM_STOPPED) return 0;
    //env.simStatus = SIM_STOPPED;
    return system("kill -9 `cat starspray.pid`");
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
    char filename[256];

    //printf("Requesting Frame %i\n", env.currentTimestep);
    snprintf(filename, sizeof(filename), "sim/starspray.%05i", env.currentTimestep);

    FILE *fp;
    
    while (!(fp = fopen(filename, "r"))) { usleep(20); }
    while (1) { fseek(fp, 0L, SEEK_END); if (ftell(fp) == (32 + 36*env.pList.nParticles)) break; usleep(20); }

    //while (flock(fileno(fp), LOCK_EX | LOCK_NB) == EWOULDBLOCK) { usleep(100); }
    fclose(fp);

    return loadTipsyPositions(filename, env.pList.pos, env.pList.vel, env.pList.nParticles);
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
        "#dExtraStore  = 10.0\n"
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
        "bParaWrite   = 1\n"
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
    fprintf(fp, config_template, ctime(&t), env.maxTimesteps);

    fclose(fp);

    return 0;
}
