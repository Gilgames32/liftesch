#ifndef LIFTMAT_H
#define LIFTMAT_H

#include <stdbool.h>
#include <SDL.h>
#include "grafika.h"
#include "adat.h"

typedef struct vector
{
    int x, y;
} vector;

typedef enum liftstate
{
    LIFTMOVE, LIFTIDLE, LIFTBOARDING
} liftstate;


typedef struct elvono
{
    // const
    char id;
    int maxppl;

    // var
    int floor;
    bool todo_from[20]; //ahonnan kell felvenni
    bool todo_to[20];   //ahová kell letenni
    liftstate state;
    int direction; //-1 le, 0 idle, 1 fel

    // passengers
    utastomb inside;

    // graf
    vector pos;
    double anim_y;      // for smooth anims
    bool anim_pre;      // check if its pre or past the snapline
    double anim_board;  // boarding animation smoother
    bool anim_flip;     // nyíl flip
} elvono;


#endif