#ifndef IO_H
#define IO_H

#include <png.h>
#ifdef __cplusplus
extern "C" {
#endif
#include "jpeglib.h"
#ifdef __cplusplus
}
#endif

int loadNextFrame();
void saveICs();
void startInputBuffer();
void stopInputBuffer();
void save_frame_buffer();
void save_frame_buffer_fp(FILE *outfile);
void print_frame_buffer();
void load_photo();
void unload_photo();
float ntohf(unsigned int x);
unsigned int htonf(float x);
void print_message(int message);
void load_jpeg_image(char *fname, JSAMPROW *buf, int *buf_h, int *buf_w);
void load_png_image(char *fname, png_bytep *buf, int *buf_h, int *buf_w);

#endif
