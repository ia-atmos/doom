# Items of the session `doom`

Every item of this session, by its number. A number is never reused nor
changed: an item that closes, or that moves to another session, keeps
it. Inside this session an item is `#N`; from another session it is
`doom#N`. The rule is the author's, of 2026-11-18 (`ia-atmos/lang`,
`.claude/plans/TODO.md`, "Standing rules"). The numbers 1 to 51 are the
ones this session already used in its `TODO.md`. The details of the open
items are in `TODO.md`.

States: **open** (waits for the author), **work** (known, not started,
no decision needed), **done**, **closed** (settled with no change),
**note** (recorded, no action).

| # | Item | State |
|---|------|-------|
| 1 | The base of the engine: Chocolate Doom at `a2dfd51` | done 2026-11-13 |
| 2 | The engine as a copy, with a first commit untouched | done 2026-11-13 |
| 3 | The game data: the shareware `DOOM1.WAD`, fetched by a script | done 2026-11-13 |
| 4 | How far to go after the flow (D6) | done 2026-11-17: the status bar and its face first, then the messages; see #52 |
| 5 | D0: the engine, the data, the test mode, the expected outputs | done 2026-11-14 |
| 6 | D1: the seam | done 2026-11-14 |
| 7 | D2: the screen wipe | done 2026-11-14 |
| 8 | D3: the intermission | done 2026-11-14 |
| 9 | D4: the finale | done 2026-11-16 |
| 10 | D5: the flow of the game | done 2026-11-16 |
| 11 | D6: the status bar, the messages, the menus, the automap | work: the face is done; the rest listed for later (#52) |
| 12 | D7: the variant, a door and the zombieman as tasks | work: in course since 2026-11-20 |
| 13 | D8: the study and the benchmark | work |
| 14 | The local clone of the compiler has the old address as its remote | note |
| 15 | The two replacements of the plan | closed 2026-11-14 |
| 16 | A permission to build and run the engine in this session | done 2026-11-14 |
| 17 | The license of the repository | done 2026-11-16: MIT outside `engine/`, GPL-2 in it |
| 18 | A test of the finale | done 2026-11-16 |
| 19 | Relax exactness for expressiveness, after full fidelity | note: an option of the author, not a task |
| 20 | `tst/bench.sh` has a grain of 1 ms | done 2026-11-20: `tst/count.sh` (#54) |
| 21 | D0 reported | closed |
| 22 | The test mode edits nothing in the engine | closed |
| 23 | The memory of the test platform gives zeros beyond a block | closed |
| 24 | `out/doom` has no sound and no network | closed |
| 25 | `tst/level` types `iddqd` | closed |
| 26 | Two commits with their trailers in the subject | closed: left as they are |
| 27 | The build needs `ATMOS=<checkout of the compiler>` | note |
| 28 | D1 reported | closed |
| 29 | The `main` stays in the engine | closed |
| 30 | `c/` keeps in C what leaves the engine | closed |
| 31 | Two doubts on the manual of the compiler | closed 2026-11-14 |
| 32 | D2 reported | closed |
| 33 | The wipe with more than one tic per frame, not verified | open: to check with a window |
| 34 | The wipe `ColorXForm` not ported | closed |
| 35 | The status of an emit made inside a call (`a_chk_aborted`) | done at D3 |
| 36 | Saves at checkpoints only, so that the table of states could go | note: no action until the author says |
| 37 | Not verified in the intermission (other episodes, Doom 2, frags) | note: needs other WADs; the second intermission is verified (#55) |
| 38 | D3 reported | closed |
| 39 | The graphics of the intermission asked by name | closed |
| 40 | `ANIM_RANDOM` not ported | closed |
| 41 | The first migration of `pin` | done 2026-11-15 |
| 42 | Not verified in the finale (other episodes, Doom 2) | note: needs other WADs |
| 43 | D4 reported | closed |
| 44 | Not verified in the flow (Ultimate, Doom 2, the secret exit itself) | note: `idclev`, the screenshot, the way back from E1M9 verified (#55) |
| 45 | `G_DoCompleted` stays in `g_game.c` | note |
| 46 | D5 reported | closed |
| 47 | The program starts at the first call from the engine | closed |
| 48 | The second migration of `pin` | done 2026-11-17 |
| 49 | The face did not become tasks | closed: reported, in `doc/study.md` |
| 50 | Findings on `pin` go to `atmos-pin`, the rest to `atmos-lang` | note |
| 51 | With no rendering, the program is slower than C (44 against 33 ms) | done 2026-11-20: the broadcast and the face (#53, #54) |
| 52 | D6: stop after the face and go to D7, or go on with the messages | done 2026-11-20 by the session, to be confirmed: D7 |
| 53 | The tic of the face to its task alone (`atm_emit_to`) | done 2026-11-20; sent to `atmos-lang` |
| 54 | `tst/count.sh`: the instructions, counted exactly | done 2026-11-20 |
| 55 | Four new tests: `dm2`, `cheat`, `secret` (and the screenshot) | done 2026-11-20 |
