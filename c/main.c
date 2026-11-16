// The engine all in C: what the program in Atmos (doom.atm) takes the
// place of. The functions below left engine/src/doom/d_main.c, and are
// here as they were.
//
// It builds out/doom-c and out/doom-c-tst, which exist to compare: the
// expected outputs of tst/ come from them, and the benchmark has them
// as the baseline.

#include <string.h>
#include <strings.h>

#include "deh_main.h"
#include "doomdef.h"
#include "doomstat.h"
#include "dstrings.h"
#include "sounds.h"

#include "d_loop.h"
#include "i_timer.h"
#include "i_video.h"
#include "s_sound.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

#include "d_main.h"
#include "f_finale.h"
#include "f_wipe.h"
#include "g_game.h"
#include "m_menu.h"

boolean D_Display (void);

void G_DoLoadLevel (void);
void G_DoNewGame (void);
void G_DoSaveGame (void);
void G_DoPlayDemo (void);
void G_DoCompleted (void);
void G_DoWorldDone (void);

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

// From engine/src/doom/d_main.c: the cycle of the title.

//
//  DEMO LOOP
//
int             demosequence;
int             pagetic;
const char                    *pagename;


//
// D_PageTicker
// Handles timing for warped projection
//
void D_PageTicker (void)
{
    if (--pagetic < 0)
	D_AdvanceDemo ();
}



//
// D_PageDrawer
//
void D_PageDrawer (void)
{
    V_DrawPatch (0, 0, W_CacheLumpName(pagename, PU_CACHE));
}


//
// D_AdvanceDemo
// Called after each demo or intro demosequence finishes
//
void D_AdvanceDemo (void)
{
    advancedemo = true;
}


//
// This cycles through the demo sequences.
// FIXME - version dependend demo numbers?
//
void D_DoAdvanceDemo (void)
{
    players[consoleplayer].playerstate = PST_LIVE;  // not reborn
    advancedemo = false;
    usergame = false;               // no save / end game here
    paused = false;
    gameaction = ga_nothing;

    // The Ultimate Doom executable changed the demo sequence to add
    // a DEMO4 demo.  Final Doom was based on Ultimate, so also
    // includes this change; however, the Final Doom IWADs do not
    // include a DEMO4 lump, so the game bombs out with an error
    // when it reaches this point in the demo sequence.

    // However! There is an alternate version of Final Doom that
    // includes a fixed executable.

    if (gameversion == exe_ultimate || gameversion == exe_final)
      demosequence = (demosequence+1)%7;
    else
      demosequence = (demosequence+1)%6;
    
    switch (demosequence)
    {
      case 0:
	if ( gamemode == commercial )
	    pagetic = TICRATE * 11;
	else
	    pagetic = 170;
	gamestate = GS_DEMOSCREEN;
	pagename = DEH_String("TITLEPIC");
	if ( gamemode == commercial )
	  S_StartMusic(mus_dm2ttl);
	else
	  S_StartMusic (mus_intro);
	break;
      case 1:
	G_DeferedPlayDemo(DEH_String("demo1"));
	break;
      case 2:
	pagetic = 200;
	gamestate = GS_DEMOSCREEN;
	pagename = DEH_String("CREDIT");
	break;
      case 3:
	G_DeferedPlayDemo(DEH_String("demo2"));
	break;
      case 4:
	gamestate = GS_DEMOSCREEN;
	if ( gamemode == commercial)
	{
	    pagetic = TICRATE * 11;
	    pagename = DEH_String("TITLEPIC");
	    S_StartMusic(mus_dm2ttl);
	}
	else
	{
	    pagetic = 200;

	    if (gameversion >= exe_ultimate)
	      pagename = DEH_String("CREDIT");
	    else
	      pagename = DEH_String("HELP2");
	}
	break;
      case 5:
	G_DeferedPlayDemo(DEH_String("demo3"));
	break;
        // THE DEFINITIVE DOOM Special Edition demo
      case 6:
	G_DeferedPlayDemo(DEH_String("demo4"));
	break;
    }

    // The Doom 3: BFG Edition version of doom2.wad does not have a
    // TITLETPIC lump. Use INTERPIC instead as a workaround.
    if (gamevariant == bfgedition && !strcasecmp(pagename, "TITLEPIC")
        && W_CheckNumForName("titlepic") < 0)
    {
        pagename = DEH_String("INTERPIC");
    }
}



//
// D_StartTitle
//
void D_StartTitle (void)
{
    gameaction = ga_nothing;
    demosequence = -1;
    D_AdvanceDemo ();
}

// From engine/src/doom/g_game.c: what G_Ticker had, to do the actions
// that were asked to the game.

void G_DoActions (void)
{
    // do things to change the game state
    while (gameaction != ga_nothing) 
    { 
	switch (gameaction) 
	{ 
	  case ga_loadlevel: 
	    G_DoLoadLevel (); 
	    break; 
	  case ga_newgame: 
	    G_DoNewGame (); 
	    break; 
	  case ga_loadgame: 
	    G_DoLoadGame (); 
	    break; 
	  case ga_savegame: 
	    G_DoSaveGame (); 
	    break; 
	  case ga_playdemo: 
	    G_DoPlayDemo (); 
	    break; 
	  case ga_completed: 
	    G_DoCompleted (); 
	    break; 
	  case ga_victory: 
	    F_StartFinale (); 
	    break; 
	  case ga_worlddone: 
	    G_DoWorldDone (); 
	    break; 
	  case ga_screenshot: 
	    V_ScreenShot("DOOM%02i.%s"); 
            players[consoleplayer].message = DEH_String("screen shot");
	    gameaction = ga_nothing; 
	    break; 
	  case ga_nothing: 
	    break; 
	} 
    }
}
