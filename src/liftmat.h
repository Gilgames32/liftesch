/**
 * \file liftmat.h
 * \brief Liftek frissítéséért és a kedvező liftek kiszámításáért felelős modul
*/

#ifndef LIFTMAT_H
#define LIFTMAT_H

#include <stdbool.h>
#include <SDL.h>
#include "grafika.h"
#include "adat.h"

/**
 * \brief Megadja a legalsó érvényes célállomás indexét
 * \param todo_to[] Szintek logikai tömbje, ahol azon állomások vannak igazzal megjelölve, ahol vannak kiszállók
 * \param todo_from[] Szintek logikai tömbje, ahol azon állomások vannak igazzal megjelölve, ahol vannak beszállók
 * \param firstonly Amennyiben igaz, csak az első, azaz csakis a todo_to lesz figyelembe véve
 * Megtelt liftek esetén hasznos
 * \return A legalsó érvényes szint indexét téríti vissza, nem a szint számát
 */
int todo_min(bool todo_to[], bool todo_from[], bool fisrtonly);

/**
 * \brief Megadja a legfelső érvényes célállomás indexét
 * \param todo_to[] Szintek logikai tömbje, ahol azon állomások vannak igazzal megjelölve, ahol vannak kiszállók
 * \param todo_from[] Szintek logikai tömbje, ahol azon állomások vannak igazzal megjelölve, ahol vannak beszállók
 * \param firstonly Amennyiben igaz, csak az első, azaz csakis a todo_to lesz figyelembe véve
 * Megtelt liftek esetén hasznos
 * \return A legfelső érvényes szint indexét téríti vissza, nem a szint számát
 */
int todo_max(bool todo_to[], bool todo_from[], bool fisrtonly);

/**
 * \brief Egy adott lift mozgatását/animálását, a be és kiszállásokat kezeli
 * Utasok kiszállásakor frissíti a statisztikákat
 * \param renderer* A renderer mutatója
 * \param deltatime Az előző képkocka frissítése óta eltelt idő miliszekundumban
 * \param l* A frissítendő lift
 * \param szintoszlop[] Adott lifthez tartozó szinteken várakozó utasok tömbje
 * \param localtime Az indítás óta eltelt miliszekundumok száma
 * Ellenben az SDL_GetTicks() fügvénnyel, ebbe nem számít bele az az idő, amíg mi egy szintet ütöttünk be és a program többi része nem futott
 * \param waitt* A várakozási idő átlag struktúrájának mutatója
 * \param travelt* Az utazási idő átlag struktúrájának mutatója
 * \return Visszatérítési értéke egy logikai változó, mely jelzi, hogy volt volt-e változás/firssítés
 */
bool updatelift(SDL_Renderer *renderer, int deltatime, elvono *l, utastomb szintoszlop[], Uint32 localtime, avg *waitt, avg *travelt);

/**
 * \brief Összehasonlítja minden liftet, és kiválasztja a nekünk (nem-teljesen-leg)jobbat.
 * A legjobb lift kiválasztása egy ilyen esetben megoldhatatlan matematikai probléma, de általában a legjobb liftet válaszja
 * \param temputas Az érkező és már ismert célállomású utas
 * \param liftek[] Az összes liftet tartalmazó tömb
 * \param szintek[][] Az összes lift és azon belül az összes szinten várakozó emberek (két dimenziós) tömbje
 * \return A legkevesebb lépés alatt ideérő lift indexével tér vissza (A = 0)
 */
int picklift(utas temputas, elvono liftek[], utastomb szintek[][20]);

/**
 * \brief Kiszámítja, hogy hány lépés alatt érne az érkező utas szintjére az adott lift
 * Addig lépteti a szimulációt, amíg a lift és az utas mozgási iránya meg nem egyezik
 * Ekkor megvizsgálja, hogy a már a rendszerben lévő utasoktól nem venné-e el a helyet, azaz a célállomásáig minden érintett szinten befére-e
 * Ha nem férünk be a liftbe, megvárjuk a következő ilyen alkalmat, és újra megvizsgáljuk az esetet
 * Amennyiben a lift álló (idle/-1) helyzetben van, az abszolút távolságát vesszük, a lépések értékével súlyozva
 * 
 * Ehhez másolatot készít a lift utasainak és a szinten tartózkodó utasok tömbjeiről, hogy az eredeti tömböket ne írja felül
 * 
 * \param temputas Az érkező és már ismert célállomású utas
 * \param l Az adott lift
 * \param eredetiszintoszlop[] Az összes lift és azon belül az összes szinten várakozó emberek (két dimenziós) tömbje, erről készül a másolat
 * \return Az adott lifthez tartozó lépéssámmal térít vissza
 * A lépésszám viszonylag időarányos
 */
int megeri(utas temputas, elvono l, utastomb eredetiszintoszlop[]);

/**
 * \brief Következő állapotba állítja a liftet és megadja az ehhez szükséges lépések számát
 * A be és kiszállásnak, illetve a mozgatásnak eltérő súlyú a lépésértéke
 * 
 * \param temputas Az érkező és már ismert célállomású utas
 * \param l Az adott lift
 * \param szintoszlop[] Az összes lift és azon belül az összes szinten várakozó emberek (két dimenziós) tömbje, módosulhat, ezért ez egy másolat kell legyen
 */
int leptet(utas temputas, elvono *l, utastomb szintoszlop[]);

#endif