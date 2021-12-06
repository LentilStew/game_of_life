#include "stdio.h"
#include "stdlib.h"
#include "display.h"
int counter = 0;
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
