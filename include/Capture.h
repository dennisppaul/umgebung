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

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/avutil.h>
#include <libavutil/log.h>
#include <libavutil/opt.h>
}

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

namespace umgebung {
    class Movie;

    class MovieListener {
    public:
        virtual ~    MovieListener()     = default;
        virtual void movieEvent(Movie m) = 0;
    };
    class Capture final : public PImage {

    public:
        int connect(const char* device_name,
                    const char* resolution,
                    const char* frame_rate,
                    const char* pixel_format) {
            avdevice_register_all(); // Register input device (camera)

            // Set the input format for your platform:
            const AVInputFormat* inputFormat = nullptr;
#ifdef _WIN32
            inputFormat              = av_find_input_format("dshow"); // For Windows
            auto default_device_name = "video=Integrated Camera";
#elif __linux__
            inputFormat              = av_find_input_format("v4l2"); // For Linux
            auto default_device_name = "/dev/video0";
#elif __APPLE__
            inputFormat              = av_find_input_format("avfoundation"); // For macOS
            auto default_device_name = "0";
#endif

            std::string device_name_str;
            if (device_name == nullptr) {
                device_name_str = default_device_name;
            } else {
                device_name_str = device_name;
            }
            const auto deviceName = device_name_str.c_str();

            AVFormatContext* formatContext = nullptr;

            AVDictionary* options = nullptr;
            if (resolution != nullptr) {
                av_dict_set(&options, "video_size", resolution, 0);
            }
            if (frame_rate != nullptr) {
                av_dict_set(&options, "framerate", frame_rate, 0);
            }
            if (pixel_format != nullptr) {
                av_dict_set(&options, "pixel_format", pixel_format, 0);
            }

            // Open the camera
            std::cout << "open the camera: \"" << deviceName << "\"" << std::endl;
            if (avformat_open_input(&formatContext, deviceName, inputFormat, &options) != 0) {
                std::cerr << "Failed to open camera" << std::endl;
                av_dict_free(&options);
                return -1;
            }

            // Retrieve stream information
            if (avformat_find_stream_info(formatContext, nullptr) < 0) {
                std::cerr << "Couldn't find stream information" << std::endl;
                return -1;
            }

            // Find the first video stream
            std::cout << "Find the first video stream" << std::endl;
            int videoStreamIndex = -1;
            for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
                if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                    videoStreamIndex = i;
                    break;
                }
            }

            if (videoStreamIndex == -1) {
                std::cerr << "Couldn't find a video stream" << std::endl;
                return -1;
            }

            // Get the codec context for the video stream
            const AVCodecParameters* codecParams = formatContext->streams[videoStreamIndex]->codecpar;
            const AVCodec*           codec       = avcodec_find_decoder(codecParams->codec_id);
            if (!codec) {
                std::cerr << "Unsupported codec" << std::endl;
                return -1;
            }

            AVCodecContext* codecContext = avcodec_alloc_context3(codec);
            if (!codecContext) {
                std::cerr << "Couldn't allocate codec context" << std::endl;
                return -1;
            }

            if (avcodec_parameters_to_context(codecContext, codecParams) < 0) {
                std::cerr << "Couldn't copy codec parameters to context" << std::endl;
                return -1;
            }

            // Open codec
            if (avcodec_open2(codecContext, codec, nullptr) < 0) {
                std::cerr << "Couldn't open codec" << std::endl;
                return -1;
            }

            // Allocate video frame
            AVFrame*  frame  = av_frame_alloc();
            AVPacket* packet = av_packet_alloc();
            int       response;

