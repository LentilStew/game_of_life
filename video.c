#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include "video.h"

int end_video_encoder(video_encoder *ve)
{
    uint8_t endcode[] = {0, 0, 1, 0xb7};

    /* flush the encoder */
    av_frame_free(&ve->frame);
    encode(ve);

    /* Add sequence end code to have a real MPEG file.
       It makes only sense because this tiny examples writes packets
       directly. This is called "elementary stream" and only works for some
       codecs. To create a valid file, you usually need to write packets
       into a proper file format or protocol; see muxing.c.
     */
    if (ve->codec_ctx->codec->id == AV_CODEC_ID_MPEG1VIDEO || ve->codec_ctx->codec->id == AV_CODEC_ID_MPEG2VIDEO)
        fwrite(endcode, 1, sizeof(endcode), ve->file);
    fclose(ve->file);

    avcodec_free_context(&ve->codec_ctx);
    av_packet_free(&ve->pkt);
    free(ve);
}

void encode(video_encoder *encoder)
{
    int ret;

    /* send the frame to the encoder */

    ret = avcodec_send_frame(encoder->codec_ctx, encoder->frame);
    if (ret < 0)
    {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }

    while (ret >= 0)
    {
        ret = avcodec_receive_packet(encoder->codec_ctx, encoder->pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0)
        {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

        fwrite(encoder->pkt->data, 1, encoder->pkt->size, encoder->file);
        av_packet_unref(encoder->pkt);
    }
}

AVFrame *create_frame(int pix_fmt, int width, int height)
{
    int ret;
    AVFrame *frame = av_frame_alloc();

    if (!frame)
    {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    frame->format = pix_fmt;
    frame->width = width;
    frame->height = height;

    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0)
    {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }

    return frame;
}

video_encoder *create_video_encoder(int width, int height, int pix_fmt, int bitrate, AVRational framerate,
                                    const char *filename, const char *codec_name)
{
    int ret;
    const AVCodec *codec;
    video_encoder *new = malloc(sizeof(video_encoder));
    if (!new)
    {
        printf("Could not allocate video_encoder");
        return NULL;
    }

    codec = avcodec_find_encoder_by_name(codec_name);
    if (!codec)
    {
        fprintf(stderr, "Codec '%s' not found\n", codec_name);
        return NULL;
    }

    new->codec_ctx = avcodec_alloc_context3(codec);
    if (!new->codec_ctx)
    {
        fprintf(stderr, "Could not allocate video codec context\n");
        return NULL;
    }
    /* put sample parameters */
    new->codec_ctx->bit_rate = bitrate;
    /* resolution must be a multiple of two */
    new->codec_ctx->width = width;
    new->codec_ctx->height = height;
    /* frames per second */
    new->codec_ctx->time_base = av_inv_q(framerate);
    new->codec_ctx->framerate = framerate;

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    new->codec_ctx->gop_size = 10;
    new->codec_ctx->max_b_frames = 1;
    new->codec_ctx->pix_fmt = pix_fmt;

    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(new->codec_ctx->priv_data, "preset", "slow", 0);

    /* open it */
    ret = avcodec_open2(new->codec_ctx, codec, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "Could not open codec: %s\n", av_err2str(ret));
        return NULL;
    }

    new->file = fopen(filename, "wb");
    if (!new->file)
    {
        fprintf(stderr, "Could not open %s\n", filename);
        return NULL;
    }

    new->frame = create_frame(pix_fmt, width, height);
    if (!new->frame)
    {
        printf("Failed creating frame\n");
        return NULL;
    }
    new->pkt = av_packet_alloc();
    if (!new->pkt)
    {
        printf("Failed creating packet\n");
        return NULL;
    }

    return new;
}
