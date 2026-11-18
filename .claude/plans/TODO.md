# TODO

Everything that is pending in the session `doom`: asked and not
decided, known and not done, or reported and not acknowledged. Keep it
current: remove an item when it is settled, add one whenever something
is left open.

The numbers do not change. Numbers that are missing were settled.

Last update: 2026-11-16.

## A. Decisions waiting for an answer

Each has the option recommended by this session.

| # | Decision | Recommended | State |
|---|----------|-------------|-------|

## B. Work known and not done

| # | Work | Note |
|---|------|------|
| 35 | When the engine first emits to the program from inside a call (`:tic`, D3 on): the C function returns the status of its emits, and the literal takes it as `st = a_chk_aborted(f());`, since the emit may abort the task that made the call | From the compiler session, 2026-11-14 (`ia-atmos/lang@dad1538`, manual "Native Literals", `tst/cases/native-10.atm`). Update the compiler checkout first |
| 11 | D6: the messages (`hu_stuff.c`) next; the menus and the automap last, or not at all | The face is done |
| 12 | D7: the variant: a door and the zombieman as tasks | |
| 13 | D8: the study and the benchmark | |
| 14 | The local clone of the compiler still has `fsantanna/ceu-atmos` as its remote | The change of the remote was denied by the permission classifier of the session. Harmless: the old address redirects, and fetches name `ia-atmos/lang` explicitly |
| 27 | The build needs `ATMOS=<checkout of ia-atmos/lang>`; the default is `../lang` | To revisit if the compiler gets an installed form |
| 19 | An option, not a task: once a part in Atmos is fully faithful (its screens identical), it may relax exactness in favor of expressiveness, as long as the intent of the original author is kept. Not for the simulation, on which the demos and the saved games depend | A rule of the author for all ports, relayed by the compiler session on 2026-11-14. Its recommendations: tag the faithful version first; one small commit per relaxation, saying what it gives up; a document with the deliberate differences |
| 36 | A possibility for after the faithful port, to take no action on until the author says: if the game is saved only at checkpoints, the state in the middle of a level never has to be data, so the table of states could go, and monsters, doors, and weapons could be tasks. It costs the saved games of the original and a save at any point; each task must be checked against the demos again | From the session `snapshots` (`session_011eECoiWKpJwsuKH43TLunQ`), 2026-11-14, with the words of the author as it quoted them: "se colocarmos essa restrição após o porte fiel, podemos eliminar a tabela de estados e usar Atmos mais vezes". Not checked in its transcript. Related: items 64 and 45 of the TODO of `ia-atmos/lang` |
| 37 | Not verified in the intermission: the map of the episodes 2 and 3 (animations `:level`), Doom 2, frags above zero, more than one player, a second intermission in one game | Needs the registered or the Doom 2 WAD, which the session does not have; the second intermission needs a route through E1M2 |
| 42 | Not verified in the finale, ported by reading and never run: the episodes 2, 3 (the scroll and "the end"), and 4; Doom 2 (the text skipped by a button, the cast). And the finale reached by playing to the end of E1M8, in the place of the timer of a deathmatch | Needs the registered or the Doom 2 WAD; the played way needs an autopilot that kills the two barons |
| 44 | Not verified in the flow: the cycle of the Ultimate Doom (`demo4`) and of Doom 2, a new game by `idclev`, the secret exit, the finale between the levels of Doom 2, a screenshot | The first and the fourth need other WADs |
| 45 | `G_DoCompleted` stays in `g_game.c` (it decides the next level and the victory): it reads static tables of par times. Moving it means copying the tables, or editing the engine more | To decide if the study needs the decisions in Atmos |
| 50 | Where to send findings: about `pin`, to the session `atmos-pin` (`session_011eECogSDhkVfyt7qme5DLh`); anything else of the compiler, to `atmos-lang` (`session_011eECogKVQ7ZzmWCoMST2wK`, formerly `ceu-atmos`) | Decided by the author on 2026-11-18, relayed by `atmos-lang` |
| 51 | With no rendering, the program is now slower than the engine all in C: 44 against 33 ms on demo1 (2026-11-18, the same machine and hour), where at D1 they were equal. The likely cause is the emit of `:st.face` at each tic (the face, D6), which traverses all the tasks; not measured apart. The compiler at `7480111` changed nothing (44 ms before and after) | To measure at D8, with the demos run several times in one process (item 20) |
| 20 | `tst/bench.sh` has a grain of 1 ms over runs of 15 to 25 ms | Run the demos several times in one process, before D8 compares builds |

