# TODO

Everything that is pending in the session `doom`: asked and not
decided, known and not done, or reported and not acknowledged. Keep it
current: remove an item when it is settled, add one whenever something
is left open.

The numbers do not change. Numbers that are missing were settled.

Last update: 2026-11-14.

## A. Decisions waiting for an answer

Each has the option recommended by this session.

| # | Decision | Recommended | State |
|---|----------|-------------|-------|
| 17 | The license of the repository. `LICENSE` is MIT, from the creation of the repository; `engine/` is GPL-2, and a program built with it is under the GPL-2. MIT is compatible, so nothing is wrong today | Keep MIT for the files outside `engine/`, as the readme says; or change `LICENSE` to GPL-2, to have one license | Not asked yet |
| 18 | The finale has no test: it needs the end of E1M8 (two barons). A scripted run with `iddqd` and an autopilot, as the one of `tst/level`, should reach it | Write it at D4, when the finale is ported | Not asked yet |
| 4 | How far to go after the flow: the status bar and its face, the messages, the menus, the automap | To decide when D2 to D5 are done | Not decided now, by the author |

## B. Work known and not done

| # | Work | Note |
|---|------|------|
| 8 | D3: the intermission | Next |
| 9 | D4: the finale | See A18 |
| 10 | D5: the flow of the game, with the title cycle, the demos, and save and load | |
| 11 | D6: the status bar, the messages, the menus, the automap | Waits for A4 |
| 12 | D7: the variant: a door and the zombieman as tasks | |
| 13 | D8: the study and the benchmark | |
| 14 | The local clone of the compiler still has `fsantanna/ceu-atmos` as its remote | The change of the remote was denied by the permission classifier of the session. Harmless: the old address redirects, and fetches name `ia-atmos/lang` explicitly |
| 27 | The build needs `ATMOS=<checkout of ia-atmos/lang>`; the default is `../lang` | To revisit if the compiler gets an installed form |
| 19 | An option, not a task: once a part in Atmos is fully faithful (its screens identical), it may relax exactness in favor of expressiveness, as long as the intent of the original author is kept. Not for the simulation, on which the demos and the saved games depend | A rule of the author for all ports, relayed by the compiler session on 2026-11-14. Its recommendations: tag the faithful version first; one small commit per relaxation, saying what it gives up; a document with the deliberate differences |
| 20 | `tst/bench.sh` has a grain of 1 ms over runs of 15 to 25 ms | Run the demos several times in one process, before D8 compares builds |

## C. Reported or decided, and not acknowledged

| # | What | Note |
|---|------|------|
| 32 | D2 is done (2026-11-14): the frame and the screen wipe are in `doom.atm`, each column of the melt as a task; the seven tests are identical | Reported to the author in this session |
| 33 | In the wipe, not verified: more than one tic of the clock per frame (the test platform always gives one). To see it, the game must be played with a window | To check by the author, or with a test clock that skips tics |
| 34 | The wipe `ColorXForm` of `f_wipe.c` was not ported: the game never uses it | Decided by this session |
| 28 | D1 is done (2026-11-14): the program in Atmos runs the frames, with two lines changed in the engine (`doc/seam.md`); the seven tests are identical with and without Atmos | Reported to the author in this session |
| 29 | The `main` stays in the engine, against the plan: `D_DoomMain` cannot return (a local variable of it holds the name of the demo), so the engine calls the program where it had its loop | Decided by this session, after the three demos failed the other way |
| 30 | `c/` keeps in C what leaves the engine, so that the engine with no Atmos still builds (`out/doom-c`, `make test-c`): the baseline of the benchmark, and the C side of the study | Decided by this session |
| 31 | Sent to the compiler session on 2026-11-14: an emit from C inside a call made by the program works; and two doubts on the manual (the status of a nested `atm_emit`, where `atmos -c` writes) | No reply needed, none yet |
| 21 | D0 is done (2026-11-14): the engine builds from the copy, the WAD is fetched from the package of Ubuntu, the test platform, seven expected outputs, the baseline of speed | Reported to the author in this session |
| 22 | The test mode edits nothing in the engine: `tst/` has its own `i_video.c`, `i_timer.c`, and `z_zone.c`. The plan had it as part of the seam | Decided by this session |
| 23 | The memory of the test platform is not the zone of the engine: reads beyond a block give zeros. Otherwise two pixels of demo1 changed at each run (the engine reads beyond the end of sprite columns, as the original) | Decided by this session. The game (`out/doom`) keeps the zone |
| 24 | `out/doom` has no sound and no network (no SDL2_mixer, no SDL2_net) | Decided by this session, to keep the dependencies to SDL2 alone |
| 25 | `tst/level` types `iddqd`: on the way to the exit of E1M1, the monsters killed the player of the autopilot | Decided by this session |
| 26 | Two commits (`1acdd84`, `fec093a`) have their trailers in the subject paragraph, by a mistake of this session in the command | Left as they are: fixing them would rewrite `main` |

## D. Older items

None.

## Settled, for the record

Answered by the author through the compiler session on 2026-11-13
("Answer doom", checked in its transcript):

- 1: the base is Chocolate Doom at `a2dfd51`; `linuxdoom-1.10` only
  as a reference for reading.
- 2: the engine is a copy of what the build needs, in one first
  commit with nothing changed.
- 3: the shareware `DOOM1.WAD` (version 1.9), fetched by a script and
  not committed. If no source is reachable, say so with the hosts
  tried; no Freedoom in its place.
- 4: how far to go after the flow is not decided now; ask again when
  the wipe, the intermission, the finale, and the flow are done.

On 2026-11-14:

- 5: D0 is done.
- 15: the two replacements of the plan (superseded by the work on it).
- 16: the author allowed the build and the run of the engine in this
  session ("go ahead").
- 6: D1 is done.
- 7: D2 is done.
