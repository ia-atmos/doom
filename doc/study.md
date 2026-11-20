# The study

The comparison of each part that went to Atmos with what it was in C.
It grows with the port; the plan has the questions.

Lines are counted with no blank lines and no comments.

## The screen wipe

| | C | Atmos |
|---|---|---|
| Files | `f_wipe.c`, and half of `D_RunFrame` | `wipe.atm`, and the loop of `doom.atm` |

- **A variable that became control.** `D_RunFrame` had `static boolean
  wipe`: each frame tested it, to be a frame of the game or a frame of
  the wipe. In `doom.atm`, a frame that starts a wipe does
  `await Wipe()`: the game does not run because its loop is waiting.
- **Each column is a task.** In C, an array `y[]` holds where each
  column is, negative while it waits. In Atmos, `Column` waits
  (`await(wait, :wipe.step)`) and then falls in a loop; its position is a
  local variable. The wipe ends with `await :all cols`.
- **What stayed in C**: the copies of pixels, as functions of the
  `:prelude`. They have no state.
- **What reads worse**: the wipe ends one frame after the last column
  stops, which in C comes from a flag `done` that a step with nothing to
  move leaves true. In Atmos it is an `await :frame` after the
  `await :all cols`, whose reason is not visible without a comment.

## The intermission

| | C (`wi_stuff.c`) | Atmos (`wi.atm`) |
|---|---|---|
| Lines | 1219 | 622, of which 56 are C in the `:prelude` |
| Variables of the file (`static`) | 57 | 3 (`WI`, `WI_POINTER`, `WI_FRAGS`) |

Of the 57 variables of C, 25 hold graphics, loaded at the
start of an intermission and freed at its end (150 lines). The Atmos
version asks the WAD for a graphic by its name each time it draws, so
it has none of that; it is a choice that C could make as well, and not
a gain of the language.

### The counters

In C, the statistics of one player are a variable `sp_state` from 1 to
10, tested in a chain of `if` at each tic: the odd states are pauses,
and the even ones are a counter each.

```c
if (sp_state == 2) {
    cnt_kills[0] += 2;
    if (!(bcnt&3)) S_StartSound(0, sfx_pistol);
    if (cnt_kills[0] >= (plrs[me].skills * 100) / wbs->maxkills) {
        cnt_kills[0] = (plrs[me].skills * 100) / wbs->maxkills;
        S_StartSound(0, sfx_barexp);
        sp_state++;
    }
}
else if (sp_state == 4) { /* the same, for the items */ }
else if (sp_state == 6) { /* the same, for the secrets */ }
...
else if (sp_state & 1) {
    if (!--cnt_pause) { sp_state++; cnt_pause = TICRATE; }
}
```

In Atmos, the sequence is the text:

```
watching :wi.accel {
    await(35, :wi.tic)
    await WI_Count(cnt, "kills", WI.kills)
    await(35, :wi.tic)
    await WI_Count(cnt, "items", WI.items)
    await(35, :wi.tic)
    await WI_Count(cnt, "secret", WI.secret)
    ...
}
```

- `sp_state`, `cnt_pause`, and `acceleratestage` are gone. The same
  for `dm_state` and `ng_state` of the net games, and for `state`, the
  variable of the three screens, which is now
  `await WI_Stats() ; await WI_Map()`.
- The key that skips the counters was a test at the top of the
  function, before the chain (`if (acceleratestage && sp_state != 10)`).
  It is now the `watching` around the sequence: what it interrupts is
  what it encloses.
- The three counters were three copies of the same code. They are one
  task, `WI_Count`, since a task can be awaited in sequence.

### The animations of the map

In C, each animation has `nexttic`, the tic of its next frame, compared
at each tic with a global counter. In Atmos, each is a task that waits
its period, and also draws itself:

```
par {
    await((`:number M_Random()` % period) + 1, :wi.tic)
    loop {
        set frame = (frame + 1) % frames
        await(period, :wi.tic)
    }
} with {
    loop on :wi.draw {
        if frame >= 0 { wi_draw(x, y, <name>) }
    }
}
```

The order of drawing (background, animations, text) is the order in
which the tasks are attached, and it had to be thought about: nothing
in the text of a task says that it draws before another.

### What reads worse

