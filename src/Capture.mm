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
};

std::vector<DeviceCapability> getDeviceCapabilities() {
    std::vector<DeviceCapability> capabilities;

    @autoreleasepool {
        NSArray *deviceTypes = @[
            AVCaptureDeviceTypeBuiltInWideAngleCamera,
            AVCaptureDeviceTypeExternal
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

                NSArray<AVFrameRateRange *> *frameRateRanges = format.videoSupportedFrameRateRanges;
                for (AVFrameRateRange *range in frameRateRanges) {
                    DeviceCapability cap;
                    cap.device_name = [device_name UTF8String];
                    cap.width = dimensions.width;
                    cap.height = dimensions.height;
                    cap.minimum_frame_rate = range.minFrameRate;
                    cap.maximum_frame_rate = range.maxFrameRate;
                    capabilities.push_back(cap);
                }
            }
        }
    }

    return capabilities;
}