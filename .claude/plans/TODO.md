# TODO

Everything that is pending in the session `doom`: asked and not
decided, known and not done, or reported and not acknowledged. Keep it
current: remove an item when it is settled, add one whenever something
is left open.

The numbers do not change. Numbers that are missing were settled.

Last update: 2026-11-13.

## A. Decisions waiting for an answer

From the plan (`261113-doom.md`, "Settled decisions", the items marked
"(confirm)"). Asked one at a time, in this order. Each has the option
recommended by this session.

| # | Decision | Recommended | State |
|---|----------|-------------|-------|
| 1 | The base of the engine: Chocolate Doom at `a2dfd51`, or `linuxdoom-1.10` | Chocolate Doom: it builds today over SDL2, runs with no screen, and exists to keep the demos in sync | Asked on 2026-11-13, no answer. It blocks all of D0 |
| 2 | The engine in the repository: a copy of what the build needs, in one first commit with nothing changed, or a submodule, or a fetch by a script | The copy: `git diff` of the folder against its first commit is then the whole seam | Not asked yet (after 1) |
| 3 | The game data: the shareware `DOOM1.WAD` (version 1.9) fetched by a script and not committed, or Freedoom | The shareware WAD by a script: its demos are the ones of the original | Not asked yet (after 2) |
| 4 | How far to go after the flow: the status bar and its face, the messages, the menus, the automap | To decide when the wipe, the intermission, the finale, and the flow are done (D6) | Not asked yet |

## B. Work known and not done

The milestones of the plan. Nothing is built yet.

| # | Work | Note |
|---|------|------|
| 5 | D0: readme with credits and licenses; the engine builds from the copy; the game data fetched; the test mode; the expected outputs (three demos, scripted runs); tics per second with no rendering | Waits for A1, then A2 and A3 |
| 6 | D1: the seam, with nothing ported; `doc/seam.md` | After D0. Read `doc/manual.md` of `ia-atmos/lang` first |
| 7 | D2: the screen wipe | |
| 8 | D3: the intermission | |
| 9 | D4: the finale | |
| 10 | D5: the flow of the game, with the title cycle, the demos, and save and load | |
| 11 | D6: the status bar, the messages, the menus, the automap | Waits for A4 |
| 12 | D7: the variant: a door and the zombieman as tasks | |
| 13 | D8: the study and the benchmark | |
| 14 | The local clone of the compiler still has `fsantanna/ceu-atmos` as its remote | The change of the remote was denied by the permission classifier of the session. Harmless: the old address redirects, and fetches name `ia-atmos/lang` explicitly |

## C. Reported or decided, and not acknowledged

| # | What | Note |
|---|------|------|
| 15 | The plan was replaced twice in the repository, as sent by the compiler session: the simulation stays in C (`5b11fca`), and the compiler is now `ia-atmos/lang` (`bd54787`) | Reported to the author in this session; no reply |

## D. Older items

None.
