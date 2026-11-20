#!/bin/sh
# Counts the instructions that the engine runs on the three demos, with
# no rendering and with it, under callgrind (valgrind). Unlike the time
# of tst/bench.sh, the count is exact and the same at each run, so two
# builds can be compared to the instruction.
#
#     tst/count.sh                  out/doom-tst
#     BIN=out/doom-c-tst tst/count.sh
#
# The start (a run that ends at its first frame) is taken out.

TST=$(cd "$(dirname "$0")" && pwd)
ROOT=$(dirname "$TST")
BIN=${BIN:-$ROOT/out/doom-tst}
WAD=$ROOT/wad/DOOM1.WAD

DIR=$(mktemp -d)
trap 'rm -rf "$DIR"' EXIT
echo "show_endoom 0" > "$DIR/extra.cfg"
echo "0 quit" > "$DIR/quit"

count () {
    ( cd "$DIR" && HOME=$DIR valgrind --tool=callgrind \
        --callgrind-out-file="$DIR/cg" "$BIN" -iwad "$WAD" -nosound \
        -config "$DIR/default.cfg" -extraconfig "$DIR/extra.cfg" \
        -savedir "$DIR/" "$@" 2>&1 ) | sed -n 's/.*Collected : //p'
}

START=$(count -tst-script "$DIR/quit")
echo "start: $START"
for demo in demo1 demo2 demo3; do
    NODRAW=$(count -timedemo $demo -nodraw -nogui)
    DRAW=$(count -playdemo $demo)
    echo "$demo: no rendering: $((NODRAW - START)); rendering: $((DRAW - START))"
done
