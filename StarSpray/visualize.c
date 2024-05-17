#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "color_ramp.h"
#include "starspray.h"
#include "visualize.h"
#include "client.h"
#include "io.h"
#include "keys.h"
#include "ic.h"

#define WITH_POINTS 0

extern Environment env;

void doSpray();
void clearParticles();
void toggleSpinning();

float ENV_RADIUS = 2;
const float SPRAY_RADIUS = 0.1;

float starColor[] = {0.8f, 0.72f, 0.0f, 1.0f};
//float starColor[] = {0.6f, 0.52f, 0.0f, 1.0f};

const int cred = 256;
int *cmass = NULL;


GLuint sphere = 1;
GLuint scene = 2;


void viz_init()
{
    if (env.fullscreen) glutFullScreen();
    //CGAssociateMouseAndMouseCursorPosition(false);
    glutSetCursor(GLUT_CURSOR_NONE);
    glClearDepth(1.0f);

    //glutEstablishOverlay();
    //glutOverlayDisplayFunc(onUpdateInfo);
    //glutShowOverlay();

    glutDisplayFunc(onUpdate);
    glutReshapeFunc(onReshape);
    glutTimerFunc(100,onTimer,0);
    //glutTimerFunc(30,onTimer,0);
    //glutIdleFunc(onIdle);

    //glutMotionFunc(on2DDrag);
    glutMouseFunc(onClick);
    glutKeyboardFunc(onKeyboard);

    glutSpaceballMotionFunc(on3DDrag);
    //glutSpaceballRotateFunc(on3DRotate);

    //glClearColor(1, 1, 1, 0);
#if 1
    switch (env.background)
    {
        case WHITE:
            glClearColor(1, 1, 1, 0);
            break;
        case BLACK:
            glClearColor(0, 0, 0, 0);
            break;
    }
#endif

    glPointSize(2.0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //========================================================================
    // Lighting
    //========================================================================
    //glShadeModel(GL_FLAT);
#if 1
    glEnable(GL_DEPTH_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

    GLfloat global_light_ambient[] = {0.1,0.1,0.1,1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_light_ambient);

    GLfloat light_ambient[]  = {.10, .10, .10, 1.0};
    GLfloat light_diffuse[]  = {0.8, 0.8, 0.8, 1.0};
    GLfloat light_specular[] = { .0,  .0,  .0, 1.0};

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat mat_shininess[]= {0.0};                   // Define shininess of surface
    //glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);   // Set material properties
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, starColor);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); // Set material properties

    //glEnable(GL_CULL_FACE);
#endif
    //========================================================================
    //========================================================================

    glGenLists(2);
    glNewList(sphere, GL_COMPILE);
    glutSolidSphere(0.02, 20, 20);
    glEndList();

    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //glPixelStorei(GL_PACK_ALIGNMENT, 1);
}

#if WITH_BUTTONS
int inButton(int i, int x, int y)
{
    int in = 0;

    if (0 <= i && i < env.nButtons)
    {
        if (env.btns[i].img_up != NULL)
        {
            int bx = env.screenWidth + env.btns[i].x;
            int by = env.screenHeight - env.btns[i].y - env.btns[i].h;
            in =  ((bx <= x && x <= (bx + env.btns[i].w))
               &&  (by <= y && y <= (by + env.btns[i].h)));
        }
    }

    return in;
}
#endif

