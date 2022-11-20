#include "grafika.h"
#include "adat.h"
#include "liftmat.h"

void ablak_cls(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void ablak_init(int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow("LifteSCH", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szeles, magas, 0);
    if (window == NULL)
    {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    //SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL)
    {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    *pwindow = window;
    *prenderer = renderer;
}

// remember to destory textures
SDL_Texture *ablak_loadtexture(SDL_Renderer *renderer, char filename[])
{
    SDL_Texture *txture = IMG_LoadTexture(renderer, filename);
    if (txture == NULL)
    {
        SDL_Log("Nem nyithato meg a kepfajl: %s", IMG_GetError());
        exit(1);
        return NULL;
    }
    return txture;
}

// feature: valamiért kerek szélek
void rectangleStrokedColor(SDL_Renderer *renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color, int stroke){
    for (int i = -stroke/2; i < stroke/2 +stroke%2; i++)
    {
        rectangleColor(renderer, x1-i, y1-i, x2+i, y2+i, color);
    }
}

void drawbox(SDL_Renderer *renderer, box b)
{
    if (b.filled)
        boxColor(renderer, b.rect.x, b.rect.y, b.rect.x + b.rect.w, b.rect.y + b.rect.h, b.color);
    else
        rectangleColor(renderer, b.rect.x, b.rect.y, b.rect.x + b.rect.w, b.rect.y + b.rect.h, b.color);
}

void drawschonherz(SDL_Renderer *renderer)
{
    // padlók
    for (int i = -1; i <= 18; i++)
    {
        // fekete körvonalas csíkok
        boxColor(renderer, SCHX1 - MARGOY, mat_szinty(i) - MARGOY, SCHX2 + MARGOY, mat_szinty(i) + MARGOY, FEKETE);
        lineColor(renderer, SCHX1, mat_szinty(i), SCHX2, mat_szinty(i), i == 0 ? FEHER : FEHER50);
        // emelet szám
        char tmpstr[3]; // 2 char + '\0'
        sprintf(tmpstr, "%2d", i);
        stringColor(renderer, SCHX2 + 4, mat_szinty(i) - 16, tmpstr, FEHER);
        // díszítés: - | / |
        //jobb
        lineColor(renderer, SCHX1 - MARGOX,             SCHY1 + 16,                 SCHX1 - 2*MARGOX,           SCHY1 + 16, FEHER);
        lineColor(renderer, SCHX1 - 2*MARGOX,           SCHY1 + 16,                 SCHX1 - 2*MARGOX,           SCHY1 + 3*SCHH/4, FEHER);
        lineColor(renderer, SCHX1 - 2*MARGOX,           SCHY1 + 3*SCHH/4,           SCHX1 - 2*MARGOX - SCHX1/2, SCHY1 + 3*SCHH/4 + SCHX1/2, FEHER);
        lineColor(renderer, SCHX1 - 2*MARGOX - SCHX1/2, SCHY1 + 3*SCHH/4 + SCHX1/2, SCHX1 - 2*MARGOX - SCHX1/2, WINY, FEHER);
        
        // bal
        lineColor(renderer, SCHX2 + MARGOX,             SCHY1 + 16,                 SCHX2 + 2*MARGOX,           SCHY1 + 16, FEHER);
        lineColor(renderer, SCHX2 + 2*MARGOX,           SCHY1 + 16,                 SCHX2 + 2*MARGOX,           SCHY1 + 3*SCHH/4, FEHER);
        lineColor(renderer, SCHX2 + 2*MARGOX,           SCHY1 + 3*SCHH/4,           SCHX2 + 2*MARGOX + SCHX1/2, SCHY1 + 3*SCHH/4 + SCHX1/2, FEHER);
        lineColor(renderer, SCHX2 + 2*MARGOX + SCHX1/2, SCHY1 + 3*SCHH/4 + SCHX1/2, SCHX2 + 2*MARGOX + SCHX1/2, WINY, FEHER);
    }
    rectangleColor(renderer, SCHX1 - MARGOX, SCHY1, SCHX2 + MARGOX, SCHY2, FEHER);
}

void drawnyil(SDL_Renderer *renderer, SDL_Texture *nyiltexture, int x, vector pos, bool flip)
{
    // 18x13 és 54x13
    SDL_Rect src = {x, 0, NYILX, NYILY};
    SDL_Rect dest = {pos.x, pos.y, NYILX, NYILY};
    SDL_RenderCopyEx(renderer, nyiltexture, &src, &dest, 0, NULL, flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

// a horgony középen lesz
void drawlift(SDL_Renderer *renderer, elvono lift, SDL_Texture *nyiltexture)
{
    // todo consts
    box liftbox = {(SDL_Rect){lift.pos.x, lift.pos.y, 32, 24}, FEHER, lift.state != LIFTBOARDING};
    // középre centerezés
    liftbox.rect.x -= liftbox.rect.w / 2;
    liftbox.rect.y -= liftbox.rect.h / 2;
    drawbox(renderer, liftbox);
    lineColor(renderer, liftbox.rect.x + liftbox.rect.w / 2, liftbox.rect.y, liftbox.rect.x + liftbox.rect.w / 2, SCHY1, FEHER25);
    char tmpstr[3]; // 2 char + '\0'
    sprintf(tmpstr, "%2d", lift.floor);
    stringColor(renderer, liftbox.rect.x+2, liftbox.rect.y+2, tmpstr, lift.state==LIFTBOARDING ? FEHER : FEKETE);

    if (lift.state == LIFTBOARDING)
    {
        drawnyil(renderer, nyiltexture, (int)lift.anim_board, (vector){lift.pos.x+16, lift.pos.y-4}, lift.anim_flip);
    }
    
}



void drawszintaccent(SDL_Renderer *renderer, vector mouse)
{
    int szinti = mat_szintbacktrack(mouse);
    if (szinti != -128)
    {
        int szinty = mat_szinty(szinti);
        rectangleStrokedColor(renderer, SCHX1, szinty, SCHX1 + SCHW, szinty - 32, FEHER, 3);
    }
}

void drawnber(SDL_Renderer *renderer, SDL_Texture *embertexture, vector pos)
{
    // 13x26
    SDL_Rect src = {0, 0, NBERX, NBERY};
    SDL_Rect dest = (SDL_Rect){pos.x, pos.y, NBERX, NBERY};
    SDL_RenderCopy(renderer, embertexture, &src, &dest);
}

void drawwaitingppl(SDL_Renderer *renderer, SDL_Texture *embertexture, utastomb szintek[][20]){
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            if (szintek[i][j].meret <= 8){
                for (int k = szintek[i][j].meret-1; k >= 0; k--)
                {
                    drawnber(renderer, embertexture, (vector){mat_liftx(i)+32 + k * 7, mat_szinty(j) + 6});
                }
            }
            else
            {
                drawnber(renderer, embertexture, (vector){mat_liftx(i)+32, mat_szinty(j) + 6});
                char tmpstr[5]; // 4 char + '\0'
                sprintf(tmpstr, "%4d", szintek[i][j].meret);
                stringColor(renderer, mat_liftx(i)+64, mat_szinty(j) + 12, tmpstr, FEHER);
            }
            
            
        }
    }
}

void uiinit(button buttons[]){
    for (int sor = 0; sor < 4; sor++)
    {
        for (int oszlop = 0; oszlop < 3; oszlop++)
        {
            int index = sor*3+oszlop;
            buttons[index] = (button){
                (SDL_Rect){PINX1 + PADDING + oszlop*(PADDING + CELLSIZE), PINY1 + PADDING + (sor+1)*(PADDING + CELLSIZE), CELLSIZE, CELLSIZE},
                0,
                false,
                NULL
            };
            if (sor != 4)
            {
                buttons[index].label = '1' + index;
            }            
        }
        
    }
    buttons[9].label = '*';
    buttons[10].label = '0';
    buttons[11].label = '-';
}

char buttonnumber(int index){
    if (!mat_inbetween(1, 9, ++index))
    {
        if (index == 11) return '0';
        else if (index == 12) return '-';
        else return '*';
    }
    return '0' + index;
}


void drawpin(SDL_Renderer *renderer, double scale, button buttons[], char panel[], double inputbar){
    rectangleColor(renderer, PINX1+PADDING, PINY1+PADDING, PINX1+3*(CELLSIZE+PADDING), PINY1+PADDING+CELLSIZE, FEHER);
    for (int i = 0; i < 12; i++)
    {
        if (buttons[i].pressed)
            drawbox(renderer, (box){buttons[i].rect, FEHER25, true});
        drawbox(renderer, (box){buttons[i].rect, FEHER, false});
        characterColor(renderer, buttons[i].rect.x+16, buttons[i].rect.y+16, buttons[i].label, FEHER);
    }
    stringColor(renderer, PINX1+2*PADDING, PINY1+2*PADDING, panel, FEHER);

    boxColor(renderer, (int)(PINX1+PADDING+(3*(CELLSIZE+PADDING)-PADDING)*(inputbar)), PINY1 + PADDING + CELLSIZE-8, PINX2 - PADDING - 1, PINY1 + PADDING + CELLSIZE -1, FEHER);

    if (scale != 1)
        boxColor(renderer, PINX1, PINY1 + (int)(PINH*scale) - PADDING, PINX2, PINY2, FEKETE);  
    rectangleColor(renderer, PINX1, PINY1, PINX2, PINY1 + (int)(PINH*scale), FEHER);
}

void drawuiaccent(SDL_Renderer *renderer, button buttons[], vector mouse, bool pressed){
    int i = mat_buttoni(buttons, 12, mouse);
    if (i == -1) return;
    rectangleStrokedColor(renderer, buttons[i].rect.x, buttons[i].rect.y, buttons[i].rect.x + buttons[i].rect.w, buttons[i].rect.y + buttons[i].rect.h, FEHER, 3);
    
}

void clearpinpad(SDL_Renderer *renderer){
    boxColor(renderer, PINX1, PINY1, PINX2, PINY2, FEKETE);
}

int szintinput(SDL_Renderer *renderer, int from){
    button buttons[12];
    uiinit(buttons);
    char paneltext[3] = "  ";
    

    //opening animation
    int timer = SDL_GetTicks();
    int animdur = 500;
    double smoothscale = 0;
    int stoptime;
    for (stoptime = animdur + SDL_GetTicks(); timer < stoptime; timer = SDL_GetTicks())
    {
        clearpinpad(renderer);
        smoothscale = 1-(double)(stoptime-timer)/animdur;
        drawpin(renderer, smoothscale, buttons, paneltext, 0);
        SDL_RenderPresent(renderer);
    }


    SDL_Event event;
    bool update = true;
    vector mouse = {0, 0};
    bool mousepressed = false;
    int result = -128;

    //input bar
    bool freezetimer = true;
    timer = SDL_GetTicks();
    int inputdur = 1000;
    smoothscale = 0;
    stoptime = inputdur + SDL_GetTicks();
    

    bool quit = false;
    while (!quit){
        
        //calculate inpoot bar
        if (!freezetimer && timer != SDL_GetTicks()){
            timer = SDL_GetTicks();
            smoothscale = 1-(double)(stoptime-timer)/inputdur;
            if (stoptime < timer)
                quit = true;
            update = true;
        }

        // processing events
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                mousepressed = true;
                // printf("%d, %d\n", mouse.x, mouse.y);
                if (mat_buttoni(buttons, 12, mouse) != -1){
                    buttons[mat_buttoni(buttons, 12, mouse)].pressed = true;
                    stoptime = inputdur + SDL_GetTicks();
                    freezetimer = false;
                }
                    
                update = true;
                break;

            case SDL_MOUSEBUTTONUP:
                mousepressed = false;
                for (int i = 0; i < 12; i++)
                {
                    if (buttons[i].pressed)
                    {
                        if (paneltext[1] == '?')
                        {
                            paneltext[0] = ' ';
                            paneltext[1] = ' ';
                        }
                        
                        char bn = buttonnumber(i);
                        if (paneltext[1] == ' ')
                        {
                            paneltext[1] = bn;
                        }
                        else if (paneltext[0] == ' ')
                        {
                            paneltext[0] = paneltext[1];
                            paneltext[1] = bn;
                            quit = true;
                        }
                    }
                    buttons[i].pressed = false;
                }
                update = true;
                break;

            case SDL_MOUSEMOTION:
                mouse = (vector){event.button.x, event.button.y};
                update = true;
                break;

            case SDL_QUIT:
                return -128;
                break;
            default:
                break;
            }
        }

        //checking if result is valid
        if (quit)
        {
            // *
            if (paneltext[1] == '*' || paneltext[0] == '*')
            {
                if (paneltext[1] == '*' && (paneltext[0] == ' ' || paneltext[0] == '*'))
                    result = 0;
                else
                    quit = false;
            }

            // ?-
            else if (paneltext[1] == '-')
            {
                // --
                if (paneltext[0] == '-')
                    result = -1;
                // X-
                else
                    result = paneltext[0]-'0' -1;
                quit = false;
            }

            // ?x
            else if (paneltext[1] != ' ')
            {
                // ?x
                result = paneltext[1]-'0';
                // -x
                if (paneltext[0] == '-')
                    result *= -1;
                // xx
                else if (paneltext[0] != ' ')
                    result += (paneltext[0]-'0')*10;
            }

            // ?_
            else
                quit = false;


            if (!quit || !mat_inbetween(-1, 18, result) || result == from)
            {
                paneltext[0] = '?';
                paneltext[1] = '?';
                freezetimer = true;
                smoothscale = 0;
                quit = false;
            }
        }
        
        //rendering
        if (update)
        {
            clearpinpad(renderer);
            drawpin(renderer, 1.0, buttons, paneltext, smoothscale);
            drawuiaccent(renderer, buttons, mouse, mousepressed);
            SDL_RenderPresent(renderer);
            update = false;
        }
        
    }


    //closing anim
    timer = SDL_GetTicks();
    for (stoptime = animdur + SDL_GetTicks(); timer < stoptime; timer = SDL_GetTicks())
    {
        clearpinpad(renderer);
        smoothscale = (double)(stoptime-timer)/animdur;
        drawpin(renderer, smoothscale, buttons, paneltext, 1);
        SDL_RenderPresent(renderer);
    }

    clearpinpad(renderer);
    SDL_RenderPresent(renderer);
   

    printf("%d\n", result);
    return result;
}