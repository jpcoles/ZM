#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "simulator.h"

#define SSH "/usr/bin/ssh"
#define SERVER "mite.physik.uzh.ch"

void startSimulator()
{
    system("rm -f starspray.0*");
    fprintf(stderr, "Uploading new ICs...");
    system("scp -q starspray-ic.std " SERVER ":/tmp/starspray");
    fprintf(stderr, "done.\n");

    fprintf(stderr, "Launching pkdgrav...");
    system(SSH " " SERVER " \"(cd /tmp/starspray && ./cntl_pkdgrav START)\" &");
    fprintf(stderr, "done.\n");

    sleep(2);
}

void stopSimulator()
{
    system(SSH " " SERVER " \"(cd /tmp/starspray && ./cntl_pkdgrav STOP)\" &");
}

void pauseSimulator()
{
    system(SSH " " SERVER " \"(cd /tmp/starspray && ./cntl_pkdgrav PAUSE)\" &");
}

void resumeSimulator()
{
    system(SSH " " SERVER " \"(cd /tmp/starspray && ./cntl_pkdgrav RESUME)\" &");
}
