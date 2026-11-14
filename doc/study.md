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

Of the 57 variables of C, 36 are pointers to graphics, loaded at the
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
