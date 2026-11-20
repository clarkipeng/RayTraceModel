#!/bin/sh
# Play animation frames (frames/anim_NNNN.ppm from ./raytrace 10 1 <width> <samples> <frames>) as an mp4.
# usage: ./make-mp4.sh <frames dir> <out.mp4> [fps]
set -e
"$(dirname "$0")/.deps/dl-ffmpeg/ffmpeg" -loglevel error -y -framerate "${3:-30}" -i "$1/anim_%04d.ppm" -c:v libx264 -pix_fmt yuv420p -crf 16 -movflags +faststart "$2"
echo "$2"
