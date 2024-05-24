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

// TODO look into audio processing
// TODO look into camera access
// TODO implement `MovieListener` including callback

using namespace umgebung;

#if !defined(DISABLE_GRAPHICS) && !defined(DISABLE_VIDEO)

#include "Umgebung.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
}

Movie::Movie(const std::string& filename, int _channels) : PImage() {
    if (init_from_file(filename, _channels) >= 0) {
        std::cout << "+++ Movie: width: " << width << ", height: " << height << ", channels: " << channels << std::endl;
        calculateFrameDuration();
        keepRunning    = true;
        isPlaying      = false;
        playbackThread = std::thread(&Movie::playbackLoop, this);
    } else {
        std::cerr << "+++ Movie: ERROR: could not initialize from file" << std::endl;
    }
}

int Movie::init_from_file(const std::string& filename, int _channels) {
    // Open the input file
    formatContext = avformat_alloc_context();
    if (avformat_open_input(&formatContext, filename.c_str(), NULL, NULL) != 0) {
        std::cerr << "+++ Movie: ERROR: Could not open file: " << filename.c_str() << std::endl;
        return -1;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        std::cerr << "+++ Movie: ERROR: Could not find stream information" << std::endl;
        return -1;
    }

    // Find the first video stream
    videoStreamIndex = -1;
    audioStreamIndex = -1;
    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        //        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
        //            videoStreamIndex = i;
        //            break;
        //        }
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && videoStreamIndex < 0) {
            videoStreamIndex = i;
        } else if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audioStreamIndex < 0) {
            audioStreamIndex = i;
        }
    }

    if (videoStreamIndex == -1) {
        std::cerr << "+++ Movie: ERROR: Could not find a video stream" << std::endl;
        return -1;
    }

    if (audioStreamIndex == -1) {
        std::cerr << "+++ Movie: ERROR: Could not find a video stream" << std::endl;
        return -1;
    }

    // Get a pointer to the codec context for the video stream
    AVCodecParameters* codecParameters = formatContext->streams[videoStreamIndex]->codecpar;
    const AVCodec*     codec           = avcodec_find_decoder(codecParameters->codec_id);
    videoCodecContext                  = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(videoCodecContext, codecParameters);
    if (avcodec_open2(videoCodecContext, codec, NULL) < 0) {
        std::cerr << "+++ Movie: ERROR: Could not open codec" << std::endl;
        return -1;
    }

    // Initialize audio codec
    const AVCodec* audioCodec = avcodec_find_decoder(formatContext->streams[audioStreamIndex]->codecpar->codec_id);
    audioCodecContext         = avcodec_alloc_context3(audioCodec);
    avcodec_parameters_to_context(audioCodecContext, formatContext->streams[audioStreamIndex]->codecpar);
    avcodec_open2(audioCodecContext, audioCodec, NULL);

    // retrieve movie framerate
    AVRational frame_rate     = formatContext->streams[videoStreamIndex]->avg_frame_rate;
    double     frame_duration = 1.0 / (frame_rate.num / (double) frame_rate.den);
    std::cout << "+++ Movie: framerate     : " << (frame_rate.num / frame_rate.den) << std::endl;
    std::cout << "+++ Movie: frame duration: " << frame_duration << std::endl;

    // Determine the pixel format and number of channels based on input file
    AVPixelFormat             src_pix_fmt = videoCodecContext->pix_fmt;
    AVPixelFormat             dst_pix_fmt;
    const AVPixFmtDescriptor* desc = av_pix_fmt_desc_get(src_pix_fmt);
    if (_channels < 0) {
        _channels   = 4;
        dst_pix_fmt = AV_PIX_FMT_RGBA;
        //        std::cout << "not looking for format. defaulting to RGBA ( 4 channels )" << std::endl;
    } else if (desc && desc->nb_components == 4) {
        _channels   = 4;
        dst_pix_fmt = AV_PIX_FMT_RGBA;
        //        std::cout << "found RGBA video" << std::endl;
    } else {
        _channels   = 3;
        dst_pix_fmt = AV_PIX_FMT_RGB24;
        //        std::cout << "found RGB video" << std::endl;
    }

    // Create a sws context for the conversion
    swsContext = sws_getContext(
        videoCodecContext->width, videoCodecContext->height, src_pix_fmt,
        videoCodecContext->width, videoCodecContext->height, dst_pix_fmt,
        SWS_FAST_BILINEAR,
        //            SWS_BICUBIC,
        NULL, NULL, NULL);

    if (!swsContext) {
        std::cerr << "+++ Movie: ERROR: Failed to create SwScale context" << std::endl;
        return -1;
    }

    // Allocate an AVFrame structure
    frame = av_frame_alloc();
    if (!frame) {
        std::cerr << "+++ Movie: ERROR: Failed to allocate frame" << std::endl;
        return -1;
    }

    // Allocate an AVFrame structure for the converted frame
    convertedFrame = av_frame_alloc();
    if (!convertedFrame) {
        std::cerr << "+++ Movie: ERROR: Failed to allocate converted frame" << std::endl;
        return -1;
    }

    int numBytes = av_image_get_buffer_size(dst_pix_fmt,
                                            videoCodecContext->width,
                                            videoCodecContext->height,
                                            1);
    buffer       = (uint8_t*) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(convertedFrame->data,
                         convertedFrame->linesize,
                         buffer,
                         dst_pix_fmt,
                         videoCodecContext->width,
                         videoCodecContext->height,
                         1);
    packet = av_packet_alloc();

    init(videoCodecContext->width, videoCodecContext->height, _channels, convertedFrame->data[0]);
    return 1;
}

