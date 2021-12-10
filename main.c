#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "game_utils.h"
#include "video.h"

typedef struct grid
{
    int width;
    int height;
    u_int8_t *grid;
    int *white_cells;
    int white_cells_len;
} grid;

void fill_white_cells(grid *grid);
int *get_array_of_neighbours(grid *grid, int coord);
int count_sourranding_white_cells(grid *grid, int coord);
int update_grid(grid *grid);
grid *build_grid(int width, int height);
void free_grid(grid *g);

int main()
{
    //params
    const char *codec = "h264_nvenc";
    const char *output_file = "game_of_life.mp4";
    int framerate = 30;
    long int seed = 13213;
    int width = 1920;
    int height = 1080;
    int times = 1;
    int out_width = width * times;
    int out_height = height * times;
    //params

    int res;

    grid *grid = build_grid(width, height);
    char *upscaled_grid;

    fill_grid_with_LFSR(grid->grid, width, height, seed);

    fill_white_cells(grid);
    
    video_encoder *ve = create_video_encoder(out_width, out_height, AV_PIX_FMT_YUV444P,
                                             45000000, (AVRational){framerate, 1},
                                             output_file, codec);

    int frame_number = 0;

    while (frame_number++ < 12000)
    {

        res = av_frame_make_writable(ve->frame);
        if (res < 0)
        {
            return 1;
        }

        upscale(grid->grid, ve->frame->data[0], width, height, times);

        ve->frame->pts = frame_number;
        encode(ve);

        res = update_grid(grid);

        if (res != 0)
        {
            printf("Failed updating grid\n");
            return 1;
        }
    }

    free_grid(grid);
    end_video_encoder(ve);
}

int update_grid(grid *grid)
{

    char *new_grid = calloc(sizeof(char), grid->width * grid->height);
    int *white_cells = calloc(sizeof(int), grid->width * grid->height);

    int white_cells_len = 0;

    if (!new_grid)
    {
        printf("Failed allocatin new_grid\n");
        return 1;
    }
    //loop all cells

    for (int i = 0; i < grid->white_cells_len; i++)
    {
        int curr_white_cell = grid->white_cells[i];

        //check if center should be alive
        int *borders = get_array_of_neighbours(grid, curr_white_cell);
        if (!borders)
        {
            printf("Failed creating array of borders\n");
            return 1;
        }

        int white_cells_counter = 0;

        for (int i = 0; i < 8; i++)
        {
            if (grid->grid[borders[i]] == 1)
                white_cells_counter++;
        }

        if (white_cells_counter == 2 || white_cells_counter == 3)
        {
            new_grid[curr_white_cell] = 1;
            white_cells[white_cells_len] = curr_white_cell;
            white_cells_len++;
        }

        //check if borders should be alive

        for (int i = 0; i < 8; i++)
        {
            if (grid->grid[borders[i]] != 0)
                continue;

            //pixels already calculated are set to two
            grid->grid[borders[i]] = 2;

            white_cells_counter = count_sourranding_white_cells(grid, borders[i]);

            if (white_cells_counter == -1)
            {
                printf("Failed counting white cells\n");
                return 1;
            }

            if (white_cells_counter == 3)
            {
                new_grid[borders[i]] = 1;
                white_cells[white_cells_len] = borders[i];
                white_cells_len++;
            }
        }

        free(borders);
    }

    free(grid->grid);
    free(grid->white_cells);

    grid->grid = new_grid;
    grid->white_cells = white_cells;
    grid->white_cells_len = white_cells_len;

    return 0;
}

void fill_white_cells(grid *grid)
{
    grid->white_cells_len = 0;
    for (int curr_white_cell = 0, limit = grid->width * grid->height; curr_white_cell < limit; curr_white_cell++)
    {
        if (grid->grid[curr_white_cell] != 1)
            continue;

        grid->white_cells[grid->white_cells_len] = curr_white_cell;
        grid->white_cells_len++;
    }
}

int count_sourranding_white_cells(grid *grid, int coord)
{
    int white_cells_counter = 0;

    int *borders = get_array_of_neighbours(grid, coord);
    if (!borders)
    {
        printf("Failed creating array of borders\n");
        return -1;
    }

    for (int i = 0; i < 8; i++)
    {
        if (grid->grid[borders[i]] == 1)
            white_cells_counter++;
    }

    free(borders);

    return white_cells_counter;
}

//returns array of size 8
int *get_array_of_neighbours(grid *grid, int coord)
{

    int *borders = malloc(sizeof(int) * 8);
    if (!borders)
    {
        printf("Failed creating array of borders\n");
        return NULL;
    }
    //top coord
    int top = (coord < grid->width) ? (grid->width * (grid->height - 1)) + coord : coord - grid->width;
    borders[1] = top;

    //bottom coord
    int bot = (coord + grid->width >= grid->width * grid->height) ? coord % grid->width : coord + grid->width;
    borders[6] = bot;

    //left coord
    int lef = (coord % grid->width == 0) ? coord + grid->width - 1 : coord - 1;
    borders[3] = lef;

    //right coord
    int rig = (coord % grid->width + 1 == grid->width) ? coord - grid->width + 1 : coord + 1;
    borders[4] = rig;

    //bottom left coord
    int botlef = ((bot / grid->width) * grid->width) + lef % grid->width;
    borders[5] = botlef;

    //bottom right coord
    int botrig = ((bot / grid->width) * grid->width) + rig % grid->width;
    borders[7] = botrig;

    //top left coord
    int toplef = ((top / grid->width) * grid->width) + lef % grid->width;
    borders[0] = toplef;

    //top right coord
    int toprig = ((top / grid->width) * grid->width) + rig % grid->width;
    borders[2] = toprig;

    return borders;
}

void free_grid(grid *g)
{
    free(g->grid);
    free(g->white_cells);
    free(g);
}
grid *build_grid(int width, int height)
{
    grid *new = malloc(sizeof(grid));

    if (!new)
    {
        printf("Failed allocating new grid \n");
        return NULL;
    }
    new->grid = calloc(sizeof(char), width * height);
    new->white_cells = calloc(sizeof(int), width * height);
    if (!new->grid || !new->white_cells)
    {
        printf("Failed allocating new grid \n");
        return NULL;
    }

    new->height = height;
    new->width = width;

    return new;
}