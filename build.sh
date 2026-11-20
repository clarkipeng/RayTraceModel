#!/bin/sh
# Build ./raytrace against the project-local Assimp and Open Image Denoise in .deps (no system installs).
# usage: ./setup-deps.sh once, then ./build.sh
set -e
cd "$(dirname "$0")"
D=.deps
O=$D/dl-oidn/oidn-2.5.1.x86_64.linux
g++ -std=c++17 -O3 -pthread main.cpp -o raytrace \
  -I external -I $D/assimp/include -I $O/include \
  -L $D/assimp/lib -L $O/lib -lassimp -lOpenImageDenoise \
  -Wl,-rpath,'$ORIGIN/'$D/assimp/lib -Wl,-rpath,'$ORIGIN/'$O/lib
echo raytrace