void doSpray()
{
    //fprintf(stderr, "on3DDrag %f %f %f\n", env.pointer.x, env.pointer.y, env.pointer.z);

    if (env.simStatus != SIM_STOPPED) return;

    float x, y, z;
    x = env.eye.x - 2*env.eye.ux;
    y = env.eye.y - 2*env.eye.uy;
    z = env.eye.z - 2*env.eye.uz;

    // a little random offset from the spray can
    float r = 0.2*drand48();

    unsigned int i, pi, ci;
    for (i=0; i < 5 && env.pList.nParticles < env.maxParticles; i++)
    {
        pi = env.pList.nParticles * 3;
        ci = env.pList.nParticles * 4;

        float xx, yy, zz;
        xx = (2*drand48() - 1) * SPRAY_RADIUS;
        yy = (2*drand48() - 1) * SPRAY_RADIUS;
        zz = (2*drand48() - 1) * SPRAY_RADIUS;

        env.pList.dest[pi+0] = env.pointer.x + xx;
        env.pList.dest[pi+1] = env.pointer.y + yy;
        env.pList.dest[pi+2] = env.pointer.z + zz;

        env.pList.pos[pi+0] = x;
        env.pList.pos[pi+1] = y;
        env.pList.pos[pi+2] = z;

        env.pList.vel[pi+0] = 
        env.pList.vel[pi+1] = 
        env.pList.vel[pi+2] = 0;

        env.pList.mass[env.pList.nParticles] = 0;
        env.pList.soft[env.pList.nParticles] = 0;

        env.pList.step[env.pList.nParticles] = 0;

        env.pList.clr[ci+0] = 0; 
        env.pList.clr[ci+1] = 0; 
        env.pList.clr[ci+2] = 0; 
        env.pList.clr[ci+3] = 0; 

        env.pList.nParticles++;
    }
}

void clearParticles()
{
    client_clear_particles();
}

void toggleSpinning()
{
    env.spinning = !env.spinning;
}

void on2DDrag(int x0, int y0)
{
    static float oldx = -10;
    static float oldz = -10;

    float x = 2.0*x0 / env.screenWidth  - 1.0;
    float z = 2.0*y0 / env.screenHeight - 1.0;

    if (oldx != -10)
        on3DDrag((x - oldx)*1000, 0, (z - oldz)*1000);

    oldx = x;
    oldz = z;

#if WITH_BUTTONS
    for (int i=0; i < env.nButtons; i++)
        env.btns[i].state &= inButton(i, x0, y0);
#endif
}

#define DOT(x0,y0,z0, x1,y1,z1) ((x0)*(x1) + (y0)*(y1) + (z0)*(z1))

#define NORM(xo,yo,zo, x0,y0,z0) do {\
        float _r = sqrt(DOT(x0,y0,z0,x0,y0,z0)); assert(_r > 0); \
        xo = (x0)/_r; \
        yo = (y0)/_r; \
        zo = (z0)/_r; } while(0)

#define CROSS(xo,yo,zo, x0,y0,z0, x1,y1,z1, f) do {\
        float _xo = y0 * z1 - z0 * y1; \
        float _yo = z0 * x1 - x0 * z1; \
        float _zo = x0 * y1 - y0 * x1; \
        float _r = sqrt(DOT(_xo,_yo,_zo,_xo,_yo,_zo)); assert(_r > 0); \
        xo = f*_xo/_r; \
        yo = f*_yo/_r; \
        zo = f*_zo/_r; } while(0)

void strafe(float v)
{
    float xd,yd,zd;

    double r0 = sqrt(env.eye.x*env.eye.x + env.eye.y*env.eye.y + env.eye.z*env.eye.z);

    CROSS(xd,yd,zd, env.eye.x,  env.eye.y,  env.eye.z,
                    env.eye.ux, env.eye.uy, env.eye.uz, 1);

    //r = env.eye.x*env.eye.x + env.eye.y*env.eye.y + env.eye.z*env.eye.z;
    //double l = r0 * 10 * M_PI/180;

    xd *= v;
    yd *= v;
    zd *= v;

    env.eye.x += xd;
    env.eye.y += yd;
    env.eye.z += zd;

    double r = sqrt(env.eye.x*env.eye.x + env.eye.y*env.eye.y + env.eye.z*env.eye.z);
    r = r0/r;
    env.eye.x *= r;
    env.eye.y *= r;
    env.eye.z *= r;
}

