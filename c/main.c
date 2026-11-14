// The engine all in C: the loop of frames, which the program in Atmos
// (doom.atm) takes the place of. It is the loop that
// engine/src/doom/d_main.c had in D_DoomLoop.
//
// It builds out/doom-c and out/doom-c-tst, which exist to compare: the
// expected outputs of tst/ come from them, and the benchmark has them
// as the baseline.

void D_RunFrame (void);

void D_DoomFrames (void)
{
    while (1)
    {
        D_RunFrame();
    }
}
