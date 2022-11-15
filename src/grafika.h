#ifndef GRAFIKA_H
#define GRAFIKA_H

#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include "liftmat.h"

enum ablak_const
{
    WINX = 960,
    WINY = 720,
    FEHER = 0xFFFFFFFF,
    FEKETE = 0x000000FF,
    FEHER50 = 0xFFFFFF88,
    FEHER25 = 0xFFFFFF44
};

enum sch_const
{
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
    LOS = 16 //liftoffset
};

enum pin_const
{
    PADDING = 16,
    CELLSIZE = 42,
    PINX1 = 752,
    PINY1 = 32,
    PINW = PADDING + 3 * (CELLSIZE + PADDING),
    PINH = PADDING + 5 * (CELLSIZE + PADDING),
    PINX2 = PINX1 + PINW,
    PINY2 = PINY1 + PINH
};

enum img_const
{
    NBERX = 13,
    NBERY = 26,
    NYILX = 18,
    NYILY = 13
};

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

#endif