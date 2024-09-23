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

#if defined(ENABLE_CAPTURE) && !defined(DISABLE_GRAPHICS) && !defined(DISABLE_VIDEO)
#ifdef __cplusplus
extern "C" {
#endif
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#ifdef __cplusplus
}
#endif
#endif // ENABLE_CAPTURE && !DISABLE_GRAPHICS && !DISABLE_VIDEO

#include <atomic>
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
    std::string pixel_format;
};

std::vector<DeviceCapability> getDeviceCapabilities();

namespace umgebung {
    class Capture;

    class CaptureListener {
    public:
        virtual void captureEvent(Capture* capture) = 0;

        virtual ~CaptureListener() = default;
    };

    class Capture final : public PImage {
    public:
        Capture();

        bool        init(const char* device_name,
                         const char* resolution,
                         const char* frame_rate,
                         const char* pixel_format);
        bool        available();
        float       frameRate() const { return 1.0f / static_cast<float>(frameDuration); }
        bool        read();
        void        start();
        void        stop();
        void        reload();
        void        set_listener(CaptureListener* listener) { this->listener = listener; }
        const char* name() const { return fDeviceName; }

        ~Capture() override;

    private:
        const char*       fDeviceName{};
        bool              fIsInitialized       = false;
        bool              fVideoFrameAvailable = false;
        std::thread       playbackThread;
        std::atomic<bool> keepRunning{};
        std::atomic<bool> isPlaying{};
        double            frameDuration{};
        CaptureListener*  listener = nullptr;
#if defined(ENABLE_CAPTURE) && !defined(DISABLE_GRAPHICS) && !defined(DISABLE_VIDEO)
        uint8_t*         buffer{};
        AVFormatContext* formatContext{};
        AVCodecContext*  codecContext     = nullptr;
        AVFrame*         frame            = nullptr;
        AVFrame*         convertedFrame   = nullptr;
        AVPacket*        packet           = nullptr;
        SwsContext*      swsContext       = nullptr;
        AVDictionary*    options          = nullptr;
        int              videoStreamIndex = -1;
        int              fFrameCounter    = 0;
#endif // ENABLE_CAPTURE && !DISABLE_GRAPHICS && !DISABLE_VIDEO

        bool processFrame();
        void playbackLoop();
        int  connect(const char* device_name,
                     const char* resolution,
                     const char* frame_rate,
                     const char* pixel_format);

        /* --- print available devices --- */

    public:
        static std::vector<std::string>      list();
        static void                          list_capabilities(const std::string& device_name);
        static std::vector<DeviceCapability> devices_and_capabilities() {
            return getDeviceCapabilities();
        }

    private:
        static std::stringstream logStream;
        static bool              fDevicesRegistered;
        static void              register_all_devices();
        static void              custom_log_callback(void* ptr, int level, const char* fmt, va_list vargs);
    };
} // namespace umgebung
