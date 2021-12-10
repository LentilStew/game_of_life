#include "stdio.h"
#include "stdlib.h"
#include "memory.h"
#include <pthread.h>

void upscale(char *grid, char *new_grid, int in_width, int in_height, int times);
char *upscale_th(const char *grid, int in_width, int in_height, int times);
void save_gray_frame(unsigned char *buf, int width, int height);
void fill_grid_with_LFSR(char *grid, int width, int height, __uint64_t seed);
unsigned long lrand();