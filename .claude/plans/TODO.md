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

Last update: 2026-11-26.

## A. Decisions waiting for an answer

Each has the option recommended by this session.

| # | Decision | Recommended | State |
|---|----------|-------------|-------|
| 73 | The general instruction of the author for the ports (2026-11-26, relayed by `atmos` from `pico-1oom#29`): fidelity is the behavior observed while running, not the shape of the code nor the formats of files; idiomatic Atmos; own files in the forms of Atmos; no bugs of safety. Answer given in this session (how much is followed, what to redo, a grade of 5). To decide: the redo of #74 | Redo `menu.atm`, `hu.atm`, `am.atm` as Atmos (state in tables, questions as awaits, menus as data with lambdas); fix the reads out of bounds of the renderer in the copy and record them | asked |

## B. Work known and not done

| # | Work | Note |
|---|------|------|
| 14 | The local clone of the compiler still has `fsantanna/ceu-atmos` as its remote | The change of the remote was denied by the permission classifier of the session. Harmless: the old address redirects, and fetches name `ia-atmos/lang` explicitly |
| 27 | The build needs `ATMOS=<checkout of ia-atmos/lang>`; the default is `../lang` | To revisit if the compiler gets an installed form |
| 19 | An option, not a task: once a part in Atmos is fully faithful (its screens identical), it may relax exactness in favor of expressiveness, as long as the intent of the original author is kept. Not for the simulation, on which the demos and the saved games depend | A rule of the author for all ports, relayed by the compiler session on 2026-11-14. Its recommendations: tag the faithful version first; one small commit per relaxation, saying what it gives up; a document with the deliberate differences |
| 36 | A possibility for after the faithful port, to take no action on until the author says: if the game is saved only at checkpoints, the state in the middle of a level never has to be data, so the table of states could go, and monsters, doors, and weapons could be tasks. It costs the saved games of the original and a save at any point; each task must be checked against the demos again | From the session `snapshots` (`session_011eECoiWKpJwsuKH43TLunQ`), 2026-11-14, with the words of the author as it quoted them: "se colocarmos essa restrição após o porte fiel, podemos eliminar a tabela de estados e usar Atmos mais vezes". Not checked in its transcript. Related: items 64 and 45 of the TODO of `ia-atmos/lang` |
| 37 | Not verified in the intermission: the map of the episodes 2 and 3 (animations `:level`), Doom 2, frags above zero, more than one player. The second intermission in one game is now verified (`dm2`, 2026-11-20) | Needs the registered or the Doom 2 WAD, which the session does not have; the second intermission needs a route through E1M2 |
| 42 | Not verified in the finale, ported by reading and never run: the episodes 2, 3 (the scroll and "the end"), and 4; Doom 2 (the text skipped by a button, the cast). And the finale reached by playing to the end of E1M8, in the place of the timer of a deathmatch | Needs the registered or the Doom 2 WAD; the played way needs an autopilot that kills the two barons |
| 44 | Not verified in the flow: the cycle of the Ultimate Doom (`demo4`) and of Doom 2, the secret exit itself (a switch in E1M3), the finale between the levels of Doom 2. Verified on 2026-11-20: `idclev`, a screenshot (`cheat`), the way back from the secret level (`secret`) | The first and the fourth need other WADs |
| 45 | `G_DoCompleted` stays in `g_game.c` (it decides the next level and the victory): it reads static tables of par times. Moving it means copying the tables, or editing the engine more | To decide if the study needs the decisions in Atmos |
| 60 | At each big step (a milestone, a change of what is on screen), show the author a few screenshots in the report, in this session. The tests already write them (`tst/*.png`, compared at each run) | A rule of the author for the graphic projects, relayed by `atmos` on 2026-11-26. Done for D8 on 2026-11-26 |
| 50 | Where to send: coordination, status, and questions for the author, to the session `atmos` (`session_011eECohhJbc4GMpAj3yyeEo`, since 2026-11-24); measurements on the ports and changes of the compiler, to `atmos-lang` (`session_011eECogKVQ7ZzmWCoMST2wK`); `pin`, to `atmos-pin` (`session_011eECogSDhkVfyt7qme5DLh`). The pause ended on 2026-11-25 | Relayed by `atmos` on 2026-11-24 (atmos-lang#119) |

## C. Reported or decided, and not acknowledged

| # | What | Note |
|---|------|------|
| 88 | The indexes by a plain name lose their parentheses (`@(key)` to `@key`): 8 lines, all in `wi.atm`; two counters became `+=`. Twenty-one tests identical, nineteen on the variant | A request of the author for every project, relayed by `atmos` on 2026-11-26 |
| 86 | Excerpt, idiomatic: a counter of the intermission, `wi.atm:345`, `set cnt@key += 2` | Shown on 2026-11-26 |
| 87 | Excerpt, bad: `WI_PLACES@(WI.epsd)@n`, `wi.atm:624`. Why: the index by a field of a table needs parentheses, so one chain mixes both forms. Better: a table of the episode kept in a local (the port), or `@` taking a path of fields (the language) | Shown on 2026-11-26 |
| 80 | Excerpt, idiomatic: the end of the game as a question awaited, `menu.atm:755` | Shown on 2026-11-26, after #74 |
| 81 | Excerpt, bad: `Ask` writes its `solo` twice, `menu.atm:466`. Why: the pattern of a `solo` is syntax, and cannot take a part on a condition (the close button only for the question to quit). Better: a pattern with a conditional part, or patterns as values (the language) | Shown on 2026-11-26 |
| 82 | Excerpt, idiomatic: the name of the map chosen by an `ifs` over tables of Atmos, `hu.atm:265` | Shown on 2026-11-26 |
| 83 | Excerpt, bad: the names of the maps read from C one by one, `hu.atm:249`. Why: they are macros of C (`d_englsh.h`), which DeHackEd replaces by their text; Atmos reads no array of C as a vector, so each name is a native literal with a cast. Better: a native literal that gives a C array of strings as a vector (the language, or the environment) | Shown on 2026-11-26 |
| 84 | Excerpt, idiomatic: the keys of the automap as a table of lambdas by the bound code, `am.atm:166` | Shown on 2026-11-26 |
| 85 | Excerpt, bad: a pan of the automap, `am.atm:137`. Why: the window of the map is a struct of C in fixed point, read by the drawing of the engine; Atmos writes no field of a struct of C, and has no macro of C (`FTOM`), so the arithmetic is C in a literal, and the answer to the engine goes by a variable (`am_eat`). Better: access to fields of C (the language, as #70), and an answer to the caller of an emit (as #68) | Shown on 2026-11-26 |
| 79 | #74, the study: the border is the save at any tic (the behavior), not the format of `p_saveg.c`, which the author freed; a snapshot of tasks is still what is missing. The variant and the face, to review by the same measure, are left: the variant is a study of the simulation as tasks, which the border keeps in C; the face is an arbiter already in one task | By the session, approved (#74) |
| 78 | #74, the bugs: two reads out of bounds of the renderer are fixed in `engine/` (a sprite past its post; a short wall texture past its column), recorded in `doc/bugs.md`; AddressSanitizer finds none left in the 21 tests; the test platform no longer pads the blocks with zeros; the expected outputs recorded again on C (only pixels changed, in 13 tests) | By the session, approved (#74) |
| 77 | #74, the automap: the keys are tables of lambdas by the bound code; its state in a table `A`. `am_map.c` stays (its drawing and geometry are arithmetic of the engine, not control) | By the session, approved (#74) |
| 76 | #74, the messages: all of `hu_stuff.c` is in `hu.atm` (out of the build of the program, not edited): the state in a table `H`, the chat as a task, the names of the maps as tables, the menu asks for a message that stays by `H.stay`; the seam of #62 is undone. Twenty-one tests identical | By the session, approved (#74) |
| 75 | #74, the menus: all of `m_menu.c` is in `menu.atm` (out of the build of the program, not edited): the menus as tables with lambdas, the questions as `await Ask`, the modals as `solo`; the seam of #63 is undone. Twenty-one tests identical | By the session, approved (#74) |
| 67 | Excerpt, idiomatic: the time of a message, `hu.atm:281`, now a `watching :hu.show` that starts the count again | Shown on 2026-11-26 |
| 68 | Excerpt, bad: a key of the chat, `hu.atm:347`. Why: the key crosses to and from C at each step (`a_dbl`, casts), and the answer to the engine is a variable of C (`hu_eat`), since an emit returns no value to the C that called it. Better: an emit that returns a value to its caller (the language), or a responder that the engine does not need answered at once (the engine, not to be changed) | Shown on 2026-11-26 |
| 69 | Excerpt, idiomatic: the skull that blinks, `menu.atm:389` | Shown on 2026-11-26 |
| 70 | Excerpt, bad: the key down in a menu, `menu.atm:505`. Why: the menus are structs of C whose items point to C functions; Atmos reads no field of a struct of C but through native literals, so the dispatcher is C between backquotes. Better: access to fields of C (the language), or the menus as data of Atmos with their functions (the port: most of `m_menu.c`) | Shown on 2026-11-26 |
| 71 | Excerpt, idiomatic: the closed map, `am.atm:204`, now an `await ... until` | Shown on 2026-11-26 |
| 72 | Excerpt, bad: the tic of the open map tested `automapactive` again inside its `watching`, `am.atm:214`. Why: the session did not know the order of the awakes; the manual fixes it (a watching awakes before its body). Fixed in `01f89e8`: the fault was of the port | Shown on 2026-11-26 |
| 66 | In each report to the author, for each group of changes, one pair of short Atmos excerpts (file and line): one idiomatic, one bad. Each excerpt has a number of this session (#N); each bad one says why, and what would make it better (the language, the environment, or the port). With the screenshots of #60. Done for D6 on 2026-11-26 (#67 to #72) | A rule of the author for all projects, relayed by `atmos` on 2026-11-26 |
| 65 | The cost of D6: the messages, the menus, and the automap add about 7,000 instructions per tic (four directed emits); the program now costs 35 to 45% more than C with no rendering, 2.2 to 2.6% with it. One emit per tic for the three would take three away; not done, to keep the parts apart | By the session, in `doc/bench.md` |
| 64 | The keys of the automap are in `am.atm` (2026-11-26): open and closed as a sequence, with a `watching` for the engine that closes it; the pans and the zooms stay variables. A new test `automap`; twenty-one identical, nineteen on the variant. With it, #11 and D6 are done | By the session, approved (#11) |
| 63 | The keys of the menus are in `menu.atm` (2026-11-26), as the same dispatcher: the modes are set by the C functions of the items, so only the skull became a sequence; the questions as awaits would need most of `m_menu.c` in Atmos. A new test `menu`; twenty identical; the variant skips it (it saves) | By the session, approved (#11) |
| 62 | The messages and the chat are in `hu.atm` (2026-11-26): the time of a message is a task that starts again, the chat a sequence; two new tests (`hu`, `chat`), nineteen identical. Not verified: the lines of other players | By the session, approved (#11) |
| 61 | #33 is verified (2026-11-26): `-tst-tics <n>` gives n tics of the clock per frame and turns `singletics` off (a sleep passes one tic, so that the engine's wait ends); the test `tics` (three per frame, two wipes) is identical to C. Seventeen tests | By the session, approved by the author ("Vá em frente com esses") |
| 59 | D8 is done (2026-11-25): `doc/study.md` opens with the answer (the border is the saved game), a table of the parts in lines, tokens, and the state variables that are gone, and the border; `doc/bench.md` has the three builds on the three demos and the peak of the heap (`tst/memory.sh`): the program adds 4.1 MiB, of which 4 MiB is the stack of values of the runtime | By the session, after the pause |
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

On 2026-11-26, the author acknowledged the reports ("ack"): #59 (D8),
#60, #61 (#33), #62, #63, #64 (D6 done), #65. The plan, D0 to D8, is
done; nothing is waiting for an answer.

On 2026-11-26, the author acknowledged the redo (#74 to #79) and the
check on the compiler `d76b547` ("Ack"); the excerpts #80 to #85 (and
#67 to #72) are to be reviewed with him one by one, later.
