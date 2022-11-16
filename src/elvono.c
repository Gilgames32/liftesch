#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_image.h>

#include "debugmalloc.h"
#include "liftmat.h"
#include "grafika.h"
#include "adat.h"



int main(int argc, char *argv[])
{

    // init
    SDL_Window *window;
    SDL_Renderer *renderer;
    ablak_init(WINX, WINY, &window, &renderer);
    ablak_cls(renderer);

    // load textures
    SDL_Texture *nyiltexture = ablak_loadtexture(renderer, "nyil.png");
    SDL_Texture *embertexture = ablak_loadtexture(renderer, "nber.png");

    // ui components
    SDL_Rect emberrect = {700, 300, NBERX, NBERY};

    // liftek init
    elvono liftek[4] = {0};
    for (int i = 0; i < 4; i++)
    {
        elvono *l = liftek + i;
        l->id = 'A' + i;
        l->maxppl = i < 2 ? 3 : 21; // A és B liftek 13 fősek, C és D 21

        l->floor = 0;
        // todo empty
        l->state = LIFTIDLE;
        l->direction = 0;

        // inside empty

        l->pos = (vector){mat_liftx(i), mat_szinty(l->floor) - 16};
        l->anim_y = (double)l->pos.y;
        l->anim_board = 0;
        l->anim_flip = false;
    }

    // szintek liftajtó:szint, miért? mert így kevesebbet kell szarakodni, A = 0
    utastomb szintek[4][20] = {0};

    // várólista fájlból
    varolistaelem *varokeleje = fajlbol();
    int varoktimer = 0;

    // ezekből lehet uint kéne?
    int prev = SDL_GetTicks(), curr = SDL_GetTicks(), deltatime = 0;
    SDL_Event event;
    bool update = true;
    vector mouse = {0, 0};
    bool mousepressed = false;
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
                mousepressed = true;
                // printf("%d, %d\n", mouse.x, mouse.y);
                if (mat_inbounds(emberrect, mouse))
                {
                    drag = true;
                }
                update = true;
                break;

            case SDL_MOUSEBUTTONUP:
                mousepressed = false;
                if (drag)
                {
                    int szinti = mat_szintbacktrack(mouse);
                    if (szinti != -128)
                    {
                        utas temputas = {-128, -128, 0, 0, 0};
                        temputas.from = szinti;
                        int toszint = szintinput(renderer, temputas.from);
                        // ha panel közben kapunk exit kódot
                        if (toszint == -128)
                        {
                            quit = true;
                            break;
                        }
                        temputas.to = toszint;
                        temputas.dir = temputas.to > temputas.from ? 1 : -1;
                        printf("%d. szint %d\n", temputas.from, temputas.to);
                        int pickedlift = picklift(temputas, liftek, szintek);
                        utastomb_append(&(szintek[pickedlift][temputas.from+1]), temputas);
                        liftek[pickedlift].todo_from[temputas.from + 1] = true;
                        deltatime = 0;
                        curr = prev;
                    }
                }
                drag = false;
                update = true;
                break;

            case SDL_MOUSEMOTION:
                mouse = (vector){event.button.x, event.button.y};
                update = true; //drag
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
        //40 fps alatt belassul a program, így nem skippel szinteket
        if (deltatime > 40) deltatime = 40;


        // várólista management
        if (varokeleje != NULL)
        {
            update = true;
            varoktimer += deltatime;
            if (varoktimer >= varokeleje->adat.varoido)
            {
                //be a szintre
                //todo: implement the liftpick here too
                utas temputas = varokeleje->adat.varo;
                int pickedlift = picklift(temputas, liftek, szintek);
                utastomb_append(&(szintek[pickedlift][temputas.from+1]), temputas);
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

            // liftek, egyelőre csak egyet renderel
            for (int lifti = 0; lifti < 4; lifti++)
            {
                elvono *l = liftek + lifti;
                if (updatelift(renderer, deltatime, l, szintek[lifti]))
                    update = true;

                drawlift(renderer, *l, nyiltexture);
            }

            drawschonherz(renderer);
            drawwaitingppl(renderer, embertexture, szintek);

            if (drag)
                drawszintaccent(renderer, mouse);
            
            drawnber(renderer, embertexture, drag ? (vector){mouse.x - NBERX / 2, mouse.y - NBERY / 3} : (vector){emberrect.x, emberrect.y});
            SDL_RenderPresent(renderer);
            
        }
    }



    // cleanup memory
    SDL_DestroyTexture(embertexture);
    SDL_DestroyTexture(nyiltexture);

    SDL_Quit();

    //szintek
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            free(szintek[i][j].utasok);
        }
    }

    //liftek
    for (int i = 0; i < 4; i++)
    {
        free(liftek[i].inside.utasok);
    }

    // maradák várólista
    varolista_free(&varokeleje);

    return 0;
}
