#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "debugmalloc.h"
#include "liftmat.h"
#include "grafika.h"
#include "adat.h"

int main(int argc, char *argv[])
{

    // inicializálás
    SDL_Window *window;
    SDL_Renderer *renderer;
    Mix_Music *music;
    ablak_init(WINX, WINY, &window, &renderer, &music);
    ablak_cls(renderer);

    // argumentum alapján zene némítás
    Mix_PlayMusic(music, -1);
    for (int i = 0; i < argc; i++)
        if (strcmp(argv[i], "mutemusic") == 0)
            Mix_HaltMusic();

    // textúrák betöltése
    SDL_Texture *nyiltexture = ablak_loadtexture(renderer, "nyil.png");
    SDL_Texture *embertexture = ablak_loadtexture(renderer, "nber.png");
    SDL_Texture *titletexture = ablak_loadtexture(renderer, "lifteschtitle.png");

    // fogd és vidd emberke hitbox
    SDL_Rect emberrect = {DNBX, DNBY, NBERX, NBERY};

    // statisztikák
    avg waittime = {}, traveltime = {};

    // liftek inicializálása
    elvono liftek[LIFTCNT] = {0};
    for (int i = 0; i < LIFTCNT; i++)
    {
        elvono *l = liftek + i;
        l->id = 'A' + i;
        l->maxppl = i < 2 ? 13 : 21; // A és B liftek 13 fősek, C és D 21

        l->floor = 0;
        l->state = LIFTIDLE;
        l->direction = 0;

        l->pos = (vector){mat_liftx(i), mat_szinty(l->floor) - SCHSZINT / 2};
        l->anim_y = (double)l->pos.y;
        l->anim_board = 0;
        l->anim_flip = false;
    }

    // szintek, [liftajtó][szint], A = 0
    utastomb szintek[LIFTCNT][SZINTCNT] = {0};

    // várólista fájlból
    varolistaelem *varokeleje = fajlbol("erkezok.txt");
    // időzítő a következő váróig
    Uint32 varoktimer = 0;

    // deltatime és segédváltozók az egységes animácósebességért, frissítésgyakoriságtól független
    Uint32 prev = SDL_GetTicks(), curr = SDL_GetTicks(), deltatime = 0, localtime = 0;

    // main event és segédváltozók
    SDL_Event event;
    bool update = true;
    vector mousepos = {0, 0};
    bool drag = false;

    bool quit = false;
    while (!quit)
    {
        // minden event feldolgozása
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                // emberre kattintáskor elkezdjük húzni
                if (mat_inbounds(emberrect, mousepos))
                {
                    drag = true;
                    update = true;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                // ha volt a kezünkben ember
                if (drag)
                {
                    // érvényes szintre rakáskor
                    int szinti = mat_szintbacktrack(mousepos);
                    if (szinti != INVALID)
                    {
                        // új utas bekészítése
                        utas temputas = {szinti, INVALID, 0, -1, -1};
                        int toszint = szintinput(renderer, temputas.from);
                        // ha panel közben akarjuk bezárni az ablakot
                        if (toszint == INVALID)
                        {
                            quit = true;
                            break;
                        }
                        temputas.to = toszint;
                        temputas.dir = temputas.to > temputas.from ? 1 : -1;
                        temputas.waiting = localtime;

                        // kedvező lift kiválasztása
                        int pickedlift = picklift(temputas, liftek, szintek);
                        utastomb_append(&(szintek[pickedlift][temputas.from + 1]), temputas);
                        liftek[pickedlift].todo_from[temputas.from + 1] = true;

                        // deltatime visszaállítása, hogy ne ugorjon a befagyás után
                        deltatime = 0;
                        curr = prev;
                    }
                }
                drag = false;
                update = true;
                break;

            case SDL_MOUSEMOTION:
                mousepos = (vector){event.button.x, event.button.y};
                update = true;
                break;

            case SDL_QUIT:
                quit = true;
                break;
            default:
                break;
            }
        }

        // deltatime kiszámítása
        curr = SDL_GetTicks();
        deltatime = curr - prev;
        prev = curr;
        // 25 fps = 40 ms alatt belassul a program, így nem skippel szinteket
        if (deltatime > 40)
            deltatime = 40;
        localtime += deltatime;

        // várólista feldolgozás
        if (varokeleje != NULL)
        {
            // idő számolás
            varoktimer += deltatime;
            // ha lejárt
            if (varoktimer >= varokeleje->adat.varoido)
            {
                // várólistáról a szintre
                utas temputas = varokeleje->adat.varo;
                temputas.waiting = localtime;
                int pickedlift = picklift(temputas, liftek, szintek);
                utastomb_append(&(szintek[pickedlift][temputas.from + 1]), temputas);
                liftek[pickedlift].todo_from[temputas.from + 1] = true;

                // időzítő visszaállítása
                varoktimer -= varokeleje->adat.varoido;
                varolista_firstremove(&varokeleje);

                update = true;
            }
        }

        // liftműveletek kiszámítása és grafika renderelés
        if (update)
        {
            update = false;
            ablak_cls(renderer);

            // liftek
            for (int lifti = 0; lifti < LIFTCNT; lifti++)
            {
                elvono *l = liftek + lifti;
                if (updatelift(renderer, deltatime, l, szintek[lifti], localtime, &waittime, &traveltime))
                    update = true;

                drawlift(renderer, *l, nyiltexture);
            }

            // schönherz és ui
            drawschonherz(renderer, titletexture);

            // váró emberek
            drawwaitingppl(renderer, embertexture, szintek);

            // ha húzunk egy embert akkor a megfelelő szintet kiemeli
            if (drag)
                drawszintaccent(renderer, mousepos);

            // fogd és vidd ember, húzás szerint
            drawnber(renderer, embertexture, drag ? (vector){mousepos.x - NBERX / 2, mousepos.y - NBERY / 3} : (vector){emberrect.x, emberrect.y});

            // statisztikák
            drawstats(renderer, waittime, traveltime);

            // képernyő lefrissítése
            SDL_RenderPresent(renderer);
        }
    }

    // felszabadítások és bezárás

    // sdl felszabadítások
    SDL_DestroyTexture(embertexture);
    SDL_DestroyTexture(nyiltexture);
    SDL_DestroyTexture(titletexture);
    IMG_Quit();

    Mix_HaltMusic();
    Mix_FreeMusic(music);
    Mix_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // utastömbök felszabadítása
    for (int i = 0; i < LIFTCNT; i++)
    {
        // liftekben
        free(liftek[i].inside.utasok);

        // szintoszlopokban
        for (int j = 0; j < SZINTCNT; j++)
        {
            free(szintek[i][j].utasok);
        }
    }

    // maradék várólista felszabadítása
    varolista_free(&varokeleje);

    return 0;
}
