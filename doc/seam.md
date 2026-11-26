# The seam

Where the engine in C and the program in Atmos meet: every change made
to the engine, and what crosses in each direction.

`engine/` is a copy of Chocolate Doom whose first commit (`8f5ee02`) has
nothing changed. So this command shows every change:

```
git diff 8f5ee02 -- engine/
```

## Changes to the engine

Five files are changed, and four files left the build.

| File | Change | Why |
|------|--------|-----|
| `src/doom/d_main.c`, `D_DoomLoop` | The loop `while (1) { D_RunFrame(); }` becomes one call, `D_DoomFrames();` | The loop of frames is control: it goes to the program |
| `src/doom/d_main.c`, the prototypes | `void D_DoomFrames (void);` | The declaration of the call above |
| `src/doom/d_main.c`, `D_RunFrame` | The function is removed (48 lines), with nothing rewritten around it | A frame, and the wipe that takes frames, are control: in `doom.atm`, the main loop and the task `Wipe` |
| `src/doom/d_main.c`, the "demo loop" | Removed (138 lines): `demosequence`, `pagetic`, `pagename`, and `D_PageTicker`, `D_PageDrawer`, `D_AdvanceDemo`, `D_DoAdvanceDemo`, `D_StartTitle` | The cycle of the title is control: the task `Title` of `flow.atm` |
| `src/doom/g_game.c`, `G_Ticker` | The loop that does the actions of `gameaction` (36 lines) becomes one call, `G_DoActions ();`, with its prototype | What comes after what is control: the task `Game` of `flow.atm` |
| `src/doom/st_stuff.c`, the face | `ST_calcPainOffset` and `ST_updateFaceWidget` are removed (they are in `c/main.c` as they were), with a prototype of the second; `plyr`, `st_oldhealth`, `oldweaponsowned`, `st_facecount`, `st_faceindex`, and `st_randomnumber` lose their `static` | The face is in `st.atm`, and reads and writes these variables, which the rest of the status bar keeps |
| `src/doom/hu_stuff.c`, the messages | `HU_Start`, `HU_Ticker`, `HU_Responder`, `StartChatInput`, and `StopChatInput` are removed (they are in `c/main.c` as they were); `plr`, `w_title`, `w_chat`, `always_off`, `chat_dest`, `w_inputbuffer`, `message_on`, `message_nottobefuckedwith`, `w_message`, `message_counter`, and `headsupactive` lose their `static` | The messages and the chat are in `hu.atm`, which keeps the widgets of the file. The last two are only for `c/main.c` |
| `src/doom/am_map.c`, the keys | `AM_Responder` and `AM_Ticker` are removed (they are in `c/main.c` as they were); `cheating`, `grid`, `amclock`, `m_paninc`, `mtof_zoommul`, `ftom_zoommul`, `f_oldloc`, `scale_ftom`, `markpointnum`, and `followplayer` lose their `static` (`plr` keeps it: it has the name of the `plr` of `hu_stuff.c`, and is the same player) | The keys of the automap are in `am.atm` |
| `src/doom/f_wipe.c` | Out of the build of the program, not edited | The screen wipe is in `wipe.atm`. The file is still in the build of the engine all in C |
| `src/doom/wi_stuff.c` | Out of the build of the program, not edited | The intermission is in `wi.atm`. The same |
| `src/doom/f_finale.c` | Out of the build of the program, not edited | The finale is in `fi.atm`. The same |
| `src/doom/m_menu.c` | Out of the build of the program, not edited | The menus are in `menu.atm`. The same |

Every function removed from the engine is in `c/main.c` as it was, for
the engine all in C.

`g_game.c` is the file the plan feared: it is the flow, the commands of
the player, the demos, and the saves. Only the dispatch left it. The
functions that do each action (`G_DoNewGame`, `G_DoLoadGame`,
`G_DoSaveGame`, `G_DoPlayDemo`, `G_DoCompleted`, `G_DoWorldDone`,
`G_DoLoadLevel`) stay, and the program calls them. `G_DoCompleted` also
decides the next level and the victory; it stays because it reads three
tables of par times that are `static` in the file.

## Who runs

1. The `main` is the one of the engine (`i_main.c`), which calls
   `D_DoomMain`. The engine reads its arguments, loads the WAD, and
   starts all its parts, as before.
