# The seam

Where the engine in C and the program in Atmos meet: every change made
to the engine, and what crosses in each direction.

`engine/` is a copy of Chocolate Doom whose first commit (`8f5ee02`) has
nothing changed. So this command shows every change:

```
git diff 8f5ee02 -- engine/
```

## Changes to the engine

Two lines, in one file.

| File | Change | Why |
|------|--------|-----|
| `src/doom/d_main.c`, `D_DoomLoop` | The loop `while (1) { D_RunFrame(); }` becomes one call, `D_DoomFrames();` | The loop of frames is control: it goes to the program |
| `src/doom/d_main.c`, the prototypes | `void D_DoomFrames (void);` | The declaration of the call above |

No file left the build.

## Who runs

1. The `main` is the one of the engine (`i_main.c`), which calls
   `D_DoomMain`. The engine reads its arguments, loads the WAD, and
   starts all its parts, as before.
2. `D_DoomMain` ends in `D_DoomLoop`, which opens the screen and then
   calls `D_DoomFrames`.
3. `D_DoomFrames` is in the `:prelude` of `doom.atm`. It is the
   environment of the program: it starts the runtime of Atmos, runs the
   main chunk, and then emits `:frame` forever.
4. The main chunk reacts to each `:frame`. Today it only calls
   `D_RunFrame`, which is still in C.
5. The game ends inside the engine: `I_Quit` exits the process.

The plan had it the other way: the `main` in the program, which would
call `D_DoomMain` and get the control back. It does not work with the
engine as it is: `D_DoomMain` cannot return. With `-playdemo`, the name
of the demo is in a local variable of `D_DoomMain` (`demolumpname`), and
the engine keeps a pointer to it until the demo ends. A first version of
this seam made `D_DoomLoop` return, and the three demos failed at their
last tic, in `W_ReleaseLumpName`, with the name already overwritten.
So the engine stays below the program in the stack, and nothing of it is
duplicated in the program.

## What crosses

From the environment to the program:

| Event | When |
|-------|------|
| `:frame` | Once per frame, forever |

From the program to the engine, as native calls:

| Call | What |
|------|------|
| `D_RunFrame()` | A whole frame: input, the tics that are due, sound, drawing |

From the engine to the program: nothing yet.

## An emit from inside a call

The next steps need the engine to emit to the program while the program
is inside a call to the engine: the program calls the engine to run the
tics of a frame, and the engine emits `:tic` at each one.

The runtime accepts it. This program was tried on the compiler at
`5171bd1`, and prints what is expected (two tics inside each frame, and
the third tic counted across frames):

```
```:prelude
static void engine_frame (void) {       // the engine: two tics per frame
    for (int i=0; i<2; i++) {
        AVal e = a_strz("tic");
        atm_emit(e);
        a_dec(e);
    }
}
```         ;; and a `main` that emits :frame three times

spawn {
    var n = 0
    loop on :tic {
        set n += 1
        print("tic", n)
    }
}
spawn {
    watching 3, :tic {
        await(false)
    }
    print("third tic")
}
loop on :frame {
    print("frame >")
    `engine_frame();`
    print("frame <")
}
```

The task that made the call is running, and so it does not react to
what is emitted inside the call. The tasks that take the tics must be
other tasks.

## The test platform is not part of the seam

`out/doom-tst` has three files of `tst/` in the place of three files of
the engine (`i_video.c`, `i_timer.c`, `z_zone.c`), which are not edited.
See the comments of those files, and the readme.

## The engine all in C

`c/main.c` has `D_DoomFrames` as the loop that the engine had. With it
in the place of the program, `out/doom-c` and `out/doom-c-tst` are the
engine with no Atmos, to compare (`make test-c`, and the benchmark).
