#!/bin/sh
# Build ./raytrace against the project-local Assimp in .deps (no system installs), or any Assimp prefix in $ASSIMP.
# usage: ./setup-deps.sh once, then ./build.sh   (or: ASSIMP=/opt/homebrew/opt/assimp ./build.sh)
set -e
cd "$(dirname "$0")"
A=${ASSIMP:-$PWD/.deps/assimp}
g++ -std=c++17 -O3 -pthread main.cpp -o raytrace \
  -I external -I "$A/include" \
  -L "$A/lib" -lassimp \
  -Wl,-rpath,"$A/lib"
echo raytrace