2. The program starts at the first call that the engine makes to it
   (`doom_start`, in `doom.atm`): the runtime of Atmos, and the main
   chunk up to its first await. This call comes from `D_DoomMain`
   (`D_StartTitle`), or from the first tic that `D_DoomLoop` runs
   before the loop of frames (`G_DoActions`), or else from
   `D_DoomFrames`.
3. `D_DoomMain` ends in `D_DoomLoop`, which opens the screen and then
   calls `D_DoomFrames`, in the `:prelude` of `doom.atm`. It is the
   environment of the program: it emits `:frame` forever.
4. The main chunk reacts to each `:frame`: it calls the engine for the
   input, the tics that are due, the sound, and the drawing.
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
| `I_StartFrame()`, `TryRunTics()`, `S_UpdateSounds(...)` | The input, the tics of the game that are due, the sound |
| `D_Display()` | Draws the game. It answers whether a wipe must start |
| `I_UpdateNoBlit()`, `M_Drawer()`, `I_FinishUpdate()` | The menu on top, and the end of a frame |
| `I_GetTime()`, `I_Sleep(1)` | The clock, for the wipe |
| `M_Random()` | The delays of the columns of the wipe |
| `I_ReadScreen(...)`, `V_DrawBlock(...)`, `I_VideoBuffer`, `Z_Malloc`, `Z_Free` | The pixels of the wipe, in C functions of the `:prelude` |
| `screenvisible`, `nodrawers`, `players[consoleplayer].mo` | Variables of the engine that the frame reads |
| `V_DrawPatch(...)`, `W_CacheLumpName(...)`, `W_CheckNumForName(...)` | The graphics of the intermission, by their names |
| `S_StartSound(...)`, `S_ChangeMusic(...)` | The sounds of the intermission |
| `G_WorldDone()` | The intermission is over: the next level may start |
| `M_Random()` | The first frame of each animation of the map |
| `wipegamestate = -1`, `gameaction = ga_worlddone`, `gamestate`, `viewactive`, `automapactive` | Variables of the engine that the finale writes: a wipe is forced, the next level is asked |
| `G_DoNewGame()`, `G_DoLoadGame()`, `G_DoPlayDemo()`, `G_DeferedPlayDemo(...)`, `G_DoCompleted()`, `G_DoWorldDone()`, `G_DoLoadLevel()`, `G_DoSaveGame()`, `V_ScreenShot(...)`, `S_StartMusic(...)` | The flow does each action with the function of the engine |
| `gameaction`, `advancedemo`, `usergame`, `paused`, `gamestate`, `viewactive` | Variables of the engine that the flow reads and writes, as the C did |
| `states[]`, `mobjinfo[]`, `sprites[]` | The table of states, read by the cast of the finale |
| `gameepisode`, `gamemap`, `gamemission` | What the finale reads to choose its screens |
| `WI_WBS->...`, `players[]`, `playeringame[]`, `gamemode`, `gameversion`, `deathmatch`, `netgame` | What the intermission reads: the statistics of the level, the buttons of the players, the kind of game |

From the engine to the program:

