/**
 * \mainpage
 * 
 * ##Builelés és futtatás##
 * A futtatható fájl elkészítéséhez szükséges egy külső könyvár, az SDL, \n
 * továbbá fontos, hogy a szöveges fájl, a texturák és a zene a futattható fájllal megegyező mappában legyen.
 *
 * \file adat.h
 * \brief A struktúrákért, tömbökért és listákért felelős modul
 * 
 * Több modul által használt matematikai függvények is helyet kapnak továbbá benne
*/


#ifndef ADAT_H
#define ADAT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include "debugmalloc.h"

/**
 * \brief Az ablakot érintő konstansok és az INVALID jel
*/
enum ablak_const
{
    WINX = 960,             /**< Az ablak szélessége*/
    WINY = 720,             /**< Az ablak magassága*/
    FEHER = 0xFFFFFFFF,     /**< A fehér szín hexadecimális kódja*/
    FEKETE = 0x000000FF,    /**< A fekete szín hexadecimális kódja*/
    FEHER50 = 0x888888FF,   /**< A fehér szín félgi áttetsző hexadecimális kódja*/
    FEHER25 = 0x444444FF,   /**< A fehér szín 75%-ban áttetsző hexadecimális kódja*/
    INVALID = -128          /**< Az INVALID jel értéke*/
};

/**
 * \brief Textúrák méretei
*/
enum img_const
{
    NBERX = 13,
    NBERY = 26,
    NYILX = 18,
    NYILY = 13,
    TITLEX = 190,
    TITLEY = 95
};

/**
 * \brief Schönhert kolégium és liftekkel kapcsolatos konstansok
*/
enum sch_const
{
    SZINTCNT = 20,                  /**< A szintek száma*/
    LIFTCNT = 4,                    /**< A liftek száma*/
    MARGOX = 24,                    /**< Vízszintes margó méret*/
    MARGOY = 2,                     /**< Függőleges margó méret*/
    SCHX1 = 128,                    /**< A kollégium bal koordinátája*/
    SCHY1 = 16,                     /**< A kollégium felső koordinátája*/
    SCHW = 512,                     /**< A kollégium szélessége*/
    SCHH = WINY - SCHY1,            /**< A kollégium magassága*/
    SCHX2 = SCHX1 + SCHW,           /**< A kollégium jobb koordinátája*/
    SCHY2 = SCHY1 + SCHH,           /**< A kollégium alsó koordinátája*/
    SCHSZINT = 32,                  /**< A szintek magassága*/
    SCHMAX = 18,                    /**< A legfelső szint*/
    SCHMIN = -1,                    /**< A legalsó szint*/
    LOS = -SCHSZINT/2,              /**< A lift padlóhoz képest vett eltolása*/
    LANIMSPEED = 10,                /**< A liftek mozgási sebességtényezőjénekének reciproka*/
    BANIMSPEED = 5,                 /**< A liftekbe szállási animáció sebességtényezőjénekének reciproka*/
    LSTEP = SCHSZINT*LANIMSPEED,    /**< A liftek mozgásának lépéssúlya*/
    BSTEP = 3*NYILX*BANIMSPEED      /**< A liftekbe szállás lépéssúlya*/
};

/**
 * \brief A felhasználói felület konstans változói
*/
enum ui_pin_const
{
    PADDING = 16,                                               /**< A margók mérete*/
    CELLSIZE = 42,                                              /**< A gombok mérete*/

    UIX1 = 752,                                                 /**< A statisztikák keretének bal koordinátája*/
    UIY1 = PADDING * 2 + TITLEY,                                /**< A statisztikák keretének felső koordinátája*/
    UIW = PADDING + 3 * (CELLSIZE + PADDING),                   /**< A statisztikák keretének szélessége*/
    UIH = PADDING + 1 * (CELLSIZE + PADDING),                   /**< A statisztikák keretének magassága*/
    UIX2 = UIX1 + UIW,                                          /**< A statisztikák keretének jobb koordinátája*/
    UIY2 = UIY1 + UIH,                                          /**< A statisztikák keretének alsó koordinátája*/

    DNBX = (int)(UIX1 + PADDING + CELLSIZE * .5 - NBERX / 2),   /**< A húzható emberke közepének x koordinátája*/
    DNBY = (int)(UIY1 + PADDING + CELLSIZE * .5 - NBERY / 2),   /**< A húzható emberke közepének y koordinátája*/
    STATX = UIX1 + PADDING + 1 * (CELLSIZE + PADDING),          /**< A statisztikák szövegének első sorának x koordinátája*/
    STATY = UIY1 + (int)(1.5*PADDING),                          /**< A statisztikák szövegének első sorának y koordinátája*/

