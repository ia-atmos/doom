// The test platform: the timer. It takes the place of engine/src/i_timer.c.
//
// There is no real time. The clock is the number of frames shown: it
// advances by one tic at each I_FinishUpdate (see i_video.c). So a run
// is the same on any machine, and the screen wipe, which the engine
// paces by the clock, takes one step per frame.
//
// With `-tst-tics` (see i_video.c), the engine may wait for the clock
// in a sleep; a sleep then passes one tic, so that the wait ends.

#include "i_timer.h"
#include "doomtype.h"
#include "d_loop.h"

int tst_clock = 0;      // in tics (1/35 s)

int I_GetTime (void)
{
    return tst_clock;
}

int I_GetTimeMS (void)
{
    return (tst_clock * 1000) / TICRATE;
}

void I_Sleep (int ms)
{
    if (!singletics)
    {
        tst_clock++;
    }
}

void I_WaitVBL (int count)
{
}

void I_InitTimer (void)
{
}
