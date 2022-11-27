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

    // init
    SDL_Window *window;
    SDL_Renderer *renderer;
    Mix_Music *music;
    ablak_init(WINX, WINY, &window, &renderer, &music);
    ablak_cls(renderer);

    // control music
    Mix_PlayMusic(music, -1);
    for (int i = 0; i < argc; i++)
        if (strcmp(argv[i], "mutemusic") == 0)
            Mix_HaltMusic();


    // load textures
    SDL_Texture *nyiltexture = ablak_loadtexture(renderer, "nyil.png");
    SDL_Texture *embertexture = ablak_loadtexture(renderer, "nber.png");
    SDL_Texture *titletexture = ablak_loadtexture(renderer, "lifteschtitle.png");

    // ui components
    SDL_Rect emberrect = {DNBX, DNBY, NBERX, NBERY};
    // button loadbutton = {(SDL_Rect){LDBTNX, LDBTNY, CELLSIZE, CELLSIZE}};

    // stats
    avg waittime = {}, traveltime = {};

    // liftek init
    elvono liftek[LIFTCNT] = {0};
    for (int i = 0; i < LIFTCNT; i++)
    {
        elvono *l = liftek + i;
        l->id = 'A' + i;
        l->maxppl = i < 2 ? 13 : 21; // A és B liftek 13 fősek, C és D 21

        l->floor = 0;
        // todo empty
        l->state = LIFTIDLE;
        l->direction = 0;

        // inside empty

        l->pos = (vector){mat_liftx(i), mat_szinty(l->floor) - SCHSZINT/2};
        l->anim_y = (double)l->pos.y;
        l->anim_board = 0;
        l->anim_flip = false;
    }

    // szintek liftajtó : szint, A = 0
    // követelmény miatt kettős indirekcijó :|
    utastomb szintek[LIFTCNT][SZINTCNT] = {0};

    // várólista fájlból
    varolistaelem *varokeleje = fajlbol();
    Uint32 varoktimer = 0;

    Uint32 prev = SDL_GetTicks(), curr = SDL_GetTicks(), deltatime = 0, localtime = 0;
    SDL_Event event;
    bool update = true;
    vector mousepos = {0, 0};
    bool drag = false;

    bool quit = false;
    while (!quit)
    {
        // processing events
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                if (mat_inbounds(emberrect, mousepos))
                {
                    drag = true;
                }
                update = true;
                break;

            case SDL_MOUSEBUTTONUP:
                if (drag)
                {
                    int szinti = mat_szintbacktrack(mousepos);
                    if (szinti != INVALID)
                    {
                        // initialize new utas
                        utas temputas = {szinti, INVALID, 0, -1, -1};
                        int toszint = szintinput(renderer, temputas.from);
                        // ha panel közben kapunk exit kódot
                        if (toszint == INVALID)
                        {
                            quit = true;
                            break;
                        }
                        temputas.to = toszint;
                        temputas.dir = temputas.to > temputas.from ? 1 : -1;
                        // timer for stats
                        temputas.waiting = localtime;

                        // add utas to lift
                        int pickedlift = picklift(temputas, liftek, szintek);
                        utastomb_append(&(szintek[pickedlift][temputas.from + 1]), temputas);
                        liftek[pickedlift].todo_from[temputas.from + 1] = true;

                        // reset deltatime
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

        // calc deltatime
        curr = SDL_GetTicks();
        deltatime = curr - prev;
        prev = curr;
        // 40 fps alatt belassul a program, így nem skippel szinteket
        if (deltatime > 40)
            deltatime = 40;
        localtime += deltatime;

        // várólista management
        if (varokeleje != NULL)
        {
            update = true;
            varoktimer += deltatime;
            if (varoktimer >= varokeleje->adat.varoido)
            {
                // be a szintre
                // todo: implement the liftpick here too
                utas temputas = varokeleje->adat.varo;
                temputas.waiting = localtime;
                int pickedlift = picklift(temputas, liftek, szintek);
                utastomb_append(&(szintek[pickedlift][temputas.from + 1]), temputas);
                liftek[pickedlift].todo_from[temputas.from + 1] = true;

                varoktimer -= varokeleje->adat.varoido;
                varolista_firstremove(&varokeleje);
            }
        }

        // apply movement and render
        if (update)
        {
            update = false;
            // render
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

            // ha húz egy embert akkor a megfelelő szintet kiemeli
            if (drag)
                drawszintaccent(renderer, mousepos);

            // dragndrop ember
            drawnber(renderer, embertexture, drag ? (vector){mousepos.x - NBERX / 2, mousepos.y - NBERY / 3} : (vector){emberrect.x, emberrect.y});

            // stats
            drawstats(renderer, waittime, traveltime);

            // apply
            SDL_RenderPresent(renderer);
        }
    }

    // felszabadítások és bezárás

    // sdl cleanup
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
        // liftek
        free(liftek[i].inside.utasok);

        // szintoszlopok
        for (int j = 0; j < SZINTCNT; j++)
        {
            free(szintek[i][j].utasok);
        }
    }

    // maradék várólista
    varolista_free(&varokeleje);

    return 0;
}
