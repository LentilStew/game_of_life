#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/avutil.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "save_video.h"

#define ERROR 0
#define INFO 1

void logging(int level, const char *fmt, ...)
{

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

file *create_output_file(int streams, const char *filename)
{
    int res;

    file *output = malloc(sizeof(file));
    if (!output)
    {
        return NULL;
    }

    output->nb_streams = streams;

    res = avformat_alloc_output_context2(&output->container, NULL, NULL, filename);
    if (res < 0)
    {
        logging(ERROR, "Failed opening output\n");
        return NULL;
    }

    output->codec = av_calloc(streams, sizeof(AVCodecContext *));
    output->frames = malloc(2 * sizeof(int));

    if (!output->codec || !output->frames)
    {
        logging(ERROR, "Failed allocating ram for file\n");
        return NULL;
    }

    for (int stream = 0; stream < streams; stream++)
    {
        output->codec[stream] = NULL;
        output->frames[stream] = 0;
    }

    return output;
}

int encode_frame(file *encoder, AVFrame *input_frame, int index)
{

    AVPacket *output_packet = av_packet_alloc();
    if (!output_packet)
    {
        logging(ERROR, "ENCODER: Failed mallocing output_package");
        return 1;
    }

    AVCodecContext *codec = encoder->codec[index];

    int response = avcodec_send_frame(codec, input_frame);
    printf("Encoding frame %i \n", response);

    while (response >= 0)
    {
        response = avcodec_receive_packet(codec, output_packet);

        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
        {
            break;
        }
        else if (response < 0)
        {
            logging(ERROR, "ENCODER: Error receiving packet");

            return 1;
        }

        output_packet->stream_index = index;
        response = av_interleaved_write_frame(encoder->container, output_packet);

        if (response != 0)
        {
            logging(ERROR, "ENCODER:failed writing frame");

            return 1;
        }
    }
    av_packet_unref(output_packet);
    av_packet_free(&output_packet);

    return 0;
}

int create_video_encoder(AVCodecContext **cod_ctx, AVFormatContext *container, const char *encoder, int width, int height,
                         int pix_fmt, AVRational sample_aspect_ratio, AVRational frame_rate, int bit_rate, int buffer_size)
{
    AVStream *stream = avformat_new_stream(container, NULL);
    if (!stream)
    {
        logging(ERROR, "CREATE VIDEO ENCODER: Failed allocating memory for stream");
        return 1;
    }
    const AVCodec *enc = avcodec_find_encoder_by_name(encoder);
    if (!enc)
    {
        logging(ERROR, "CREATE VIDEO ENCODER: Failed searching encoder");

        return 1;
    }

    cod_ctx[0] = avcodec_alloc_context3(enc);

    if (!cod_ctx[0])
    {
        logging(ERROR, "CREATE VIDEO ENCODER: Failed allocation codec context");
        return 1;
    }

    cod_ctx[0]->height = height;
    cod_ctx[0]->width = width;
    cod_ctx[0]->pix_fmt = pix_fmt;

    cod_ctx[0]->sample_aspect_ratio = sample_aspect_ratio;
    cod_ctx[0]->time_base = av_inv_q(frame_rate); //av_inv_q(frame_rate);
    cod_ctx[0]->framerate = frame_rate;
    cod_ctx[0]->bit_rate = bit_rate;
    cod_ctx[0]->rc_buffer_size = buffer_size;
    cod_ctx[0]->rc_max_rate = buffer_size;
    cod_ctx[0]->rc_min_rate = buffer_size;

    stream->time_base = cod_ctx[0]->time_base; //cod_ctx[0]->time_base;

    int res = 0;

    res = avcodec_open2(cod_ctx[0], enc, NULL);
    if (res < 0)
    {
        logging(ERROR, "CREATE VIDEO ENCODER: couldn't open codec");
        return 1;
    }

    res = avcodec_parameters_from_context(stream->codecpar, cod_ctx[0]);

    if (res < 0)
    {
        logging(ERROR, "CREATE VIDEO ENCODER: failed setting codec parameters from context");
        return 1;
    }

    return 0;
}