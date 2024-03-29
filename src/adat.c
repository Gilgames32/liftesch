#include "adat.h"

bool mat_inbetween(int a, int b, int c)
{
    return (a >= c && c >= b) || (b >= c && c >= a);
}

bool mat_inbounds(SDL_Rect b, vector c)
{
    return mat_inbetween(b.x, b.x + b.w, c.x) && mat_inbetween(b.y, b.y + b.h, c.y);
}

int mat_szinty(int szinti)
{
    return 64 + SCHSZINT * (18 - szinti);
}
int mat_szinti(int szinty)
{
    return 18 - ((szinty) / SCHSZINT - 1);
}
int mat_liftx(int lifti)
{
    return lifti * (512 / 4) + 128 + 32;
}

int mat_szintbacktrack(vector mouse)
{
    if (mat_inbetween(SCHX1 - MARGOX, SCHX1 + SCHW + MARGOX, mouse.x) && mat_inbetween(-1, 18, mat_szinti(mouse.y)))
        return mat_szinti(mouse.y);
    else
        return INVALID;
}

int mat_buttoni(button buttons[], int meret, vector mouse)
{
    for (int i = 0; i < meret; i++)
    {
        if (mat_inbounds(buttons[i].rect, mouse))
        {
            return i;
        }
    }
    return INVALID;
}

void utastomb_append(utastomb *ul, utas ut)
{
    utastomb tempul = {NULL, ul->meret + 1};
    tempul.utasok = (utas *)malloc(tempul.meret * sizeof(utas));
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
    tempul.utasok = (utas *)malloc(tempul.meret * sizeof(utas));
    for (int i = 0; i < tempul.meret; i++)
    {
        tempul.utasok[i] = ul->utasok[i < index ? i : i + 1];
    }
    free(ul->utasok);
    *ul = tempul;
}

utastomb utastomb_clone(utastomb ul)
{
    utastomb tempul = {NULL, ul.meret};
    tempul.utasok = (utas *)malloc(tempul.meret * sizeof(utas));
    for (int i = 0; i < ul.meret; i++)
    {
        tempul.utasok[i] = ul.utasok[i];
    }
    return tempul;
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

void varolista_append(varolistaelem **eleje, varoutas add)
{
    varolistaelem **mozgo = eleje;
    while (*mozgo != NULL)
    {
        mozgo = &(*mozgo)->kov;
    }

    varolistaelem *ujelem = (varolistaelem *)malloc(sizeof(varolistaelem));
    ujelem->adat = add;
    ujelem->kov = NULL;
    *mozgo = ujelem;
}

void varolista_firstremove(varolistaelem **eleje)
{
    if (eleje == NULL) return; 
    varolistaelem *ujeleje = (*eleje)->kov;
    free(*eleje);
    *eleje = ujeleje;
}

varolistaelem *fajlbol(char *fajlnev)
{
    FILE *fp;
    fp = fopen(fajlnev, "r");
    if (fp != NULL)
    {
        varolistaelem *eleje = NULL;
        int vwait, vfrom, vto;

        while (fscanf(fp, "%d %d %d\n", &vwait, &vfrom, &vto) == 3)
        {
            if (vfrom != vto && mat_inbetween(-1, 18, vfrom) && mat_inbetween(-1, 18, vto))
            {
                varolista_append(&eleje, (varoutas){vwait, {vfrom, vto, vto > vfrom ? 1 : -1, -1, -1}});
            }
            else
                perror("Hibás sor");
        }

        fclose(fp);
        return eleje;
    }

    else
        perror("Érvénytelen fájl");

    return NULL;
}