Movie::~Movie() {
    keepRunning = false;
    if (playbackThread.joinable()) {
        playbackThread.join();
    }
    av_freep(&buffer);
    av_frame_free(&frame);
    av_frame_free(&convertedFrame);
    avcodec_close(videoCodecContext);
    avcodec_free_context(&videoCodecContext);
    avformat_close_input(&formatContext);
    avformat_free_context(formatContext);
    av_packet_free(&packet);
    sws_freeContext(swsContext);
}

void Movie::calculateFrameDuration() {
    AVRational frame_rate = formatContext->streams[videoStreamIndex]->avg_frame_rate;
    frameDuration         = 1.0 / (frame_rate.num / (double) frame_rate.den);
}

void Movie::playbackLoop() {
    while (keepRunning) {
        if (isPlaying) {
            auto frame_start = std::chrono::steady_clock::now();
            if (available()) {
                if (processFrame()) {
                    // TODO flag that a texture reload is required
                    // TODO callback with `MovieListener`
                    //                } else {
                    //                    // Check if the end of the video is reached
                    //                    bool isEndOfVideo = false;
                    //                    if (isEndOfVideo) {
                    //                        std::cout << "+++ end of video reached" << std::endl;
                    //                        if (isLooping) {
                    //                            // Seek back to the start of the video
                    //                            av_seek_frame(formatContext, videoStream, 0, AVSEEK_FLAG_BACKWARD);
                    //                            mFrameCounter = 0; // Reset frame counter
                    //                        } else {
                    //                            // Stop playback if not looping
                    //                            pause();
                    //                        }
                    //                    }
                }
            }

            auto                          frame_end = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed   = frame_end - frame_start;

            if (elapsed.count() < frameDuration) {
                std::this_thread::sleep_for(std::chrono::duration<double>(frameDuration - elapsed.count()));
            }
        } else {
            // If not playing, sleep for a short duration to prevent busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void Movie::play() {
    isPlaying = true;
}

void Movie::pause() {
    isPlaying = false;
}

bool Movie::available() {
    int ret = av_read_frame(formatContext, packet);
    if (ret >= 0) {
        if (packet->stream_index == videoStreamIndex) {
            mVideoFrameAvailable = true;
            avcodec_send_packet(videoCodecContext, packet);
        } else if (packet->stream_index == audioStreamIndex) {
            // Decode audio frame
            avcodec_send_packet(audioCodecContext, packet);
            // TODO implement audio processing
            //            while (avcodec_receive_frame(audioCodecContext, frame) == 0) {
            //                process_audio_frame(frame);
            //                av_frame_unref(frame);
            //            }
        }
        av_packet_unref(packet);
    } else if (ret == AVERROR_EOF) {
        //        std::cout << "AVERROR_EOF" << std::endl;
        if (isLooping) {
            // Seek back to the start of the video
            av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
            mFrameCounter = 0; // Reset frame counter
        } else {
            // Stop playback if not looping
            pause();
        }
    } else {
        char err_buf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, err_buf, AV_ERROR_MAX_STRING_SIZE);
        printf("Error occurred: %s\n", err_buf);
    }
    return mVideoFrameAvailable;
}

bool Movie::processFrame() {
    if (mVideoFrameAvailable) {
        int ret = avcodec_receive_frame(videoCodecContext, frame);
        if (ret == 0) {
            // Successfully received a frame
            mFrameCounter++;

            // Convert data to RGBA or RGB
            sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height, convertedFrame->data, convertedFrame->linesize);

            av_frame_unref(frame);
            mVideoFrameAvailable = false;
            return true;
        } else {
            if (ret == AVERROR_EOF) {
                // Handle end of stream
                std::cout << "+++ AVERROR_EOF" << std::endl;
            } else if (ret == AVERROR(EAGAIN)) {
                // No frame available right now, try again later
                //                std::cout << "+++ AVERROR(EAGAIN)" << std::endl;
            } else {
                // std::cerr << "+++ Movie: ERROR: Error receiving frame: " << av_err2str(ret));
            }
            av_frame_unref(frame);
            return false;
        }
    }
    return false;
}