void on3DDrag(int x0, int y0, int z0)
{
    float x1, y1, z1;
    float xd, yd, zd;
    float xn, yn, zn;
    float x, y, z;
    float r;

    z1 = -y0 / 1000.0;
    x1 = -x0 / 1000.0;
    y1 =  z0 / 1000.0;

    //fprintf(stderr, "%i %i %i\n", x0,y0,z0);

    if (env.simStatus == SIM_STOPPED)
    {
        //x1 = -x0 / 1000.0;
        //y1 = -z0 / 1000.0;
        //z1 = -y0 / 1000.0;

        //xd = x1*cos(-env.eye.angle) - z1*sin(-env.eye.angle);
        //yd = y1;
        //zd = x1*sin(-env.eye.angle) + z1*cos(-env.eye.angle);

        x = env.pointer.x;
        y = env.pointer.y;
        z = env.pointer.z;

        CROSS(xd,yd,zd, env.eye.x,  env.eye.y,  env.eye.z,
                        env.eye.ux, env.eye.uy, env.eye.uz, 1);
        x += xd*x1;
        y += yd*x1;
        z += zd*x1;

        NORM(xd,yd,zd, env.eye.ux, env.eye.uy, env.eye.uz);
        x += xd*z1;
        y += yd*z1;
        z += zd*z1;

        NORM(xd,yd,zd, env.eye.x, env.eye.y, env.eye.z);
        x -= xd*y1;
        y -= yd*y1;
        z -= zd*y1;

        r = sqrt(x*x + y*y + z*z);
        double r0 = sqrt(env.eye.x*env.eye.x + env.eye.y*env.eye.y + env.eye.z*env.eye.z);
        r0 *= 0.4;

        if (r > r0)
        {
            x /= r/r0;
            y /= r/r0;
            z /= r/r0;
        }

        env.pointer.x = x; //fminf(fmaxf(-1, env.pointer.x + x0 / 1000.0), 1);
        env.pointer.y = y; //fminf(fmaxf(-1, env.pointer.y - z0 / 1000.0), 1);
        env.pointer.z = z; //fminf(fmaxf(-1, env.pointer.z + y0 / 1000.0), 1);
        
        doSpray();
    }
    else
    {

        double r;


//      double dot = (x1*env.eye.x + y1*env.eye.y + z1*env.eye.z)/r;
//      xd = env.eye.x * dot;
//      yd = env.eye.y * dot;
//      zd = env.eye.z * dot;

//      yd = env.eye.x * y1 - env.eye.y * x1;
//      zd = env.eye.y * z1 - env.eye.z * y1;
//      xd = env.eye.z * x1 - env.eye.x * z1;

        //env.eye.x -= z1;
        //env.eye.y -= z1;
        //env.eye.z -= z1;
        r = sqrt(env.eye.x*env.eye.x + env.eye.y*env.eye.y + env.eye.z*env.eye.z);

        NORM(xn,yn,zn, env.eye.x, env.eye.y, env.eye.z);

        while (fabs(y1) > 1e-10) 
        {
            xd = xn * y1;
            yd = yn * y1;
            zd = zn * y1;

            double d = DOT(env.eye.x,    env.eye.y,    env.eye.z, 
                           env.eye.x-xd, env.eye.y-yd, env.eye.z-zd);

            if (d > 0)
            {
                env.eye.x -= xd;
                env.eye.y -= yd;
                env.eye.z -= zd;
                r = sqrt(env.eye.x*env.eye.x + env.eye.y*env.eye.y + env.eye.z*env.eye.z);
                if (r < 1.01)
                {
                    env.eye.x /= r/1.01;
                    env.eye.y /= r/1.01;
                    env.eye.z /= r/1.01;
                }
                else if (r > 30.01)
                {
                    env.eye.x /= r/30.01;
                    env.eye.y /= r/30.01;
                    env.eye.z /= r/30.01;
                }
                break;
            }
            else
            {
                y1 /= 2;
            }
        }

        double r0 = sqrt(env.eye.x*env.eye.x + env.eye.y*env.eye.y + env.eye.z*env.eye.z);

        strafe(x1*r0 * 10 * M_PI/180);

        //fprintf(stderr, "%f %f %f %f %f\n", r, l, xd, yd, zd);


        CROSS(xd,yd,zd, env.eye.x,  env.eye.y,  env.eye.z,
                        env.eye.ux, env.eye.uy, env.eye.uz, -1);

        CROSS(env.eye.ux, env.eye.uy, env.eye.uz, 
              env.eye.x,  env.eye.y,  env.eye.z,
              xd, yd, zd, 1);

        //fprintf(stderr, "%f %f %f\n", env.eye.ux, env.eye.uy, env.eye.uz);

        //--------------------------------------------------------------------------------


        xd = env.eye.ux;
        yd = env.eye.uy;
        zd = env.eye.uz;
        double l = r0 * 10 * M_PI/180;

        xd *= l * z1;
        yd *= l * z1;
        zd *= l * z1;

        env.eye.x += xd;
        env.eye.y += yd;
        env.eye.z += zd;

        r = sqrt(env.eye.x*env.eye.x + env.eye.y*env.eye.y + env.eye.z*env.eye.z);
        r = r0/r;
        env.eye.x *= r;
        env.eye.y *= r;
        env.eye.z *= r;

        //fprintf(stderr, "%f %f %f %f %f\n", r, l, xd, yd, zd);

        CROSS(xd,yd,zd, env.eye.x,  env.eye.y,  env.eye.z,
                        env.eye.ux, env.eye.uy, env.eye.uz, -1);

        CROSS(env.eye.ux, env.eye.uy, env.eye.uz, 
              env.eye.x,  env.eye.y,  env.eye.z,
              xd, yd, zd, 1);

    }
}