## C. Reported or decided, and not acknowledged

| # | What | Note |
|---|------|------|
| 49 | The face of the status bar is in `st.atm` (2026-11-18), and did not become tasks: it is an arbiter of priorities checked at each tic, and an expired face keeps its priority for one more tic. Six variables of `st_stuff.c` lost their `static`. Thirteen tests identical | Reported to the author in this session |
| 46 | D5 is done (2026-11-16): the flow is in `flow.atm` (the title, the demos, the levels, as tasks; the actions of `gameaction` as events); thirteen tests are identical | Reported to the author in this session |
| 47 | The program starts at the first call from the engine (`doom_start`), which comes before `D_DoomFrames`, and tells a game started by `-warp` by `viewactive` | Decided by this session |
| 43 | D4 is done (2026-11-16): the finale is in `fi.atm`; eleven tests are identical. The test of the finale ends E1M8 by the timer of a deathmatch, and not by playing (it was item 18) | Reported to the author in this session |
| 38 | D3 is done (2026-11-14): the intermission is in `wi.atm` (one player, deathmatch, cooperative, and the map); ten tests are identical; `doc/study.md` has the first comparison | Reported to the author in this session |
| 39 | In `wi.atm`, the graphics are asked to the WAD by name at each draw, and not loaded once as in C: 150 lines less, and nothing changes on screen | Decided by this session |
| 40 | `ANIM_RANDOM` of `wi_stuff.c` was not ported: no animation of the game is of that kind | Decided by this session |
| 32 | D2 is done (2026-11-14): the frame and the screen wipe are in `doom.atm`, each column of the melt as a task; the seven tests are identical | Reported to the author in this session |
| 33 | In the wipe, not verified: more than one tic of the clock per frame (the test platform always gives one). To see it, the game must be played with a window | To check by the author, or with a test clock that skips tics |
| 34 | The wipe `ColorXForm` of `f_wipe.c` was not ported: the game never uses it | Decided by this session |
| 28 | D1 is done (2026-11-14): the program in Atmos runs the frames, with two lines changed in the engine (`doc/seam.md`); the seven tests are identical with and without Atmos | Reported to the author in this session |
| 29 | The `main` stays in the engine, against the plan: `D_DoomMain` cannot return (a local variable of it holds the name of the demo), so the engine calls the program where it had its loop | Decided by this session, after the three demos failed the other way |
| 30 | `c/` keeps in C what leaves the engine, so that the engine with no Atmos still builds (`out/doom-c`, `make test-c`): the baseline of the benchmark, and the C side of the study | Decided by this session |
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
- 31: the compiler session answered the two doubts on the manual (see B35).
- 8: D3 is done.
- 9 and 18: D4 is done, with its test.

On 2026-11-16, the author said to follow the recommendations of this
session:

- 17: the license stays as it is. `LICENSE` (MIT) is for the files
  outside `engine/`, which is GPL-2; a program built with the engine is
  under the GPL-2. The readme says so.
- 10: D5 is done.

On 2026-11-17, through the compiler session: the author approved the
removal of the workarounds of `pin` (the constructor of `WI` stays),
and declared the rules of `pin` stable until the next round.

- 41, 48: the two migrations of `pin` are done and acknowledged.
- 4: on 2026-11-17 the author said to go on with D6: the status bar
  and its face first, then the messages.
