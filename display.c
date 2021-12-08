#include "stdio.h"
#include "stdlib.h"
#include "display.h"
#include "memory.h"
#include <pthread.h>

int counter = 0;
void draw_square(char *grid, int grid_width, int grid_height, int x, int y, int side_len);

void save_gray_frame(unsigned char *buf, int width, int height)
{
    FILE *f;
    int i;

    char filename[50];

    sprintf(filename, "output/%i.pgm", counter);

    counter++;

    f = fopen(filename, "w");

    fprintf(f, "P5\n%d %d\n%d\n", width, height, 1);

    //printf("Saving file %s\n", filename);
    fwrite(buf, 1, width * height, f);

    fclose(f);
}

char *upscale(char *grid, int in_width, int in_height, int times)
{
    char *new_grid = calloc(sizeof(char), in_width * times * in_height * times);

    int out_width = in_width * times;
    int out_height = in_height * times;

    for (int y = 0; y < in_height; y++)
    {
        for (int x = 0; x < in_width; x++)
        {
            if (grid[y * in_width + x] == 0)
                continue;
            draw_square(new_grid, out_width, out_height, x, y, times);
        }
    }

    return new_grid;
}

typedef struct _upscale_params
{
    char *old_grid;
    char *new_grid;
    int in_width;
    int in_height;
    int times;
} _upscale_params;

void *_upscale(void *params);

#define THREADS 1
pthread_t threads[THREADS];
_upscale_params *params[THREADS];

char *upscale_th(char *grid, int in_width, int in_height, int times)
{
    char *new_grid = calloc(sizeof(char), in_width * times * in_height * times);

    for (int th = 0; th < THREADS; th++)
    {

        params[th] = malloc(sizeof(_upscale_params));

        //calculate new height
        params[th]->in_height = in_height / THREADS;

        params[th]->in_width = in_width;
        params[th]->times = times;

        //calculate start of the old grid
        params[th]->old_grid = grid + in_height / THREADS * th * in_width;

        //calculate start of the new grid
        params[th]->new_grid = new_grid + in_height / THREADS * th * times * in_width * times;

        pthread_create(&threads[th], NULL, _upscale, params[th]);
    }
    for (int th = 0; th < THREADS; th++)
    {
        //wait all threads end
        pthread_join(threads[th], NULL);
        free(params[th]);
    }
    return new_grid;
}

void *_upscale(void *params)
{
    _upscale_params *in = (_upscale_params *)params;

    int out_width = in->in_width * in->times;
    int out_height = in->in_height * in->times;

    for (int y = 0; y < in->in_height; y++)
    {
        for (int x = 0; x < in->in_width; x++)
        {
            if (in->old_grid[y * in->in_width + x] == 0)
                continue;

            draw_square(in->new_grid, out_width, out_height, x, y, in->times);
        }
    }
}

void draw_square(char *grid, int grid_width, int grid_height, int x, int y, int side_len)
{
    void *start = grid + y * side_len * grid_width + x * side_len;

    for (int i = 0; i < side_len; i++)
    {
        memset(start + i * grid_width,
               0b1,
               side_len);
    }
}
