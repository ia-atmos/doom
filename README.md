# Doom, with its control in Atmos

A study over a highly specialized engine. The engine of Doom stays in C,
untouched; the control around its simulation is rewritten in
[Atmos](https://github.com/ia-atmos/lang), a language of structured
reactive concurrency.

- The simulation of a level (the objects and their table of states, the
  weapons, the doors and lifts) stays in C: a saved game covers it, so
  it must be data.
- What no save covers goes to Atmos, as tasks: the screen wipe, the
  intermission, the finale, and the flow of the game (title, demos,
  level, intermission, finale).

The plan is in [`.claude/plans/261113-doom.md`](.claude/plans/261113-doom.md).
So far the program in Atmos ([`doom.atm`](doom.atm)) has the loop of
frames, the screen wipe, and the intermission. The comparison with C is
in [`doc/study.md`](doc/study.md). The changes to the engine are in
[`doc/seam.md`](doc/seam.md).

## Build and test

It needs a C compiler, `make`, SDL2 (`libsdl2-dev`), and a checkout of
the compiler of Atmos ([ia-atmos/lang](https://github.com/ia-atmos/lang)),
already built with `make`, at the commit `5593be2` or later. `ATMOS` is its
path (default `../lang`).

```
./wad.sh                    # fetches the shareware DOOM1.WAD into wad/
make ATMOS=<path>           # builds the four programs below
make ATMOS=<path> test      # runs the tests of tst/
out/doom -iwad wad/DOOM1.WAD
```

- `out/doom` is the game, with a window. It has no sound and no network:
  SDL2_mixer and SDL2_net are left out.
- `out/doom-tst` is the game over a test platform: no window, one tic
  per frame with no real time, and input from a script. Three files of
  `tst/` take the place of three files of the engine (`i_video.c`,
  `i_timer.c`, `z_zone.c`).
- `out/doom-c` and `out/doom-c-tst` are the engine all in C, to compare:
  `c/main.c` takes the place of the program in Atmos. `make test-c` runs
  the tests on it.

## The tests

A test is a run of `out/doom-tst` that writes one line per frame, with a
checksum of the simulation and a checksum of the 320x200 screen. The
trace must be identical to the recorded one (`tst/<name>.trace`), which
comes from the engine all in C. The `tst/<name>-*.png` are screens of the
recorded runs, kept for the eye.

| Test | What it runs |
|------|--------------|
| `demo1`, `demo2`, `demo3` | The three demos of the game |
| `title` | The cycle of the title, with no input |
| `newgame` | From the title to a new game, through the menus |
| `level` | The end of E1M1, its intermission, and the start of E1M2 |
| `save` | A save, some play, a load, and the same play again |
| `skip` | The intermission of E1M1 skipped by a key |
| `dm` | A deathmatch of one player, ended by the timer: its intermission |
| `net` | A cooperative net game of one player: the end of E1M1 and its intermission |

`tst/bench.sh` measures tics per second; see [`doc/bench.md`](doc/bench.md).

## Folders

| Folder | What |
|--------|------|
| `doom.atm`, `wipe.atm`, `wi.atm` | The program in Atmos: the frames, the screen wipe, the intermission |
| `engine/` | A copy of Chocolate Doom. Its first commit has nothing changed, so `git diff 8f5ee02 -- engine/` is every change made to the engine |
| `c/` | The loop of frames in C, for the engine with no Atmos |
| `cfg/` | `config.h`, which Chocolate Doom generates with CMake |
| `tst/` | The test platform, the tests, and their expected outputs |
| `doc/` | Documents of the study |
| `wad/` | The game data, fetched by `wad.sh` and not in the repository |

## Credits and licenses

- **Doom** is by id Software (1993). Its source code was released in
  1997, and under the GNU GPL version 2 in 1999:
  [id-Software/DOOM](https://github.com/id-Software/DOOM).
- **Chocolate Doom** is by Simon Howard and its contributors (see
  [`engine/AUTHORS`](engine/AUTHORS)), under the GNU GPL version 2:
  [chocolate-doom/chocolate-doom](https://github.com/chocolate-doom/chocolate-doom).
  `engine/` is a copy of the 301 files that the build of Doom reads, at
  the commit `a2dfd51` (2026-10-24). Its license is in
  [`engine/COPYING.md`](engine/COPYING.md).
- **The game data** (`DOOM1.WAD`, the shareware episode, version 1.9) is
  by id Software. It may be distributed freely, but its license is not
  an open one, so it is not in this repository: `wad.sh` fetches it.
- The files outside `engine/` are under the license of [`LICENSE`](LICENSE)
  (MIT). A program built from them with the engine is a work derived
  from the engine, under the GNU GPL version 2.
