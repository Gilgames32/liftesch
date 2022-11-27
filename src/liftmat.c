#include "liftmat.h"

// indexként térít vissza, nem szintként!
int todo_min(bool todo_to[], bool todo_from[], bool fisrtonly)
{
    for (int i = 0; i < 20; i++)
        if (todo_to[i] || (todo_from[i] && !fisrtonly))
            return i;

    return INVALID;
}

// indexként térít vissza, nem szintként!
int todo_max(bool todo_to[], bool todo_from[], bool fisrtonly)
{
    for (int i = 20 - 1; i >= 0; i--)
        if (todo_to[i] || (todo_from[i] && !fisrtonly))
            return i;

    return INVALID;
}

bool updatelift(SDL_Renderer *renderer, int deltatime, elvono *l, utastomb szintoszlop[], Uint32 localtime, avg *waitt, avg *travelt)
{
    bool update = false;
    switch (l->state)
    {

    // üres todolist esetén idle
    case LIFTIDLE:
        // ha pont arra a szintre érkezett valaki
        if (szintoszlop[l->floor + 1].meret != 0)
        {
            l->state = LIFTBOARDING;
            // nulla nem maradhat mert bebugol
            l->direction = -1;
            update = true;
            break;
        }

        // ha máshova érkeztek
        for (int j = 0; j < 20; j++)
        {
            if (l->todo_from[j] || l->todo_to[j])
            {
                // megfelelő irányba elindítás
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
            ((l->direction == +1 && l->pos.y <= mat_szinty(l->floor) - LOS) ||
             (l->direction == -1 && l->pos.y >= mat_szinty(l->floor) - LOS)))
        {
            l->anim_pre = false;

            // ha van kit letenni, vagy még nincs tele és van kit felvenni
            if (l->todo_to[l->floor + 1] || (l->inside.meret < l->maxppl && l->todo_from[l->floor + 1]))
            {
                // megláll és snap
                l->pos.y = mat_szinty(l->floor) - LOS;
                l->anim_y = (double)l->pos.y;
                l->state = LIFTBOARDING;
                break;
            }
        }

        // mozgatás
        l->anim_y -= (double)deltatime / LANIMSPEED * l->direction;
        l->pos.y = (int)l->anim_y;

        break;

    case LIFTBOARDING:
        update = true;

        utastomb *varok = &(szintoszlop[l->floor + 1]);
        bool *vankiszallo = &(l->todo_to[l->floor + 1]);

        // vissza idlebe ha nincsenek utasok
        if (varok->meret == 0 && l->inside.meret == 0)
        {
            l->direction = 0;
            l->state = LIFTIDLE;
            l->todo_from[l->floor + 1] = false;
            l->todo_to[l->floor + 1] = false;
            break;
        }

        // vissza moveba ha tele van
        if (l->inside.meret >= l->maxppl && !*vankiszallo)
        {
            l->state = LIFTMOVE;
            break;
        }

        // nyíl anim apply
        l->anim_board += (double)deltatime / BANIMSPEED;
        l->anim_flip = *vankiszallo;

        // 1 cycle
        if (l->anim_board >= 3*NYILX)
        {
            // reset cycle
            l->anim_board = 0;

            // nyíl iránya alapján :stuff:
            if (*vankiszallo)
            {
                // ki
                for (int i = 0; i < l->inside.meret; i++)
                {
                    if (l->inside.utasok[i].to == l->floor)
                    {
                        // calc stats
                        waitt->sum += localtime - l->inside.utasok[i].waiting;
                        waitt->cnt++;
                        travelt->sum += localtime - l->inside.utasok[i].traveling;
                        travelt->cnt++;
                        utastomb_indexremove(&(l->inside), i);

                        // van e még kiszálló?
                        *vankiszallo = false;
                        for (int i = 0; i < l->inside.meret; i++)
                            if (l->inside.utasok[i].to == l->floor)
                            {
                                *vankiszallo = true;
                                break;
                            }

                        // egyszerre csak egy
                        break;
                    }
                }
            }
            else
            {
                // legelső váró be
                utas temputas = varok->utasok[0];
                temputas.traveling = localtime;
                // várókból a liftbe rajka
                utastomb_indexremove(varok, 0);
                utastomb_append(&(l->inside), temputas);
                // célállomás bejelölése
                l->todo_to[temputas.to + 1] = true; //!

                // van e még beszálló?
                l->todo_from[l->floor + 1] = varok->meret != 0;
            }

            // indulás tovább
            // ha már nincs kiszálló és vannak bent utasok, nincsenek felszállók, vagy már megtelt a fülke
            if ((!*vankiszallo && (l->inside.meret != 0 && varok->meret == 0)) || l->inside.meret >= l->maxppl)
            {
                // ha felfele dir de ez a legfelső pont vagy
                // ha lefele dir de ez a legalsó pont
                // this is a terrible way of doing this
                if (l->floor == 18 || (l->direction == +1 && l->floor + 1 >= todo_max(l->todo_to, l->todo_from, l->inside.meret >= l->maxppl)))
                    l->direction = -1;
                if (l->floor == -1 || (l->direction == -1 && l->floor + 1 <= todo_min(l->todo_to, l->todo_from, l->inside.meret >= l->maxppl)))
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

int picklift(utas temputas, elvono liftek[], utastomb szintek[][20])
{
    // printf("picklift called from %d to %d\n", temputas.from, temputas.to);
    int tavok[4] = {};
    for (int i = 0; i < 4; i++)
    {
        tavok[i] = megeri(temputas, liftek[i], szintek[i]);
        printf("%c lift - %d steps\n", 'A' + i, tavok[i]);
    }

    int minindex = 0;
    for (int i = 0; i < 4; i++)
    {
        if (tavok[minindex] > tavok[i])
        {
            minindex = i;
        }   
    }
    
    printf("picked: %c\n", 'A' + minindex);
    return minindex;
}

int megeri(utas temputas, elvono l, utastomb eredetiszintoszlop[]){
    // erre az emeletre mikor jön jó irányban
    // amikor van ilyen megvizsgáljuk hogy befére (minden útbaeső szinten beférek-e)
    // ha igen
        // return lépésszám
    // ha nem
        // jön e majd ide?
            // minden szinten beférek e?
        // ha nem
            // a végén jöjjön értem

    // 1 beszálló: 3*nyilx*banim lépés
    // 1 szint = schszint*lanim lépés
    // nagyjából időarányos

    // klónozás: liftinside és szintoszlop dinamikus ezért klónozzuk hogy ne csessze szét a többit
    l.inside = utastomb_clone(l.inside);
    // ha már elindult erről a szintről a lift akkor az ilyen szempontból már a következőn van;
    // if (!l.anim_pre && l.state == LIFTMOVE) l.floor += l.direction;
    // temporary medicine
    if (l.direction == 0) l.state = LIFTIDLE;
    
    utastomb szintoszlop[20] = {};
    for (int i = 0; i < 20; i++)
    {
        szintoszlop[i] = utastomb_clone(eredetiszintoszlop[i]); 
    }
    

    int lepesek = 0;
    while (true)
    {
        // itt van? jó irányba megy? beférek?
        if (l.floor == temputas.from && (l.direction == LIFTIDLE || l.direction == temputas.dir) && l.inside.meret < l.maxppl)
        {
            // a többi útbaeső szinten is befér?
            bool befer = true;
            int utasok = l.inside.meret;
            for (int i = temputas.from+1; i < temputas.to; i++)
            {
                // ki mindenki
                for (int j = 0; j < l.inside.meret; j++)
                {
                    if (l.inside.utasok[j].to == l.floor)
                    {
                        utasok--;
                    }
                }
                // be aki tud
                utasok += szintoszlop[l.floor+1].meret;
                if (utasok > l.maxppl)
                {
                    befer = false;
                    break;
                }
            }

            if (befer) break;
        }

        int lepes = leptet(temputas, &l, szintoszlop);

        if (lepes == -1) // azaz a lift végzett mindennel
        {
            // milyen messze van?
            lepesek += abs(l.floor - temputas.from) * LSTEP;
            break;
        }
        else
        {
            lepesek += lepes;
        }
        

    }
    
    // felszabadítás
    free(l.inside.utasok);
    for (int i = 0; i < 20; i++)
    {
        free(szintoszlop[i].utasok);
    }

    return lepesek;
}

int leptet(utas temputas, elvono *l, utastomb szintoszlop[]){
    // liftupdate kibelezett verziója
    int dur = 0;
    switch (l->state)
    {

    case LIFTIDLE:
        for (int j = 0; j < 20; j++)
        {
            if (l->todo_from[j] || l->todo_to[j])
            {
                // ha a jelenlegi szintre érkezett
                if (j == l->floor + 1)
                {
                    l->state = LIFTBOARDING;
                }
                else
                {
                    // megfelelő irányba elindítás
                    l->direction = l->floor < j ? 1 : -1;
                    l->state = LIFTMOVE;
                }
                break;
            }
        }
        // true idle
        return -1;
        break;

    // mozgatás
    case LIFTMOVE:
        // ha van kit letenni, vagy még nincs tele és van kit felvenni
        if (l->todo_to[l->floor + 1] || (l->inside.meret < l->maxppl && l->todo_from[l->floor + 1]))
        {
            // megláll
            l->state = LIFTBOARDING;
            break;
        }

        // mozgatás
        l->floor += l->direction;

        dur += LSTEP;
        break;

    case LIFTBOARDING:
        ; // ha ezt törlöd meghal a program (real)
        utastomb *varok = &(szintoszlop[l->floor + 1]);

        // vissza idlebe ha nincsenek utasok
        if (varok->meret == 0 && l->inside.meret == 0)
        {
            l->direction = 0;
            l->state = LIFTIDLE;
            l->todo_from[l->floor + 1] = false;
            l->todo_to[l->floor + 1] = false;
            break;
        }

        // ki mindenki
        for (int i = 0; i < l->inside.meret; i++)
        {
            if (l->inside.utasok[i].to == l->floor)
            {
                utastomb_indexremove(&(l->inside), i);
                dur += 3*NYILX*BANIMSPEED;
                i--;
            }
        }
        l->todo_to[l->floor + 1] = false;
        // be aki tud
        while (varok->meret > 0 && l->inside.meret < l->maxppl)
        {
            utas temputas = varok->utasok[0];
            // várókból a liftbe rajka
            utastomb_indexremove(varok, 0);
            utastomb_append(&(l->inside), temputas);
            dur += BSTEP;

            // célállomás bejelölése
            l->todo_to[temputas.to + 1] = true;

            // van e még beszálló?
            l->todo_from[l->floor + 1] = varok->meret != 0;
        }

        // indulás tovább
        // ha vannak bent utasok, nincsenek felszállók, vagy már megtelt a fülke
        if ((l->inside.meret != 0 && varok->meret == 0) || l->inside.meret >= l->maxppl)
        {
            // ha felfele dir de ez a legfelső pont vagy
            // ha lefele dir de ez a legalsó pont
            // this is a terrible way of doing this
            if (l->floor == 18 || (l->direction == +1 && l->floor + 1 >= todo_max(l->todo_to, l->todo_from, l->inside.meret >= l->maxppl)))
                l->direction = -1;
            if (l->floor == -1 || (l->direction == -1 && l->floor + 1 <= todo_min(l->todo_to, l->todo_from, l->inside.meret >= l->maxppl)))
                l->direction = +1;

            // és elindul
            l->state = LIFTMOVE;
        }

        break;

    default:
        break;
    }

    // printf("%c lift %d szinten %d stateben %d todoval\n", l->id, l->floor, l->state, l->todo_to[l->floor+1]);
    return dur;
}