- **The "you are here" that blinks.** C: `snl_pointeron = (cnt & 31) <
  20`, one line. Atmos: a loop with `await(12, ...)` and
  `await(20, ...)`, and a first time of 12 tics that only the
  arithmetic of C explains. The line of C is better.
- **Two quirks of the original are now visible as globals.**
  `WI_POINTER` and `WI_FRAGS` keep a value from one intermission to the
  next, because C had them as `static` and never cleared them. As
  local variables they would be cleared, and the first frame of the map
  would differ.
- **Reading the engine.** Each field of C read by the program is a
  native literal (`` `:number WI_WBS->epsd` ``), copied into a table when
  the intermission starts.

### What was verified, and what was not

| Verified by a test, identical to C | Not verified |
|---|---|
| One player: the counters to the end, and skipped (`level`, `skip`) | The map of the episodes 2 and 3, with the animations of kind `:level`: the shareware WAD has only the episode 1 |
| The map: the blinking, left by a key and by itself | |
| The second intermission of a game, with the "you are here" as the first map left it (`dm2`) | |
| The way back from the secret level, E1M9, to E1M4 (`secret`) | |
| Deathmatch with one player and no frags (`dm`) | Frags above zero, and more than one player |
| A cooperative game with one player (`net`) | Doom 2 (`commercial`), which has no map |

The animations of kind `ANIM_RANDOM` are not in the data of the game,
and were not ported.

## The finale

| | C (`f_finale.c`) | Atmos (`fi.atm`) |
|---|---|---|
| Lines | 528 | 445, of which 183 are C in the `:prelude` |

Almost half of the Atmos file is C: the table of the texts, the order of the
cast with its sounds, and the copies of pixels (a tiled background, the
scroll of two screens, a sprite of the table of states). They are data
and drawing, with no state.

### The stages

In C, `finalestage` says which of the three stages is on (the text, the
last screen, the cast), and `finalecount` counts the tics of the stage.
Every function starts by testing them. In Atmos:

```
watching ((#text * 3) + 251), :fi.tic {
    await FI_Text(text)
}
`wipegamestate = -1;`       ;; forces a screen wipe
await FI_Art(episode)
```

- The typing of the text was arithmetic at each draw (`count =
  (finalecount - 10) / TEXTSPEED`). It is now a task of `FI_Text`:
  `await(10, :fi.tic)`, and then one more character in a
  `loop on 3, :fi.tic`.
- The time of the text on screen was a test at each tic
  (`finalecount > strlen(finaletext)*TEXTSPEED + TEXTWAIT`). It is the
  `watching` above.

### What reads worse

- **The cast** follows the table of states of the engine: a row, its
  tics, the next row. In Atmos it is still that machine, with the same
  six variables, in one loop that waits the tics of a row and then
  decides the next. It reads no better than C, and each field of the
  table is a native literal
  (`` `:number states[(int)a_dbl($st)].nextstate` ``). The only gain is
  the key that kills a monster: a `par :any` between the tics of the row
  and the key, where C had a second function that changed the variables
  of the first.
- **A sound started by the drawing.** In the end of the episode 3, C
  starts a shot when it draws a new stage of "the end", and not when a
  tic comes. The Atmos version keeps it in the drawing, to be the same.
- **Three more globals for quirks of the original**: the text and the
  background are kept from one finale to the next (in C, in the
  `:prelude`), and so is the last stage of "the end" (`FI_END`).

### What was verified, and what was not

| Verified by a test, identical to C | Not verified |
|---|---|
| The episode 1: the text typed, its time, the wipe, the last screen (`finale`) | The episodes 2, 3 (the scroll and "the end"), and 4 |
| | Doom 2: the text skipped by a button, and the whole cast |
| | The finale reached by playing: the test ends E1M8 with the timer of a deathmatch, which calls the same function |

The shareware WAD has only the episode 1. The cast, the scroll, and the
skip were ported by reading the C, and have never run.

## The flow of the game

| | C | Atmos (`flow.atm`) |
|---|---|---|
| Lines | 125: the "demo loop" of `d_main.c` (88), and the dispatch of `G_Ticker` (37) | 186, of which 56 are C in the `:prelude` |
| Variables | `demosequence`, `pagetic`, `pagename` | none |

### The title

In C, a counter `demosequence` goes from 0 to 5, and a `switch` on it
says what each step is; `pagetic` counts the tics of a page, and
`advancedemo` asks for the next step:

```c
demosequence = (demosequence+1)%6;
switch (demosequence) {
  case 0: pagetic = 170; gamestate = GS_DEMOSCREEN;
          pagename = "TITLEPIC"; S_StartMusic(mus_intro); break;
  case 1: G_DeferedPlayDemo("demo1"); break;
  case 2: pagetic = 200; gamestate = GS_DEMOSCREEN;
          pagename = "CREDIT"; break;
  case 3: G_DeferedPlayDemo("demo2"); break;
  ...
}
```

In Atmos, the cycle is the text, in order:

```
loop {
    watching :title.next { title_music() ; await Page("TITLEPIC", 170) }
    watching :title.next { await Demo("demo1") }
    watching :title.next { await Page("CREDIT", 200) }
    watching :title.next { await Demo("demo2") }
    ...
}
```

- `demosequence` and `pagetic` are gone: the step is where the task
  waits, and the time of a page is `await((tics + 1), :page.tic)`.
- `D_StartTitle` set `demosequence = -1` to start again. In Atmos the
  task `Title` is aborted and spawned again (`watching :title.start`).
- What reads worse: each step is wrapped in a `watching :title.next`,
  the same five times. The step does not end by itself: a page asks for
  the next one and goes on showing until the engine answers, one tic
  later, and a demo ends inside the engine. The `watching` is the
  answer, and the repetition is its price.

### The game

In C, the dispatch was a `switch` on `gameaction`, with one case per
action, and no order between them. In Atmos, the order is visible:

```
task Game (start) {
    <G_DoNewGame, G_DoLoadGame, or G_DoPlayDemo>
    loop {
        await :g.completed          ;; the level ends
        `G_DoCompleted();`         ;; the intermission, or the victory
        if <victory> {
            await :g.victory
            `F_StartFinale();`
        }
        await :g.worlddone          ;; the next level
        `G_DoWorldDone();`
    }
}
```

- A level comes, then its end, then the intermission or the finale,
  then the next level. In C, this order is spread over the places that
  set `gameaction`: `G_ExitLevel`, `G_DoCompleted`, `G_WorldDone`.
- What may come at any time is apart: a death that loads the level
  again, a save, a screenshot, each a `loop on` of its own.
- A new game, a saved game, or a demo aborts the `Game` that is on and
  starts another; the title aborts it too.

### What reads worse, and what did not move

- **The decisions stayed in C.** Which level is next, and whether it
  is the victory, are in `G_DoCompleted`, which stays in `g_game.c`: it
  reads tables of par times that are `static` there. So the Atmos `Game`
  asks the engine after the fact (`` `:boolean gameaction ==
  ga_victory` ``).
- **An action with no task is an error at run time.** In C, a `switch`
  ignores an action that it does not know. In Atmos, an event that no
  task awaits is lost, and the loop of the engine would ask for it
  forever, so `G_DoActions` checks that the action was done.
- **The program must start before its loop.** `D_DoomMain` starts the
  title, and runs a first tic, before it calls the program. The program
  now starts at the first call from the engine, whatever it is, and it
  must tell a game started by the engine before it (`-warp`) from no
  game at all, which it does by `viewactive`.

### What was verified, and what was not

| Verified by a test, identical to C | Not verified |
|---|---|
| The cycle of the title, with its three demos, and back to the title (`title`) | The cycle of the Ultimate Doom, with `demo4`, and of Doom 2 |
| A new game by the menu (`newgame`), and a game ended by the menu (`endgame`) | |
| A new game by a cheat, `idclev13` (`cheat`) | |
| A level, its intermission, and the next level (`level`, `skip`, `dm`, `dm2`, `net`) | The secret exit itself (a switch in E1M3), and the secret level reached by it |
| The way back from the secret level (`secret`, by `-warp 1 9`) | |
| The victory, and the finale (`finale`) | Doom 2: the finale between levels, and the next level after it |
| A save and a load (`save`) | |
| A screenshot, with `-devparm` and F1 (`cheat`) | |
| A death that loads the level again (`death`) | |
| A game started before the program (`dm`, `net`, `finale`, with `-warp`) | |

## The face of the status bar