#if 0
void on3DRotate(int rx0, int ry0, int rz0)
{
    float x1 = rx0 / 180.0;
    float y1 = ry0 / 180.0;
    float z1 = rz0 / 180.0;

    xd = x1*cos(-env.eye.angle) - z1*sin(-env.eye.angle);
    yd = y1;
    zd = x1*sin(-env.eye.angle) + z1*cos(-env.eye.angle);

    env.eye.pitch   -= xd / 180.0;
    env.eye.heading -= yd / 180.0;
    env.eye.roll    += zd / 180.0;
    //fprintf(stderr, "rotate %i %i %i\n", rx0, ry0, rz0);
}
#endif

#define runIC(f) do { \
    client_stop_simulation(); \
    f; \
    env.sceneChanged = 1; \
    env.eye.x = env.eye.ox = 0;  \
    env.eye.y = env.eye.oy = 3; \
    env.eye.z = env.eye.oz = 3; \
    env.eye.ux = 0; \
    env.eye.uy = 1; \
    env.eye.uz = 0; \
} while(0)

void onClick(int button, int state, int x, int y) 
{
#if WITH_BUTTONS
    for (int i=0; i < env.nButtons; i++)
    {
        if (inButton(i, x, y))
        {
            if (state == GLUT_DOWN)
                env.btns[i].state = 1;
            else
            {
                if (env.btns[i].state)
                {
                    switch (env.btns[i].id)
                    {
                        case BTN_INFO_CLOSE:
                            break;
                        case BTN_ENGLISH_INFO:
                            if (env.showInfo == INFO_ENGLISH)
                                env.showInfo = INFO_NONE;
                            else
                                env.showInfo = INFO_ENGLISH;
                            break;
                        case BTN_DEUTSCH_INFO:
                            if (env.showInfo == INFO_DEUTSCH)
                                env.showInfo = INFO_NONE;
                            else
                                env.showInfo = INFO_DEUTSCH;
                            break;
                        case BTN_PLAY:
                            client_start_simulation();
                            env.sceneChanged = 1;
                            break;
                        case BTN_STOP:
                            client_stop_simulation();
                            env.sceneChanged = 1;
                            break;
                        case BTN_RESET:
                            runIC(clearParticles());
                            break;

                        case BTN_DEMO1:
                            runIC(ic_sphere());
                            break;
                        case BTN_DEMO2:
                            runIC(ic_cold_sphere());
                            break;
                        case BTN_DEMO3:
                            runIC(ic_cold_sphere());
                            break;
                        case BTN_DEMO4:
                            runIC(ic_cold_sphere());
                            break;
                        case BTN_DEMO5:
                            runIC(ic_figure_eight());
                            break;
                        case BTN_DEMO6:
                            runIC(ic_cold_sphere());
                            break;
                    }
                }

                env.btns[i].state = 0;
            }
        }
        else
        {
            env.btns[i].state = 0;
        }
    }
#endif
}

