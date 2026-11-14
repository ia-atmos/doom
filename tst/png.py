# Converts a screen written by the test platform (PPM, binary) to PNG.
#     python3 png.py <in.ppm> <out.png>

import struct
import sys
import zlib

def chunk (kind, data):
    crc = zlib.crc32(kind + data) & 0xffffffff
    return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", crc)

src = open(sys.argv[1], "rb").read()
magic, size, depth, pixels = src.split(b"\n", 3)
assert magic == b"P6" and depth == b"255"
w, h = map(int, size.split())
rows = b"".join(b"\0" + pixels[y*w*3 : (y+1)*w*3] for y in range(h))

open(sys.argv[2], "wb").write(
    b"\x89PNG\r\n\x1a\n" +
    chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0)) +
    chunk(b"IDAT", zlib.compress(rows, 9)) +
    chunk(b"IEND", b"")
)
