#ifndef GRAFIKA_H
#define GRAFIKA_H

#include <stdbool.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "adat.h"


void ablak_cls(SDL_Renderer *renderer);

void ablak_init(int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer, Mix_Music **pmusic);

SDL_Texture *ablak_loadtexture(SDL_Renderer *renderer, char filename[]);

void rectangleStrokedColor(SDL_Renderer *renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color, int stroke);

void drawbox(SDL_Renderer *renderer, box b);

void drawschonherz(SDL_Renderer *renderer, SDL_Texture *title);

void drawnyil(SDL_Renderer *renderer, SDL_Texture *nyiltexture, int x, vector pos, bool flip);

void drawlift(SDL_Renderer *renderer, elvono lift, SDL_Texture *nyiltexture);

void drawszintaccent(SDL_Renderer *renderer, vector mouse);

void drawnber(SDL_Renderer *renderer, SDL_Texture *embertexture, vector pos);

void drawwaitingppl(SDL_Renderer *renderer, SDL_Texture *embertexture, utastomb szintek[][20]);

void drawstats(SDL_Renderer *renderer, avg waitt, avg travelt);

void uiinit(button buttons[]);

char buttonnumber(int index);

void drawpin(SDL_Renderer *renderer, double scale, button buttons[], char panel[], double inputbar);

void drawuiaccent(SDL_Renderer *renderer, button buttons[], vector mouse, bool pressed);

void clearpinpad(SDL_Renderer *renderer);

int szintinput(SDL_Renderer *renderer, int from);

#endif