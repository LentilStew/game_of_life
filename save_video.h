#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/avutil.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct file
{
    AVFormatContext *container;
    AVCodecContext **codec;
    int nb_streams;
    int *frames;
} file;

file *create_output_file(int streams, const char *filename);
int encode_frame(file *encoder, AVFrame *input_frame, int index);
int create_video_encoder(AVCodecContext **cod_ctx, AVFormatContext *container, const char *encoder, int width, int height,
                         int pix_fmt, AVRational sample_aspect_ratio, AVRational frame_rate, int bit_rate, int buffer_size);
