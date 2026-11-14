// The engine all in C: what the program in Atmos (doom.atm) takes the
// place of. The functions below left engine/src/doom/d_main.c, and are
// here as they were.
//
// It builds out/doom-c and out/doom-c-tst, which exist to compare: the
// expected outputs of tst/ come from them, and the benchmark has them
// as the baseline.

#include "doomdef.h"
#include "doomstat.h"

#include "d_loop.h"
#include "i_timer.h"
#include "i_video.h"
#include "s_sound.h"

#include "f_wipe.h"
#include "m_menu.h"

boolean D_Display (void);

//
//  D_RunFrame
//
void D_RunFrame()
{
    int nowtime;
    int tics;
    static int wipestart;
    static boolean wipe;

    if (wipe)
    {
        do
        {
            nowtime = I_GetTime ();
            tics = nowtime - wipestart;
            I_Sleep(1);
        } while (tics <= 0);

        wipestart = nowtime;
        wipe = !wipe_ScreenWipe(wipe_Melt
                               , 0, 0, SCREENWIDTH, SCREENHEIGHT, tics);
        I_UpdateNoBlit ();
        M_Drawer ();                            // menu is drawn even on top of wipes
        I_FinishUpdate ();                      // page flip or blit buffer
        return;
    }

    // frame syncronous IO operations
    I_StartFrame ();

    TryRunTics (); // will run at least one tic

    S_UpdateSounds (players[consoleplayer].mo);// move positional sounds

    // Update display, next frame, with current state if no profiling is on
    if (screenvisible && !nodrawers)
    {
        if ((wipe = D_Display ()))
        {
            // start wipe on this frame
            wipe_EndScreen(0, 0, SCREENWIDTH, SCREENHEIGHT);

            wipestart = I_GetTime () - 1;
        } else {
            // normal update
            I_FinishUpdate ();              // page flip or blit buffer
        }
    }
}

// The loop that D_DoomLoop had.

void D_DoomFrames (void)
{
    while (1)
    {
        D_RunFrame();
    }
}
