#ifndef LENS_CPU_H
#define LENS_CPU_H

#include <sys/types.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
#include "jpeglib.h"
#ifdef __cplusplus
}
#endif

void lens(int h, int w, JSAMPROW image, int *mass,
          int sh, int sw, JSAMPROW source,
          int cred, int *cmass);

#endif

