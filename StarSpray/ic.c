#include <math.h>
#include "starspray.h"
#include "ic.h"

extern Environment env;
extern float starColor[];

double new_point(double Rmin, double Rmax, double slope)
{
    double c, r;

    // Normalization constant
    c = (slope == -3)
      ? log(Rmax/Rmin)
      : (pow(Rmax,3+slope) - pow(Rmin,3+slope)) / (3+slope);

    do
    {
        r = (slope == -3)
          ? Rmin * exp(drand48()*c)
          : pow((3+slope)*drand48()*c + pow(Rmin, 3+slope), 1/(3+slope));
    } while (r < Rmin);

    return r;
}

void ic_cold_sphere()
{
    int i;
    double r, z, t, w, x,y, d, v;
    env.pList.nParticles = 5000;
    if (env.pList.nParticles > env.maxParticles)
        env.pList.nParticles = env.maxParticles;

    env.pList.movingParticleIndex = env.pList.nParticles;

    float  mass = 1.0 / env.pList.nParticles;
    double soft = 1.0 / sqrtf(env.pList.nParticles);

    for (i=0; i < env.pList.nParticles; i++)
    {
        r = new_point(0.1, 1, -2);

        z = 2.0 * drand48() - 1.0;

        t = 2.0 * M_PI * drand48();
        w = sqrt(1 - z*z);
        x = w * cos(t);
        y = w * sin(t);

        env.pList.pos[3*i+0] = x * r;
        env.pList.pos[3*i+1] = y * r;
        env.pList.pos[3*i+2] = z * r;

        env.pList.vel[3*i+0] = 0;
        env.pList.vel[3*i+1] = 0;
        env.pList.vel[3*i+2] = 0;

//      env.pList.clr[4*i+0] = starColor[0];
//      env.pList.clr[4*i+1] = starColor[1];
//      env.pList.clr[4*i+2] = starColor[2];
//      env.pList.clr[4*i+3] = starColor[3];
    }
}

int compar(const void *a0, const void *b0)
{
    float *a = (float *)a0;
    float *b = (float *)b0;
    float ar = a[0]*a[0] + a[1]*a[1] + a[2]*a[2];
    float br = b[0]*b[0] + b[1]*b[1] + b[2]*b[2];
    if (ar < br) return -1;
    if (ar > br) return +1;
    return 0;
}

void ic_sphere()
{
    int i,j;
    double r, z, t, w, x,y, d, v;
    env.pList.nParticles = 10000;
    if (env.pList.nParticles > env.maxParticles)
        env.pList.nParticles = env.maxParticles;

    env.pList.movingParticleIndex = env.pList.nParticles;

    float  mass = 1.0 / env.pList.nParticles;
    double soft = 1.0 / sqrtf(env.pList.nParticles);

    for (i=0; i < env.pList.nParticles; i++)
    {
        r = new_point(0.1, 1, -1);

        z = 2.0 * drand48() - 1.0;

        t = 2.0 * M_PI * drand48();
        w = sqrt(1 - z*z);
        x = w * cos(t);
        y = w * sin(t);

        //env.pList.pos[3*i+0] = 0.1 * (drand48()-0.5); //x * r;
        env.pList.pos[3*i+0] = x * r;
        env.pList.pos[3*i+1] = 0.001 * (drand48()-0.5); //y * r;  // drand48 fixes pkdgrav bug with assert(u>0) in moment.c:163 failing
        env.pList.pos[3*i+2] = z * r;

    }

    qsort(env.pList.pos, env.pList.nParticles, 3*sizeof(*env.pList.pos), compar);

    for (i=0; i < env.pList.nParticles; i++)
    {
        x = env.pList.pos[3*i+0];
        y = env.pList.pos[3*i+1];
        z = env.pList.pos[3*i+2];

        r = sqrt(x*x + y*y + z*z);

        //x = 0.1;
        d = 2*r;

        double vx, vy, vz;
        double theta = atan2(z*r, x*r);
        double m = (float)(env.pList.nParticles - i) / env.pList.nParticles;

        vx = 0;
        vy = 0;
        vz = sqrt(m * d*r/pow(d*d + soft, 1.5));

        env.pList.vel[3*i+0] = vx*cos(theta) - vz*sin(theta);
        env.pList.vel[3*i+1] = 0;
        env.pList.vel[3*i+2] = vx*sin(theta) + vz*cos(theta);

//      env.pList.clr[4*i+0] = starColor[0];
//      env.pList.clr[4*i+1] = starColor[1];
//      env.pList.clr[4*i+2] = starColor[2];
//      env.pList.clr[4*i+3] = starColor[3];

//      float r = fabsf(env.pList.vel[pi+0]); 
//      float g = fabsf(env.pList.vel[pi+1]); 
//      float b = fabsf(env.pList.vel[pi+2]); 
//      color_ramp_astro(&r, &g, &b);
    }

}
