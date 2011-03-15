#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "client.h"
#include "io.h"
#include "display.h"
#include "starspray.h"
#include "kiosk.h"
#if defined(HAVE_GPU) && defined(HAVE_NVIDIA_GPU)
#include "nvidia.h"
#endif

Environment env;

void help()
{
    fprintf(stderr, "starspray\n");
    exit(1);
}

int main(int argc, char **argv)
{
    //if (argc < 3) help();

    static struct option long_options[] = {
        {"movie",  required_argument, 0, 0},
        {"frames",  required_argument, 0, 0},
        {"help", no_argument, 0, 'h'},
        {"bcolor", required_argument, 0, 0},
        {"fullscreen", no_argument, 0, 0},
        {0, 0, 0, 0}
    };

    //env.photo_path = "/Users/jonathan/Pictures/Photo Booth";
    env.background = BLACK;
    env.maxParticles = 10000;
    env.fullscreen = 0;
    env.screenWidth = 1024;
    env.screenHeight = 768;
    env.eye.x = env.eye.ox = 0; 
    env.eye.y = env.eye.oy = 3;
    env.eye.z = env.eye.oz = 3;
    env.eye.ux = 0;
    env.eye.uy = 1;
    env.eye.uz = 0;

    env.showInfo = INFO_NONE;

    env.eye.angle = 0;
    env.eye.roll = 
    env.eye.pitch =
    env.eye.heading = 0;
    env.make_movie = 0;
    env.movie_prefix = NULL;
    env.frame_buffer = NULL;
    env.current_movie_frame = 0;
    env.message = NO_MESSAGE;
    env.message_time = 0;
    env.sceneChanged = 1;

    //if (argc < 2) help();

    /*========================================================================
     * Process the command line flags
     *======================================================================*/
    while (1)
    {
        int option_index = 0;
        int c = getopt_long(argc, argv, "ho:",
                            long_options, &option_index);

        if (c == -1) break;

        switch (c)
        {
            case 0:
                if (!strcmp("movie", long_options[option_index].name))
                {
                    env.make_movie = 1;
                    if (optarg)
                    {
                        if (sscanf(optarg, "%ix%i", &(env.screenWidth), &(env.screenHeight)) != 2)
                            help();
                    }
                } 
                else if (!strcmp("frames", long_options[option_index].name))
                {
                    env.maxTimesteps = atoi(optarg);
                }
                else if (!strcmp("fullscreen", long_options[option_index].name))
                {
                    env.fullscreen = 1;
                }
                else if (!strcmp("bcolor", long_options[option_index].name))
                {
                    if (!strcasecmp(optarg, "black"))
                        env.background = BLACK;
                    else if (!strcasecmp(optarg, "white"))
                        env.background = WHITE;
                    else
                    {
                        fprintf(stderr, "ERROR: Background color can be only black or white\n");
                        exit(2);
                    }
                }
                break;

            case 'h': help(); break;
            case 'o': 
                env.movie_prefix = MALLOC(char, strlen(optarg)+1); assert(env.movie_prefix != NULL);
                strcpy(env.movie_prefix, optarg);
                break;
            case '?': break;
        }
    }

    if (env.screenWidth <= 0 || env.screenHeight <= 0) help();
    //if (optind >= argc) help();

#if defined(HAVE_GPU) && defined(HAVE_NVIDIA_GPU)
    nvidia_init();
#endif

    /*========================================================================
     * Allocate frame buffer if we are making movie frames.
     *======================================================================*/
    env.frame_buffer     = (JSAMPROW)MALLOC(char, env.screenWidth * env.screenHeight * 4); assert(env.frame_buffer     != NULL);
    env.message_image    = (JSAMPROW)MALLOC(char, env.screenWidth * env.screenHeight * 3); assert(env.message_image    != NULL);

    int o=40;
    int s=10;
#define O(x) (o+=x+s,-o)
    ButtonImage btns[] = 
        { 
          {"Buttons/buttons_info.png",    "Buttons/buttons_info.png",    -(o),   8, 0,0, NULL,NULL, 0, BTN_INFO},
          {"Buttons/buttons_english.png", "Buttons/buttons_english.png", O(120), 8, 0,0, NULL,NULL, 0, BTN_ENGLISH},
          {"Buttons/buttons_deutsch.png", "Buttons/buttons_deutsch.png", O(120), 8, 0,0, NULL,NULL, 0, BTN_DEUTSCH},
          {"Buttons/buttons_zurueck.png", "Buttons/buttons_zurueck.png", O(80),  8, 0,0, NULL,NULL, 0, BTN_RESET},
          {"Buttons/buttons_stopp.png",   "Buttons/buttons_stopp.png",   O(80),  8, 0,0, NULL,NULL, 0, BTN_STOP},
          {"Buttons/buttons_start.png",   "Buttons/buttons_start.png",   O(40),  8, 0,0, NULL,NULL, 0, BTN_PLAY},
          {"Buttons/buttons_info.png",    "Buttons/buttons_info.png",    O(120), 8, 0,0, NULL,NULL, 0, BTN_DEMO1},
          {"Buttons/buttons_info.png",    "Buttons/buttons_info.png",    O(40), 8, 0,0, NULL,NULL, 0, BTN_DEMO2},
          //{"CHlang2.png", 1920-40, 0, 0,0,0}
        };

    fprintf(stderr, "%ld %ld\n", sizeof(btns), sizeof(btns[0]));
    env.nButtons = sizeof(btns) / sizeof(btns[0]);
    env.btns     = btns;

    int i;
    for (i=0; i < env.nButtons; i++)
    {
        //fprintf(stderr, "%s %i %i %i %i\n", env.btns[i].fname, env.btns[i].x, env.btns[i].y, env.btns[i].w, env.btns[i].h);
        //load_jpeg_image(env.btns[i].fname_up,   &env.btns[i].img_up,   &env.btns[i].h, &env.btns[i].w);
        //load_jpeg_image(env.btns[i].fname_down, &env.btns[i].img_down, &env.btns[i].h, &env.btns[i].w);
        load_png_image(env.btns[i].fname_up,   &env.btns[i].img_up,   &env.btns[i].h, &env.btns[i].w);
        load_png_image(env.btns[i].fname_down, &env.btns[i].img_down, &env.btns[i].h, &env.btns[i].w);
    }

    int h,w;
    load_png_image("info_english.png", &env.info_english, &h, &w);
    load_png_image("info_deutsch.png", &env.info_deutsch, &h, &w);

    if (env.make_movie)
    {

        if (env.movie_prefix == NULL)
        {
            env.movie_prefix = MALLOC(char, strlen("spray")+1); assert(env.movie_prefix != NULL);
            strcpy(env.movie_prefix, "spray");
        }
        printf("Making a %i-frame %ix%i movie with prefix %s.\n", 
               env.maxTimesteps, env.screenWidth, env.screenHeight, env.movie_prefix);
    }


    //print_message(MSG_LENSING);

    client_start(argc, argv);

#if 0
    if (connect_to_server())
    {
        fprintf(stderr, "Unable to connect to server!\n");
        exit(0);
    }
#endif

    kiosk();
    display_start(argc, argv); // never returns

    return 0;
}
