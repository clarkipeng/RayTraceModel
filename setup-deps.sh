#!/bin/sh
# Vendor the build dependencies project-locally in .deps (no system installs):
# CMake (to build Assimp), Assimp, Intel Open Image Denoise, and a static ffmpeg for make-mp4.sh.
set -e
cd "$(dirname "$0")"
mkdir -p .deps && cd .deps
if [ ! -d dl-cmake/cmake-4.5.1-linux-x86_64 ]; then
  mkdir -p dl-cmake && curl -sL https://github.com/Kitware/CMake/releases/download/v4.5.1/cmake-4.5.1-linux-x86_64.tar.gz | tar xz -C dl-cmake
fi
if [ ! -d assimp/lib ]; then
  mkdir -p dl-assimp && curl -sL https://github.com/assimp/assimp/archive/refs/tags/v6.0.6.tar.gz | tar xz -C dl-assimp
  C=$PWD/dl-cmake/cmake-4.5.1-linux-x86_64/bin/cmake
  $C -S dl-assimp/assimp-6.0.6 -B dl-assimp/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$PWD/assimp \
     -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_WARNINGS_AS_ERRORS=OFF \
     -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF -DASSIMP_BUILD_OBJ_IMPORTER=ON -DBUILD_SHARED_LIBS=ON
  $C --build dl-assimp/build -j"$(nproc)" && $C --install dl-assimp/build
fi
if [ ! -d dl-oidn/oidn-2.5.1.x86_64.linux ]; then
  mkdir -p dl-oidn && curl -sL https://github.com/RenderKit/oidn/releases/download/v2.5.1/oidn-2.5.1.x86_64.linux.tar.gz | tar xz -C dl-oidn
fi
if [ ! -x dl-ffmpeg/ffmpeg ]; then
  mkdir -p dl-ffmpeg && curl -sL https://johnvansickle.com/ffmpeg/releases/ffmpeg-release-amd64-static.tar.xz | tar xJ -C dl-ffmpeg --strip-components=1
fi
echo "deps ready in .deps"
