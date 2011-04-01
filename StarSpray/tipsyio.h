#ifndef TIPSYIO_H
#define TIPSYIO_H

#include "starspray.h"

#ifdef __cplusplus
extern "C" {
#endif

int loadTipsyPositions(char *filename, float *pos, float *vel, unsigned int nParticles);
int writeTipsyPositions(char *filename, ParticleList *pList);
#ifndef SERVER
int loadSprayPattern(char *filename, SprayPattern *sp);
#endif
#ifdef __cplusplus
}
#endif

#endif

