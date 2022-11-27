/**
 * \file grafika.h
 * \brief Grafikai elemekért és megjelenítésükért felelős modul
 * 
 * Itt található minden grafikai függvény, az ablak és a grafikus bemenetek kezelsése
*/

#ifndef GRAFIKA_H
#define GRAFIKA_H

#include <stdbool.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "adat.h"

/**
 * \brief Törli a renderert, azaz az ablak tartalmát
 * \param renderer* A renderer mutatója
 */
void ablak_cls(SDL_Renderer *renderer);

/**
 * \brief Inicializál és létrehoz egy SDL ablakot, \n
 * továbba betölti a zenét és beállítja az ikont
 * 
 * A zene, a renderer és az ablak "megsemmisítése" a programozó feladata
 * \param szeles Az ablak szélessége
 * \param magas Az ablak magassága
 * \param renderer** A renderer mutatója
 * \param window** Az ablak mutatója
 * \param pmusic** A mixer mutatója
 */
void ablak_init(int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer, Mix_Music **pmusic);

/**
 * \brief Fájlnév alapján betölt egy textúrát
 * 
 * A textúra használat utáni felszabadítása a programozó feladata
 * \param renderer* A renderer mutatója
 * \param fliename[] A betöltendő fájl neve
 * \return A betöltött textúrával tér vissza
 */
SDL_Texture *ablak_loadtexture(SDL_Renderer *renderer, char filename[]);

/**
 * \brief Egy vastagon körvonalazott téglalapot rajzol a rendererre
 * \param renderer* A renderer mutatója
 * \param koordináták x1, y1, x2, y1: a téglalap 2 szemközti sarkának koordinátái
 * \param color A rajz színe
 */
void rectangleStrokedColor(SDL_Renderer *renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color, int stroke);

/**
 * \brief Egy dobozt rajzol rajzol a rendererre
 * 
 * Az SDL-es rectangleColor függvénytől eltérően, bal felső sarok, szélesség és magasság alapján rajzol
 * \param renderer* A renderer mutatója
 * \param b A rajzolandó doboz
 */
void drawbox(SDL_Renderer *renderer, box b);

/**
 * \brief Lerajzolja a kollégiumot, a LifeSCH logót, és a statisztikák dobozát
 * \param renderer* A renderer mutatója
 * \param title* A logó textúrájának mutatója
 */
void drawschonherz(SDL_Renderer *renderer, SDL_Texture *title);

/**
 * \brief Egy nyilat rajzol
 * \param renderer* A renderer mutatója
 * \param nyiltexture* A logó textúrájának mutatója
 * \param x Az animáció, avagy az eltolás mértéke
 * \param pos A nyíl helye (koordinátái)
 * \param flip A nyíl mutatásának iránya
 */
void drawnyil(SDL_Renderer *renderer, SDL_Texture *nyiltexture, int x, vector pos, bool flip);

/**
 * \brief Egy liftet és egy hozzá tartozó "kötelet" rajzol
 * 
 * Adott esetben a liftbe épp be vagy ki szállókhoz tartozó nyíl rajzolásának függvényének hívását is intézi
 * 
 * A a kirjazolás viszonyítási pontja, azaz a horgony, a lift közepén van, \n
 * a liftekre rákerül a bent tartózkodó utasok száma
 * \param renderer* A renderer mutatója
 * \param lift A kirajzolandó lift struktúra
 * \param nyiltexture* A nyíl textúrája
 */
void drawlift(SDL_Renderer *renderer, elvono lift, SDL_Texture *nyiltexture);

/**
 * \brief Vastagon kiemel egy szintet az egér pozíciója alapján
 * \param renderer* A renderer mutatója
 * \param mouse Az egér pozíciója
 */
void drawszintaccent(SDL_Renderer *renderer, vector mouse);

/**
 * \brief Egy embert rajzol
 * 
 * A horgony a bal felső sarokban van
 * \param renderer* A renderer mutatója
 * \param embertexture* Az ember textúra
 * \param pos A rajzolás helye
 */
void drawnber(SDL_Renderer *renderer, SDL_Texture *embertexture, vector pos);

/**
 * \brief Minden szinten és minden liftnél váró embertömeget kirajzol
 * 
 * Amennyiben túllógnának, egy szám jelenik meg, de ez nem lehet 4-nél több számjegyű
 * \param renderer* A renderer mutatója
 * \param embertexture* Az ember textúra
 * \param szintek[][] Az összes lift és azon belül az összes szinten várakozó emberek (két dimenziós) tömbje
 */
void drawwaitingppl(SDL_Renderer *renderer, SDL_Texture *embertexture, utastomb szintek[][20]);

/**
 * \brief Kiírja (rajzolja) a képernyőre a várakozási és utazási satisztikákat
 * 
 * Legfeljebb 5 számot (4 számot a tizedesvessző előtt és egyet utána) képes kiírni
 * \param renderer* A renderer mutatója
 * \param waitt A várakozási idő átlag struktúrája
 * \param waitt Az utazási idő átlag struktúrája
 */
void drawstats(SDL_Renderer *renderer, avg waitt, avg travelt);

/**
 * \brief Előkészíti a szintválasztó felület gombjait, \n
 * beállítja a gombok feliratát, pozícióját
 * \param buttons[] A gombok tömbje
 */
void uiinit(button buttons[]);

/**
 * \brief Kirajzolja a szintválasztó számbillentyűzetet, az animációtól függő mértékben
 * \param renderer* A renderer mutatója
 * \param scale Az animáció, azaz a függőleges kirajzolás mértéke
 * \param buttons[] A gombok tömbje
 * \param panel[] A jelenleg beütött karakterek tömbje
 * \param inputbar A szintválasztásból hátralevő időt jelző sáv vízszintes mérete
 */
void drawpin(SDL_Renderer *renderer, double scale, button buttons[], char panel[], double inputbar);

/**
 * \brief Kiemeli az éppen egér alatt lévő, és/vagy lenyomott gombokat
 * \param renderer* A renderer mutatója
 * \param buttons[] A gombok tömbje
 * \param mouse Az egér pozíciója
 * \param pressed Az egér lenyomottsági állapota
 */
void drawpinaccent(SDL_Renderer *renderer, button buttons[], vector mouse, bool pressed);

/**
 * \brief Törli a képernyő azon részét ahol a szintválasztó billentyűzet volt/van
 * \param renderer* A renderer mutatója
 */
void clearpinpad(SDL_Renderer *renderer);

/**
 * \brief A szintválasztás bemenetét és animációját kezeli
 * 
 * Először lefut a megjelenítés animációja (fentről le)
 * A program többi része függvény futásának idejére megáll
 * Addig fut amíg nincs érvényesen megadott szint, vagy be nem zárjuk az ablakot
 * Legvégül lefut az eltűnés animációja (lentről fel)
 * 
 * \param renderer* A renderer mutatója
 * \return Ha szintválasztás közben próbáljuk bezárni az ablakot, akkor INVALID jelzéssel tér vissza
 * Ellenkező esetben a kiválasztott célállomást kapjuk meg
 */
int szintinput(SDL_Renderer *renderer, int from);

#endif