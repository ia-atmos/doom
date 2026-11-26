# The study

What is left for a language of structured reactive concurrency when the
engine is somebody else's and cannot change, and where is the border.
Each part that went to Atmos is compared below with what it was in C;
this first section joins them (D8, 2026-11-25).

## The answer

- **The border is the saved game.** What a save covers must be data at
  every tic: the objects and their table of states, the weapons, the
  doors and lifts. It stays in C, as the state machines it is. What no
  save covers is control around the simulation, and became tasks: the
  loop of frames, the screen wipe, the intermission, the finale, the
  flow of the game.
- **The control around the simulation reads better as tasks.** The
  intermission lost its counters of stages (`sp_state`, `ng_state`,
  `dm_state`, `cnt_pause`, `acceleratestage`) and is a sequence of
  awaits under one `watching`; the title lost `demosequence` and
  `pagetic`; the wipe lost its flag and the array of positions. The
  order of what happens is the order of the text.
- **A machine that is polled at each tic stays a machine.** The face of
  the status bar chooses, at each tic, among conditions of the player;
  in Atmos it is the same arbiter, in one task.
- **The simulation can become tasks, at a price.** In the variant, the
  door and the zombieman are tasks, with the demos identical: the jumps
  from the engine are one `watching`, the hidden jumps of the actions
  are visible. But the variant has no saved games, its objects must
  still write their row for the renderer and the engine, and it costs
  2.7 to 3.3 times the instructions of the simulation with no rendering.
- **Exactness held at every step.** Twenty-one runs, 51,576 frames, compare
  a checksum of the simulation and of the screen at each frame with the
  engine all in C, after every commit; the variant passes nineteen of
  them (all but the two that save).

## The parts, in numbers

Lines with no blank lines and no comments; tokens are names, numbers,
strings, and symbols. The Atmos side has two columns: the code in Atmos,
and the C of its `:prelude` (data and copies of pixels, with no state).

| Part | C | Atmos | C in the `:prelude` | State variables of C that are gone |
|------|---|-------|---------------------|-----------------------------------|
| Screen wipe | 256 lines, 1290 tokens | 40, 270 | 40, 278 | the flag `wipe` of the frame, `y[]` of the columns |
| Intermission | 1219, 6964 | 544, 3775 | 53, 288 | `state`, `sp_state`, `ng_state`, `dm_state`, `cnt_pause`, `acceleratestage`, `bcnt`, `cnt`, the `nexttic` of each animation |
| Finale | 528, 2800 | 248, 1454 | 183, 1270 | `finalestage`, `finalecount` |
| Flow of the game | 128, 489 | 122, 552 | 67, 308 | `demosequence`, `pagetic` |
| Face of the status bar | 176, 752 | 80, 460 | 48, 255 | none (the same machine, local to a task) |
| Messages and chat (all of `hu_stuff.c`) | 550, 2062 | 168, 1191 | 172, 1320 | `message_counter`, `message_nottobefuckedwith`, `message_dontfuckwithme` (a table `H`), the static `lastmessage`, `num_nobrainers`; the chat is a task |
| Menus (all of `m_menu.c`) | 1533, 6918 | 751, 5393 | 232, 1241 | `messageToPrint`, `messageRoutine`, `messageLastMenuActive`, `saveStringEnter`, `saveCharIndex`, `saveOldString`, `skullAnimCounter`; the menus as data of Atmos; the questions as awaits |
| Keys of the automap | 197, 890 | 107, 751 | 84, 443 | none (open and closed is a position of the task; the pans and the zooms stay variables, read by the tic) |
| Door, in the variant | 111, 452 | 52, 316 | 115, 754 | `topcountdown` |
| Zombieman, in the variant | 33 rows, 596 | 166, 1137 | 135, 894 | the row as the position (`state`, `nextstate`) |

The wipe counts all of `f_wipe.c`, with a second kind of wipe that the
game never uses and that was not ported; the door counts the thinker of
all the kinds of door; the zombieman counts its 33 rows, whose actions
are the same functions of the engine on both sides. The Atmos side of
the door and of the zombieman also has the glue that the engine needs
to meet a task (the renamed functions, the records of the program).

## The border

Why the simulation stays a state machine: the saved game writes the
number of the row of each object, and the fields of each door, lift,
and light (`p_saveg.c`). A task suspended in an await is a point in the
code with its local variables, and cannot be written. To keep saved
games is to keep the table.

What the table does better, seen from the variant:

