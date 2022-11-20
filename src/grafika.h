#ifndef GRAFIKA_H
#define GRAFIKA_H

#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include "liftmat.h"
#include "adat.h"

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
    LOS = 16 // liftoffset
};

enum pin_const
{
    PADDING = 16,
    CELLSIZE = 42,
    PINX1 = 752,
    PINY1 = 256,
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
/*
void ablak_cls(SDL_Renderer *renderer);

void ablak_init(int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer);

SDL_Texture *ablak_loadtexture(SDL_Renderer *renderer, char filename[]);

void rectangleStrokedColor(SDL_Renderer *renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color, int stroke);

void drawbox(SDL_Renderer *renderer, box b);

void drawschonherz(SDL_Renderer *renderer);

void drawnyil(SDL_Renderer *renderer, SDL_Texture *nyiltexture, int x, vector pos, bool flip);

void drawlift(SDL_Renderer *renderer, elvono lift, SDL_Texture *nyiltexture);

void drawszintaccent(SDL_Renderer *renderer, vector mouse);

void drawnber(SDL_Renderer *renderer, SDL_Texture *embertexture, vector pos);

void drawwaitingppl(SDL_Renderer *renderer, SDL_Texture *embertexture, utastomb szintek[][20]);

void uiinit(button buttons[]);

char buttonnumber(int index);

void drawpin(SDL_Renderer *renderer, double scale, button buttons[], char panel[], double inputbar);

void drawuiaccent(SDL_Renderer *renderer, button buttons[], vector mouse, bool pressed);

void clearpinpad(SDL_Renderer *renderer);

int szintinput(SDL_Renderer *renderer, int from);
*/
#endif