#!/bin/sh
# Turn progressive frames into a GIF that shows the image converging.
# usage: ./make-gif.sh <frames dir> <out.gif> [width]
# The renderer already saves frames at log-spaced sample counts; this plays them in order, then holds the final image.
set -e
dir=$1; out=$2; width=${3:-400}
n=$(ls "$dir"/frame_*.ppm | wc -l | tr -d ' ')
tmp=$(mktemp -d)
seq 0 $((n-1)) > "$tmp/pick"
i=0
for p in $(cat "$tmp/pick"); do cp "$(printf '%s/frame_%04d.ppm' "$dir" "$p")" "$(printf '%s/g_%04d.ppm' "$tmp" "$i")"; i=$((i+1)); done
last=$(printf '%s/g_%04d.ppm' "$tmp" $((i-1)))
for k in $(seq 1 20); do cp "$last" "$(printf '%s/g_%04d.ppm' "$tmp" "$i")"; i=$((i+1)); done
"$(dirname "$0")/.deps/dl-ffmpeg/ffmpeg" -loglevel error -y -framerate 12 -i "$tmp/g_%04d.ppm" -vf "scale=$width:-1:flags=lanczos,split[a][b];[a]palettegen=max_colors=256:stats_mode=full[p];[b][p]paletteuse=dither=sierra2_4a" -loop 0 "$out"
rm -rf "$tmp"
echo "$out: $i frames"