| | C (`ST_updateFaceWidget`, `ST_calcPainOffset`) | Atmos (`st.atm`) |
|---|---|---|
| Lines | 149 | 130, of which 43 are C in the `:prelude` |
| State | `priority`, `lastattackdown`, `lastcalc`, `oldhealth` (`static` in the functions), `st_facecount` (of the file) | `prio`, `count`, `rampage`, local to the task `Face` |

The plan named the face as the best case left for tasks: "a face with
priorities and timers". Read closely, it is not.

- **It is an arbiter, checked at each tic.** The conditions are states
  of the player, not events: the player is dead, has a damage count, has
  a new weapon, holds fire. At each tic, the face of the highest
  priority that applies replaces the one on screen if its priority is
  higher. A sequence of awaits cannot say this: there is nothing to
  await but the tic, and every condition must be tested again at each.
- **An expired face keeps its priority for one more tic.** The count is
  tested after the priorities, so the face whose count ended still holds
  its priority against the checks of the tic in which it is replaced. A
  version with a `watching` per face would release the priority one tic
  earlier, and the screens would differ.
- **So the Atmos version is the same machine**, in one `loop on
  :st.face`, with the same tests in the same order. What it gains is
  small: the state is local to one task instead of `static` variables
  spread over two functions and the file, and the cache of the pain
  offset is gone (it is a pure function of the health).
- **What it costs**: six variables of `st_stuff.c` lost their `static`,
  so that the program can read and write them; the angle of the attacker
  stayed in C (`st_turn`), since it is arithmetic of the engine.

This is a result for the border: a state machine that is polled at each
tic, whose states are conditions of the simulation, stays a state
machine in Atmos. Tasks help when the states follow each other in time
(the intermission, the title, the flow), not when they are chosen anew
at each tic.

### What was verified, and what was not

The thirteen tests are identical, and their screens include the face
(the demos have attacks from monsters, pickups, rapid fire; `death` the
dead face; `level` god mode). Which of the branches each test reaches was
not measured.

## The variant: a door and the zombieman as tasks

A second program (`var.atm`, `make var`), for the comparison only: the
door that opens, waits, and closes, and the zombieman, are tasks, where
the engine has a thinker that steps a state at each tic. It has no saved
games. Its oracle is the main build: `make test-var` runs every test but
`save` against the same expected outputs, and the fifteen are identical,
tic by tic, simulation and screen.

How it meets the engine, with nothing of it edited: `p_doors.c` and
`p_mobj.c` leave the build of the variant and are included in the
program as they are, with a few functions renamed by the preprocessor
(`EV_DoDoor`, `EV_VerticalDoor`; `P_SetMobjState`, `P_SpawnMobj`,
`P_SpawnMapThing`). The program has its own, which make tasks of the
doors and zombiemen and pass everything else to the originals.

- **The list of thinkers stays the scheduler.** A door or a zombieman
  keeps its thinker in the list, in the place it had; the thinker only
  gives its tic to the task (`atm_emit_to`). So the order of the calls
  to the random generator is the original one, and the demos stay in
  sync.
- **What the demos reach**: 2241 tics of doors in demo1; 83 zombiemen in
  the three demos, with 299 thousand of their tics, their stand, chase,
  attack, pain, death, and death in pieces, and the gibs of a crushed
  corpse. Not reached: the resurrection (no arch-vile in the shareware
  episode), and the jump to chase that damage gives to a standing
  zombieman.

### The door

| | C (`T_VerticalDoor`, the normal door) | Atmos (`Door`, `var/doors.atm`) |
|---|---|---|
| State | `direction` (1, 0, -1), `topcountdown`, in a record | where the task waits, and `dir` |

C, at each tic, a `switch` on `direction`, and inside it a `switch` on
the type of the door; the use of a door (`EV_VerticalDoor`) writes
`direction` from outside. Atmos:

```
loop {
    val next = par :any {
        <up: loop on :door.tic until the top>      ;; then 0
        <top: await(150, :door.tic)>                ;; then -1
        <down: loop on :door.tic until the floor or something under it>
    } with {
        await :door.use until (dir == -1) || (it == "door.use.player")
        if dir == -1 => 1 => (-1)
    }
    ...
}
```

- The wait at the top is `await(150, :door.tic)`: `topcountdown` is
  gone.
