/*
 * Umgebung
 *
 * This file is part of the *Umgebung* library (https://github.com/dennisppaul/umgebung).
 * Copyright (c) 2023 Dennis P Paul.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Movie.h"

// TODO where is `-lavdevice -lswscale`?!?

#if !defined(DISABLE_GRAPHICS) && !defined(DISABLE_VIDEO)

#include "Umgebung.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

Movie::Movie(const std::string &filename) : PImage() {
#define USE_32BIT_FORMAT
    int _channels;
#ifdef USE_32BIT_FORMAT
    _channels = 4;
    AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGBA;
#else
    _channels = 3;
    AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGB24;
#endif

    // Open the input file
    formatContext = avformat_alloc_context();
    if (avformat_open_input(&formatContext, filename.c_str(), NULL, NULL) != 0) {
        fprintf(stderr, "Could not open file %s\n", filename.c_str());
//        return -1;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
//        return -1;
    }

    // Find the first video stream
    videoStream = -1;
    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }

    if (videoStream == -1) {
        fprintf(stderr, "Could not find a video stream\n");
//        return -1;
    }

    // Get a pointer to the codec context for the video stream
    AVCodecParameters *codecParameters = formatContext->streams[videoStream]->codecpar;
    const AVCodec     *codec           = avcodec_find_decoder(codecParameters->codec_id);
    codecContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecContext, codecParameters);
    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
//        return -1;
    }

    // Create a sws context for the conversion
    AVPixelFormat src_pix_fmt = codecContext->pix_fmt;
    swsContext = sws_getContext(
            codecContext->width, codecContext->height, src_pix_fmt,
            codecContext->width, codecContext->height, dst_pix_fmt,
            SWS_BICUBIC, NULL, NULL, NULL);

    if (!swsContext) {
        fprintf(stderr, "Failed to create SwScale context\n");
//        return -1;
    }

    // Allocate an AVFrame structure
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Failed to allocate frame\n");
//        return -1;
    }

    // Allocate an AVFrame structure for the converted frame
    convertedFrame = av_frame_alloc();
    if (!convertedFrame) {
        fprintf(stderr, "Failed to allocate converted frame\n");
//        return -1;
    }

    int     numBytes = av_image_get_buffer_size(dst_pix_fmt,
                                                codecContext->width, codecContext->height, 1);
    uint8_t *buffer  = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(convertedFrame->data, convertedFrame->linesize, buffer,
                         dst_pix_fmt,
                         codecContext->width, codecContext->height, 1);
    packet = av_packet_alloc();

    init(codecContext->width, codecContext->height, _channels, convertedFrame->data[0]);

    std::cout << "Movie: width: " << width << ", height: " << height << ", channels: " << channels << std::endl;
}

Movie::~Movie() {
    av_frame_free(&frame);
    av_frame_free(&convertedFrame); // TODO does this also free `uint8_t *buffer`?
    avcodec_close(codecContext);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
    avformat_free_context(formatContext);
    av_packet_free(&packet);
    sws_freeContext(swsContext);
}

bool Movie::available() {
    bool mAvailable = false;
    if (av_read_frame(formatContext, packet) >= 0) {
        if (packet->stream_index == videoStream) {
            avcodec_send_packet(codecContext, packet);
            int ret = avcodec_receive_frame(codecContext, frame);
            if (ret == 0) {
                // Successfully received a frame
//                fprintf(stdout, "Successfully received a frame: %i\n", mFrameCounter);
                mFrameCounter++;
                mAvailable = true;
                // TODO update texture ... move this to `read()`?
                // convert data to RGBA
                sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height, convertedFrame->data, convertedFrame->linesize);
                // SDL_UpdateTexture(texture, NULL, convertedFrame->data[0], convertedFrame->linesize[0]);
            } else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                fprintf(stdout, "No more frames : %i\n", ret);
                // No more frames
                mAvailable = false;
            } else {
                fprintf(stderr, "Error receiving frame: %s\n", av_err2str(ret));
                mAvailable = false;
            }
        }
        av_frame_unref(frame);
//        av_frame_unref(convertedFrame); // TODO this creates `bad dst image pointers`
        av_packet_unref(packet);
    }
    return mAvailable;
}

void Movie::read() {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 width, height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, convertedFrame->data[0]);
}

#else
Movie::Movie(const std::string &filename) : PImage() {
    std::cerr << "Movie - ERROR: video is disabled" << std::endl;
}
Movie::~Movie() {}
bool Movie::available() { return false; }
void Movie::read() {}
#endif // DISABLE_GRAPHICS && DISABLE_VIDEO