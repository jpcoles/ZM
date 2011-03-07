#ifndef LENS_GPU_H
#define LENS_GPU_H

#include <sys/types.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
void lens(int h, int w, unsigned char *image, int *mass,
          int sh, int sw, unsigned char *source,
          int cred, int *cmass);
#ifdef __cplusplus
}
#endif

#endif

