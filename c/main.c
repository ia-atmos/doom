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
#include "r_main.h"

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

// From engine/src/doom/st_stuff.c: the face of the status bar, with the
// constants and the variables that it uses there.

#define ST_NUMPAINFACES		5
#define ST_NUMSTRAIGHTFACES	3
#define ST_NUMTURNFACES		2
#define ST_NUMSPECIALFACES		3

#define ST_FACESTRIDE \
          (ST_NUMSTRAIGHTFACES+ST_NUMTURNFACES+ST_NUMSPECIALFACES)

#define ST_NUMEXTRAFACES		2

#define ST_NUMFACES \
          (ST_FACESTRIDE*ST_NUMPAINFACES+ST_NUMEXTRAFACES)

#define ST_TURNOFFSET		(ST_NUMSTRAIGHTFACES)
#define ST_OUCHOFFSET		(ST_TURNOFFSET + ST_NUMTURNFACES)
#define ST_EVILGRINOFFSET		(ST_OUCHOFFSET + 1)
#define ST_RAMPAGEOFFSET		(ST_EVILGRINOFFSET + 1)
#define ST_GODFACE			(ST_NUMPAINFACES*ST_FACESTRIDE)
#define ST_DEADFACE			(ST_GODFACE+1)

#define ST_EVILGRINCOUNT		(2*TICRATE)
#define ST_STRAIGHTFACECOUNT	(TICRATE/2)
#define ST_TURNCOUNT		(1*TICRATE)
#define ST_OUCHCOUNT		(1*TICRATE)
#define ST_RAMPAGEDELAY		(2*TICRATE)

#define ST_MUCHPAIN			20

extern player_t* plyr;
extern int st_oldhealth;
extern boolean oldweaponsowned[NUMWEAPONS];
extern int st_facecount;
extern int st_faceindex;
extern int st_randomnumber;

int ST_calcPainOffset(void)
{
    int		health;
    static int	lastcalc;
    static int	oldhealth = -1;
    
    health = plyr->health > 100 ? 100 : plyr->health;

    if (health != oldhealth)
    {
	lastcalc = ST_FACESTRIDE * (((100 - health) * ST_NUMPAINFACES) / 101);
	oldhealth = health;
    }
    return lastcalc;
}


