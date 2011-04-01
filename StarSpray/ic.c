#include <math.h>
#include "starspray.h"
#include "ic.h"

extern Environment env;
extern float starColor[];

typedef struct
{
    double w,x,y,z;
} quaternion_t;

#define NEW_QUATERNION(w,x,y,z) {(w),(x),(y),(z)}
void new_quaternion(quaternion_t *q, double w, double x, double y, double z)
{
    q->w = w;
    q->x = x;
    q->y = y;
    q->z = z;
}

void mulq(quaternion_t *q0, quaternion_t *q1, quaternion_t *q2)
{
    q2->w = q0->w*q1->w - q0->x*q1->x - q0->y*q1->y - q0->z*q1->z;
    q2->x = q0->w*q1->x + q0->x*q1->w + q0->y*q1->z - q0->z*q1->y;
    q2->y = q0->w*q1->y - q0->x*q1->z + q0->y*q1->w + q0->z*q1->x;
    q2->z = q0->w*q1->z + q0->x*q1->y - q0->y*q1->x + q0->z*q1->w;
}

void invq(quaternion_t *q0, quaternion_t *q1)
{
    new_quaternion(q1, q0->w, -q0->x, -q0->y, -q0->z);
}

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

        env.pList.mass[i] = mass;
        env.pList.soft[i] = soft;

//      env.pList.clr[4*i+0] = starColor[0];
//      env.pList.clr[4*i+1] = starColor[1];
//      env.pList.clr[4*i+2] = starColor[2];
//      env.pList.clr[4*i+3] = starColor[3];
    }
}

int compar_r(const void *a0, const void *b0)
{
    float *a = (float *)a0;
    float *b = (float *)b0;
    if (*a < *b) return -1;
    if (*a > *b) return +1;
    return 0;
}

void ic_plummer_sphere()
{
    int i;
    double r, z, t, w, x,y, d, v;
    env.pList.nParticles = 2500;
    if (env.pList.nParticles > env.maxParticles)
        env.pList.nParticles = env.maxParticles;

    env.pList.movingParticleIndex = env.pList.nParticles;

    float  mass = 1.0 / env.pList.nParticles;
    double soft = 1.0 / sqrtf(env.pList.nParticles);

    float *rs = (float *)malloc(env.pList.nParticles * sizeof(*rs));
    for (i=0; i < env.pList.nParticles; i++)
        rs[i] = new_point(0.1, 1, -1);

    qsort(rs, env.pList.nParticles, sizeof(*rs), compar_r);

    double M = 0;
    for (i=0; i < env.pList.nParticles; i++)
    {
        r = rs[i]; //new_point(0.1, 1, -2);

        //----------------------------------------------------------------------
        // Pick two points (a,b) and (c,d) such that·
        //     a^2+b^2 < 1 and c^2+d^2 < 1
        // From http://mathworld.wolfram.com/HyperspherePointPicking.html
        //----------------------------------------------------------------------
        double a, b;
        do { a = 2*drand48()-1; b = 2*drand48()-1; } while ((a*a + b*b) >= 1);

        double c, d;
        do { c = 2*drand48()-1; d = 2*drand48()-1; } while ((c*c + d*d) >= 1);

        double s = sqrt((1.0 - a*a - b*b) / (c*c + d*d));
        c *= s;
        d *= s;

        quaternion_t qinv,qt;
        quaternion_t q = NEW_QUATERNION(a,b,c,d);
        invq(&q,&qinv);

        quaternion_t qx = NEW_QUATERNION(0,r,0,0);
        mulq(&q,&qx,&qt);
        mulq(&qt,&qinv,&qx);

        M += mass;
        quaternion_t qv = NEW_QUATERNION(0,0,-sqrt(M/r),0);
        mulq(&q,&qv,&qt);
        mulq(&qt,&qinv,&qv);

        env.pList.pos[3*i+0] = qx.x * r;
        env.pList.pos[3*i+1] = qx.y * r;
        env.pList.pos[3*i+2] = qx.z * r;

        env.pList.vel[3*i+0] = qv.x;
        env.pList.vel[3*i+1] = qv.y;
        env.pList.vel[3*i+2] = qv.z;

        env.pList.mass[i] = mass;
        env.pList.soft[i] = soft;
    }

    free(rs);
}


