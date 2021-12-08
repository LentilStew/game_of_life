#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include <unistd.h>
#include "seed.h"

typedef struct grid
{
    int width;
    int height;
    u_int8_t *grid;
    int *white_cells;
    int white_cells_len;
} grid;

unsigned long lrand()
{
    unsigned long r = 0;

    for (int i = 0; i < 5; ++i)
    {
        r = (r << 15) | (rand() & 0x7FFF);
    }

    return r & 0xFFFFFFFFUL;
}

void search_white_cells(grid *grid);
int *get_array_of_neighbours(grid *grid, int coord);
int count_sourranding_white_cells(grid *grid, int coord);
int update_grid(grid *grid);
grid *build_grid(int width, int height);
void free_grid(grid *g);

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
    if (!new->grid || new->white_cells)
    {
        printf("Failed allocating new grid \n");
        return NULL;
    }

    new->height = height;
    new->width = width;

    return new;
}
int main()
{

    int res;
    grid *grid1 = build_grid(100, 100);
    char *upscaled_grid;
    int times = 8;
    fill_grid_with_LFSR(grid1->grid, grid1->width, grid1->height, 0xFFAA465AF);
    /*
    grid1->grid[(20) * grid1->width + 85 + 1] = 1;
    grid1->grid[(20 + 1) * grid1->width + 85] = 1;
    grid1->grid[(20 + 1) * grid1->width + 85 + 1] = 1;
    grid1->grid[(20 + 2) * grid1->width + 85 + 1] = 1;
    grid1->grid[(20) * grid1->width + 85 + 2] = 1;
*/
    int counter = 0;

    while (counter < 10020)
    {
        counter++;
        printf("%i\n", counter);

        //upscaled_grid = upscale(grid1->grid, grid1->width, grid1->height, times);
        //save_gray_frame(upscaled_grid, grid1->width * times, grid1->height * times);
        //free(upscaled_grid);

        res = update_grid(grid1);

        if (res != 0)
        {
            printf("Failed updating grid\n");
            return 1;
        }
    }
    free_grid(grid1);
}

int update_grid(grid *grid)
{

    char *new_grid = calloc(sizeof(char), grid->width * grid->height);
    int *white_cells = calloc(sizeof(int), grid->width * grid->height);

    int white_cells_counter = 0;

    if (!new_grid)
    {
        printf("Failed allocatin new_grid\n");
        return 1;
    }
    //loop all cells
    for (int curr_white_cell = 0, limit = grid->width * grid->height; curr_white_cell < limit; curr_white_cell++)
    {
        if (grid->grid[curr_white_cell] != 1)
            continue;
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
            white_cells[white_cells_counter] = curr_white_cell;
            white_cells_counter++;
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
                white_cells[white_cells_counter] = borders[i];
                white_cells_counter++;
            }
        }

        free(borders);
    }

    free(grid->grid);

    grid->grid = new_grid;

    return 0;
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
    int top = (coord < grid->width) ? (grid->height * (grid->width - 1)) + coord : coord - grid->width;
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
    free(g);
}