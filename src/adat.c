#include "adat.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include "debugmalloc.h"

void utastomb_append(utastomb *ul, utas ut)
{
    utastomb tempul = {NULL, ul->meret + 1};
    tempul.utasok = (utas*) malloc(tempul.meret * sizeof(utas));
    for (int i = 0; i < ul->meret; i++)
    {
        tempul.utasok[i] = ul->utasok[i];
    }
    free(ul->utasok);
    tempul.utasok[tempul.meret - 1] = ut;
    *ul = tempul;
}
void utastomb_indexremove(utastomb *ul, int index)
{
    utastomb tempul = {NULL, ul->meret - 1};
    tempul.utasok = (utas*) malloc(tempul.meret * sizeof(utas));
    for (int i = 0; i < tempul.meret; i++)
    {
        tempul.utasok[i] = ul->utasok[i<index ? i : i+1];
    }
    free(ul->utasok);
    *ul = tempul;
}

void varolista_free(varolistaelem **eleje)
{
    for (varolistaelem *mozgo = *eleje; mozgo != NULL;)
    {
        varolistaelem *kovi = mozgo->kov;
        free(mozgo);
        mozgo = kovi;
    }
    *eleje = NULL;
}

// pointer pointer my beloved ,,,uwu,,,
void varolista_append(varolistaelem **eleje, varoutas add){
    varolistaelem **mozgo = eleje;
    while (*mozgo != NULL){
        mozgo = &(*mozgo)->kov;
    }
    
    varolistaelem *ujelem = (varolistaelem*) malloc(sizeof(varolistaelem));
    ujelem->adat = add;
    ujelem->kov = NULL;
    *mozgo = ujelem;
}

void varolista_firstremove(varolistaelem **eleje){
    varolistaelem *ujeleje = (*eleje)->kov;
    free(*eleje);
    *eleje = ujeleje;
}

varolistaelem* fajlbol(void) {
    FILE* fp;
    fp = fopen("erkezok.txt", "r");
    if (fp != NULL)
    {
        varolistaelem *eleje = NULL;
        int vwait, vfrom, vto;

        while (fscanf(fp, "%d %d %d\n", &vwait, &vfrom, &vto) == 3)
        {
            if (vfrom != vto && mat_inbetween(-1, 18, vfrom) && mat_inbetween(-1, 18, vto))
            {
                varolista_append(&eleje, (varoutas){vwait, {vfrom, vto, vto > vfrom ? 1 : -1}});
            }
            else
                // ezt egyelőre még a végén is kiteszi
                perror("Hibás sor");
        }

        fclose(fp);
        return eleje;
    }



    else
        perror("Érvénytelen fájl");

    return NULL;
}