void onKeyboard(unsigned char key, int x, int y)
{
#if 0
    fprintf(stderr, "key %i\n", key);
    switch (key)
    {
#ifdef KEY_CLEAR
        KEY_CLEAR
            clearParticles();
            break;
#endif

#ifdef KEY_SPIN
        KEY_SPIN
            toggleSpinning();
            break;
#endif

#ifdef KEY_START
        KEY_START
            client_start_simulation();
            break;
#endif

#ifdef KEY_STOP
        KEY_STOP
            client_stop_simulation();
            break;
#endif

#ifdef KEY_PAUSE
        KEY_PAUSE
            client_toggle_pause();
            break;
#endif

        default: 
            break;
    }
#endif

    fprintf(stderr, "key %i\n", key);
    switch (key)
    {
        case BTN_INFO_CLOSE:
            env.showInfo = INFO_NONE;
            break;
        case BTN_ENGLISH_INFO:
            if (env.showInfo == INFO_ENGLISH)
                env.showInfo = INFO_NONE;
            else
                env.showInfo = INFO_ENGLISH;
            break;
        case BTN_DEUTSCH_INFO:
            if (env.showInfo == INFO_DEUTSCH)
                env.showInfo = INFO_NONE;
            else
                env.showInfo = INFO_DEUTSCH;
            break;
        case BTN_PLAY:
            client_start_simulation();
            env.sceneChanged = 1;
            break;
        case BTN_STOP:
            client_stop_simulation();
            env.sceneChanged = 1;
            break;
        case BTN_RESET:
            client_stop_simulation();
            clearParticles();
            env.sceneChanged = 1;
            env.eye.x = env.eye.ox = 0; 
            env.eye.y = env.eye.oy = 3;
            env.eye.z = env.eye.oz = 3;
            env.eye.ux = 0;
            env.eye.uy = 1;
            env.eye.uz = 0;
            break;

        case BTN_DEMO2:
            client_stop_simulation();
            ic_sphere();
            env.sceneChanged = 1;
            env.eye.x = env.eye.ox = 0; 
            env.eye.y = env.eye.oy = 3;
            env.eye.z = env.eye.oz = 3;
            env.eye.ux = 0;
            env.eye.uy = 1;
            env.eye.uz = 0;
            client_start_simulation();
            break;
        case BTN_DEMO1:
            client_stop_simulation();
            ic_cold_sphere();
            env.sceneChanged = 1;
            env.eye.x = env.eye.ox = 0; 
            env.eye.y = env.eye.oy = 3;
            env.eye.z = env.eye.oz = 3;
            env.eye.ux = 0;
            env.eye.uy = 1;
            env.eye.uz = 0;
            client_start_simulation();
            break;
        case BTN_DEMO5:
            client_stop_simulation();
            ic_figure_eight();
            env.sceneChanged = 1;
            env.eye.x = env.eye.ox = 0; 
            env.eye.y = env.eye.oy = 3;
            env.eye.z = env.eye.oz = 3;
            env.eye.ux = 0;
            env.eye.uy = 1;
            env.eye.uz = 0;
            client_start_simulation();
            break;
        case BTN_DEMO3:
            client_stop_simulation();
            ic_plummer_sphere();
            env.sceneChanged = 1;
            env.eye.x = env.eye.ox = 0; 
            env.eye.y = env.eye.oy = 3;
            env.eye.z = env.eye.oz = 3;
            env.eye.ux = 0;
            env.eye.uy = 1;
            env.eye.uz = 0;
            client_start_simulation();
            break;
        case BTN_DEMO6:
            client_stop_simulation();
            ic_colliding_plummer_spheres();
            env.sceneChanged = 1;
            env.eye.x = env.eye.ox = 0; 
            env.eye.y = env.eye.oy = 3;
            env.eye.z = env.eye.oz = 3;
            env.eye.ux = 0;
            env.eye.uy = 1;
            env.eye.uz = 0;
            client_start_simulation();
            break;
        case BTN_DEMO4:
            client_stop_simulation();
            ic_logo();
            env.sceneChanged = 1;
            env.eye.x = env.eye.ox = 0; 
            env.eye.y = env.eye.oy = 3;
            env.eye.z = env.eye.oz = 3;
            env.eye.ux = 0;
            env.eye.uy = 1;
            env.eye.uz = 0;
            client_start_simulation();
            break;
    }

    env.sceneChanged = 1;
}

