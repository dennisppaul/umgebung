/*
 * Umfeld
 *
 * This file is part of the *Umfeld* library (https://github.com/dennisppaul/umfeld).
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

#ifdef ENABLE_CAPTURE

#include <iostream>
#include <vector>
#include <string>

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

struct DeviceCapability {
    std::string device_name;
    int width;
    int height;
    double minimum_frame_rate;
    double maximum_frame_rate;
    std::string pixel_format;
};

std::vector<DeviceCapability> getDeviceCapabilities() {
    std::vector<DeviceCapability> capabilities;

    @autoreleasepool {
        NSArray *deviceTypes = @[
            AVCaptureDeviceTypeBuiltInWideAngleCamera
        ];

        AVCaptureDeviceDiscoverySession *discoverySession = [AVCaptureDeviceDiscoverySession
            discoverySessionWithDeviceTypes:deviceTypes
                                  mediaType:AVMediaTypeVideo
                                   position:AVCaptureDevicePositionUnspecified];

        NSArray<AVCaptureDevice *> *devices = discoverySession.devices;

        for (AVCaptureDevice *device in devices) {
            NSString *device_name = device.localizedName;

            for (AVCaptureDeviceFormat *format in device.formats) {
                CMFormatDescriptionRef desc = format.formatDescription;
                CMVideoDimensions dimensions = CMVideoFormatDescriptionGetDimensions(desc);

                FourCharCode mediaSubType = CMFormatDescriptionGetMediaSubType(desc);

                // Convert FourCharCode to NSString
                char formatCode[5];
                formatCode[0] = (mediaSubType >> 24) & 0xFF;
                formatCode[1] = (mediaSubType >> 16) & 0xFF;
                formatCode[2] = (mediaSubType >> 8) & 0xFF;
                formatCode[3] = mediaSubType & 0xFF;
                formatCode[4] = '\0';
                NSString *pixelFormatString = [NSString stringWithUTF8String:formatCode];

                // Map FourCC to FFmpeg pixel format string
                NSString *ffmpegPixelFormat = nil;
                if (mediaSubType == kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange ||
                    mediaSubType == kCVPixelFormatType_420YpCbCr8BiPlanarFullRange) {
                    ffmpegPixelFormat = @"nv12";
                } else if (mediaSubType == kCVPixelFormatType_422YpCbCr8) {
                    ffmpegPixelFormat = @"uyvy422";
                } else if (mediaSubType == kCVPixelFormatType_32BGRA) {
                    ffmpegPixelFormat = @"bgra";
                } else if (mediaSubType == kCVPixelFormatType_32ARGB) {
                    ffmpegPixelFormat = @"argb";
                } else if (mediaSubType == kCVPixelFormatType_32RGBA) {
                    ffmpegPixelFormat = @"rgba";
                } else {
                    ffmpegPixelFormat = @"unknown";
                }

                NSArray<AVFrameRateRange *> *frameRateRanges = format.videoSupportedFrameRateRanges;
                for (AVFrameRateRange *range in frameRateRanges) {
                    DeviceCapability cap;
                    cap.device_name = [device_name UTF8String];
                    cap.width = dimensions.width;
                    cap.height = dimensions.height;
                    cap.minimum_frame_rate = range.minFrameRate;
                    cap.maximum_frame_rate = range.maxFrameRate;
                    cap.pixel_format = [ffmpegPixelFormat UTF8String];
                    capabilities.push_back(cap);
                }
            }
        }
    }

    return capabilities;
}

#endif // ENABLE_CAPTURE