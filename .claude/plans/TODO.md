# TODO

Everything that is pending in the session `doom`: asked and not
decided, known and not done, or reported and not acknowledged. Keep it
current: remove an item when it is settled, add one whenever something
is left open.

The numbers do not change. Numbers that are missing were settled.

Last update: 2026-11-13.

## A. Decisions waiting for an answer

| # | Decision | Recommended | State |
|---|----------|-------------|-------|
| 16 | A permission to build and run the engine in this session. The first build of Chocolate Doom (cmake and make, on a clone outside the repository) was denied by the permission classifier, as code from an external source | Allow it: D0 is to build and run this code | Asked on 2026-11-13. It blocks all of D0 |

## B. Work known and not done

The milestones of the plan. Nothing is built yet.

| # | Work | Note |
|---|------|------|
| 5 | D0: readme with credits and licenses; the engine builds from the copy; the game data fetched; the test mode; the expected outputs (three demos, scripted runs); tics per second with no rendering | Waits for A16. Chocolate Doom is cloned at `a2dfd51`, and SDL2 is installed (2.30.0, no mixer, no net) |
| 6 | D1: the seam, with nothing ported; `doc/seam.md` | After D0. Read `doc/manual.md` of `ia-atmos/lang` first |
| 7 | D2: the screen wipe | |
| 8 | D3: the intermission | |
| 9 | D4: the finale | |
| 10 | D5: the flow of the game, with the title cycle, the demos, and save and load | |
| 11 | D6: the status bar, the messages, the menus, the automap | To ask the author when D2 to D5 are done (item 4) |
| 12 | D7: the variant: a door and the zombieman as tasks | |
| 13 | D8: the study and the benchmark | |
| 14 | The local clone of the compiler still has `fsantanna/ceu-atmos` as its remote | The change of the remote was denied by the permission classifier of the session. Harmless: the old address redirects, and fetches name `ia-atmos/lang` explicitly |

## C. Reported or decided, and not acknowledged

| # | What | Note |
|---|------|------|
| 15 | The plan was replaced twice in the repository, as sent by the compiler session: the simulation stays in C (`5b11fca`), and the compiler is now `ia-atmos/lang` (`bd54787`) | Reported to the author in this session; no reply |

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