void onReshape(int width, int height) 
{
    env.screenWidth = width;
    env.screenHeight = height;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);                // Select The Projection Matrix
    glLoadIdentity();                   // Reset The Projection Matrix

    // Calculate The Aspect Ratio Of The Window
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.01f, 101.0f); 

    glMatrixMode(GL_MODELVIEW);             // Select The Modelview Matrix
    glLoadIdentity();                   // Reset The Modelview Matrix

}

void onUpdateInfo() 
{
    glutStrokeCharacter(GLUT_STROKE_ROMAN, 'H');
}

void MakeScene(int LorR, double eyesep)
{
    unsigned int i;

    float cx = env.pointer.x;
    float cy = env.pointer.y;
    float cz = env.pointer.z;

    double aperture = 50;
    double focallength = 70;
    double near = .1;
    double far = 10000.;
    double ratio = env.screenWidth / env.screenHeight;
    double radians = M_PI/180. * aperture / 2;
    double wd2 = near * tan(radians);
    double ndfl = near / focallength;
    double left, right, top, bottom;

    int buffer;

    switch (LorR)
    {
        case -1: buffer = GL_BACK_LEFT;  break;
        case +1: buffer = GL_BACK_RIGHT; break;
        default: buffer = GL_BACK; break;
    }

    Coord r;

    CROSS(r.x,r.y,r.z, 
          -env.eye.x, -env.eye.y, -env.eye.z,
          env.eye.ux, env.eye.uy, env.eye.uz, 1);

    NORM(r.x,r.y,r.z, r.x,r.y,r.z);

    r.x *= eyesep / 2 * LorR;
    r.y *= eyesep / 2 * LorR;
    r.z *= eyesep / 2 * LorR;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(45.0f,(GLfloat)env.screenWidth/(GLfloat)env.screenHeight,0.01f, 101.0f); 

    left  = - ratio * wd2 + 0.5 * eyesep * ndfl * LorR;
    right =   ratio * wd2 + 0.5 * eyesep * ndfl * LorR;
    top    =   wd2;
    bottom = - wd2;
    glFrustum(left,right,bottom,top,near,far);

    glMatrixMode(GL_MODELVIEW);
//  glDrawBuffer(LorR < 0 ? GL_BACK_LEFT : GL_BACK_RIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(env.eye.x + r.x, 
              env.eye.y + r.y,
              env.eye.z + r.z,  
              r.x, r.y, r.z,
              env.eye.ux, env.eye.uy, env.eye.uz);  

    if (env.simStatus == SIM_STOPPED)
    {
        glPushMatrix();

        switch (env.background)
        {
            case WHITE:
                glColor4f(0,0,0,1);
                break;
            case BLACK:
                glColor4f(1,1,1,1);
                break;
        }


        glTranslatef(env.pointer.x, env.pointer.y, env.pointer.z);
        glutWireSphere(.2, 15, 15);

        glPopMatrix();
    }

    GLfloat light_pos[] = {env.eye.x, env.eye.y, env.eye.z, 1};
    glLightfv(GL_LIGHT0, GL_POSITION,       light_pos);

