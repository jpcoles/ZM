#if defined(HAVE_GPU) && defined(HAVE_NVIDIA_GPU)

#include <assert.h>
#include <math.h>
#include <string.h>
#include "lens.h"
#include <cutil.h>

#define BLOCK_SIZE 8

__global__ 
void lensd(int h,  int w,  unsigned char *image, 
           int sh, int sw, unsigned char *source,
           int cred, int *cmass, float thee);

__host__
void lens(int h, int w, unsigned char *image, int *mass,
          int sh, int sw, unsigned char *source,
          int cred, int *cmass)
  /* assumes these sizes:
     image[h][w], mass[h][w], source[sh][sw], cmass[h/cred][w/cred]
  */
{ 
    int size;
    unsigned char *imaged, *sourced;
    int *cmassd;


    int x,y;
    int z,cz;        /* 1D array indices */
    float thee=0;     /* thee means theta_E */
    float cw = w/cred;
    /* reduce mass density to coarse grid */
    float Mtotal=0;
    for (y=0; y<h; y++)
    {
        int cy = (float)y/cred;
        for (x=0; x<w; x++)
        { 
            int cx = (float)x/cred; 
            z  = w*y + x;
            cz = cw*cy + cx;
            float m = mass[z];
            cmass[cz] += m;
            Mtotal    += m;
        }
    }

    //fprintf(stderr, "ch=%f cw=%f\n", ch, cw);

    //thee = .35 * h*h / thee;    /* set theta_E to 1/3 the height */
    if (Mtotal != 0) 
        thee = .33 * h/Mtotal;    /* set theta_E to 1/3 the height */
    else
        thee = 0;

    size = sh * sw * 3 * sizeof(char);
    //fprintf(stderr, "size=%i\n", size);
    CUDA_SAFE_CALL(cudaMalloc((void **)&sourced, size));
    CUDA_SAFE_CALL(cudaMemcpy(sourced, source, size, cudaMemcpyHostToDevice));

    size = h/cred * w/cred * sizeof(int);
    //fprintf(stderr, "size=%i\n", size);
    CUDA_SAFE_CALL(cudaMalloc((void **)&cmassd, size));
    CUDA_SAFE_CALL(cudaMemcpy(cmassd, cmass, size, cudaMemcpyHostToDevice));

    size = h * w * 4 * sizeof(char);
    //fprintf(stderr, "size=%i\n", size);
    CUDA_SAFE_CALL(cudaMalloc((void **)&imaged, size));

    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 dimGrid(w/dimBlock.x, h/dimBlock.y);

    //fprintf(stderr, "dimGrid.x=%i dimGrid.y=%i\n", dimGrid.x, dimGrid.y);

    lensd<<<dimGrid, dimBlock>>>(h, w, imaged, sh, sw, sourced, cred, cmassd, thee);

    CUDA_SAFE_CALL(cudaMemcpy(image, imaged, size, cudaMemcpyDeviceToHost));

    cudaFree(imaged);
    cudaFree(sourced);
    cudaFree(cmassd);
}

__global__ 
void lensd(int h,  int w,  unsigned char *image, 
           int sh, int sw, unsigned char *source,
           int cred, int *cmass, float thee)
{
    int x = blockIdx.x * BLOCK_SIZE + threadIdx.x;
    int y = blockIdx.y * BLOCK_SIZE + threadIdx.y;

    if (x > w || y > h) return;

    int sx = x + (sw - w)/2.0;  
    int sy = y + (sh - h)/2.0;  /* align centers */

    int cx, cy;
    float ch = h/cred;
    float cw = w/cred;
    int *cmptr = cmass;
    for (cy=0; cy<ch; cy++)
    {
        float dy = y - cy*cred - cred/2;
        float dy2 = dy*dy;
        for (cx=0; cx<cw; cx++)
        { 
            float dx = x - cx*cred;
            float fac = (*cmptr++) * thee/sqrtf((dy2 + dx*dx + 10));
            sx -= dx*fac; 
            sy -= dy*fac;
        }
    }

    int i_offs = 4 * (y * w + x);
    if (0 < sx && sx < sw-1 && 0 < sy && sy < sh-1)
    { 
        int sz = (int)(sw*sy + sx) * 3;
        image[i_offs+0] = source[sz+0];
        image[i_offs+1] = source[sz+1];
        image[i_offs+2] = source[sz+2];
        image[i_offs+3] = 255;
    }
    else
    {
        image[i_offs+0] = 0;
        image[i_offs+1] = 0;
        image[i_offs+2] = 0;
        image[i_offs+3] = 255;
    }

}

#endif
