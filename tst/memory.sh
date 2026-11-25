#!/bin/sh
# Measures the memory of the engine on a demo (demo1, or DEMO), with no
# rendering: the peak of the heap, by massif (valgrind). The test
# platform allocates each block of the engine apart (tst/z_zone.c), so
# the heap is what the engine uses, and not the zone of a fixed size of
# the game.
#
#     tst/memory.sh                 out/doom-tst
#     BIN=out/doom-c-tst tst/memory.sh

TST=$(cd "$(dirname "$0")" && pwd)
ROOT=$(dirname "$TST")
BIN=${BIN:-$ROOT/out/doom-tst}
WAD=$ROOT/wad/DOOM1.WAD
DEMO=${DEMO:-demo1}

DIR=$(mktemp -d)
trap 'rm -rf "$DIR"' EXIT
echo "show_endoom 0" > "$DIR/extra.cfg"

ARGS="-iwad $WAD -nosound -config $DIR/default.cfg -extraconfig $DIR/extra.cfg -savedir $DIR/ -timedemo $DEMO -nodraw -nogui"

# the peak of the heap
( cd "$DIR" && HOME=$DIR valgrind --tool=massif --massif-out-file="$DIR/ms" "$BIN" $ARGS > /dev/null 2>&1 )
HEAP=$(grep "^mem_heap_B=" "$DIR/ms" | cut -d= -f2 | sort -n | tail -1)

echo "$DEMO: heap peak: $((HEAP / 1024)) KiB"
