#include "stdio.h"
#include "stdlib.h"

char *upscale(char *grid, int in_width, int in_height, int times);
char *upscale_th(char *grid, int in_width, int in_height, int times);
void save_gray_frame(unsigned char *buf, int width, int height);