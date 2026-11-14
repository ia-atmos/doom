#!/bin/sh
# Measures the speed of the engine on the three demos: tics per second,
# with no rendering (-nodraw) and with it. The time of a run that ends
# at its first frame is taken out, as the time to start.
#
#     tst/bench.sh [<runs>]     the best of <runs> (default 5)
#
# Sound is off and there is no window in both cases (out/doom-tst).
# With no rendering, the run is a -timedemo: the engine only takes
# -nodraw there, and ends it with an error message, which is ignored.

TST=$(cd "$(dirname "$0")" && pwd)
ROOT=$(dirname "$TST")
BIN=${BIN:-$ROOT/out/doom-tst}
WAD=$ROOT/wad/DOOM1.WAD
RUNS=${1:-5}

DIR=$(mktemp -d)
trap 'rm -rf "$DIR"' EXIT
echo "show_endoom 0" > "$DIR/extra.cfg"
echo "0 quit" > "$DIR/quit"

# the best time of $RUNS runs, in ms
best () {
    BEST=
    for i in $(seq "$RUNS"); do
        T0=$(date +%s%N)
        ( cd "$DIR" && HOME=$DIR "$BIN" -iwad "$WAD" -nosound \
            -config "$DIR/default.cfg" -extraconfig "$DIR/extra.cfg" \
            -savedir "$DIR/" "$@" > /dev/null 2>&1 )
        T1=$(date +%s%N)
        T=$(( (T1 - T0) / 1000000 ))
        if [ -z "$BEST" ] || [ "$T" -lt "$BEST" ]; then
            BEST=$T
        fi
    done
    echo "$BEST"
}

START=$(best -tst-script "$DIR/quit")
echo "start: $START ms"

for demo in demo1 demo2 demo3; do
    TICS=$(tail -1 "$TST/$demo.trace" | cut -d' ' -f2)
    NODRAW=$(best -timedemo $demo -nodraw -nogui)
    DRAW=$(best -playdemo $demo)
    echo "$demo: $TICS tics;" \
         "no rendering: $((NODRAW - START)) ms," \
         "$(( TICS * 1000 / (NODRAW - START) )) tics/s;" \
         "rendering: $((DRAW - START)) ms," \
         "$(( TICS * 1000 / (DRAW - START) )) tics/s"
done
