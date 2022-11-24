#ifndef ADAT_H
#define ADAT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>

typedef struct avg
{
    int avgerage, cnt;
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
/*
void utastomb_append(utastomb *ul, utas ut);
void utastomb_indexremove(utastomb *ul, int index);

varoutas sorbe(void);

void utastomb_append(utastomb *ul, utas ut);

void utastomb_indexremove(utastomb *ul, int index);

void varolista_free(varolistaelem **eleje);

void varolista_append(varolistaelem **eleje, varoutas add);

void varolista_firstremove(varolistaelem **eleje);

varolistaelem *fajlbol(void);
*/
#endif