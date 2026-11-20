#!/bin/sh
# Play frames as an mp4: anim_NNNN.ppm (the push-in) or frame_NNNN.ppm (rays accumulating, ./raytrace 10 <frames> <width> <samples>).
# usage: ./make-mp4.sh <frames dir> <out.mp4> [fps] [anim|frame]; the last frame is held for a second.
set -e
"$(dirname "$0")/.deps/dl-ffmpeg/ffmpeg" -loglevel error -y -framerate "${3:-30}" -i "$1/${4:-anim}_%04d.ppm" -vf tpad=stop_mode=clone:stop_duration=1 -c:v libx264 -pix_fmt yuv420p -crf 16 -movflags +faststart "$2"
echo "$2"
