#include <assert.h>
#include <math.h>
#include <sndfile.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>


int convert(FILE *in, FILE *out, FILE *txtout)
{
    int i,j;
    float last_ap=0;
    float avg_power;
    struct 
    {
        int8_t r,i;
    } *block, *tblock;
    
    int Nseconds      = 120;

    long samples_per_second = 8738133.0;
#if 0
    long blocks_per_second = 64;
    int block_size  = samples_per_second / blocks_per_second;
#else
    int block_size  = 1<<17;
    long blocks_per_second = samples_per_second / block_size;
#endif
    long Nblocks     = Nseconds * blocks_per_second;
    long blocks_read = 0;
    int upsamples = 44100 / blocks_per_second;

    if (upsamples < 1) upsamples = 1;

    block  = malloc(block_size * sizeof(*block));
    tblock = malloc(block_size * sizeof(*tblock));

    int chunk_time = 40; // [s]
    long chunk_size = blocks_per_second * chunk_time;
    float *chunk = malloc(chunk_size * sizeof(*chunk));
    float *tchunk = malloc(chunk_size * sizeof(*tchunk));

    const int   Nchannels = 1;
    float median = 205.2181015; //205.305099;
    float max    = 11.8288045; //20.234253;


    SF_INFO info = {0, 44100, Nchannels, SF_FORMAT_WAV | SF_FORMAT_PCM_16, 0, 0};
    SNDFILE *sout = sf_open_fd(fileno(out), SFM_WRITE, &info, 0);

    if (sout == NULL)
    {
        fprintf(stderr, "Error opening output file.\n");
        return 1;
    }

    printf("%ld Hz - original sample rate\n", samples_per_second);
    printf("%i seconds to be read\n", Nseconds);
    printf("%i Hz per averaged chunk\n", block_size);
    printf("%ld blocks per second\n", blocks_per_second);
    printf("%ld blocks to read\n", Nblocks);
    printf("%i upsampling\n", upsamples);

    void report(int sig)
    {
        static char buf[256];

        if (sig != SIGALRM) return;
        sprintf(buf, "\r%ld/%ld", blocks_read, Nblocks);
        write(STDERR_FILENO, buf, sizeof(buf));
        fsync(STDERR_FILENO);
        alarm(1);
    }

    signal(SIGALRM, report);
    alarm(3);

    last_ap = -2;
    while (!feof(in) && blocks_read < Nblocks)
    {
        int c;
        int chunk_length = 0;

        median = 0;
        max    = 0;

        for (c=0; c < chunk_size && blocks_read < Nblocks; c++, chunk_length++)
        {
            if (fread(block, sizeof(block[0]), block_size, in) < block_size) goto on_error;
            blocks_read++;

            avg_power = 0;
            for (i=0; i < block_size; i++)
                avg_power += pow(block[i].r,2) + pow(block[i].i,2);
            avg_power /= block_size;

            chunk[c] = avg_power;
            if (avg_power > max) max = avg_power;
            median += avg_power;
        }


        int ccompar(const void *a0, const void *b0)
        {
            float a = fabs(*(float *)a0);
            float b = fabs(*(float *)b0);
            if (a < b) return -1;
            if (a > b) return 1;
            return 0;
        }
        memcpy(tchunk, chunk, sizeof(*tchunk) * chunk_length);
        qsort(tchunk, chunk_length, sizeof(*tchunk), ccompar);

        if ((chunk_length % 1) == 0)
            median = tchunk[chunk_length/2];
        else
            median = (tchunk[chunk_length/2-1] + tchunk[chunk_length/2]) / 2.;

#if 0
        if (chunk_size > 1)
        {
            median /= chunk_length;
            max -= median;
        }
        else
        {
            median = 0;
            max = 1;
        }
#endif

        if (chunk_size > 1)
        {
            max -= median;

            for (c=0; c < chunk_length; c++)
            {
                chunk[c] -= median;
                chunk[c] /= max;
            }
        }


        if (last_ap == -2)
            last_ap = chunk[0];

        for (c=0; c < chunk_length; c++)
        {
            // loop to upsample
            for (j=1; j <= upsamples; j++)
            {
                float channel[2] = {0,0};

                //float t = ((chunk[c]-last_ap)/upsamples * j + last_ap);
                float t = chunk[c];

                if (txtout) fprintf(txtout, "%f\n", t);

                if (Nchannels == 2)
                {
                    float s = 4;
                    if (t < 0.5)
                    {
                        channel[1] = t; //fabs(s*t) > 1 ? 0 : s*t;
                    }
                    else
                    {
                        channel[1] = fabs(s*t) > 1 ? 0 : s*t;
                        channel[0] = fabs(2*t) > 1 ? copysign(1,t) : 2*t;
                    }
                }
                else
                {
                    channel[0] = 1.1*t; //copysign(pow(fabs(t),1), t);
                    if (channel[0] > 0.3) channel[0] *= 1 + (1-fabs(channel[0]));
                    if (channel[0] > 1) channel[0] = 1;
                    if (channel[0] < -1) channel[0] = -1;
                }

                if (sf_write_float(sout, channel, Nchannels) != Nchannels)
                {
                    fprintf(stderr, "Error writing channels!\n");
                    return 1;
                }
            }

            last_ap = chunk[c];
        }

on_error:
        ;
    }

    printf("\n%ld blocks read ~ %ld seconds\n", blocks_read, blocks_read / blocks_per_second);

    sf_close(sout);
    free(chunk);
    free(tchunk);
    free(block);

    return 0;
}

int main(int argc, char **argv)
{
    FILE *in = stdin, *out = stdout;
    FILE *txtout = NULL;

    if (argc >= 2)
    {
        in = fopen(argv[1], "rb");
        assert(in != NULL);
    }

    if (argc >= 3)
    {
        out = fopen(argv[2], "wb");
        assert(out != NULL);
    }

    if (argc == 4)
    {
        txtout = fopen(argv[3], "wb");
        assert(txtout != NULL);
    }

    convert(in, out, txtout);


    if (in  != stdin) fclose(in);
    if (out != stdin) fclose(out);
    if (txtout != stdin) fclose(txtout);

    return 0;
}

