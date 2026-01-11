#!/bin/bash

#sudo apt install \
#    cmake \
#    pkg-config \
#    libsndfile1-dev \
#    libfftw3-dev \
#    ocl-icd-opencl-dev \
#    libopenal-dev \
#    libfreetype6-dev \
#    libxrandr-dev \
#    libudev-dev \
#    libgl1-mesa-dev

# cd ~
# git clone https://github.com/SFML/SFML.git
# cd SFML
# mkdir build && cd build
# cmake .. -DCMAKE_BUILD_TYPE=Release
# make -j$(nproc)

#cd ~
#git clone https://github.com/texus/TGUI.git
#cd TGUI
#mkdir build && cd build
#
#cmake .. \
#  -DCMAKE_BUILD_TYPE=Release \
#  -DTGUI_BACKEND=SFML \
#  -DSFML_DIR=$HOME/SFML/build/lib/cmake/SFML
#
#mkdir build-linux
#cd build-linux
#make -j$(nproc)

cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=$HOME/SFML/build

make -j$(nproc)

