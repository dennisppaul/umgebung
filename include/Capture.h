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

#ifndef DISABLE_GRAPHICS
#ifndef DISABLE_VIDEO
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}
#endif // DISABLE_VIDEO
#endif // DISABLE_GRAPHICS

#include <vector>
#include <string>
#include <sstream>
#include <thread>

#include "PImage.h"

struct DeviceCapability {
    std::string device_name;
    int         width;
    int         height;
    double      minimum_frame_rate;
    double      maximum_frame_rate;
};

std::vector<DeviceCapability> getDeviceCapabilities();

namespace umgebung {
    class Capture;

    class CaptureListener {
    public:
        virtual void captureEvent(Capture m) = 0;

        virtual ~CaptureListener() = default;
    };

    class Capture final : public PImage {
    public:
        Capture(const char* device_name,
                const char* resolution,
                const char* frame_rate,
                const char* pixel_format);

        bool available();
        bool read();
        void play();
        void pause();
        void reload();

        ~Capture() override;

    private:
        bool              mVideoFrameAvailable = false;
        std::thread       playbackThread;
        std::atomic<bool> keepRunning{};
        std::atomic<bool> isPlaying{};
        double            frameDuration{};
#ifndef DISABLE_GRAPHICS
#ifndef DISABLE_VIDEO
        uint8_t*         buffer{};
        AVFormatContext* formatContext{};
        AVCodecContext*  codecContext     = nullptr;
        AVFrame*         frame            = nullptr;
        AVFrame*         convertedFrame   = nullptr;
        AVPacket*        packet           = nullptr;
        SwsContext*      swsContext       = nullptr;
        AVDictionary*    options          = nullptr;
        int              videoStreamIndex = -1;
        int              mFrameCounter    = 0;
#endif // DISABLE_VIDEO
#endif // DISABLE_GRAPHICS

        bool processFrame();
        void playbackLoop();
        int  connect(const char* device_name,
                     const char* resolution,
                     const char* frame_rate,
                     const char* pixel_format);

        /* --- print available devices --- */

    public:
        static bool print_available_devices(std::vector<std::string>& devices);
        static void list_capabilities(const char* device_name);

    private:
        static std::stringstream logStream;
        static bool              fDevicesRegistered;
        static void              register_all_devices();
        static void              custom_log_callback(void* ptr, int level, const char* fmt, va_list vargs);
    };
} // namespace umgebung