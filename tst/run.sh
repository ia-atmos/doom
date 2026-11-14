#!/bin/sh
# Runs the tests of tst/ on out/doom-tst and compares each trace with
# its expected output.
#
#     tst/run.sh              runs all and compares
#     tst/run.sh <name>...    only these
#     tst/run.sh -r [<name>]  records the expected outputs (and the shots)
#
# A test <name> is:
#
#     tst/<name>.args      the arguments of the engine (one line)
#     tst/<name>.script    the input (optional; see tst/i_video.c)
#     tst/<name>.trace     the expected trace, one line per frame
#     tst/<name>-*.png     the screens that the script asks for with
#                          `shot`, kept for the eye and not compared
#
# Each run has its own empty directory for the configuration and the
# saved games, so that nothing of the machine reaches it.

TST=$(cd "$(dirname "$0")" && pwd)
ROOT=$(dirname "$TST")
BIN=${BIN:-$ROOT/out/doom-tst}
WAD=$ROOT/wad/DOOM1.WAD

RECORD=0
if [ "$1" = "-r" ]; then
    RECORD=1
    shift
fi

[ -x "$BIN" ] || { echo "$BIN: not built (make)"; exit 1; }
[ -f "$WAD" ] || { echo "$WAD: not there (./wad.sh)"; exit 1; }

if [ $# -eq 0 ]; then
    set -- $(cd "$TST" && ls *.args | sed 's/\.args$//')
fi

FAIL=0
for name in "$@"; do
    DIR=$(mktemp -d)
    echo "show_endoom 0" > "$DIR/extra.cfg"

    SCRIPT=
    [ -f "$TST/$name.script" ] && SCRIPT="-tst-script $TST/$name.script"

    (
        cd "$DIR" &&
        HOME=$DIR "$BIN" -iwad "$WAD" -nosound \
            -config "$DIR/default.cfg" -extraconfig "$DIR/extra.cfg" \
            -savedir "$DIR/" \
            $(cat "$TST/$name.args") $SCRIPT -tst-trace "$DIR/trace" \
            > "$DIR/out" 2>&1
    )
    RC=$?

    if [ $RC -ne 0 ] || [ ! -s "$DIR/trace" ]; then
        echo "FAIL  $name: the engine ended with $RC"
        tail -3 "$DIR/out"
        FAIL=1
    elif [ $RECORD -eq 1 ]; then
        cp "$DIR/trace" "$TST/$name.trace"
        for ppm in "$DIR"/*.ppm; do
            [ -f "$ppm" ] || continue
            python3 "$TST/png.py" "$ppm" "$TST/$name-$(basename "$ppm" .ppm).png"
        done
        echo "REC   $name: $(wc -l < "$DIR/trace") frames"
    elif cmp -s "$DIR/trace" "$TST/$name.trace"; then
        echo "ok    $name: $(wc -l < "$DIR/trace") frames"
    else
        echo "FAIL  $name: first difference (expected, then got):"
        diff "$TST/$name.trace" "$DIR/trace" | sed -n '2p;/^>/{p;q}'
        FAIL=1
    fi

    rm -rf "$DIR"
done

exit $FAIL
