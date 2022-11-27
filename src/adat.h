#ifndef ADAT_H
#define ADAT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include "debugmalloc.h"

enum ablak_const
{
    WINX = 960,
    WINY = 720,
    FEHER = 0xFFFFFFFF,
    FEKETE = 0x000000FF,
    FEHER50 = 0xFFFFFF88,
    FEHER25 = 0xFFFFFF44,
    INVALID = -128
};

enum img_const
{
    NBERX = 13,
    NBERY = 26,
    NYILX = 18,
    NYILY = 13,
    TITLEX = 190,
    TITLEY = 95,
    FLOPPYX = 16,
    FLOPPYY = 16
};

enum sch_const
{
    SZINTCNT = 20,
    LIFTCNT = 4,
    MARGOX = 24,
    MARGOY = 2,
    SCHX1 = 128,
    SCHY1 = 16,
    SCHW = 512,
    SCHH = WINY - SCHY1,
    SCHX2 = SCHX1 + SCHW - 1,
    SCHY2 = SCHY1 + SCHH - 1,
    SCHSZINT = 32,
    SCHMAX = 18,
    SCHMIN = -1,
    LOS = SCHSZINT/2,               // liftoffset
    LANIMSPEED = 10,                 // lift animation speed (1/x)
    BANIMSPEED = 5,                 // boarding animation speed (1/x)
    LSTEP = SCHSZINT*LANIMSPEED,    // lift moving steps count
    BSTEP = 3*NYILX*BANIMSPEED      // lift boarding steps count
};

enum ui_pin_const
{
    PADDING = 16,
    CELLSIZE = 42,

    UIX1 = 752,
    UIY1 = PADDING * 2 + TITLEY,
    UIW = PADDING + 3 * (CELLSIZE + PADDING),
    UIH = PADDING + 1 * (CELLSIZE + PADDING),
    UIX2 = UIX1 + UIW,
    UIY2 = UIY1 + UIH,

    DNBX = (int)(UIX1 + PADDING + CELLSIZE * .5 - NBERX / 2),
    DNBY = (int)(UIY1 + PADDING + CELLSIZE * .5 - NBERY / 2),
    STATX = UIX1 + PADDING + 1 * (CELLSIZE + PADDING),
    STATY = UIY1 + (int)(1.5*PADDING),

    PINX1 = UIX1,
    PINY1 = UIY2 + PADDING,
    PINW = UIW,
    PINH = PADDING + 5 * (CELLSIZE + PADDING),
    PINX2 = PINX1 + PINW,
    PINY2 = PINY1 + PINH
};

typedef struct avg
{
    Uint32 sum;
    int cnt;
} avg;

typedef struct utas
{
    int from, to, dir;
    Uint32 waiting, traveling;
} utas;

typedef struct utastomb
{
    utas *utasok;
    int meret;
} utastomb;

typedef struct varoutas
{
    Uint32 varoido;
    utas varo;
} varoutas;

typedef struct varolistaelem
{
    varoutas adat;
    struct varolistaelem *kov;
} varolistaelem;

typedef struct vector
{
    int x, y;
} vector;

typedef enum liftstate
{
    LIFTMOVE,
    LIFTIDLE,
    LIFTBOARDING
} liftstate;

typedef struct elvono
{
    // const
    char id;
    int maxppl;

    // var
    int floor;
    bool todo_from[20]; // ahonnan kell felvenni
    bool todo_to[20];   // ahová kell letenni
    liftstate state;
    int direction; //-1 le, 0 idle, 1 fel

    // passengers
    utastomb inside;

    // graf
    vector pos;
    double anim_y;     // for smooth anims
    bool anim_pre;     // check if its pre or past the snapline
    double anim_board; // boarding animation smoother
    bool anim_flip;    // nyíl flip
} elvono;

typedef struct box
{
    SDL_Rect rect;
    Uint32 color;
    bool filled;
} box;

typedef struct button
{
    SDL_Rect rect;
    char label;
    bool pressed;
    void (*cb)();
} button;

bool mat_inrange(int a, int b, int range);

bool mat_inbetween(int a, int b, int c);

bool mat_inbounds(SDL_Rect b, vector c);

int mat_szinty(int szinti);

int mat_szinti(int szinty);

int mat_liftx(int lifti);

int mat_szintbacktrack(vector mouse);

int mat_buttoni(button buttons[], int meret, vector mouse);

void utastomb_append(utastomb *ul, utas ut);

void utastomb_indexremove(utastomb *ul, int index);

utastomb utastomb_clone(utastomb ul);

varoutas sorbe(void);

void utastomb_append(utastomb *ul, utas ut);

void utastomb_indexremove(utastomb *ul, int index);

void varolista_free(varolistaelem **eleje);

void varolista_append(varolistaelem **eleje, varoutas add);

void varolista_firstremove(varolistaelem **eleje);

varolistaelem *fajlbol(void);

#endif