//
// This is a not-very-pretty routine which handles
//  the face states and their timing.
// the precedence of expressions is:
//  dead > evil grin > turned head > straight ahead
//
void ST_updateFaceWidget(void)
{
    int		i;
    angle_t	badguyangle;
    angle_t	diffang;
    static int	lastattackdown = -1;
    static int	priority = 0;
    boolean	doevilgrin;

    if (priority < 10)
    {
	// dead
	if (!plyr->health)
	{
	    priority = 9;
	    st_faceindex = ST_DEADFACE;
	    st_facecount = 1;
	}
    }

    if (priority < 9)
    {
	if (plyr->bonuscount)
	{
	    // picking up bonus
	    doevilgrin = false;

	    for (i=0;i<NUMWEAPONS;i++)
	    {
		if (oldweaponsowned[i] != plyr->weaponowned[i])
		{
		    doevilgrin = true;
		    oldweaponsowned[i] = plyr->weaponowned[i];
		}
	    }
	    if (doevilgrin) 
	    {
		// evil grin if just picked up weapon
		priority = 8;
		st_facecount = ST_EVILGRINCOUNT;
		st_faceindex = ST_calcPainOffset() + ST_EVILGRINOFFSET;
	    }
	}

    }
  
    if (priority < 8)
    {
	if (plyr->damagecount
	    && plyr->attacker
	    && plyr->attacker != plyr->mo)
	{
	    // being attacked
	    priority = 7;
	    
	    if (plyr->health - st_oldhealth > ST_MUCHPAIN)
	    {
		st_facecount = ST_TURNCOUNT;
		st_faceindex = ST_calcPainOffset() + ST_OUCHOFFSET;
	    }
	    else
	    {
		badguyangle = R_PointToAngle2(plyr->mo->x,
					      plyr->mo->y,
					      plyr->attacker->x,
					      plyr->attacker->y);
		
		if (badguyangle > plyr->mo->angle)
		{
		    // whether right or left
		    diffang = badguyangle - plyr->mo->angle;
		    i = diffang > ANG180; 
		}
		else
		{
		    // whether left or right
		    diffang = plyr->mo->angle - badguyangle;
		    i = diffang <= ANG180; 
		} // confusing, aint it?

		
		st_facecount = ST_TURNCOUNT;
		st_faceindex = ST_calcPainOffset();
		
		if (diffang < ANG45)
		{
		    // head-on    
		    st_faceindex += ST_RAMPAGEOFFSET;
		}
		else if (i)
		{
		    // turn face right
		    st_faceindex += ST_TURNOFFSET;
		}
		else
		{
		    // turn face left
		    st_faceindex += ST_TURNOFFSET+1;
		}
	    }
	}
    }
  
    if (priority < 7)
    {
	// getting hurt because of your own damn stupidity
	if (plyr->damagecount)
	{
	    if (plyr->health - st_oldhealth > ST_MUCHPAIN)
	    {
		priority = 7;
		st_facecount = ST_TURNCOUNT;
		st_faceindex = ST_calcPainOffset() + ST_OUCHOFFSET;
	    }
	    else
	    {
		priority = 6;
		st_facecount = ST_TURNCOUNT;
		st_faceindex = ST_calcPainOffset() + ST_RAMPAGEOFFSET;
	    }

	}

    }
  
    if (priority < 6)
    {
	// rapid firing
	if (plyr->attackdown)
	{
	    if (lastattackdown==-1)
		lastattackdown = ST_RAMPAGEDELAY;
	    else if (!--lastattackdown)
	    {
		priority = 5;
		st_faceindex = ST_calcPainOffset() + ST_RAMPAGEOFFSET;
		st_facecount = 1;
		lastattackdown = 1;
	    }
	}
	else
	    lastattackdown = -1;

    }
  
    if (priority < 5)
    {
	// invulnerability
	if ((plyr->cheats & CF_GODMODE)
	    || plyr->powers[pw_invulnerability])
	{
	    priority = 4;

	    st_faceindex = ST_GODFACE;
	    st_facecount = 1;

	}

    }

    // look left or look right if the facecount has timed out
    if (!st_facecount)
    {
	st_faceindex = ST_calcPainOffset() + (st_randomnumber % 3);
	st_facecount = ST_STRAIGHTFACECOUNT;
	priority = 0;
    }

    st_facecount--;

}

// From engine/src/doom/am_map.c: the keys of the automap and its tic,
// with the constants and the variables that they use there.

#include <limits.h>
#include "am_map.h"
#include "doomkeys.h"
#include "dstrings.h"
#include "i_joystick.h"
#include "m_controls.h"
#include "m_misc.h"
#include "m_cheat.h"
#include "m_fixed.h"

typedef struct
{
    fixed_t x,y;
} mpoint_t;

#define F_PANINC	4
#define M_ZOOMIN        ((int) (1.02*FRACUNIT))
#define M_ZOOMOUT       ((int) (FRACUNIT/1.02))
#define FTOM(x) FixedMul(((x)<<FRACBITS),scale_ftom)

extern int		cheating;
extern int		grid;
extern int		amclock;
extern mpoint_t		m_paninc;
extern fixed_t		mtof_zoommul;
extern fixed_t		ftom_zoommul;
extern mpoint_t		f_oldloc;
extern fixed_t		scale_ftom;
extern int		markpointnum;
extern int		followplayer;
extern cheatseq_t	cheat_amap;
// `plr` of am_map.c stays `static` there; it is &players[consoleplayer]
// while a level runs, which the functions below use by this name.
#define plr (&players[consoleplayer])

void AM_Start (void);
void AM_saveScaleAndLoc(void);
void AM_restoreScaleAndLoc(void);
void AM_minOutWindowScale(void);
void AM_addMark(void);
void AM_clearMarks(void);
void AM_changeWindowScale(void);
void AM_doFollowPlayer(void);
void AM_changeWindowLoc(void);

