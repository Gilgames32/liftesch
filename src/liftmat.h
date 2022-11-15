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
    bool todo[20];
    liftstate state;
    int direction; //-1 le, 0 idle, 1 fel

    utastomb inside;

    // graf
    vector pos;
    double animy; // for smooth anims
    double animb; // boarding animation smoother
    bool animf;   // nyíl flip
} elvono;


#endif