| Call | What |
|------|------|
| `D_DoomFrames()` | Once: the engine gives the control to the program |
| `wipe_StartScreen(...)` | `D_Display` is about to draw a screen that changes the state of the game. A C function of the `:prelude`, which keeps the old screen; it emits nothing |
| `WI_Start(wbs)` | A level ended (`G_DoCompleted`). Emits `:wi.start` |
| `WI_Ticker()` | A tic of the game in the intermission (`G_Ticker`). Emits `:wi.tic` |
| `WI_Drawer()` | The intermission must be drawn (`D_Display`). Emits `:wi.draw` |
| `WI_End()` | The game left the intermission (`G_Ticker`). Emits `:wi.end` |
| `G_DoActions()` | The start of each tic (`G_Ticker`). For each action in `gameaction`, emits its event (`:g.newgame`, `:g.loadgame`, `:g.playdemo`, `:g.completed`, `:g.victory`, `:g.worlddone`, `:g.loadlevel`, `:g.savegame`, `:g.screenshot`). An action that no task does is an error |
| `D_StartTitle()` | The title starts (`D_DoomMain`, the menu that ends a game). Emits `:title.start` |
| `D_DoAdvanceDemo()` | The next step of the title (`RunTic`, when `advancedemo` is set). Emits `:title.next` |
| `D_PageTicker()`, `D_PageDrawer()` | A tic, and a frame, of a page of the title. Emit `:page.tic` and `:page.draw` |
| `D_AdvanceDemo()` | A demo ended (`G_CheckDemoStatus`). Sets `advancedemo`, as before; emits nothing |
| `ST_updateFaceWidget()` | A tic of the status bar (`ST_Ticker`). Emits `:st.face` |
| `HU_Start()` | A level starts (`P_SpawnPlayer`). Makes the widgets, as before, and emits `:hu.start` |
| `HU_Ticker()` | A tic of a level (`G_Ticker`). Emits `:hu.tic`, then `:hu.check` |
| `HU_Responder(ev)` | An input event in a level (`G_Responder`). The shifts and the alts are handled in C; a key down emits `:hu.key`, and the function answers what the task Hud set in `hu_eat` |
| `M_Responder(ev)` | An input event, before the game sees it (`D_ProcessEvents`). The test of the controls, the mouse and the joystick stay in C; the close button emits `:m.quit`; a key emits `:m.key`, and the function answers what the task Menus left in `m_eat` |
| `M_Ticker()`, `M_Drawer()` | A tic, and a frame (over everything). Emit `:m.tic` and `:m.draw` |
| `M_StartControlPanel()` | The engine opens the main menu (a key on the title or in a demo, `G_Responder`). Emits `:m.open` |
| `M_Init()` | Nothing: the menus are made when the program starts, after it (`M_Init` comes before the game that `-warp` starts, and the program must not start there) |
| `AM_Responder(ev)` | An input event in a level (`G_Responder`). Emits `:am.key`, and the function answers what the task Automap left in `am_eat` |
| `AM_Ticker()` | A tic of a level (`G_Ticker`). Emits `:am.tic` |
| `F_StartFinale()` | An episode ended (`G_Ticker`, `G_DoCompleted`). Sets the state of the game, chooses the text, and emits `:fi.start` |
| `F_Ticker()` | A tic of the game in the finale (`G_Ticker`). Emits `:fi.tic` |
| `F_Drawer()` | The finale must be drawn (`D_Display`). Emits `:fi.draw` |
| `F_Responder(ev)` | An input event in the finale (`G_Responder`). During the cast, a key down emits `:fi.key`, and the function answers that it took the event |

These calls come while the program is inside a call to the engine
(`TryRunTics`, `D_Display`). The status of their emits is kept in a
variable of the `:prelude`, which the literal that made the call takes:
`` `TryRunTics(); st = a_chk_aborted(DOOM_ST);` ``.

## An emit from inside a call

The intermission needs the engine to emit to the program while the program
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

## The variant (var.atm)

A second program, for the study only (`make var`, `make test-var`). It
changes nothing of the engine either: two more files leave its build and
are included, unedited, in the program, with some functions renamed by
the preprocessor, which the program then defines.

| File | In the variant | The program defines |
|------|----------------|---------------------|
| `src/doom/p_doors.c` | Included by `var/doors.atm`, with `EV_DoDoor` and `EV_VerticalDoor` renamed | `EV_DoDoor`, `EV_VerticalDoor`: a task for each normal door, the rest to the originals |
| `src/doom/p_mobj.c` | Included by `var/zombie.atm`, with `P_SetMobjState`, `P_SpawnMobj`, and `P_SpawnMapThing` renamed | The three: a task for each zombieman, the rest to the originals |

What crosses, besides the main build:

| Direction | What |
|-----------|------|
| Engine to program | `T_DoorTask`, the thinker of a door: `:door.tic` to its task. `EV_VerticalDoor` on such a door: `:door.use.player` or `:door.use.monster` to its task |
| Engine to program | `Z_Thinker`, the thinker of a zombieman: it moves as `P_MobjThinker`, then `:z.tic` to its task. `P_SetMobjState` on a zombieman from outside its action: `:z.jump` to its task |
| Engine to program | `:door.spawn`, `:z.spawn`: a task is born |
| Program to engine | `T_MovePlane`, `S_StartSound`, `P_RemoveThinker` (the door); `A_Look`, `A_Chase`, `A_FaceTarget`, `A_PosAttack`, `A_Pain`, `A_Scream`, `A_XScream`, `A_Fall` (the zombieman); the rows of the table, as data |

Every comparison of a thinker with `P_MobjThinker` in the engine
(`A_BossDeath`, `A_KeenDie`, the teleport, the precache, the saved game)
does not see the zombieman of the variant, whose thinker is another. None
of them is about the zombieman, but for the precache of its sprites,
which are then read when first drawn. The test platform counts it as an
object (`tst/i_video.c`, a weak symbol).
