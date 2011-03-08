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

const float ENV_RADIUS = 2;
const float SPRAY_RADIUS = 0.1;

float starColor[] = {0.8f, 0.72f, 0.0f, 1.0f};
//float starColor[] = {0.6f, 0.52f, 0.0f, 1.0f};

const int cred = 256;
int *cmass = NULL;


GLuint sphere;

void viz_init()
{
    if (env.fullscreen) glutFullScreen();
    //CGAssociateMouseAndMouseCursorPosition(false);
    //glutSetCursor(GLUT_CURSOR_NONE);
    glClearDepth(1.0f);

    //glutEstablishOverlay();
    //glutOverlayDisplayFunc(onUpdateInfo);
    //glutShowOverlay();

    glutDisplayFunc(onUpdate);
    glutReshapeFunc(onReshape);
    glutTimerFunc(100,onTimer,0);
    //glutTimerFunc(30,onTimer,0);
    //glutIdleFunc(onIdle);

    glutMotionFunc(on2DDrag);
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

    sphere = glGenLists(1);
    glNewList(sphere, GL_COMPILE);
    glutSolidSphere(0.02, 10, 10);
    glEndList();

    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //glPixelStorei(GL_PACK_ALIGNMENT, 1);
}

int inButton(int i, int x, int y)
{
    int in = 0;

    if (0 <= x && x < env.nButtons)
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

void doSpray()
{
    //fprintf(stderr, "on3DDrag %f %f %f\n", env.pointer.x, env.pointer.y, env.pointer.z);

    if (env.simStatus != SIM_STOPPED) return;

    float x, y, z;
    x = env.eye.x/2 - env.pointer.x;
    y = env.eye.y/2-4 - env.pointer.y;
    z = env.eye.z/2 - env.pointer.z;

    // a little random offset from the spray can
    float r = 0.2*drand48();
    x *= r;
    y *= r;
    z *= r;

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

        //env.pList.pos[pi+0] = env.eye.x/2 + xx + x;
        //env.pList.pos[pi+1] = env.eye.y/2-4 + yy + y;
        //env.pList.pos[pi+2] = env.eye.z/2 + zz + z;

        env.pList.pos[pi+0] = env.eye.x/2 + x;
        env.pList.pos[pi+1] = env.eye.y/2-4 + y;
        env.pList.pos[pi+2] = env.eye.z/2 + z;

        env.pList.vel[pi+0] = 
        env.pList.vel[pi+1] = 
        env.pList.vel[pi+2] = 0;

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

    for (int i=0; i < env.nButtons; i++)
        env.btns[i].state &= inButton(i, x0, y0);
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
    double l = r0 * 10 * M_PI/180;

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

        y1 = -y0 / 1000.0;
        x1 = -x0 / 1000.0;
        z1 = -z0 / 1000.0;

    fprintf(stderr, "%i %i %i\n", x0,y0,z0);

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
        x += x1*xd;
        y += x1*yd;
        z += x1*zd;

        NORM(xd,yd,zd, env.eye.ux, env.eye.uy, env.eye.uz);
        x -= xd*z1;
        y += yd*z1;
        z -= zd*z1;

        NORM(xd,yd,zd, env.eye.x, env.eye.y, env.eye.z);
        x -= xd*y1;
        y -= yd*y1;
        z -= zd*y1;

        r = x*x + y*y + z*z;

        if (r > ENV_RADIUS)
        {
            x /= r/ENV_RADIUS;
            y /= r/ENV_RADIUS;
            z /= r/ENV_RADIUS;
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
                else if (r > 70.01)
                {
                    env.eye.x /= r/70.01;
                    env.eye.y /= r/70.01;
                    env.eye.z /= r/70.01;
                }
                break;
            }
            else
            {
                y1 /= 2;
            }
        }

        strafe(x1 * 10 * M_PI/180);

        //fprintf(stderr, "%f %f %f %f %f\n", r, l, xd, yd, zd);


        CROSS(xd,yd,zd, env.eye.x,  env.eye.y,  env.eye.z,
                        env.eye.ux, env.eye.uy, env.eye.uz, -1);

        CROSS(env.eye.ux, env.eye.uy, env.eye.uz, 
              env.eye.x,  env.eye.y,  env.eye.z,
              xd, yd, zd, 1);

        //fprintf(stderr, "%f %f %f\n", env.eye.ux, env.eye.uy, env.eye.uz);

        //--------------------------------------------------------------------------------

        double r0 = sqrt(env.eye.x*env.eye.x + env.eye.y*env.eye.y + env.eye.z*env.eye.z);

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

void onClick(int button, int state, int x, int y) 
{
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
                        case BTN_INFO:
                            break;
                        case BTN_ENGLISH:
                            break;
                        case BTN_DEUTSCH:
                            break;
                        case BTN_DEMO1:
                            client_stop_simulation();
                            ic_sphere();
                            break;
                        case BTN_DEMO2:
                            client_stop_simulation();
                            ic_cold_sphere();
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
}

void onKeyboard(unsigned char key, int x, int y)
{
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

void onUpdate() 
{
    unsigned int i;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();                   // Reset The Modelview Matrix

//    glutSwapBuffers();
//    return;

    if (0) //env.simStatus & SIM_RUNNING)
    {
        glRotatef(env.eye.roll, 0, 0, 1);
        glRotatef(env.eye.pitch, 0, 1, 0);
        glRotatef(env.eye.heading, 1, 0, 0);
        glTranslated(env.eye.x, env.eye.y, -env.eye.z);
        //glTranslatef(0, 0, -5);
    }
    else
    {
        gluLookAt(env.eye.x, env.eye.y, env.eye.z,  
                  0.0, 0.0, 0.0,  
                  env.eye.ux, env.eye.uy, env.eye.uz);  
        //gluLookAt(env.eye.x, env.eye.y, env.eye.z,  0.0, 0.0, 0.0,  0.0, 1.0, 0.);
    }

    //glColor4f(1,1,1,.5);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //glRectf(-.5, -.5, .5, .5);

    //env.eye.roll = env.eye.pitch = env.eye.heading = 0;

    //gluLookAt(env.eye.x, env.eye.y, env.eye.z,  env.pointer.x, env.pointer.y, env.pointer.z,  0.0, 1.0, 0.); 

    float cx = env.pointer.x;
    float cy = env.pointer.y;
    float cz = env.pointer.z;

#if 0

    glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor4f(1,0,0,1);
    //glLineWidth(2.0);
    glRasterPos2f(0,0);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'H');
    //glLineWidth(1);
    glPopMatrix();

#endif

    //cx = -1*cos(-env.eye.angle) - cz*sin(-env.eye.angle);
    //cz = -1*sin(-env.eye.angle) + cz*cos(-env.eye.angle);
    //cx = -1*cos(-env.eye.angle)
    //cz = -1*sin(-env.eye.angle) + cz*cos(-env.eye.angle);

    if (env.simStatus == SIM_STOPPED)
    {
        glDisable(GL_LIGHTING);
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
        //glRotated(-env.eye.angle * 180/M_PI, 0, 1, 0);
        glutWireSphere(.2, 15, 15);

        glPopMatrix();
    }

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);                 // Enable light source

#if 1
    GLfloat light_pos[] = {env.eye.x, env.eye.y, env.eye.z, 1};

    GLfloat light_dir[] = {env.pointer.x-env.eye.x, env.pointer.y-env.eye.y, env.pointer.z-env.eye.z};
    //GLfloat light_dir[] = {env.eye.x-env.pointer.x, env.eye.y-env.pointer.y, env.eye.z-env.pointer.z};

    glLightfv(GL_LIGHT0, GL_POSITION,       light_pos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_dir);

#endif

#if  0
    glBegin(GL_LINES);
    glColor4f(1,0,0,1);
    glVertex3f(-cos(-env.eye.angle)-env.pointer.x,cy,cz);
    glVertex3f(cos(-env.eye.angle)+env.pointer.x,cy,cz);
    glColor4f(0,1,0,1);
    glVertex3f(cx,cy-1,cz);
    glVertex3f(cx,cy+1,cz);
    glColor4f(0,0,1,1);
    glVertex3f(cx,cy,cz-1);
    glVertex3f(cx,cy,cz+1);
    glEnd();
#endif

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

#if 0
    glPushMatrix();
    glTranslatef(0,0,-3);
    glutSolidSphere( 1.0,  20,  20);
    glPopMatrix();
#endif

#if 1
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
    
    for (int i=0; i < env.nButtons; i++)
    {
        if (env.btns[i].img_up != NULL)
        {
            glRasterPos2i(env.screenWidth + env.btns[i].x, env.btns[i].y);
            glPixelStoref(GL_UNPACK_ALIGNMENT, 4);
            if (env.btns[i].state == 1)
            {
                glDrawPixels(env.btns[i].w,
                             env.btns[i].h,
                             GL_RGB,
                             GL_UNSIGNED_BYTE, 
                             env.btns[i].img_down);
            }
            else
            {
                glDrawPixels(env.btns[i].w,
                             env.btns[i].h,
                             GL_RGB,
                             GL_UNSIGNED_BYTE, 
                             env.btns[i].img_up);
            }
        }
    }
    
    //glEnable(GL_DEPTH_TEST);
    //glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);                // Select The Projection Matrix
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);                // Select The Projection Matrix
    glPopMatrix();



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
    }
    else if (env.simStatus != SIM_PAUSED)
    {
        ParticleList *pl = &(env.pList);
        unsigned int i, pi, ci;
        for (i=pl->movingParticleIndex; i < pl->nParticles; i++)
        {
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

