//#include <ApplicationServices/ApplicationServices.h>
//#include <GLUT/glut.h>

#include <stdlib.h>
#include <pthread.h>
#include "starspray.h"
#include "simulator.h"
#include "client.h"
#include "io.h"

static void *client_run(void *arg);

extern Environment env;

enum ACTIONS { DO_NOTHING, LOAD_NEXT_FRAME, START_SIMULATOR, STOP_SIMULATOR };

static pthread_cond_t client_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static enum ACTIONS action = DO_NOTHING;

int client_start(int argc, char **argv)
{
    env.pList.nParticles = 0;
    env.pList.movingParticleIndex=0;
    env.pList.pos  = MALLOC(float, env.maxParticles * 3);        assert(env.pList.pos  != NULL);
    env.pList.vel  = MALLOC(float, env.maxParticles * 3);        assert(env.pList.vel  != NULL);
    env.pList.dest = MALLOC(float, env.maxParticles * 3);        assert(env.pList.dest != NULL);
    env.pList.step = MALLOC(unsigned int, env.maxParticles * 1); assert(env.pList.step != NULL);
    env.pList.clr  = MALLOC(float, env.maxParticles * 4);        assert(env.pList.clr  != NULL);

    env.spinning = 0;
    env.simStatus = SIM_STOPPED;
    env.maxTimesteps = 1500;

    pthread_create(&(env.client_thread), NULL, client_run, NULL);

    return 0;
}

static void *client_run(void *arg)
{
    while (true)
    {
        pthread_mutex_lock(&mutex);
        int current_action;
        while ((current_action = action) == DO_NOTHING)
            pthread_cond_wait(&client_cond, &mutex);

        switch (current_action)
        {
            case DO_NOTHING: 
                assert(0); // Shouldn't be woken up to do nothing!
                break;
            case START_SIMULATOR:
                startSimulator();
                env.simStatus = SIM_RUNNING;
                fprintf(stderr, "Simulation started.\n");
                action = DO_NOTHING;
                break;
            case STOP_SIMULATOR:
                stopSimulator();
                env.simStatus = SIM_STOPPED;
                fprintf(stderr, "Simulation stopped.\n");
                action = DO_NOTHING;
                break;
            case LOAD_NEXT_FRAME:
                action = DO_NOTHING;
                if (env.currentTimestep <= env.maxTimesteps)
                {
                    pthread_mutex_unlock(&mutex);
                    int r = loadNextFrameFromSimulator();
                    pthread_mutex_lock(&mutex);
                    switch (r)
                    {
                        case 0:
                            env.currentTimestep++;
                            break;
                        case -1:
                            break;
                        default:
                            //client_stop_simulation();
                            action = STOP_SIMULATOR;
                            fprintf(stderr, "Simulation finished after %i steps.\n", env.currentTimestep);
                            break;
                    }

                    //fprintf(stderr, "Simulation on step %i\n", env.currentTimestep);
                }
                else
                {
                    //client_stop_simulation();
                    action = STOP_SIMULATOR;
                    fprintf(stderr, "Simulation finished after %i steps.\n", env.currentTimestep);
                }
                break;
        }
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

void client_start_simulation()
{
    pthread_mutex_lock(&mutex);
    if (env.simStatus != SIM_RUNNING 
    &&  env.simStatus != SIM_PAUSED
    &&  env.simStatus != SIM_STARTING) 
    {
        if (env.pList.nParticles != 0) 
        {
            if (env.pList.movingParticleIndex == env.pList.nParticles) 
            {
                env.simStatus = SIM_STARTING;
                env.currentTimestep = 1;
                env.loadingTimestep = 1;

                action = START_SIMULATOR;
                pthread_cond_signal(&client_cond);
                fprintf(stderr, "Simulation starting.\n");
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

void client_stop_simulation()
{
    pthread_mutex_lock(&mutex);
    if (env.simStatus != SIM_STOPPED && env.simStatus != SIM_STOPPING) 
    {
        env.simStatus = SIM_STOPPING;
        action = STOP_SIMULATOR;
        pthread_cond_signal(&client_cond);
        fprintf(stderr, "Simulation stopping.\n");
    }
    pthread_mutex_unlock(&mutex);
}

void client_pause_simulation()
{
    pthread_mutex_lock(&mutex);
    if (env.simStatus == SIM_RUNNING)
    {
        env.simStatus = SIM_PAUSED;
        pauseSimulator();
        fprintf(stderr, "Simulation paused.\n");
    }
    pthread_mutex_unlock(&mutex);
}

void client_resume_simulation()
{
    pthread_mutex_lock(&mutex);
    if (env.simStatus == SIM_PAUSED)
    {
        env.simStatus = SIM_RUNNING;
        resumeSimulator();
        fprintf(stderr, "Simulation resumed.\n");
    }
    pthread_mutex_unlock(&mutex);
}

void client_toggle_pause()
{
    if (env.simStatus == SIM_PAUSED)
        client_resume_simulation();
    else
        client_pause_simulation();
}

void client_load_next_frame()
{
    pthread_mutex_lock(&mutex);
    action = LOAD_NEXT_FRAME;
    pthread_cond_signal(&client_cond);
    pthread_mutex_unlock(&mutex);
}

void client_clear_particles()
{
    pthread_mutex_lock(&mutex);
    if (env.simStatus == SIM_STOPPED)
    {
        env.pList.nParticles = 0;
        env.pList.movingParticleIndex = 0;
    }
    pthread_mutex_unlock(&mutex);
}