- **It is data.** It can be saved, patched (DeHackEd edits its rows),
  listed, and read by code that knows nothing of the monster (the cast
  of the finale plays the rows of every monster).
- **It accepts any row.** The engine may set a row that the zombieman
  never planned, as the gibs of a crushed corpse; the task must fall
  back to playing the table for it.
- **It is cheap.** A thinker steps a row in a few dozen instructions;
  a tic delivered to a task costs about two thousand in this runtime.

What tasks do better: the order of a behavior is the text (stand,
chase, attack, pain), the jumps from outside are one `watching`, and
the jumps that the actions hide in the table are visible at each row
that may have one.

What a snapshot of tasks would need from the language (an item of the
compiler, `atmos-lang#45`): the position of each task (which await of
which block), its local variables, and the native values that it holds
(here, pointers into the engine), each with a way to be written and
rebuilt. The runtime of this backend makes the first two possible (a
task is a record, not a C stack); the third needs every binding to say
how. Saving only at checkpoints, so that the middle of a level never has
to be data, is the other way, recorded as a possibility (#36) and not
taken.

## The parts, one by one

The sections below were written as each part was done, and keep their
numbers of that day; the table above has the current ones.

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
- **More than one tic per frame** (verified on 2026-11-26, test `tics`):
  a window on a slow machine gives the wipe several tics of the clock in
  one frame, and `loop in (now - t)` takes as many steps. The test
  platform gives three tics per frame with `-tst-tics 3`; the two wipes
  of the test are identical to C.

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

## The messages and the chat

First done on 2026-11-26 from the three functions of `hu_stuff.c`
(#62); redone the same day after the general instruction of the author
(#73, #74): all of `hu_stuff.c` left the build of the program, and is in
`hu.atm`. The widgets that draw the lines stay in `hu_lib.c`.

| | C (`hu_stuff.c`) | Atmos (`hu.atm`) |
|---|---|---|
| Lines | 550 (most of them the names of the maps) | 168, and 172 of C in the `:prelude` (the font, the widgets, the queue of the characters that go to the others, the lines that come from them, the names of the maps as macros) |
| State | `message_counter`, `message_nottobefuckedwith`, `message_dontfuckwithme`, `chat_on`, the statics `lastmessage`, `altdown`, `num_nobrainers` | one table `H` (`fixed`, `stay`, `nobrainers`, the names of the maps as tables); the position of the tasks; `message_on` and `chat_on` stay in C, since the widgets draw by them |

- **The time of a message is a task that starts again**: a `watching
  :hu.show` around four seconds of tics. The counter is gone.
- **The chat is a task**, `Chat`, spawned by the key that starts it and
  done at enter, escape, or a macro; the macro is a loop over a string
  of Atmos.
- **The menu asks for a message that stays** by `H.stay`, a field of the
  table, where it was a variable of C written by `m_menu.c`
  (`message_dontfuckwithme`).
- **The names of the maps are tables of Atmos**, made once from the
  strings of the engine; the choice of the name by the game is an
  `ifs`.
- **A level starts it all again**: one `watching :hu.start`.
- **What reads worse.** `HU_Responder` must answer at once whether it
  took the key: the task writes a variable of C (`hu_eat`), that the
  native function returns after the emit. The order inside a tic (the
  end of a message, then the new ones) needs two events, `:hu.tic` and
  `:hu.check`. The queue of the characters that go to the others stays
  a ring of bytes in C, since the engine takes one at each tic.

### What was verified, and what was not

Two new tests, recorded on C (nineteen in all): `hu` (a cheat, its end
at the exact tic, the key that shows it again, a message that replaces
another, messages off with its message fixed and a cheat that shows
nothing, messages on with a cheat that waits for the fixed message to
go) and `chat` (a net game of one player: a line sent, a character
erased, a macro, the message shown again). The demos have the messages
of what the player picks up. Not verified: the lines that come from
other players and the talks to oneself, which need more than one player
(as for #37).

## The menus

First done on 2026-11-26 as the same dispatcher (#63); redone the same
day as Atmos, after the general instruction of the author (#73, #74):
all of `m_menu.c` left the build of the program (as `wi_stuff.c` did),
and is in `menu.atm`.

| | C (`m_menu.c`) | Atmos (`menu.atm`) |
|---|---|---|
| Lines | 1533 | 751, and 232 of C in the `:prelude` (the mouse and the joystick made keys, the files of the saves) |
| State | `currentMenu`, `itemOn`, `messageToPrint`, `messageString`, `messageRoutine`, `messageNeedsInput`, `messageLastMenuActive`, `saveStringEnter`, `saveSlot`, `saveCharIndex`, `saveOldString`, `savegamestrings`, `quickSaveSlot`, `epi`, `screenSize`, `whichSkull`, `skullAnimCounter` | one table `M` (the menu, the item, the skull, the quick slot, the names of the saves, the slot being typed); `menuactive` stays in C, which the engine reads |

- **The menus are data.** Each menu is a table: where it is drawn, a
  lambda that draws its title, the menu before, and its items; each item
  a table with its graphic, its shortcut, its kind, and its action (a
  lambda, or a task when it asks). The versions of the game edit the
  tables (`versions`).
- **A question is an await.** `if (await Ask(text, true, false)) ==
  K.confirm { ... }`, in the place of a callback for the answer: the
  quick save, the quick load, the nightmare, the end of the game, the
  quit. A notice is the same, with any key.
- **The modals are `solo`.** A question takes the keys and the drawing
  from the menus behind it, which see nothing until it ends; the name of
  a saved game being typed takes the keys. The question to quit also
  takes the close button of the window, which the other questions leave
  to the menus, which then ask to quit in their place (in C, the
  question to quit replaced any message).
- **Closed and open are two tasks in sequence**, inside a `watching` of
  the close button; the skull is a trail of two awaits.
- **What reads worse**: the keys and the sounds of the engine are
  numbers read from C into tables (`K`, `SFX`); the configuration
  (`showMessages`, the volumes, the sensitivity) stays in variables of
  C that the native literals write; the order of the sound of a key and
  of its action needs the action spawned, the sound played, and then the
  action awaited (`act`).
- **A difference that no test reaches**: in C, the close button while a
  name is typed starts the question to quit, but the keys still go to
  the name first. Here the `watching` ends the typing, and the question
  takes the keys.

### What was verified, and what was not

The test `menu` (and `newgame`, `save`, `endgame`, `cheat`, `hu`) go
through the menus, the questions, the notices, the typing, the quick
save and load, the sliders, the help; twenty-one tests identical. Not
verified: the mouse, the joystick, the close button of the window, the
second page of help and the versions of Doom 2 and of the Ultimate Doom.

## The keys of the automap

Done on 2026-11-26 (#11). In `am.atm`.

| | C (`AM_Responder`, `AM_Ticker`) | Atmos (`am.atm`) |
|---|---|---|
| Lines | 197 (with the constants and the variables of the file) | 107, and 84 of C in the `:prelude` |
| State | `automapactive` (read by both), the static `bigstate`, `m_paninc`, the two multipliers of the zoom | `automapactive` stays (the engine reads it, and closes the map); open and closed is also the position of the task; `am_bigstate` lasts as long as the program, as the static |

- **Open and closed is a sequence**: closed, the task awaits the key of
  the map; open, a `watching` holds the tic that moves the map and the
  keys, until the key of the map (the task closes it) or a tic that
  finds the map closed by the engine. The engine closes it in three
  places (the end of a level, a new level, the death of the player); in
  all, a tic of the automap follows in the same tic of the game, before
  any key, so the `watching` sees it first.
- **The pans and the zooms stay variables.** A held key sets a
  multiplier, its release clears it, and the tic applies it: the last
  key down wins, and any release of the pair stops both (zoom out held,
  zoom in down and up: no zoom). A task per held key would not say
  this more simply, and the work of a tic has an order (follow, then
  zoom, then pan) that one loop keeps.
- **What reads worse**: the answer of `AM_Responder` is a variable of C
  again (`am_eat`); the key of the map, when it closes the map, still
  goes through the cheat of the map, as in C.

### What was verified, and what was not

A new test, `automap`, recorded on C (twenty-one in all): open, follow
off, the grid, a pan and a zoom while held, the whole map and back, two
marks and their clearing, the cheat twice, follow with the player
walking, closed and opened again, a new level by a cheat with the map
open (closed by the engine), and open in the new level. Not verified:
the button of the joystick, the death of the player with the map open,
and the end of a level with it open (the `level` test exits with it
closed).

## The variant: a door and the zombieman as tasks

A second program (`var.atm`, `make var`), for the comparison only: the
door that opens, waits, and closes, and the zombieman, are tasks, where
the engine has a thinker that steps a state at each tic. It has no saved
games. Its oracle is the main build: `make test-var` runs every test but
`save` against the same expected outputs, and the fifteen are identical,
tic by tic, simulation and screen (on 2026-11-26, nineteen: all but
`save` and `menu`, which save).

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
