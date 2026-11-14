// The test platform: video and input. It takes the place of
// engine/src/i_video.c.
//
// - No window: the engine draws into I_VideoBuffer (320x200, 8 bits per
//   pixel), and nothing shows it.
// - One tic per frame: `singletics` of the engine is set, and the clock
//   advances by one tic per frame (see i_timer.c).
// - Input comes from a script (`-tst-script <file>`): events posted at
//   given frames.
// - Output is a trace (`-tst-trace <file>`): one line per frame, with a
//   checksum of the simulation and a checksum of the screen.
//
// A frame is one call to I_FinishUpdate. Frames are counted from 0.
//
// The script, one command per line (`#` starts a comment):
//
//     <frame> down  <key>      the key goes down
//     <frame> up    <key>      the key goes up
//     <frame> press <key>      down at <frame>, up at <frame>+1
//     <frame> shot  <file>     writes the screen of <frame> (a PPM file)
//     <frame> quit             ends the run after <frame>
//
// A key is a name of the table below, or a single character.
//
// The trace, one line per frame:
//
//     <frame> <gametic> <gamestate> <rndindex> <prndindex> <sim> <screen> <x> <y> <angle>
//
// <sim> covers the objects (position, momentum, health, row, tics), the
// sectors (heights, light, special), and the players (health, armor,
// ammo, weapons, rows of the weapon sprites). <screen> covers the 64000
// pixels and the palette in use. <x> <y> <angle> are the position of the
// player (map units, degrees) inside a level, for who reads the trace:
// <sim> already covers them.

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "doomtype.h"
#include "doomkeys.h"
#include "d_event.h"
#include "d_loop.h"
#include "i_system.h"
#include "i_video.h"
#include "m_argv.h"
#include "doom/m_random.h"
#include "z_zone.h"

#include "doom/doomstat.h"
#include "doom/p_local.h"
#include "doom/r_state.h"

extern int tst_clock;
extern int rndindex, prndindex;     // m_random.c

// What the rest of the engine expects from i_video.c.

pixel_t *I_VideoBuffer = NULL;
boolean screenvisible = true;
boolean screensaver_mode = false;
char *video_driver = "";
char *window_position = "";
int usegamma = 0;
int screen_width = SCREENWIDTH;
int screen_height = SCREENHEIGHT;
int fullscreen = false;
int aspect_ratio_correct = false;
int integer_scaling = false;
int smooth_pixel_scaling = false;
int vga_porch_flash = false;
int force_software_renderer = false;
int png_screenshots = false;
int usemouse = 0;
unsigned int joywait = 0;

void I_GraphicsCheckCommandLine (void) {}
void I_ShutdownGraphics (void) {}
void I_UpdateNoBlit (void) {}
void I_BeginRead (void) {}
void I_SetWindowTitle (const char *title) {}
void I_CheckIsScreensaver (void) {}
void I_SetGrabMouseCallback (grabmouse_callback_t func) {}
void I_DisplayFPSDots (boolean dots_on) {}
void I_BindVideoVariables (void) {}
void I_InitWindowTitle (void) {}
void I_RegisterWindowIcon (const unsigned int *icon, int width, int height) {}
void I_InitWindowIcon (void) {}
void I_StartFrame (void) {}
void I_EnableLoadingDisk (int xoffs, int yoffs) {}

void I_GetWindowPosition (int *x, int *y, int w, int h)
{
    *x = 0;
    *y = 0;
}

// The palette.

static byte tst_palette[768];

void I_SetPalette (byte *palette)
{
    memcpy(tst_palette, palette, 768);
}

int I_GetPaletteIndex (int r, int g, int b)
{
    int best = 0;
    int best_diff = INT_MAX;
    int i;

    for (i = 0; i < 256; i++)
    {
        int dr = r - tst_palette[3*i];
        int dg = g - tst_palette[3*i+1];
        int db = b - tst_palette[3*i+2];
        int diff = dr*dr + dg*dg + db*db;

        if (diff < best_diff)
        {
            best = i;
            best_diff = diff;
        }
    }

    return best;
}

void I_ReadScreen (pixel_t *scr)
{
    memcpy(scr, I_VideoBuffer, SCREENWIDTH * SCREENHEIGHT * sizeof(*scr));
}

// The script.

typedef enum { TST_DOWN, TST_UP, TST_SHOT, TST_QUIT } tst_kind_t;

typedef struct
{
    int frame;
    tst_kind_t kind;
    int key;
    char file[256];
} tst_cmd_t;

static tst_cmd_t *tst_cmds = NULL;
static int tst_ncmds = 0;
static int tst_frame = 0;
static FILE *tst_trace = NULL;