#if WITH_POINTS
    //if (first_time)
    glDisable(GL_LIGHTING);
    {
        //first_time = false;
          
        glVertexPointer(3, GL_FLOAT, 0, env.pList.pos);
        glColorPointer(4, GL_FLOAT, 0, env.pList.clr);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);	  
    }

    glDrawArrays(GL_POINTS, 0, env.pList.nParticles);
#else

#if 1
    if (!env.sceneChanged)
    {
        glCallList(scene);
    } 
    else
    {
        glNewList(scene, GL_COMPILE_AND_EXECUTE);

        unsigned int pi=0, ci=0;
        const float R = 0.02;
        for (i=0; i < env.pList.nParticles; i++, pi+=3, ci+=4)
        {
            glPushMatrix();
            glTranslatef(env.pList.pos[pi+0],env.pList.pos[pi+1],env.pList.pos[pi+2]);
            //glColor4f(env.pList.clr[ci+0],env.pList.clr[ci+1],env.pList.clr[ci+2],env.pList.clr[ci+3]);
            //glColor3f(env.pList.clr[ci+0],env.pList.clr[ci+1],env.pList.clr[ci+2]);

            float r = fabsf(env.pList.vel[pi+0]); 
            float g = fabsf(env.pList.vel[pi+1]); 
            float b = fabsf(env.pList.vel[pi+2]); 


            //if (r == 0 && g == 0 && b == 0)
            if (i >= env.pList.movingParticleIndex)
            {
                r = env.pList.clr[ci+0];
                g = env.pList.clr[ci+1];
                b = env.pList.clr[ci+2];
            }
            else
            {
                color_ramp_astro(&r, &g, &b);
                //color_ramp_wrbb(&r, &g, &b);
                //color_ramp_grey(&r, &g, &b);
                //color_ramp_hot2cold(&r, &g, &b);
            }
            glColor4f(r, g, b, 0.7);

            if (env.simStatus == SIM_RUNNING)
            {
            }
            else
            {
                //glColor3fv(starColor);
            }

            glCallList(sphere);
            glPopMatrix();
        }

        glEndList();
        env.sceneChanged = 0;
    }
#endif

#endif

    glMatrixMode(GL_PROJECTION);                // Select The Projection Matrix
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);                // Select The Projection Matrix
    glPushMatrix();


    //glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);                // Select The Projection Matrix
    glLoadIdentity();                   // Reset The Projection Matrix
    gluOrtho2D(0,env.screenWidth, 0, env.screenHeight);
    
    glMatrixMode(GL_MODELVIEW);                // Select The Projection Matrix
    glLoadIdentity();                   // Reset The Projection Matrix
    
#if WITH_BUTTONS
    for (int i=0; i < env.nButtons; i++)
    {
        if (env.btns[i].img_up != NULL)
        {
            glRasterPos2i(env.screenWidth + env.btns[i].x, env.btns[i].y);
            if (env.btns[i].state == 1)
            {
                glDrawPixels(env.btns[i].w,
                             env.btns[i].h,
                             GL_RGBA,
                             GL_UNSIGNED_BYTE, 
                             env.btns[i].img_down);
            }
            else
            {
                glDrawPixels(env.btns[i].w,
                             env.btns[i].h,
                             GL_RGBA,
                             GL_UNSIGNED_BYTE, 
                             env.btns[i].img_up);
            }
        }
    }
