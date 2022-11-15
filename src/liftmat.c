#include "liftmat.h"
#include "adat.h"
#include "grafika.h"

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

bool mat_inrange(int a, int b, int range)
{
    return range >= abs(a - b);
}

// egyszerű between függvény, a-c-b, egyenlőség meg van engedve mind2 oldalon
bool mat_inbetween(int a, int b, int c)
{
    return (a >= c && c >= b) || (b >= c && c >= a);
}

// megnézi hogy c b-n belül van-é
bool mat_inbounds(SDL_Rect b, vector c)
{
    return mat_inbetween(b.x, b.x + b.w, c.x) && mat_inbetween(b.y, b.y + b.h, c.y);
}

// -128 invalidot jelez (igen mert van -1. szint bruhhhh)
int mat_szintbacktrack(vector mouse)
{
    if (mat_inbetween(SCHX1 - MARGOX, SCHX1 + SCHW + MARGOX, mouse.x) && mat_inbetween(-1, 18, mat_szinti(mouse.y)))
        return mat_szinti(mouse.y);
    else
        return -128;
}

int mat_buttoni(button buttons[], int meret, vector mouse){
    for (int i = 0; i < meret; i++)
    {
        if (mat_inbounds(buttons[i].rect, mouse))
        {
            return i;
        }
    }
    return -1;
}

vector vectoradd(vector v1, vector v2)
{
    return (vector){v1.x + v2.x, v1.y + v2.y};
}
vector vectorscale(vector v, double w, double h)
{
    return (vector){v.x * w, v.y * h};
}


// indexként térít vissza, nem szintként!
int todo_min(bool todo_from[], bool todo_to[]){
    for (int i = 0; i < 20; i++)
    if (todo_from[i] || todo_to[i])
            return i;

    return -128;
}

// indexként térít vissza, nem szintként!
int todo_max(bool todo_from[], bool todo_to[]){
    for (int i = 20-1; i >= 0; i--)
    if (todo_from[i] || todo_to[i])
        return i;

    return -128;
}

int picklift(utas temputas, elvono liftek[], utas szintek[][20])
{
    return 0;
}

bool updatelift(SDL_Renderer *renderer, int deltatime, elvono *l, utastomb szint[]){
    bool update = false;
    switch (l->state)
    {


    // üres todolist esetén idle
    case LIFTIDLE:
        // ha pont arra a szintre érkezett valaki
        if (szint[l->floor+1].meret != 0)
        {
            l->state = LIFTBOARDING;
            update = true;
            break;
        }

        // ha máshova érkeztek
        for (int j = 0; j < 20; j++)
        {
            if (l->todo_from[j] || l->todo_to[j])
            {
                //megfelelő irányba elindítás
                l->direction = l->floor < j ? 1 : -1;
                l->state = LIFTMOVE;
                update = true;
                break;
            }
        }

        break;


    // mozgatás
    case LIFTMOVE:
        update = true;

        // ha új szintre ér akkor már a snapvonal előtt lesz, nem utána
        if (l->floor != mat_szinti(l->pos.y))
        {
            l->floor = mat_szinti(l->pos.y);
            l->anim_pre = true;
        }

        // ha pre de már a snapvonal után van, azaz elérte
        if (l->anim_pre &&
            (
                l->direction == +1 && l->pos.y <= mat_szinty(l->floor)-LOS
                ||
                l->direction == -1 && l->pos.y >= mat_szinty(l->floor)-LOS
            )
        )
        {
            l->anim_pre = false;

            // ha van kit letenni, vagy még nincs tele és van kit felvenni
            if (l->todo_to[l->floor+1] || (l->inside.meret < l->maxppl && l->todo_from[l->floor+1]))
            {
                // megláll és snap
                l->pos.y = mat_szinty(l->floor)-LOS;
                l->anim_y = (double)l->pos.y;
                l->state = LIFTBOARDING;
                break;
            }
        }

        //mozgatás
        l->anim_y -= (double)deltatime/10 * l->direction;
        l->pos.y = (int)l->anim_y;

        break;


    case LIFTBOARDING:
        update = true;

        utastomb *varok = &(szint[l->floor+1]);
        bool *vankiszallo = &(l->todo_to[l->floor+1]);

        // vissza idlebe ha nincsenek utasok
        if(varok->meret == 0 && l->inside.meret == 0)
        {
            l->direction = 0;
            l->state = LIFTIDLE;
            l->todo_from[l->floor+1] = false;
            l->todo_to[l->floor+1] = false;
            printf("backtoidle\n");
            break;
        }

        //vissza moveba ha tele van
        if (l->inside.meret >= l->maxppl && !*vankiszallo)
        {
            l->state = LIFTMOVE;
            printf("backtomov\n");
            break;
        }

        //nyíl anim apply
        l->anim_board += (double)deltatime/5;
        l->anim_flip = *vankiszallo;

        //1 cycle
        if (l->anim_board >= 54)
        {
            printf("%d", *vankiszallo);
            //reset cycle
            l->anim_board = 0;

            // nyíl iránya alapján :stuff:
            if (*vankiszallo)
            {
                //ki
                for (int i = 0; i < l->inside.meret; i++)
                {
                    if (l->inside.utasok[i].to == l->floor)
                    {
                        utastomb_indexremove(&(l->inside), i);
                        
                        // van e még kiszálló?
                        *vankiszallo = false;
                        for (int i = 0; i < l->inside.meret; i++)
                            if (l->inside.utasok[i].to == l->floor){
                                *vankiszallo = true;
                                break;
                            }
                        

                        printf("ki\n");
                        //egyszerre csak egy
                        break;
                    }
                }
            }
            else
            {
                //be
                for (int i = 0; i < varok->meret; i++)
                {
                    utas temputas = varok->utasok[i];
                    // várókból a liftbe rajka
                    utastomb_indexremove(varok, i);
                    utastomb_append(&(l->inside), temputas);
                    // célállomás bejelölése
                    l->todo_to[temputas.to+1] = temputas.dir;
                    
                    // van e még beszálló?
                    l->todo_from[l->floor+1] = varok->meret != 0;

                    printf("be\n");
                    //egyszerre csak egy
                    break;
                }
            }

            // indulás tovább
            // ha már nincs kiszálló és vannak bent utasok, nincsenek felszállók, vagy már megtelt a fülke
            if (!*vankiszallo && (l->inside.meret != 0 && varok->meret == 0) || l->inside.meret >= l->maxppl)
            {
                // ha felfele dir de ez a legfelső pont vagy
                // ha lefele dir de ez a legalsó pont
                if (l->direction == +1 && l->floor+1 >= todo_max(l->todo_from, l->todo_to))
                    l->direction = -1;
                if (l->direction == -1 && l->floor+1 <= todo_min(l->todo_from, l->todo_to))
                    l->direction = +1;

                // és elindul
                l->state = LIFTMOVE;
                
            }

        }
        
        
        break; 
    
    default:
        break;
    }

    return update;
}