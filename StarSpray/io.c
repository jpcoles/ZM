#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "starspray.h"
#include "io.h"
#include "tipsyio.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "jpeglib.h"
#ifdef __cplusplus
}
#endif

extern Environment env;

float *vel = NULL;
FILE *ftpcmds = NULL;
FILE *ftpresp = NULL;
int   ftpresp_fd = -1;

char line0[180];
char line1[180];

int ignore_frame = 0;

#define MIN(a,b) ((a) < (b) ? (a) : (b))

void startInputBuffer()
{
    //system("rm ftpresp");
    //system("mkfifo ftpresp");

    if (ftpcmds == NULL)
    {
        fprintf(stderr, "Opening command channel. ");
        ftpcmds = fopen("ftpcmds", "w");
        assert(ftpcmds != NULL);
        fprintf(stderr, "done.\n");
        //fprintf(ftpcmds, "XXX\n");
        //fflush(ftpcmds);
    }

    if (ftpresp == NULL)
    {
        //fclose(ftpresp);
        fprintf(stderr, "Opening response channel. ");
        //ftpresp = fopen("ftpresp", "r");
        //ftpresp_fd = open("ftpresp", O_RDONLY | O_NONBLOCK, 0);
        ftpresp_fd = open("ftpresp", O_RDONLY, 0);
        fprintf(stderr, "fd=%i ", ftpresp_fd);
        ftpresp = fdopen(ftpresp_fd, "r");
        //ftpresp = fdopen(open("ftpresp", O_RDONLY , 0), "r");
        assert(ftpresp != NULL);
        assert(ftpresp_fd != -1);
        fprintf(stderr, "done.\n");
    }

    ignore_frame = 0;
}

void stopInputBuffer()
{
    system("killall -INT sftp");
    //system("rm ftpresp");
    //ftpresp = fopen("ftpresp", "r");
    //while (fgetc(ftpresp) != EOF) {}
    //fclose(ftpresp); 

    //if (ftpresp != NULL) { fclose(ftpresp); ftpresp = NULL; }
    //if (ftpcmds != NULL) { fclose(ftpcmds); ftpcmds = NULL; }
}

int loadNextFrame()
{
    return 0;
}

void saveICs()
{
    //vel = (float *)realloc(vel, env.pList.nParticles * 3 * sizeof(float));
    //memset(vel, 0, env.pList.nParticles * 3 * sizeof(float));

    if (writeTipsyPositions("starspray-ic.std", env.pList.pos, env.pList.vel, env.pList.nParticles))
        assert(0);
}

#if 0
int loadSprayPattern()
{
    //return loadSprayPattern("cdm.std", &(env.sPattern));
    return 0;
}
#endif

void save_frame_buffer()
{
    assert(env.make_movie);

    FILE * outfile;
    static char *filename = NULL;
    if (filename == NULL)
        filename = MALLOC(char, strlen(env.movie_prefix)+20+1);

    sprintf(filename, "%s.%05i.jpg", env.movie_prefix, ++env.current_movie_frame);
    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "Can't open %s\n", filename);
        exit(1);
    }

    save_frame_buffer_fp(outfile);

    fclose(outfile);
}

void save_frame_buffer_fp(FILE *outfile)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width      = env.screenWidth;
    cinfo.image_height     = env.screenHeight;
    cinfo.input_components = 3;
    cinfo.in_color_space   = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];              /* pointer to a single row */
    int row_stride;                       /* physical row width in buffer */

    row_stride = cinfo.image_width * 3;   /* JSAMPLEs per row in image_buffer */

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = & (env.frame_buffer[(cinfo.image_height-cinfo.next_scanline-1) * row_stride]);
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

}

void print_frame_buffer()
{
    FILE *lpstat;

#if 0
    lpstat = popen("lpstat", "r");
    if (lpstat != NULL)
    {
        fgetc(lpstat);
        if (!feof(lpstat)) { pclose(lpstat); return; };
    }
    pclose(lpstat);
#endif

    FILE * outfile;
    char buf[128];
    sprintf(buf, "starspray_toprint.jpg");
    if ((outfile = fopen(buf, "wb")) == NULL) {
        fprintf(stderr, "Can't open %s\n", buf);
        return;
    }

    save_frame_buffer_fp(outfile);

    fclose(outfile);

#if 0
    char cmd[256];
    //sprintf(cmd, "open %s", buf);
    sprintf(cmd, "lpr -o fitplot %s", buf);

    system(cmd);
#endif
}


void load_jpeg_image(char *fname, JSAMPROW *buf, int *buf_h, int *buf_w)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    FILE * infile;

    if ((infile = fopen(fname, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", fname);
        return;
    }

    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    fprintf(stderr, "%i %i\n", cinfo.output_components, cinfo.out_color_space);
    fprintf(stderr, "%i\n", cinfo.out_color_components);
    cinfo.output_components = 3;
    cinfo.out_color_components = 3;
    cinfo.out_color_space   = JCS_RGB;
    jpeg_start_decompress(&cinfo);

    *buf_w = cinfo.output_width;
    *buf_h = cinfo.output_height;
    *buf  = (JSAMPROW)MALLOC(char, *buf_w * *buf_h * 3);

    JSAMPROW row_pointer[1];              /* pointer to a single row */
    int row_stride;                       /* physical row width in buffer */

    row_stride = cinfo.output_width * 3;   /* JSAMPLEs per row in image_buffer */

    while (cinfo.output_scanline < cinfo.output_height) 
    {
        row_pointer[0] = & ((*buf)[(cinfo.output_height-cinfo.output_scanline-1) * row_stride]);
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

}

void print_message(int message)
{
    char *msg_files[] = 
    { 
        "msg_printing.jpg", "msg_uploading.jpg", 
        "msg_lensing.jpg", "msg_still_printing.jpg"
    };


    env.message = NO_MESSAGE;
	env.message_time = 0;

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    FILE * infile;

    if ((infile = fopen(msg_files[message], "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", msg_files[message]);
        return;
    }
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    fprintf(stderr, "%i %i\n", cinfo.output_components, cinfo.out_color_space);
    fprintf(stderr, "%i\n",    cinfo.out_color_components);
    cinfo.output_components    = 3;
    cinfo.out_color_components = 3;
    cinfo.out_color_space      = JCS_RGB;
    jpeg_start_decompress(&cinfo);

    JSAMPROW row_pointer[1];              /* pointer to a single row */
    int row_stride;                       /* physical row width in buffer */

    row_stride = cinfo.output_width * 3;   /* JSAMPLEs per row in image_buffer */

    while (cinfo.output_scanline < cinfo.output_height) 
    {
        row_pointer[0] = & (env.message_image[(cinfo.output_height-cinfo.output_scanline-1) * row_stride]);
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    env.message = message;
    if (message == MSG_LENSING)
    	env.message_time = 5;
    else
    	env.message_time = 50;
}
