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
int todo_min(bool todo[]){
    for (int i = 0; i < 20; i++)
    if (todo[i])
            return i;

    return -128;
}

// indexként térít vissza, nem szintként!
int todo_max(bool todo[]){
    for (int i = 20-1; i >= 0; i--)
    if (todo[i])
        return i;

    return -128;
}

int picklift(utas temputas, elvono liftek[], utas szintek[][20])
{
    return 0;
}

bool updatelift(SDL_Renderer *renderer, int deltatime, elvono *l, utastomb szint[]){
    bool update = false;
    //elvono *l = liftek + lifti;
    switch (l->state)
    {
    // üres todolist
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
            if (l->todo[j])
            {
                l->state = LIFTMOVE;
                //megfelelő irányba elindítás
                l->direction = l->floor < j ? 1 : -1;
                update = true;
                break;
            }
        }
        break;

    case LIFTMOVE:
        update = true;
        //known bug: pont mikor már elindult arról a szintről és raksz oda valakit visszasnappel
        l->floor = mat_szinti(l->pos.y + (l->direction*LOS));
        // ha nincs todo szinten vagy tele van
        if (!l->todo[l->floor+1]) // || tele van
        {
            //mozgatás
            l->animy -= (double)deltatime/10 * l->direction;
            l->pos.y = (int)l->animy;
        }
        // van a szinten todo
        else
        {
            // megláll és snap
            l->pos.y = mat_szinty(l->floor)-LOS;
            l->todo[l->floor+1] = false; //? ez kell de miért (igen enélkül meghal a program)
            l->state = LIFTBOARDING;
        }
        break;

    case LIFTBOARDING:
        update = true;
        utastomb *varok = &(szint[l->floor+1]);

        // vissza idlebe ha nincsenek utasai
        if(varok->meret == 0 && l->inside.meret == 0)
        {
            l->direction = 0;
            l->state = LIFTIDLE;
            l->todo[l->floor+1] = false;
            printf("backtoidle\n");
            break;
        }

        l->animf = false;
        // ez framenként nem előnyös but it is what it is :shrug:
        for (int i = 0; i < l->inside.meret; i++)
            if (l->inside.utasok[i].to == l->floor)
                l->animf = true;
                
        //nyíl anim
        l->animb += (double)deltatime/10;

        //1 cycle
        if (l->animb >= 54)
        {
            //reset cycle
            l->animb = 0;

            // nyíl iránya alapján :stuff:
            if (l->animf)
            {
                //ki
                for (int i = 0; i < l->inside.meret; i++)
                {
                    if (l->inside.utasok[i].to == l->floor)
                    {
                        utastomb_indexremove(&(l->inside), i);
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
                    l->todo[temputas.to+1] = temputas.dir;
                    printf("be\n");
                    //egyszerre csak egy
                    break;
                }
            }

            // indulás tovább
            // ha van utas és mindenki aki tudott felszállt
            if (l->inside.meret != 0 && varok->meret == 0) // || tele van
            {
                // ha felfele dir de ez a legfelső pont vagy
                // ha lefele dir de ez a legalsó pont
                printf("%d, %d, %d, %d\n", l->direction, l->floor, todo_max(l->todo)-1, todo_min(l->todo)-1);
                if ((l->direction == +1 && l->floor+1 >= todo_max(l->todo)) ||
                    (l->direction == -1 && l->floor+1 <= todo_min(l->todo)))
                {
                    l->direction *= -1; // megfordul
                }
                l->state = LIFTMOVE;
                
            }
            // sidenote: ez hova is kéne pontosan?
            // ha már nincsenek várók akkor todo false
            l->todo[l->floor+1] = varok->meret != 0;

        }
        
        
        break; 
    
    default:
        break;
    }

    return update;
}