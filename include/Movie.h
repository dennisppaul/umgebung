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

#include <thread>
#include <chrono>
#include <atomic>

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

namespace umgebung {

    class Movie;

    class MovieListener {
    public:
        virtual void movieEvent(Movie m) = 0;
    };

    class Movie : public PImage {
    public:
        Movie(const std::string& filename, int _channels = -1);

        ~Movie();

        bool available();

        bool read();

        void play();

        void pause();

        void reload();

        float frameRate() const;   //	Sets how often frames are read from the movie.
        void  speed(float factor); //	Sets the relative playback speed of the movie.
        float duration() const;    //	Returns the length of the movie in seconds.
        void  jump(float seconds); //	Jumps to a specific location within a movie.
        float time() const;        //	Returns the location of the playback head in seconds.
        void  loop();              //	Plays a movie continuously, restarting it when it's over.
        void  noLoop();            //	If a movie is looping, this will cause it to play until the end and then stop on the last

    private:
        std::atomic<bool> isLooping            = false;
        bool              mVideoFrameAvailable = false;
        std::thread       playbackThread;
        std::atomic<bool> keepRunning;
        std::atomic<bool> isPlaying;
        double            frameDuration; // Duration of each frame in seconds
#ifndef DISABLE_GRAPHICS
#ifndef DISABLE_VIDEO
        uint8_t*         buffer;
        AVFrame*         frame;
        AVFrame*         convertedFrame;
        AVCodecContext*  videoCodecContext;
        AVCodecContext*  audioCodecContext;
        AVFormatContext* formatContext;
        AVPacket*        packet;
        SwsContext*      swsContext;
        int              videoStreamIndex;
        int              audioStreamIndex;
        int              mFrameCounter = 0;
#endif // DISABLE_VIDEO
#endif // DISABLE_GRAPHICS

        int init_from_file(const std::string& filename, int _channels = -1);

        void playbackLoop();

        void calculateFrameDuration();

        bool processFrame();
    };

} // namespace umgebung