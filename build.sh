#!/bin/sh
# Build ./raytrace against the project-local Assimp in .deps (no system installs).
# usage: ./setup-deps.sh once, then ./build.sh
set -e
cd "$(dirname "$0")"
D=.deps
g++ -std=c++17 -O3 -pthread main.cpp -o raytrace \
  -I external -I $D/assimp/include \
  -L $D/assimp/lib -lassimp \
  -Wl,-rpath,'$ORIGIN/'$D/assimp/lib
echo raytrace