void Movie::reload() {
    GLint mFormat = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, mFormat, width, height, 0, mFormat, GL_UNSIGNED_BYTE, convertedFrame->data[0]);
}

bool Movie::read() {
    if (!processFrame()) {
        return false; // No frame available or error processing frame
    }
    GLint mFormat = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, mFormat, width, height, 0, mFormat, GL_UNSIGNED_BYTE, convertedFrame->data[0]);
    return true;
}

// Example of frameRate() method
float Movie::frameRate() const {
    AVRational frame_rate = formatContext->streams[videoStreamIndex]->avg_frame_rate;
    return frame_rate.num / static_cast<float>(frame_rate.den);
}

// Example of setting playback speed
void Movie::speed(float factor) {
    //    std::lock_guard<std::mutex> lock(mutex);
    frameDuration /= factor; // Adjust frame duration based on speed factor
}

// Example of duration() method
float Movie::duration() const {
    return static_cast<float>(formatContext->duration) / AV_TIME_BASE;
}

// Example of jump() method
void Movie::jump(float seconds) {
    int64_t timestamp = seconds * AV_TIME_BASE;
    av_seek_frame(formatContext, videoStreamIndex, timestamp, AVSEEK_FLAG_ANY);
    // Reset any buffers or states as needed
}

float Movie::time() const {
    //    std::lock_guard<std::mutex> lock(mutex);
    return mFrameCounter / frameRate();
}

void Movie::loop() {
    isLooping = true;
}

void Movie::noLoop() {
    isLooping = false;
}

#else

Movie::Movie(const std::string& filename, int _channels) : PImage() {
    std::cerr << "Movie - ERROR: video is disabled" << std::endl;
}

Movie::~Movie() {}

bool Movie::available() { return false; }

bool Movie::read() { return false; }

int Movie::init_from_file(const std::string& filename, int _channels) { return -1; }

void Movie::playbackLoop() {}

void Movie::calculateFrameDuration() {}

void Movie::play() {}

void Movie::pause() {}

bool Movie::processFrame() { return false; }

#endif // DISABLE_GRAPHICS && DISABLE_VIDEO