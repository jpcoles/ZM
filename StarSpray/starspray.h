#ifndef STARSPRAY
#define STARSPRAY

#include <sys/socket.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <png.h>
#ifdef __cplusplus
extern "C" {
#endif
#include "jpeglib.h"
#ifdef __cplusplus
}
#endif


#define SIM_STOPPED  0
#define SIM_STOPPING 1
#define SIM_STARTING 2
#define SIM_RUNNING  4
#define SIM_PAUSED   8

#define SVR_CMD_START_SIM   1000
#define SVR_CMD_NEXT_FRAME  1001
#define SVR_CMD_STOP_SIM    1002

#define MALLOC(type, n) (type *)malloc((n) * sizeof(type));

#define BLACK 0
#define WHITE 255

#define NO_MESSAGE -1
#define MSG_PRINTING 0
#define MSG_UPLOADING 1
#define MSG_LENSING 2
#define MSG_STILL_PRINTING 3

#define BTN_INFO        0
#define BTN_ENGLISH     1
#define BTN_DEUTSCH     2
#define BTN_DEMO1       3
#define BTN_DEMO2       4
#define BTN_PLAY        5
#define BTN_STOP        6
#define BTN_RESET       7

#define INFO_NONE       0
#define INFO_ENGLISH    1
#define INFO_DEUTSCH    2

typedef struct
{
    float x, y, z;
} Coord;

typedef struct
{
    float ox, oy, oz; /* Original position */
    float x, y, z;
    float ux, uy, uz; /* Up vector */
    double angle;

    float roll, pitch, heading;

} Eye;

typedef struct 
{
    unsigned int nParticles;
    unsigned int movingParticleIndex;
    float *pos;
    float *vel;
    float *clr;

    float *dest;
    unsigned int   *step;
} ParticleList;

typedef struct 
{
    unsigned int nPoints;
    float *pos;
    float *vel;

} SprayPattern;

typedef struct
{
    char *fname_up;
    char *fname_down;
    int x,y, w,h;
    png_bytep img_up;
    png_bytep img_down;
    int state;
    int id;
} ButtonImage;

typedef struct
{
    int maxParticles;
    int fullscreen;

    int screenWidth;
    int screenHeight;

    Coord pointer;
    Eye eye;


    int maxTimesteps;
    int currentTimestep;
    int loadingTimestep;
    ParticleList pList;
    SprayPattern sPattern;

    int spinning;

    int simStatus;
    int showInfo;
    png_bytep info_english;
    png_bytep info_deutsch;

    //char *addr;
    //int port;
    //int sock;

    pthread_t client_thread;

    char *movie_prefix;
    int make_movie;
    JSAMPROW frame_buffer; 
    int current_movie_frame;

    int background;

    int message;
    int message_time;
    JSAMPROW message_image;; 

    int nButtons;
    ButtonImage *btns;

    int sceneChanged;

} Environment;

#endif

