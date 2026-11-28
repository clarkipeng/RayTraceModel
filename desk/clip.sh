#!/bin/sh
# desk/clip.sh [accum|bounces|all] [workdir]: the Raytracer hover clip for clarkipeng.github.io, from scratch.
#
# One camera on the backpack between the two mirrors, four stages; every stage keeps what it finished, so rerun to resume:
#   stills  2400x1600, 256 spp, at 2, 3, 5 and 9 max bounces           (bounces cut only)
#   accum   1200x800 converging from 1 to 256 spp, one frame per line of checkpoints.txt
#   orbit   46 frames swinging the camera 0 -> -45 degrees, 2400x1600 at 64 spp, rendered 4 frames per job
#   encode  accum (or the bounce stills) -> the orbit out and back -> 250 ms dissolve to the first frame (seamless loop)
#
# accum.mp4 is the site clip; bounces.mp4 is the alternate. Output: 1200x800, 30 fps, H.264 High, BT.709, no audio.
# Each render runs through ~/.agentworld/bin/heavy when it exists, so it queues behind other heavy jobs on the machine.
set -e
D=$(cd "$(dirname "$0")" && pwd); R=$D/../raytrace
mode=${1:-accum}; W=${2:-$D/../desk-out}
mkdir -p "$W/stills" "$W/accum" "$W/orbit"
[ -x "$R" ] || "$D/../build.sh"
export PATH="$D/../.deps/dl-ffmpeg:$PATH" # the vendored ffmpeg from setup-deps.sh, if any
H=; [ -x "$HOME/.agentworld/bin/heavy" ] && H=$HOME/.agentworld/bin/heavy
export HEAVY_WAIT=${HEAVY_WAIT:-36000}
export RT_THREADS=${RT_THREADS:-8} RT_ASPECT=1.5 RT_LOOKFROM="-200 -40 -190" RT_LOOKAT="0 6 25" RT_VFOV=60 RT_APERTURE=16 RT_CLAMP=10
ORBIT_FRAMES=46

stills() {
  cd "$W/stills"
  for d in 2 3 5 9; do
    [ -s depth$d.ppm ] && continue
    $H env RT_MAXDEPTH=$d "$R" 10 1 2400 256 && mv image.ppm depth$d.ppm
  done
}

accum() {
  last=$(printf 'frames/frame_%04d.ppm' $(($(wc -l < "$D/checkpoints.txt") - 1)))
  cd "$W/accum"; [ -s "$last" ] && return
  mkdir -p frames; $H env RT_MAXDEPTH=9 RT_CHECKPOINTS="$D/checkpoints.txt" "$R" 10 2 1200 256
}

orbit() {
  cd "$W/orbit"; mkdir -p frames
  a=0
  while [ $a -lt $ORBIT_FRAMES ]; do
    b=$((a + 3)); f=$a; todo=
    while [ $f -le $b ] && [ $f -lt $ORBIT_FRAMES ]; do [ -s "$(printf frames/anim_%04d.ppm $f)" ] || todo=1; f=$((f + 1)); done
    [ -n "$todo" ] && $H env RT_MAXDEPTH=9 RT_ORBIT="0 -45" RT_FRAMES="$a $b" "$R" 10 1 2400 64 $ORBIT_FRAMES
    a=$((a + 4))
  done
}

encode() { # encode accum|bounces
  ENC="-an -c:v libx264 -profile:v high -preset slow -crf 23 -maxrate 2M -bufsize 4M -colorspace bt709 -color_primaries bt709 -color_trc bt709 -color_range tv -movflags +faststart"
  S="scale=1200:800:flags=lanczos,setsar=1,fps=30,format=yuv420p"
  tmp=$(mktemp -d); i=0 # the orbit out and back: 0..45, 44..0
  for f in $(seq 0 $((ORBIT_FRAMES - 1))) $(seq $((ORBIT_FRAMES - 2)) -1 0); do
    ln -s "$W/orbit/$(printf frames/anim_%04d.ppm $f)" "$tmp/$(printf s_%04d.ppm $i)"; i=$((i + 1))
  done
  orbit=$(echo "$i / 30" | bc -l)
  if [ "$1" = accum ]; then # 90 converge frames (3 s); the orbit dissolves in over their last 250 ms
    first=$W/accum/frames/frame_0000.ppm; start=2.75
    IN="-framerate 30 -i $W/accum/frames/frame_%04d.ppm"
    CHAIN="[0]$S[a];[1]$S[o];[2]$S[z];[a][o]xfade=fade:duration=0.25:offset=2.75[ao]"
  else # one second per bounce count, 250 ms dissolves, then the orbit
    first=$W/stills/depth2.ppm; start=3; IN=
    for d in 2 3 5 9; do IN="$IN -loop 1 -framerate 30 -t 1 -i $W/stills/depth$d.ppm"; done
    CHAIN="[0]$S[a];[1]$S[b];[2]$S[c];[3]$S[d];[4]$S[o];[5]$S[z];[a][b]xfade=fade:duration=0.25:offset=0.75[ab];[ab][c]xfade=fade:duration=0.25:offset=1.5[abc];[abc][d]xfade=fade:duration=0.25:offset=2.25[abcd];[abcd][o]xfade=fade:duration=0.25:offset=3[ao]"
  fi
  ffmpeg -v error -y $IN -framerate 30 -i "$tmp/s_%04d.ppm" -loop 1 -framerate 30 -t 0.3 -i "$first" \
    -filter_complex "$CHAIN;[ao][z]xfade=fade:duration=0.25:offset=$(echo "$start + $orbit - 0.25" | bc -l),format=yuv420p[v]" \
    -map "[v]" $ENC "$W/$1.mp4"
  rm -rf "$tmp"; echo "$W/$1.mp4"
}

case $mode in
  accum)   (accum); (orbit); encode accum ;;
  bounces) (stills); (orbit); encode bounces ;;
  all)     (accum); (stills); (orbit); encode accum; encode bounces ;;
  *) echo "usage: desk/clip.sh [accum|bounces|all] [workdir]" >&2; exit 2 ;;
esac
