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
| The map: the blinking, left by a key and by itself | The second intermission of a game (the "you are here" as the first map left it) |
| Deathmatch with one player and no frags (`dm`) | Frags above zero, and more than one player |
| A cooperative game with one player (`net`) | Doom 2 (`commercial`), which has no map |

The animations of kind `ANIM_RANDOM` are not in the data of the game,
and were not ported.

## The finale

| | C (`f_finale.c`) | Atmos (`fi.atm`) |
|---|---|---|
| Lines | 547 | 413, of which 190 are C in the `:prelude` |

Half of the Atmos file is C: the table of the texts, the order of the
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
