#ifndef LENS_H
#define LENS_H

#if defined(HAVE_GPU)
#include "lensGPU.h"
#else
#include "lensCPU.h"
#endif

#endif

