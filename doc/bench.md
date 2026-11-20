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

## Instructions, exactly

The times above are too short for their clock (15 to 25 ms, with a grain
of 1 ms). `tst/count.sh` counts the instructions instead, under
callgrind: the count is the same at each run, so two builds compare to
the instruction. The start of the engine is taken out.

On 2026-11-20, with the wipe, the intermission, the finale, the flow,
and the face in Atmos (compiler `2a55288`):

| Demo | C, no rendering | Atmos, no rendering | | C, rendering | Atmos, rendering | |
|------|-----------------|---------------------|---|--------------|------------------|---|
| demo1 | 207.9 M | 263.4 M | +27% | 4753.9 M | 4828.0 M | +1.6% |
| demo2 | 144.9 M | 186.5 M | +29% | 3505.0 M | 3564.7 M | +1.7% |
| demo3 | 105.2 M | 128.3 M | +22% | 2028.4 M | 2068.9 M | +2.0% |

With no rendering, a tic of the simulation costs about 41 thousand
instructions in C, and the program adds about 11 thousand to it (demo1:
55.5 M over 5026 tics). Where they go, by callgrind:

- **The face**, about 4.9 thousand per tic: its code in Atmos, a loop
  of tests over the state of the player (each value boxed, each native
  literal a call, and the names of each step released at its end).
- **The frame**, about 5.6 thousand per frame: the broadcast of
  `:frame`, which visits each task that awaits (about 13 in a level),
  and the loop of `doom.atm`.
- **A broadcast costs about 180 instructions for each task that awaits
  another event** (measured apart, with 1, 10, and 40 tasks). The face
  first had its tic as a broadcast, at each tic: 21.6 M of demo1. It now
  goes to the task alone (`atm_emit_to`).

With rendering, all of it is less than 2%: the renderer dominates.