void ic_colliding_plummer_spheres()
{
    float x0, y0, z0, vx, vy, vz;

    int i,j,k;
    double r, z, t, w, x,y, d, v;
    env.pList.nParticles = 5000;
    if (env.pList.nParticles > env.maxParticles)
        env.pList.nParticles = env.maxParticles;

    env.pList.movingParticleIndex = env.pList.nParticles;

    float  mass = 1.0 / env.pList.nParticles;
    double soft = 1.0 / sqrtf(env.pList.nParticles);

    float *rs = (float *)malloc(env.pList.nParticles * sizeof(*rs));

    for (i=0; i < env.pList.nParticles; i++)
        rs[i] = new_point(0.1, 1, -1);
    qsort(rs, env.pList.nParticles/2, sizeof(*rs), compar_r);
    qsort(rs+env.pList.nParticles/2, (env.pList.nParticles-env.pList.nParticles/2), sizeof(*rs), compar_r);

    i=0;
    x0 = -1.5;
    y0 = 0;
    z0 = 0;
    vx = 1;
    vy = 0;
    vz = 0;
    for (k=0; k < 2; k++)
    {
        double M = 0;
        for (; i < env.pList.nParticles; i++)
        {
            if (k==0 && i >= env.pList.nParticles/2) break;

            r = rs[i]; //new_point(0.1, 1, -2);

            //----------------------------------------------------------------------
            // Pick two points (a,b) and (c,d) such that·
            //     a^2+b^2 < 1 and c^2+d^2 < 1
            // From http://mathworld.wolfram.com/HyperspherePointPicking.html
            //----------------------------------------------------------------------
            double a, b;
            do { a = 2*drand48()-1; b = 2*drand48()-1; } while ((a*a + b*b) >= 1);

            double c, d;
            do { c = 2*drand48()-1; d = 2*drand48()-1; } while ((c*c + d*d) >= 1);

            double s = sqrt((1.0 - a*a - b*b) / (c*c + d*d));
            c *= s;
            d *= s;

            quaternion_t qinv,qt;
            quaternion_t q = NEW_QUATERNION(a,b,c,d);
            invq(&q,&qinv);

            quaternion_t qx = NEW_QUATERNION(0,r,0,0);
            mulq(&q,&qx,&qt);
            mulq(&qt,&qinv,&qx);

            M += mass;
            quaternion_t qv = NEW_QUATERNION(0,0,-sqrt(M/r),0);
            mulq(&q,&qv,&qt);
            mulq(&qt,&qinv,&qv);

            env.pList.pos[3*i+0] = qx.x * r + x0;
            env.pList.pos[3*i+1] = qx.y * r + y0;
            env.pList.pos[3*i+2] = qx.z * r + z0;

            env.pList.vel[3*i+0] = qv.x + vx;
            env.pList.vel[3*i+1] = qv.y + vy;
            env.pList.vel[3*i+2] = qv.z + vz;

            env.pList.mass[i] = mass;
            env.pList.soft[i] = soft;
        }

        x0 *= -1;
        y0 *= -1;
        z0 *= -1;
        vx *= -1;
        vy *= -1;
        vz *= -1;
    }

    free(rs);
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
    env.pList.nParticles = 5000;
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

        env.pList.mass[i] = mass;
        env.pList.soft[i] = soft;

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

void ic_figure_eight()
{
    int i,j;
    double r, z, t, w, x,y, d, v;
    env.pList.nParticles = 3;
    if (env.pList.nParticles > env.maxParticles)
        env.pList.nParticles = env.maxParticles;

    env.pList.movingParticleIndex = env.pList.nParticles;

    float  mass = 1; //1. / env.pList.nParticles;
    double soft = 1e-1; //1.0 / sqrtf(env.pList.nParticles);

    env.pList.pos[3*0+0] = -0.995492; 
    env.pList.pos[3*0+1] = 0.0; 
    env.pList.pos[3*0+2] = 0.0; 
    env.pList.vel[3*0+0] = -0.347902;
    env.pList.vel[3*0+1] = -0.53393;
    env.pList.vel[3*0+2] = 0.0; 
    env.pList.mass[0] = mass;
    env.pList.soft[0] = soft;

    env.pList.pos[3*1+0] = 0.995492; 
    env.pList.pos[3*1+1] = 0.0; 
    env.pList.pos[3*1+2] = 0.0; 
    env.pList.vel[3*1+0] = -0.347902;
    env.pList.vel[3*1+1] = -0.53393;
    env.pList.vel[3*1+2] = 0.0; 
    env.pList.mass[1] = mass;
    env.pList.soft[1] = soft;

    env.pList.pos[3*2+0] = 0.0; 
    env.pList.pos[3*2+1] = 0.0; 
    env.pList.pos[3*2+2] = 0.0; 
    env.pList.vel[3*2+0] = 0.695804;
    env.pList.vel[3*2+1] = 1.06786;
    env.pList.vel[3*2+2] = 0.0; 
    env.pList.mass[2] = mass;
    env.pList.soft[2] = soft;
}