    PINX1 = UIX1,                                               /**< A számbillentyűzet bal koordinátája*/
    PINY1 = UIY2 + PADDING,                                     /**< A számbillentyűzet felső koordinátája*/
    PINW = UIW,                                                 /**< A számbillentyűzet szélessége*/
    PINH = PADDING + 5 * (CELLSIZE + PADDING),                  /**< A számbillentyűzet magassága*/
    PINX2 = PINX1 + PINW,                                       /**< A számbillentyűzet jobb koordinátája*/
    PINY2 = PINY1 + PINH                                        /**< A számbillentyűzet alsó koordinátája*/
};

/**
 * \brief Időátlag számolásához használt struktúra
 * \param sum Az idők összege
 * \param cnt A hányados
*/
typedef struct avg
{
    Uint32 sum;
    int cnt;
} avg;

/**
 * \brief Egy utas adatait tartalmazó struktúra
 * \param from Honnan
 * \param to Hova
 * \param dir Az utazás iránya (fel = 1, le = -1)
 * \param waiting A várakozás kezdetének időbélyege
 * \param traveling Az utazás kezdetének időbélyege
*/
typedef struct utas
{
    int from, to, dir;
    Uint32 waiting, traveling;
} utas;

/**
 * \brief Utasok dinamikus tömbje és a hozzá tartozó méret
 * \param utasok* A tömb első elemére mutató pointer
 * \param meret A tömb mérete
*/
typedef struct utastomb
{
    utas *utasok;
    int meret;
} utastomb;

/**
 * \brief Fájlból beolvasott, még el nem helyezett utas segédstruktúrája
 * \param varoido Az előző utas elhelyezése után várandó idő miliszekundumban
 * \param varo A váró utas többi adata
*/
typedef struct varoutas
{
    Uint32 varoido;
    utas varo;
} varoutas;

/**
 * \brief Az elhelyezésre várakozók listájának egy eleme
 * \param adat A listaelem adatot hordozó része
 * \param kov* A lista következő elemére mutató pointer
*/
typedef struct varolistaelem
{
    varoutas adat;
    struct varolistaelem *kov;
} varolistaelem;

/**
 * \brief Egy helyvektor struktúrája, magáért beszél
*/
typedef struct vector
{
    int x, y;
} vector;

/**
 * \brief A lift felvett állapotai
*/
typedef enum liftstate
{
    LIFTMOVE,
    LIFTIDLE,
    LIFTBOARDING
} liftstate;

/**
 * \brief Egy lift fő struktúrája
 * \param id A lift betűjele
 * \param maxppl A lift maximális létszáma
 * \param floor A lift jelenlegi szintje
 * \param todo_from[] Szintek logikai tömbje, ahol azon állomások vannak igazzal megjelölve, ahol vannak beszállók
 * \param todo_to[] Szintek logikai tömbje, ahol azon állomások vannak igazzal megjelölve, ahol vannak kiszállók
 * \param state A lift jelenlegi állapota
 * \param direction A lift mozgásának jelenlegi iránya (le = -1, nyugalmi = 0, fel = 1)
 * \param inside A liftben tartózkodó utasok tömb-struktúrája
 * \param pos A lift jelenlegi pozíciója egészekben
 * \param anim_y A lift jelenlegi pozíciója törtként, a lift simított mozgatásához
 * \param anim_pre Azt jelzi, hogy a lift beért-e már a szintre, vagy még az előzőt hagyja el (még menni kell esetén true)
 * \param anim_board A be illetve kiszállás animációjának jelenlegi mértéke törtként, a simított animációhoz
 * \param anim_flip Az átszállás írányát jelző logikai változó (true = jobbra = kiszállás)
*/
typedef struct elvono
{
    char id;
    int maxppl;

    int floor;
    bool todo_from[20];
    bool todo_to[20];
    liftstate state;
    int direction;

    utastomb inside;

    vector pos;
    double anim_y;
    bool anim_pre;
    double anim_board;
    bool anim_flip;
} elvono;

/**
 * \brief Egy doboz struktúrája
 * \param rect SDL által definiált téglalap, melynet egy bal felső sarka, szélessége és magassága határoz meg
 * \param color A doboz színe
 * \param filled A doboz kitöltöttsége (true = teli)
*/
typedef struct box
{
    SDL_Rect rect;
    Uint32 color;
    bool filled;
} box;