            // Prepare for frame conversion (if you need to scale or convert pixel format)
            SwsContext* swsContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
                                                    codecContext->width, codecContext->height, AV_PIX_FMT_RGB24,
                                                    SWS_BILINEAR, nullptr, nullptr, nullptr);
            AVFrame*    rgbFrame   = av_frame_alloc();
            const int   numBytes   = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);
            const auto  buffer     = static_cast<uint8_t*>(av_malloc(numBytes * sizeof(uint8_t)));
            av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);

            // Capture and process frames
            while (av_read_frame(formatContext, packet) >= 0) {
                // Ensure the packet belongs to the video stream
                if (packet->stream_index == videoStreamIndex) {
                    response = avcodec_send_packet(codecContext, packet);
                    if (response < 0) {
                        std::cerr << "Error while sending packet to decoder: " << av_err2str(response) << std::endl;
                        break;
                    }

                    while (response >= 0) {
                        response = avcodec_receive_frame(codecContext, frame);
                        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                            break;
                        } else if (response < 0) {
                            std::cerr << "Error while receiving frame from decoder: " << av_err2str(response) << std::endl;
                            break;
                        }

                        // Convert the frame to RGB
                        sws_scale(swsContext, frame->data,
                                  frame->linesize,
                                  0,
                                  codecContext->height,
                                  rgbFrame->data,
                                  rgbFrame->linesize);

                        // Process the RGB frame (you could display it or save it)
                        std::cout << "Captured frame with width " << codecContext->width << " and height " << codecContext->height << std::endl;
                    }
                }
                av_packet_unref(packet);
            }

            // Cleanup
            av_dict_free(&options);
            av_free(buffer);
            av_frame_free(&rgbFrame);
            av_frame_free(&frame);
            av_packet_free(&packet);
            avcodec_free_context(&codecContext);
            avformat_close_input(&formatContext);

            return 0;
        }

        static std::stringstream logStream;

        static void custom_log_callback(void* ptr, int level, const char* fmt, va_list vargs) {
            // Append the log messages to the logStream
            char message[1024];
            vsnprintf(message, sizeof(message), fmt, vargs);
            logStream << message;
        }

        static bool print_available_devices(std::vector<std::string>& devices) {
            avdevice_register_all();

            // Set the custom log callback
            av_log_set_callback(custom_log_callback);

            // List DirectShow devices on Windows
            AVFormatContext* formatContext = avformat_alloc_context();
            AVDictionary*    options       = nullptr;
            av_dict_set(&options, "list_devices", "true", 0);

#ifdef _WIN32
            auto inputFormatString = "dshow";
#elif __linux__
            auto inputFormatString = "v4l2";
#elif __APPLE__
            auto inputFormatString = "avfoundation";
#endif
            const AVInputFormat* inputFormat = av_find_input_format(inputFormatString);

            if (!inputFormat) {
                std::cerr << "Input format not found" << std::endl;
                return false;
            }

            // Intercept available devices log
            avformat_open_input(&formatContext, "video=dummy", inputFormat, &options);

            // Convert logStream to a string and parse it
            const std::string  logOutput = logStream.str();
            std::istringstream logStreamParser(logOutput);
            std::string        line;

            bool inVideoSection = false; // Track when we're in the video devices section

            // Parse the log to find video device names
            while (std::getline(logStreamParser, line)) {
                // Detect the start of the video devices section
                if (line.find("video devices:") != std::string::npos) {
                    inVideoSection = true;
                    continue; // Skip the header line
                }

                // Detect the end of the video devices section (when audio devices start)
                if (line.find("audio devices:") != std::string::npos) {
                    inVideoSection = false;
                    break; // Exit once we've finished the video device section
                }

                // Parse device names within the video devices section
                if (inVideoSection) {
                    size_t startPos = line.find("] ") + 2; // Find the start of the device name (after "] ")
                    if (startPos != std::string::npos) {
                        std::string device = line.substr(startPos);
                        devices.push_back(device); // Add the device to the vector
                    }
                }
            }

            av_dict_free(&options);
            avformat_free_context(formatContext);

            // Reset log callback to default
            av_log_set_callback(av_log_default_callback);

            return true;
        }
    };

    std::stringstream Capture::logStream;
} // namespace umgebung