//
// Handle events (user inputs) in automap mode
//
boolean
AM_Responder
( event_t*	ev )
{

    int rc;
    static int bigstate=0;
    static char buffer[20];
    int key;

    rc = false;

    if (ev->type == ev_joystick && joybautomap >= 0
        && (ev->data1 & (1 << joybautomap)) != 0)
    {
        joywait = I_GetTime() + 5;

        if (!automapactive)
        {
            AM_Start ();
            viewactive = false;
        }
        else
        {
            bigstate = 0;
            viewactive = true;
            AM_Stop ();
        }

        return true;
    }

    if (!automapactive)
    {
	if (ev->type == ev_keydown && ev->data1 == key_map_toggle)
	{
	    AM_Start ();
	    viewactive = false;
	    rc = true;
	}
    }
    else if (ev->type == ev_keydown)
    {
	rc = true;
        key = ev->data1;

        if (key == key_map_east)          // pan right
        {
            if (!followplayer) m_paninc.x = FTOM(F_PANINC);
            else rc = false;
        }
        else if (key == key_map_west)     // pan left
        {
            if (!followplayer) m_paninc.x = -FTOM(F_PANINC);
            else rc = false;
        }
        else if (key == key_map_north)    // pan up
        {
            if (!followplayer) m_paninc.y = FTOM(F_PANINC);
            else rc = false;
        }
        else if (key == key_map_south)    // pan down
        {
            if (!followplayer) m_paninc.y = -FTOM(F_PANINC);
            else rc = false;
        }
        else if (key == key_map_zoomout)  // zoom out
        {
            mtof_zoommul = M_ZOOMOUT;
            ftom_zoommul = M_ZOOMIN;
        }
        else if (key == key_map_zoomin)   // zoom in
        {
            mtof_zoommul = M_ZOOMIN;
            ftom_zoommul = M_ZOOMOUT;
        }
        else if (key == key_map_toggle)
        {
            bigstate = 0;
            viewactive = true;
            AM_Stop ();
        }
        else if (key == key_map_maxzoom)
        {
            bigstate = !bigstate;
            if (bigstate)
            {
                AM_saveScaleAndLoc();
                AM_minOutWindowScale();
            }
            else AM_restoreScaleAndLoc();
        }
        else if (key == key_map_follow)
        {
            followplayer = !followplayer;
            f_oldloc.x = INT_MAX;
            if (followplayer)
                plr->message = DEH_String(AMSTR_FOLLOWON);
            else
                plr->message = DEH_String(AMSTR_FOLLOWOFF);
        }
        else if (key == key_map_grid)
        {
            grid = !grid;
            if (grid)
                plr->message = DEH_String(AMSTR_GRIDON);
            else
                plr->message = DEH_String(AMSTR_GRIDOFF);
        }
        else if (key == key_map_mark)
        {
            M_snprintf(buffer, sizeof(buffer), "%s %d",
                       DEH_String(AMSTR_MARKEDSPOT), markpointnum);
            plr->message = buffer;
            AM_addMark();
        }
        else if (key == key_map_clearmark)
        {
            AM_clearMarks();
            plr->message = DEH_String(AMSTR_MARKSCLEARED);
        }
        else
        {
            rc = false;
        }

        if ((!deathmatch || gameversion <= exe_doom_1_8)
         && cht_CheckCheat(&cheat_amap, ev->data2))
        {
            rc = false;
            cheating = (cheating + 1) % 3;
        }
    }
    else if (ev->type == ev_keyup)
    {
        rc = false;
        key = ev->data1;

        if (key == key_map_east)
        {
            if (!followplayer) m_paninc.x = 0;
        }
        else if (key == key_map_west)
        {
            if (!followplayer) m_paninc.x = 0;
        }
        else if (key == key_map_north)
        {
            if (!followplayer) m_paninc.y = 0;
        }
        else if (key == key_map_south)
        {
            if (!followplayer) m_paninc.y = 0;
        }
        else if (key == key_map_zoomout || key == key_map_zoomin)
        {
            mtof_zoommul = FRACUNIT;
            ftom_zoommul = FRACUNIT;
        }
    }

    return rc;

}


//
// Updates on Game Tick
//
void AM_Ticker (void)
{

    if (!automapactive)
	return;

    amclock++;

    if (followplayer)
	AM_doFollowPlayer();

    // Change the zoom if necessary
    if (ftom_zoommul != FRACUNIT)
	AM_changeWindowScale();

    // Change x,y location
    if (m_paninc.x || m_paninc.y)
	AM_changeWindowLoc();

    // Update light level
    // AM_updateLightLev();

}