- The use from outside is an event that interrupts the phase, in a
  `par :any`; the rule of who may close a door ("bad guys never close
  doors") is the predicate of its `await`.
- What reads worse: the direction is still a variable (`dir`), since a
  use decides by it, and the record of the engine must stay, since other
  code reads it (`sector->specialdata`). The three phases are a dispatch
  on `dir`, which is the `switch` of C in another form.

### The zombieman

In the table, the zombieman is 33 rows. From the table alone one cannot
tell that standing leads to chasing, nor chasing to attacking: those
jumps are inside `A_Look` and `A_Chase`. In the task they are the text:

```
task Stand (z, born) {                  task Chase (z) {
    ...                                     loop {
    loop {                                      loop i in 8 {
        val j2 = enter(z, S_STND2, look)            val j = enter(z, S_RUN1 + i, chase)
        if j2 { return(j2) }                        if j { return(j) }
        await Tics(z)                               await Tics(z)
        val j1 = enter(z, S_STND, look)         }
        if j1 { return(j1) }                    }
        await Tics(z)                       }
    }
}
task Attack (z) {                       task Zombie (z) {
    enter(z, S_ATK1, face)                  var st = nil
    await Tics(z)                           loop {
    enter(z, S_ATK1 + 1, shoot)                 watching :z.jump {
    await Tics(z)                                   await Life(z, st)
    enter(z, S_ATK1 + 2, nil)                   }
    await Tics(z)                               set st = <the row of the jump>
    S_RUN1                                  }
}                                       }
```

- **The jumps from outside are one `watching`.** Pain, death, and the
  resurrection come from code of the engine (`P_DamageMobj`,
  `P_KillMobj`) into any row. In the task, they abort the life at any
  point, and it starts again from the row of the jump. This is the
  sketch of the plan, with one difference: the plan nested a `watching`
  per kind of jump (death around pain around chase); in practice every
  jump replaces the whole behavior, so one `watching` of all jumps, and
  a dispatch on the row, is what the engine does.
- **The jumps from inside are not events.** `A_Look` and `A_Chase` call
  the engine back to set a row while the task runs them, and a running
  task cannot await. The program keeps the row (`pending`), and the task
  takes it when the action returns: `if j { return(j) }`. These are the
  hidden jumps of the table, and in the task they are visible, at each
  row that may have one. The action of the new row runs after the
  action that asked for it returns, where C ran it inside; nothing
  between them reads it, and the demos agree.
- **The task still writes the row.** The renderer draws the frame of the
  row, and the engine reads `mobj->state` and `mobj->tics` (the damage
  compares the state to the spawn state; a death shortens the tics by a
  random amount). So the wait of a row is the count of `mobj->tics`, as
  the engine left it, and not a number in the code.
- **What the table does better**: it is data. It can be written to a
  file (the saved game: the variant has none), patched (DeHackEd edits
  the rows), and listed. A row that is not the zombieman's, the gibs of
  a crushed corpse, is played from the table by the task (`Rows`): the
  task cannot know in advance all the rows that the engine may give.

### The cost

`tst/count.sh` on the variant, against the main build (millions of
instructions):

| Demo | Main, no rendering | Variant, no rendering | | Main, rendering | Variant, rendering | |
|------|--------------------|-----------------------|---|-----------------|--------------------|---|
| demo1 | 263.4 | 711.7 | 2.7x | 4828.0 | 5277.8 | +9% |
| demo2 | 186.5 | 499.1 | 2.7x | 3564.7 | 3878.5 | +9% |
| demo3 | 128.3 | 415.5 | 3.2x | 2068.9 | 2357.7 | +14% |

Where it goes, in demo1 with no rendering (callgrind):

- **The tic of each zombieman**: about 2.2 thousand instructions, where
  the C thinker steps a row in a few dozen. The event goes to one task
  (`atm_emit_to`), and is matched down its nested awaits: the
  `watching` of the jumps, the behavior, the `Tics` of the row.
- **The broadcast of `:frame`**, about 35 thousand instructions per
  frame more than in the main build: it visits every door and every
  zombieman, at all their awaits, although none awaits `:frame`.
- In the runtime, `emit_trail`, `emit_node`, `kit_next`, and the matches
  of tags (`leaf_match1`, `a_is`) are about 40% of all the instructions.

With rendering, the simulation is a small part of the time, and the
variant costs 9 to 14% more.
