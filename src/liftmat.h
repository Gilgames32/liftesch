#ifndef LIFTMAT_H
#define LIFTMAT_H

#include <stdbool.h>
#include <SDL.h>
#include "grafika.h"
#include "adat.h"

int todo_min(bool todo_to[], bool todo_from[], bool fisrtonly);

int todo_max(bool todo_to[], bool todo_from[], bool fisrtonly);

bool updatelift(SDL_Renderer *renderer, int deltatime, elvono *l, utastomb szint[]);

int picklift(utas temputas, elvono liftek[], utastomb szintek[][20]);

#endif