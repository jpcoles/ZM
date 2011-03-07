#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "starspray.h"
#include "display.h"
#include "visualize.h"

extern Environment env;

void display_start(int argc, char **argv)
{
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    //glutInitWindowPosition(100,100);
    glutInitWindowSize(env.screenWidth,env.screenHeight);

    glutInit(&argc,argv);
    glutCreateWindow("Star Spray");
    viz_init();
    glutMainLoop();
}

