#include <math.h>
#include <iostream>
#include <assert.h>
#include <ftipsy.hpp>
#include "starspray.h"
#include "tipsyio.h"

using namespace std;

int loadTipsyPositions(char *filename, ParticleList *pList)
{
    ifTipsy in;

    TipsyHeader h;
    TipsyDarkParticle d;

    in.open(filename, "standard");
    if (!in.is_open()) { assert(0); return 1; }

    in >> h;

    if (h.h_nDark != pList->nParticles) {assert(0); return 2; }

    float *pos = pList->pos;
    float *vel = pList->vel;
    float *mass = pList->mass;
    float *soft = pList->soft;

    for (unsigned int i=0; i < h.h_nDark; i++)
    {
        in >> d;
        *pos++ = d.pos[0];
        *pos++ = d.pos[1];
        *pos++ = d.pos[2];

        *vel++ = d.vel[0];
        *vel++ = d.vel[1];
        *vel++ = d.vel[2];

        *mass++ = d.mass;
        *soft++ = d.eps;

        //cerr << d.pos[0] << " " << d.pos[1] << " " << d.pos[2] << endl;
    }

    in.close();

    return 0;
}

int writeTipsyPositions(char *filename, ParticleList *pList) //float *pos, float *vel, unsigned int nParticles)
{
    ofTipsy out;

    TipsyHeader h;
    TipsyDarkParticle d;

    out.open(filename, "standard");
    if (!out.is_open()) return 1;

    h.h_time = 0;
    h.h_nBodies = pList->nParticles;
    h.h_nDims = 3;
    h.h_nDark = h.h_nBodies;
    h.h_nSph  = 
    h.h_nStar = 0;
    
    out << h;

    float *pos = pList->pos;
    float *vel = pList->vel;
    float *mass = pList->mass;
    float *soft = pList->soft;

    //float mass = 1.0f / h.h_nBodies;
    ////float soft = 1.0f / sqrtf(h.h_nBodies);
    for (unsigned int i=0; i < h.h_nDark; i++)
    {
        if (*mass == 0)
        {
            d.mass = 1.0f / h.h_nBodies;
            d.eps  = 1.0f / sqrtf(h.h_nBodies);
        }
        else
        {
            d.mass    = *mass++;
            d.eps     = *soft++;
        }
        d.phi     = 0;
        d.density = 0;

        d.pos[0] = *pos++;
        d.pos[1] = *pos++;
        d.pos[2] = *pos++;

        d.vel[0] = *vel++;
        d.vel[1] = *vel++;
        d.vel[2] = *vel++;

        out << d;
    }

    out.close();

    return 0;
}

#ifndef SERVER
int loadSprayPattern(char *filename, SprayPattern *sp)
{
#if 0
    ifTipsy in;

    TipsyHeader h;
    TipsyDarkParticle d;

    in.open(filename, "standard");
    if (!in.is_open()) { assert(0); return 1; }

    in >> h;

    if (h.h_nDark != nParticles) {assert(0); return 2; }

    for (unsigned int i=0; i < h.h_nDark; i++)
    {
        in >> d;
        *sp->pos++ = d.pos[0];
        *sp->pos++ = d.pos[1];
        *sp->pos++ = d.pos[2];

        *sp->vel++ = d.vel[0];
        *sp->vel++ = d.vel[1];
        *sp->vel++ = d.vel[2];
    }

    in.close();
#endif

    return 0;

}
#endif
