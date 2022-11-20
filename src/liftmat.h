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
/*
int mat_szinty(int szinti);

int mat_szinti(int szinty);

int mat_liftx(int lifti);

bool mat_inrange(int a, int b, int range);

bool mat_inbetween(int a, int b, int c);

// sdl függvényre csere
bool mat_inbounds(SDL_Rect b, vector c);

int mat_szintbacktrack(vector mouse);

int mat_buttoni(struct button buttons[], int meret, vector mouse);

vector vectoradd(vector v1, vector v2);

vector vectorscale(vector v, double w, double h);

int todo_min(bool todo_to[], bool todo_from[], bool fisrtonly);

int todo_max(bool todo_to[], bool todo_from[], bool fisrtonly);

int picklift(utas temputas, elvono liftek[], utas szintek[][20]);

bool updatelift(SDL_Renderer *renderer, int deltatime, elvono *l, utastomb szint[]);
*/
#endif