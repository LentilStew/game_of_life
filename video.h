
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libavcodec/avcodec.h>

#include <libavutil/opt.h>
#include <libavutil/imgutils.h>

typedef struct video_encoder
{
    FILE *file;
    AVCodecContext *codec_ctx;
    AVPacket *pkt;
    AVFrame *frame;
} video_encoder;
int end_video_encoder(video_encoder *ve);

video_encoder *create_video_encoder(int width, int height, int pix_fmt, int bitrate, AVRational framerate,
                                    const char *filename, const char *codec_name);

AVFrame *create_frame(int pix_fmt, int width, int height);

void encode(video_encoder *encoder);
