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

#pragma once

#include <iostream>
#include <string>
#include "PImage.h"

#ifndef DISABLE_GRAPHICS
#ifndef DISABLE_VIDEO
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/parseutils.h>
#include <libavutil/imgutils.h>
#include <libavdevice/avdevice.h>
}
#endif // DISABLE_VIDEO
#endif // DISABLE_GRAPHICS

class Movie : public PImage {
public:
    Movie(const std::string &filename, int _channels = -1);

    ~Movie();

    bool available();

    void read();

private:
#ifndef DISABLE_GRAPHICS
#ifndef DISABLE_VIDEO
    AVFrame         *frame;
    AVFrame         *convertedFrame;
    AVCodecContext  *codecContext;
    AVFormatContext *formatContext;
    AVPacket        *packet;
    SwsContext      *swsContext;
    int             videoStream;
    int             mFrameCounter = 0;
#endif // DISABLE_VIDEO
#endif // DISABLE_GRAPHICS

    int init_from_file(const std::string &filename, int _channels = -1);
};