/**
 * \brief Egy gomb struktúrája
 * \param rect SDL által definiált téglalap, melynet egy bal felső sarka, szélessége és magassága határoz meg
 * \param label A gombon szereplő karakter
 * \param pressed A gomb állapota, lenyomottság szempontjából (true = lenyomott)
*/
typedef struct button
{
    SDL_Rect rect;
    char label;
    bool pressed;
} button;

/**
 * \brief Megadja, hogy c benne van-e [a;b] intervallumban, \n
 * a és b sorrendje indifferens
 * \return Igazat ad, ha a feltétel teljesül
*/
bool mat_inbetween(int a, int b, int c);

/**
 * \brief Megadja, hogy c helyvektor végpontja b téglalapon belülre esik-e
 * \param b A téglalap
 * \param c A vizsgált helyvektor
 * \return Igazat ad, ha a feltétel teljesül
*/
bool mat_inbounds(SDL_Rect b, vector c);

/**
 * \brief Kiszámítja egy adott szint száma alapján annak padlójának y koordinátáját
 * \param szinti A szint száma
 * \return A padló y koordinátája
*/
int mat_szinty(int szinti);

/**
 * \brief Kiszámítja egy adott y koordináta alapján a szint számát
 * \param szinti A szint száma
 * \return A padló y koordinátája
*/
int mat_szinti(int szinty);

/**
 * \brief Kiszámítja egy adott lift indexe alapján annak x koordinátáját
 * \param szinti A lift indexe
 * \return A lift közepének x koordinátája
*/
int mat_liftx(int lifti);

/**
 * \brief Kiszámítja az egér pozíciója alapján a mutatott szintet
 * \param mouse Az egér pozíciója
 * \return INVALID, ha nem mutat egy szintre sem, különben a szint száma
*/
int mat_szintbacktrack(vector mouse);

/**
 * \brief Kiszámítja az egér pozíciója alapján a mutatott gomb indexét
 * \param buttons[] A gombok tömbje
 * \param meret A gombok tömjének mérete
 * \param mouse Az egér pozíciója
 * \return INVALID, ha nem mutat egy gombra sem sem, különben a gomb indexe
*/
int mat_buttoni(button buttons[], int meret, vector mouse);

/**
 * \brief Egy dinamikusan foglalt tömb végére tesz egy utast
 * 
 * A tömb használat utáni felszabadítása a programozó feladata
 * 
 * \param ul* A tömbstruktúrára mutató pointer
 * \param ut A hozzáadandó utas
*/
void utastomb_append(utastomb *ul, utas ut);

/**
 * \brief Egy dinamikusan foglalt tömb adott indexű elemét törli
 * \param ul* A tömbstruktúrára mutató pointer
 * \param index A törlendő elem indexe
*/
void utastomb_indexremove(utastomb *ul, int index);

/**
 * \brief Lemásol egy utasokból álló tömböt
 * 
 * A tömb használat utáni felszabadítása a programozó feladata
 * 
 * \param ul Az eredeti tömbstruktúra
 * \return A tömbstruktúra másolata
*/
utastomb utastomb_clone(utastomb ul);

/**
 * \brief Felszabadítja a használt láncolt listát
 * \param eleje** Az első elemre mutató mutató pointere
 * \brief A függvény az eleje pointert futásának végén NULL-ra állítja vissza 
*/
void varolista_free(varolistaelem **eleje);

/**
 * \brief Hozzáfűz egy elemet a használt láncolt lista végéhez
 * \param eleje** Az első elemre mutató mutató pointere
 * \brief A függvény üres lista esetén képes felülírni az eleje pointert
*/
void varolista_append(varolistaelem **eleje, varoutas add);

/**
 * \brief Töröl egy elemet a használt láncolt lista elejéről
 * \param eleje** Az első elemre mutató mutató pointere
 * \brief A függvény megváltoztatja az eleje pointert, amennyiben az nem NULL
*/
void varolista_firstremove(varolistaelem **eleje);

/**
 * \brief Beolvas egy szöveges fájlt, és egy láncolt listát készít belőle
 * 
 * A fájlban az adatokat az alábbi formátum szerint kell megadni (<> karakterek nélkül):
 * 
 *      <várt idő az előző ember után [ms]> <indulási szint> <érkezési szint>
 * 
 * \param fajlnev* A fájl nevét tartalmazó string
 * \return Az első listaelem címével tér vissza
*/
varolistaelem *fajlbol(char *fajlnev);

#endif