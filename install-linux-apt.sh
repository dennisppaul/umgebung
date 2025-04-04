#!/bin/bash

# fail fast if any command fails and elevate privileges
set -e
echo "--- requesting sudo access once    ---"
sudo -v
# keep-alive: update existing `sudo` time stamp until script is done
# (this runs in background and exits when this script finishes)
while true; do sudo -n true; sleep 60; done 2>/dev/null &
KEEP_SUDO_ALIVE_PID="$!"
# trap exit to clean up background sudo keeper
trap 'kill "$KEEP_SUDO_ALIVE_PID"' EXIT

# update and upgrade system packages
echo "--- updating apt                   ---"
sudo apt-get update -y
sudo apt-get upgrade -y

# install basic build tools and graphics utilities
echo "--- installing build tools         ---"
sudo apt-get install -y \
  git \
  clang \
  cmake \
  mesa-utils

# install umgebung dependencies
echo "--- installing dependencies        ---"
sudo apt-get install -y \
  libglew-dev \
  libharfbuzz-dev \
  libfreetype6-dev \
  libglm-dev \
  ffmpeg \
  libavcodec-dev \
  libavformat-dev \
  libavutil-dev \
  libswscale-dev \
  libavdevice-dev \
  librtmidi-dev \
  portaudio19-dev

# install SDL3 from source into system
echo "--- installing SDL3 from source... ---"

TMP_DIR=$(mktemp -d)
cd "$TMP_DIR"

git clone --depth=1 https://github.com/libsdl-org/SDL.git
cd SDL

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build

# clean up
cd ~
rm -rf "$TMP_DIR"

echo "--- installed SDL3 successfully    ---"