static const struct { const char *name; int key; } tst_keys[] =
{
    { "enter",     KEY_ENTER      },
    { "escape",    KEY_ESCAPE     },
    { "tab",       KEY_TAB        },
    { "space",     ' '            },
    { "backspace", KEY_BACKSPACE  },
    { "up",        KEY_UPARROW    },
    { "down",      KEY_DOWNARROW  },
    { "left",      KEY_LEFTARROW  },
    { "right",     KEY_RIGHTARROW },
    { "ctrl",      KEY_RCTRL      },
    { "shift",     KEY_RSHIFT     },
    { "alt",       KEY_RALT       },
    { "pause",     KEY_PAUSE      },
    { "f1",        KEY_F1         },
    { "f2",        KEY_F2         },
    { "f3",        KEY_F3         },
    { "f4",        KEY_F4         },
    { "f5",        KEY_F5         },
    { "f6",        KEY_F6         },
    { "f7",        KEY_F7         },
    { "f8",        KEY_F8         },
    { "f9",        KEY_F9         },
    { "f10",       KEY_F10        },
    { "f11",       KEY_F11        },
    { "minus",     KEY_MINUS      },
    { "equals",    KEY_EQUALS     },
};

static int TST_Key (const char *name, const char *file, int line)
{
    int i;

    for (i = 0; i < arrlen(tst_keys); i++)
    {
        if (!strcmp(name, tst_keys[i].name))
        {
            return tst_keys[i].key;
        }
    }

    if (strlen(name) == 1)
    {
        return name[0];
    }

    I_Error("%s:%d: unknown key '%s'", file, line, name);
    return 0;
}

static void TST_Add (int frame, tst_kind_t kind, int key, const char *file)
{
    tst_cmd_t *cmd;

    tst_cmds = realloc(tst_cmds, (tst_ncmds + 1) * sizeof(*tst_cmds));
    cmd = &tst_cmds[tst_ncmds++];
    cmd->frame = frame;
    cmd->kind = kind;
    cmd->key = key;
    cmd->file[0] = '\0';

    if (file != NULL)
    {
        snprintf(cmd->file, sizeof(cmd->file), "%s", file);
    }
}

static void TST_Load (const char *file)
{
    char buf[512];
    int line = 0;
    FILE *f = fopen(file, "r");

    if (f == NULL)
    {
        I_Error("%s: cannot open the script", file);
    }

    while (fgets(buf, sizeof(buf), f) != NULL)
    {
        int frame;
        char cmd[32], arg[256];
        char *hash = strchr(buf, '#');
        int n;

        line++;

        if (hash != NULL)
        {
            *hash = '\0';
        }

        arg[0] = '\0';
        n = sscanf(buf, "%d %31s %255s", &frame, cmd, arg);

        if (n <= 0)
        {
            continue;       // an empty line
        }

        if (n >= 3 && !strcmp(cmd, "down"))
        {
            TST_Add(frame, TST_DOWN, TST_Key(arg, file, line), NULL);
        }
        else if (n >= 3 && !strcmp(cmd, "up"))
        {
            TST_Add(frame, TST_UP, TST_Key(arg, file, line), NULL);
        }
        else if (n >= 3 && !strcmp(cmd, "press"))
        {
            TST_Add(frame,     TST_DOWN, TST_Key(arg, file, line), NULL);
            TST_Add(frame + 1, TST_UP,   TST_Key(arg, file, line), NULL);
        }
        else if (n >= 3 && !strcmp(cmd, "shot"))
        {
            TST_Add(frame, TST_SHOT, 0, arg);
        }
        else if (n >= 2 && !strcmp(cmd, "quit"))
        {
            TST_Add(frame, TST_QUIT, 0, NULL);
        }
        else
        {
            I_Error("%s:%d: invalid command", file, line);
        }
    }

    fclose(f);
}

// Input: the events of the script whose frame is the current one.
// The engine calls I_StartTic once per frame, except during a screen
// wipe, when it reads no input: an event that falls in a wipe is posted
// at the first frame after it.

void I_StartTic (void)
{
    int i;

    for (i = 0; i < tst_ncmds; i++)
    {
        tst_cmd_t *cmd = &tst_cmds[i];

        if (cmd->frame > tst_frame || cmd->frame < 0)
        {
            continue;
        }

        if (cmd->kind == TST_DOWN || cmd->kind == TST_UP)
        {
            event_t ev;

            ev.type = (cmd->kind == TST_DOWN) ? ev_keydown : ev_keyup;
            ev.data1 = cmd->key;    // the key, as the game sees it
            ev.data2 = cmd->key;    // the key, as text typed in a menu
            ev.data3 = cmd->key;
            ev.data4 = 0;
            ev.data5 = 0;
            D_PostEvent(&ev);
            cmd->frame = -1;        // done
        }
    }
}

// The checksums (FNV-1a, 32 bits).

static unsigned int tst_sum;

static void TST_Sum (const void *data, size_t len)
{
    const byte *p = data;
    size_t i;

    for (i = 0; i < len; i++)
    {
        tst_sum = (tst_sum ^ p[i]) * 16777619u;
    }
}

static void TST_SumInt (int v)
{
    TST_Sum(&v, sizeof(v));
}

