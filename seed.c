#include <stdio.h>
#include "seed.h"

void printBits(__uint64_t num);

//source https://youtu.be/Ks1pw1X22y4?t=518
void fill_grid_with_LFSR(char *grid, int width, int height, __uint64_t seed)
{

    for (int i = 0, limit = width * height; i < limit; i++)
    {
        __uint64_t newbit = (seed ^ (seed >> 1)) & 1;
        seed = (seed >> 1) | (newbit << 63);
        //print_bits(seed);
        grid[i] = newbit;
    }
}
//source https://stackoverflow.com/questions/9280654/c-printing-bits
void print_bits(__uint64_t num)
{
    for (int bit = 0; bit < (sizeof(__uint64_t) * 8); bit++)
    {
        printf("%li ", num & 0x01);
        num = num >> 1;
    }
    putchar('\n');
}