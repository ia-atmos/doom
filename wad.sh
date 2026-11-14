#!/bin/sh
# Fetches the shareware DOOM1.WAD (version 1.9) into wad/DOOM1.WAD.
#
# The file is not in the repository: id Software allows its free
# distribution, but its license is not an open one.
#
# Sources, tried in order: the package doom-wad-shareware of Ubuntu and
# of Debian (a .deb with the file inside), then the file itself.
# Whatever the source, the result must have the SHA-1 below.

set -e

SHA1=5b2e249b9c5133ec987b3ea77596381dc0d6bc1d
DIR=$(dirname "$0")/wad
OUT=$DIR/DOOM1.WAD

ok () {
    [ -f "$1" ] && [ "$(sha1sum "$1" | cut -d' ' -f1)" = "$SHA1" ]
}

if ok "$OUT"; then
    echo "$OUT: already there"
    exit 0
fi

mkdir -p "$DIR"
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

DEB=pool/multiverse/d/doom-wad-shareware/doom-wad-shareware_1.9.fixed-4_all.deb
for url in \
    http://archive.ubuntu.com/ubuntu/$DEB \
    http://deb.debian.org/debian/pool/non-free/d/doom-wad-shareware/doom-wad-shareware_1.9.fixed-4_all.deb \
    https://distro.ibiblio.org/slitaz/sources/packages/d/doom1.wad
do
    echo "trying $url"
    rm -rf "$TMP"/*
    curl -fsSL --max-time 120 -o "$TMP/file" "$url" || continue
    case $url in
        *.deb)
            dpkg-deb -x "$TMP/file" "$TMP/deb" || continue
            f=$(find "$TMP/deb" -iname 'doom1.wad' | head -1)
            ;;
        *)
            f=$TMP/file
            ;;
    esac
    if ok "$f"; then
        cp "$f" "$OUT"
        echo "$OUT: fetched"
        exit 0
    fi
    echo "  wrong checksum"
done

echo "DOOM1.WAD: no source could be reached" >&2
exit 1