#endif

    if (env.showInfo == INFO_ENGLISH)
    {
        glRasterPos2i((env.screenWidth-1280), (env.screenHeight-1387));
        glDrawPixels(1280,
                     1387,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE, 
                     env.info_english);
    }
    if (env.showInfo == INFO_DEUTSCH)
    {
        glRasterPos2i((env.screenWidth-1280), (env.screenHeight-1387));
        glDrawPixels(1280,
                     1387,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE, 
                     env.info_deutsch);
    }

    char text[256];
    char *tp;
    sprintf(text, "%5i / %i", env.pList.movingParticleIndex, env.maxParticles);

    glDisable(GL_LIGHTING);
    glColor3f(1,1,1);
    glRasterPos2i(5,30);
    for (tp = text; *tp != '\0'; tp++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *tp);

    if (env.simStatus == SIM_RUNNING)
    {
        tp = "|/-\\";
        glColor3f(1,1,1);
        glRasterPos2i(5,15);
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, tp[env.currentTimestep % 4]);
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
        for (int i=0; i < env.maxTimesteps; i+=150)
        {
            if (i < env.currentTimestep)
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '|');
            else
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '-');
        }
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '|');
    }
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_MODELVIEW);                // Select The Projection Matrix
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);                // Select The Projection Matrix
    glPopMatrix();
}

void onUpdate() 
{
    if (env.stereo)
    {
        MakeScene(-1, 4);
        MakeScene(+1, 4);
    }
    else
    {
        MakeScene(0, 0);
    }


//  if (env.make_movie)
//  {
//      glFinish();
//      glReadPixels(0,0,env.screenWidth, env.screenHeight, GL_RGB, GL_UNSIGNED_BYTE, env.frame_buffer);
//      save_frame_buffer();
//  }

    //glFlush();
    glutSwapBuffers();
}

void onTimer(int value)
{
    glutTimerFunc(20, onTimer,0);

    //fprintf(stderr, "-- %i\n", env.simStatus);
    if (env.simStatus == SIM_RUNNING)
    {
        client_load_next_frame();
        env.sceneChanged = 1;
    }
    else if (env.simStatus != SIM_PAUSED)
    {
        ParticleList *pl = &(env.pList);
        unsigned int i, pi, ci;
        for (i=pl->movingParticleIndex; i < pl->nParticles; i++)
        {
            env.sceneChanged = 1;
            pi = i * 3;
            ci = i * 4;

            pl->pos[pi+0] += -(pl->pos[pi+0] - pl->dest[pi+0]) / 8;
            pl->pos[pi+1] += -(pl->pos[pi+1] - pl->dest[pi+1]) / 8;
            pl->pos[pi+2] += -(pl->pos[pi+2] - pl->dest[pi+2]) / 8;
#if 1
            pl->clr[ci+0] = starColor[0];
            pl->clr[ci+1] = (starColor[1] / 5) * (pl->step[i] + 1);
            pl->clr[ci+2] = starColor[2];
            pl->clr[ci+3] = starColor[3];
#endif
#if 0
            pl->vel[pi+0] = starColor[0];
            pl->vel[pi+1] = (starColor[1] / 5) * (pl->step[i] + 1);
            pl->vel[pi+2] = starColor[2];
#endif
#if 0
            pl->vel[pi+0] = sqrt(pow((pl->step[i] + 1),1) / 10);
            pl->vel[pi+1] = pl->vel[pi+0];
            pl->vel[pi+2] = pl->vel[pi+0];
#endif

            pl->step[i]++;
            if (pl->step[i] == 20)
            {
                pl->pos[pi+0] = pl->dest[pi+0];
                pl->pos[pi+1] = pl->dest[pi+1];
                pl->pos[pi+2] = pl->dest[pi+2];

                pl->vel[pi+0] = 0;
                pl->vel[pi+1] = 0;
                pl->vel[pi+2] = 0;
#if 0
                pl->clr[ci+0] = starColor[0];
                pl->clr[ci+1] = starColor[1];
                pl->clr[ci+2] = starColor[2];
                pl->clr[ci+3] = starColor[3];
#endif
                pl->movingParticleIndex++;
            }
        }
        strafe(0.01);
    }

        //const double eye.angle = .5 * 3.14159265358979/180.0;
    //env.eye.x = env.eye.ox*cos(env.eye.angle) - env.eye.oz*sin(env.eye.angle);
    //env.eye.z = env.eye.ox*sin(env.eye.angle) + env.eye.oz*cos(env.eye.angle);
    //env.eye.y = env.eye.oy;

    glutPostRedisplay();
    //glutPostOverlayRedisplay();
}