static unsigned int TST_SumSim (void)
{
    thinker_t *th;
    int i, j;

    tst_sum = 2166136261u;

    if (gamestate != GS_LEVEL)
    {
        return tst_sum;
    }

    for (th = thinkercap.next; th != &thinkercap; th = th->next)
    {
        if (th->function.acp1 == (actionf_p1) P_MobjThinker)
        {
            mobj_t *mo = (mobj_t *) th;

            TST_SumInt(mo->type);
            TST_SumInt(mo->x);
            TST_SumInt(mo->y);
            TST_SumInt(mo->z);
            TST_SumInt(mo->angle);
            TST_SumInt(mo->momx);
            TST_SumInt(mo->momy);
            TST_SumInt(mo->momz);
            TST_SumInt(mo->health);
            TST_SumInt(mo->flags);
            TST_SumInt(mo->tics);
            TST_SumInt(mo->state - states);
        }
    }

    for (i = 0; i < numsectors; i++)
    {
        TST_SumInt(sectors[i].floorheight);
        TST_SumInt(sectors[i].ceilingheight);
        TST_SumInt(sectors[i].lightlevel);
        TST_SumInt(sectors[i].special);
        TST_SumInt(sectors[i].floorpic);
    }

    for (i = 0; i < MAXPLAYERS; i++)
    {
        player_t *p = &players[i];

        if (!playeringame[i])
        {
            continue;
        }

        TST_SumInt(p->playerstate);
        TST_SumInt(p->health);
        TST_SumInt(p->armorpoints);
        TST_SumInt(p->armortype);
        TST_SumInt(p->readyweapon);
        TST_SumInt(p->pendingweapon);
        TST_SumInt(p->viewz);
        TST_SumInt(p->killcount);
        TST_SumInt(p->itemcount);
        TST_SumInt(p->secretcount);
        TST_Sum(p->ammo, sizeof(p->ammo));
        TST_Sum(p->weaponowned, sizeof(p->weaponowned));
        TST_Sum(p->cards, sizeof(p->cards));
        TST_Sum(p->powers, sizeof(p->powers));

        for (j = 0; j < NUMPSPRITES; j++)
        {
            pspdef_t *psp = &p->psprites[j];

            TST_SumInt(psp->state ? psp->state - states : -1);
            TST_SumInt(psp->tics);
            TST_SumInt(psp->sx);
            TST_SumInt(psp->sy);
        }
    }

    return tst_sum;
}

static unsigned int TST_SumScreen (void)
{
    tst_sum = 2166136261u;
    TST_Sum(I_VideoBuffer, SCREENWIDTH * SCREENHEIGHT);
    TST_Sum(tst_palette, sizeof(tst_palette));
    return tst_sum;
}

static void TST_Shot (const char *file)
{
    FILE *f = fopen(file, "wb");
    int i;

    if (f == NULL)
    {
        I_Error("%s: cannot write the screen", file);
    }

    fprintf(f, "P6\n%d %d\n255\n", SCREENWIDTH, SCREENHEIGHT);

    for (i = 0; i < SCREENWIDTH * SCREENHEIGHT; i++)
    {
        fwrite(&tst_palette[3 * I_VideoBuffer[i]], 1, 3, f);
    }

    fclose(f);
}

// The end of a frame.

void I_FinishUpdate (void)
{
    boolean quit = false;
    int i;

    if (tst_trace != NULL)
    {
        mobj_t *mo = players[consoleplayer].mo;
        boolean in = (gamestate == GS_LEVEL) && (mo != NULL);

        fprintf(tst_trace, "%d %d %d %d %d %08x %08x %d %d %d\n",
                tst_frame, gametic, (int) gamestate,
                rndindex, prndindex, TST_SumSim(), TST_SumScreen(),
                in ? mo->x >> FRACBITS : 0,
                in ? mo->y >> FRACBITS : 0,
                in ? (int) (mo->angle / (ANG90 / 90)) : 0);
    }

    for (i = 0; i < tst_ncmds; i++)
    {
        tst_cmd_t *cmd = &tst_cmds[i];

        if (cmd->frame != tst_frame)
        {
            continue;
        }

        if (cmd->kind == TST_SHOT)
        {
            TST_Shot(cmd->file);
        }
        else if (cmd->kind == TST_QUIT)
        {
            quit = true;
        }
    }

    tst_frame++;
    tst_clock++;

    if (quit)
    {
        I_Quit();
    }
}

static void TST_Close (void)
{
    if (tst_trace != NULL)
    {
        fclose(tst_trace);
        tst_trace = NULL;
    }
}

void I_InitGraphics (void)
{
    int p;

    I_VideoBuffer = Z_Malloc(SCREENWIDTH * SCREENHEIGHT * sizeof(*I_VideoBuffer),
                             PU_STATIC, NULL);
    memset(I_VideoBuffer, 0, SCREENWIDTH * SCREENHEIGHT * sizeof(*I_VideoBuffer));
    memset(tst_palette, 0, sizeof(tst_palette));

    singletics = true;

    p = M_CheckParmWithArgs("-tst-script", 1);

    if (p > 0)
    {
        TST_Load(myargv[p + 1]);
    }

    p = M_CheckParmWithArgs("-tst-trace", 1);

    if (p > 0)
    {
        tst_trace = fopen(myargv[p + 1], "w");

        if (tst_trace == NULL)
        {
            I_Error("%s: cannot write the trace", myargv[p + 1]);
        }

        I_AtExit(TST_Close, true);
    }
}
