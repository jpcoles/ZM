#ifndef HAVE_GPU
#include <math.h>
#include <string.h>
#include "lens.h"

void lens(int h, int w, JSAMPROW image, int *mass,
          int sh, int sw, JSAMPROW source,
          int cred, int *cmass)
  /* assumes these sizes:
     image[h][w], mass[h][w], source[sh][sw], cmass[h/cred][w/cred]
  */
{ 
    //fprintf(stderr, "h=%i w=%i  sh=%i sw=%i  cred=%i\n", h, w, sh, sw, cred);
    //memset(image, 0x0, h*w*4);
    //return;

    int x,y;
    int sx,sy,cx,cy;  /* 2D array indices */
    int z,sz,cz,k;        /* 1D array indices */
    float ch,cw;  
    float dx,dy;
    float fac,thee=0;     /* thee means theta_E */
    ch = h/cred; 
    cw = w/cred;
    /* reduce mass density to coarse grid */
#if 1
    float Mtotal = 0;
    for (y=0; y<h; y++)
    {
        cy = (float)y/cred;
        for (x=200; x<w; x++)
        { 
            cx = (float)x/cred; 
            z  = w*y + x;
            cz = cw*cy + cx;
            float m = mass[z];
            //cmass[cz] += m;
            //if (y == h/2)
            {
                cmass[cz] += m;
                Mtotal    += m;
            }
        }
    }
#endif

    fprintf(stderr, "ch=%f cw=%f\n", ch, cw);

#if 0
    for (cy=0; cy<ch; cy++)
    {
        for (cx=0; cx<cw; cx++)
        { 
            z = (w*cy + cx) * 4;
            cz = cw*cy + cx;
            if (cmass[cz] != 0)
                for (k=0; k<4; k++) image[z+k] = 0;
        }
    }

    return;
#endif

    //thee = .35 * h*h / thee;    /* set theta_E to 1/3 the height */
    if (Mtotal != 0) 
        thee = .4 * h/Mtotal;    /* set theta_E to 1/3 the height */
    else
        thee = 0;

    int i_offs = 0;

    //thee = 10000; //h*h/(3*3);
    /* now the main loop */
    for (y=0; y<h; y++)
    {
        for (x=0; x<w; x++)
        { 
#if 0
            z = w*y + x;
            if (mass[z] != 0)
            {
                z *= 4;
                for (k=0; k<3; k++) image[z+k] = 255;
                image[z+3] = 255;
            }
            continue;
#endif

            sx = x + (sw - w)/2.0;  
            sy = y + (sh - h)/2.0;  /* align centers */

            //fprintf(stderr, "sx=%f sy=%f\n", sx, sy);
            //fprintf(stderr, "sx=%i sy=%i\n", sx, sy);
            int *cmptr = cmass;
#if 1
            const float cred2 = cred*cred;
            for (cy=0; cy<ch; cy++)
            {
                dy = y - cy*cred - cred/2;
                const float dy2 = dy*dy;
                for (cx=0; cx<cw; cx++)
                { 
                    cz = cw*cy + cx;
                    dx = x - cx*cred;
                    fac = (*cmptr++) * thee/sqrtf((dy2 + dx*dx + 1000));
                    sx -= dx*fac; 
                    sy -= dy*fac;
                }
            }
#endif
            //z = (w*y + x) * 4;
            //fprintf(stderr, "x=%i y=%i z=%i\n", x,y,z);
#if 1
            if (0 < sx && sx < sw-1 && 0 < sy && sy < sh-1)
            { 
                sz = (int)(sw*sy + sx) * 3;
                //fprintf(stderr, "sz=%i\n", sz);
                image[i_offs+0] = source[sz+0];
                image[i_offs+1] = source[sz+1];
                image[i_offs+2] = source[sz+2];
                image[i_offs+3] = 255;
            }
#endif

            i_offs += 4;
        }
    }
}

#endif
