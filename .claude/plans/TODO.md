# TODO

Everything that is pending in the session `doom`: asked and not
decided, known and not done, or reported and not acknowledged. Keep it
current: remove an item when it is settled, add one whenever something
is left open.

The numbers are the items of this session, `#N` (from another session,
`doom#N`): unique, in order, never reused nor changed. Their index, with
the state of each, is `ISSUES.md`. Numbers that are missing here were
settled. A rule of the author of 2026-11-18: everything put to him has a
number, and nothing else is enumerated with bare numbers.

Last update: 2026-11-20.

## A. Decisions waiting for an answer

Each has the option recommended by this session.

| # | Decision | Recommended | State |
|---|----------|-------------|-------|

## B. Work known and not done

| # | Work | Note |
|---|------|------|
| 11 | D6: the messages (`hu_stuff.c`), the menus, and the automap: listed for later | Stopped after the face, by #52 |
| 13 | D8: the study and the benchmark | Next. `doc/study.md` and `doc/bench.md` have each part as it was done; D8 joins them, with the border |
| 14 | The local clone of the compiler still has `fsantanna/ceu-atmos` as its remote | The change of the remote was denied by the permission classifier of the session. Harmless: the old address redirects, and fetches name `ia-atmos/lang` explicitly |
| 27 | The build needs `ATMOS=<checkout of ia-atmos/lang>`; the default is `../lang` | To revisit if the compiler gets an installed form |
| 19 | An option, not a task: once a part in Atmos is fully faithful (its screens identical), it may relax exactness in favor of expressiveness, as long as the intent of the original author is kept. Not for the simulation, on which the demos and the saved games depend | A rule of the author for all ports, relayed by the compiler session on 2026-11-14. Its recommendations: tag the faithful version first; one small commit per relaxation, saying what it gives up; a document with the deliberate differences |
| 36 | A possibility for after the faithful port, to take no action on until the author says: if the game is saved only at checkpoints, the state in the middle of a level never has to be data, so the table of states could go, and monsters, doors, and weapons could be tasks. It costs the saved games of the original and a save at any point; each task must be checked against the demos again | From the session `snapshots` (`session_011eECoiWKpJwsuKH43TLunQ`), 2026-11-14, with the words of the author as it quoted them: "se colocarmos essa restrição após o porte fiel, podemos eliminar a tabela de estados e usar Atmos mais vezes". Not checked in its transcript. Related: items 64 and 45 of the TODO of `ia-atmos/lang` |
| 37 | Not verified in the intermission: the map of the episodes 2 and 3 (animations `:level`), Doom 2, frags above zero, more than one player. The second intermission in one game is now verified (`dm2`, 2026-11-20) | Needs the registered or the Doom 2 WAD, which the session does not have; the second intermission needs a route through E1M2 |
| 42 | Not verified in the finale, ported by reading and never run: the episodes 2, 3 (the scroll and "the end"), and 4; Doom 2 (the text skipped by a button, the cast). And the finale reached by playing to the end of E1M8, in the place of the timer of a deathmatch | Needs the registered or the Doom 2 WAD; the played way needs an autopilot that kills the two barons |
| 44 | Not verified in the flow: the cycle of the Ultimate Doom (`demo4`) and of Doom 2, the secret exit itself (a switch in E1M3), the finale between the levels of Doom 2. Verified on 2026-11-20: `idclev`, a screenshot (`cheat`), the way back from the secret level (`secret`) | The first and the fourth need other WADs |
| 45 | `G_DoCompleted` stays in `g_game.c` (it decides the next level and the victory): it reads static tables of par times. Moving it means copying the tables, or editing the engine more | To decide if the study needs the decisions in Atmos |
| 50 | Where to send: coordination, status, and questions for the author, to the session `atmos` (`session_011eECohhJbc4GMpAj3yyeEo`, since 2026-11-24); measurements on the ports and changes of the compiler, to `atmos-lang` (`session_011eECogKVQ7ZzmWCoMST2wK`); `pin`, to `atmos-pin` (`session_011eECogSDhkVfyt7qme5DLh`). A pause until the next credits: no new work, only what arrives | Relayed by `atmos` on 2026-11-24 (atmos-lang#119) |

## C. Reported or decided, and not acknowledged

| # | What | Note |
|---|------|------|
| 56 | D7 is done (2026-11-20): the variant `var.atm`, with the normal door and the zombieman as tasks; fifteen tests identical to the main build; it costs 2.7 to 3.2 times the instructions with no rendering, 9 to 14% more with it | By the session, after #52; in `doc/study.md` and `doc/seam.md` |
| 57 | In the variant, the jump of a zombieman's own action (A_Look, A_Chase) is kept and taken when the action returns, so the action of the new row runs after the old action returns, where C ran it inside. Nothing between them reads it, and the demos agree; it is a difference of order that a future action could expose | By the session |
| 58 | Not reached in the variant: the resurrection of a zombieman (no arch-vile in the shareware episode), and the jump to chase that damage gives to a standing zombieman; a zombieman respawned in Nightmare stays a C object | Note |
| 52 | D6 stops after the face, and D7 starts; the messages, the menus, and the automap stay listed for later (#11) | By the session, to be confirmed: the author's rule "work with no input" of 2026-11-20, relayed by `atmos-lang` and checked in its transcript, with the recommendation of this session |
| 53 | The tic of the face goes to its task alone (`atm_emit_to`): a broadcast costs about 180 instructions for each task that awaits another event, and the face had one at each tic | By the session. Sent to `atmos-lang` with the smallest program (2026-11-20) |
| 54 | `tst/count.sh` counts the instructions exactly (callgrind), in the place of the times of `tst/bench.sh` for comparisons (#20): the program costs 22 to 29% more than C with no rendering, under 2% with it (#51) | By the session, in `doc/bench.md` |
| 55 | Three new tests, recorded on C: `dm2` (two intermissions), `cheat` (a screenshot with `-devparm`, and `idclev13`), `secret` (the way back from E1M9); the sixteen are identical | By the session (#37, #44) |
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

On 2026-11-18:

- 35: done at D3 (`TryRunTics(); st = a_chk_aborted(DOOM_ST);`).

On 2026-11-20, by the rule "work with no input" of the author:

- 20: `tst/count.sh` (#54).
- 51: measured and named (#53, #54).
- 52: settled by the session, to be confirmed (see C).

On 2026-11-22, the author acknowledged the reports ("Ack"): the items of
section C are read, and the work goes on later (D8, and #33 with him).
