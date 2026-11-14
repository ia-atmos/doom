# Benchmark

Tics per second of the simulation, on the three demos of the shareware
`DOOM1.WAD`. A tic is 1/35 s of game time, so the game needs 35 tics
per second.

Measured by `tst/bench.sh` on `out/doom-tst` (no window, no sound): the
best of 7 runs, with the time to start (7 ms) taken out.

## The engine all in C

The baseline (`out/doom-c-tst`), measured before the seam.

| Demo | Tics | No rendering | Rendering |
|------|------|--------------|-----------|
| demo1 (E1M5) | 5026 | 24 ms, 209 thousand tics/s | 306 ms, 16 thousand tics/s |
| demo2 (E1M3) | 3836 | 19 ms, 202 thousand tics/s | 261 ms, 15 thousand tics/s |
| demo3 (E1M7) | 2134 | 15 ms, 142 thousand tics/s | 148 ms, 14 thousand tics/s |

- Machine: Intel Xeon at 2.10 GHz (a cloud container), gcc 13.3, `-O2`.
- The runs with no rendering take 15 to 25 ms, and the clock of the
  script has a grain of 1 ms: read these numbers with an error of
  about 5%. For a finer comparison, the demos must run several times
  in one process.
- "Rendering" draws the 320x200 buffer of each tic and shows it
  nowhere.

## The seam, with nothing ported

The program in Atmos runs the frames (`:frame`, then `D_RunFrame`). In
the same session, 9 runs each:

| Demo | The engine all in C | The program in Atmos |
|------|---------------------|----------------------|
| demo1, no rendering | 22 ms | 25 ms |
| demo2, no rendering | 21 ms | 20 ms |
| demo3, no rendering | 13 ms | 14 ms |
| demo1, rendering | 295 ms | 303 ms |
| demo2, rendering | 222 ms | 237 ms |
| demo3, rendering | 132 ms | 134 ms |

No difference above the error of the measure. These runs have one tic
per frame, so the program takes one emit per tic: 5026 emits in demo1,
which do not show in 22 ms.
