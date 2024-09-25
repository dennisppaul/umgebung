#!/bin/bash

# TODO this is not tested on RPI

echo "+++ setting up umgebung      +++"

echo "+++ updating package manager +++"

sudo apt-get update -y
sudo apt-get upgrade -y

echo "+++ installing packages      +++"

sudo apt-get install git clang mesa-utils cmake pkg-config libsdl2-dev libftgl-dev libglew-dev ffmpeg libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libavdevice-dev librtmidi-dev -y

echo