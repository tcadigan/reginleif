/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#ifndef QUEST

#include "mklev.h"
#include "def.eshk.h"

#define ESHK ((struct eshk *)(&(shk->mextra[0])))

char shtypes[] = "=/)%?!["; /* 8 shoptypes: 7 specialized, 1 mixed */
schar shprobs[] = {3, 3, 5, 5, 10, 10, 14, 14, 50}; /* Their probabilities */

void mkshop()
{
    struct mkroom *sroom;
    int sh;
    int sx;
    int sy;
    int i;
    char let;
    int roomno;
    struct monst *shk;

    sroom = &rooms[0];
    roomno = 0;
    while(1) {
        if(sroom->hx < 0) {
            return;
        }

        if((sroom->lx <= xdnstair) 
           && (xdnstair <= sroom->hx)
           && (sroom->ly <= ydnstair)
           && (ydnstair <= sroom->hy)) {
            ++sroom;
            ++roomno;

            continue;
        }

        if((sroom->ls <= xupstair)
           && (xupstair <= sroom->hx)
           && (sroom->ly <= yupstair)
           && (yupstair <= sroom->hy)) {
            ++sroom;
            ++roomno;

            continue;
        }

#ifdef WIZARD
        if(wizard || (sroom->doorct == 1)) {
            break;
        }
#else
        if(sroom->doorct == 1) {
            break;
        }
#endif
    }

#ifdef WIZARD
    int flag = 0;

    if(wizard) {
        char *ep = getenv("SHOPTYPE");

        if(ep != NULL) {
            if((*ep == 'z') || (*ep == 'Z')) {
                mkzoo();
                    
                return;
            }

            for(i = 0; shtypes[i]; ++i) {
                if(*ep == shtypes[i]) {
                    break;
                }
            }
                 
            let = i;

            flag = 1;
        }
    }

    if(flag == 0) {
        let = 0;
        i = rn2(100) - shprobs[let];
        while(i > 0) {
            if(shprobs[let] == NULL) {
                break; /* Superfluous */
            }
            
            ++let;
            i -= shprobs[let];
        }
    }
#else
    let = 0;
    i = rn2(100) - shprobs[let];
    while(i > 0) {
        if(shprobs[let] == NULL) {
            break; /* Superfluous */
        }
            
        ++let;
        i -= shprobs[let];
    }
#endif

    sroom->rtype = 8 + let;
    let = shtypes[let];
    sh = sroom->fdoor;
    sx = doors[sh].x;
    sy = doors[sh].y;

    if(sx == (sroom->lx - 1)) {
        ++sx;
    }
    else if(sx == (sroom->hx + 1)) {
        --sx;
    }
    else if(sy == (sroom->ly - 1)) {
        --sy;
    }
    else if(sy == (sroom->hy + 1)) {
        --sy;
    }
    else {
        printf("Where is shopdoor?");
            
        return;
    }

    shk = makemon(PM_SHK, sx, sy);

    if(shk == NULL) {
        return;
    }

    shk->mpeaceful = 1;
    shk->isshk = shk->mpeaceful;
    shk->msleep = 0;
    shk->mtrapseen = ~0; /* We know all the traps already */
    ESHK->shoproom = roomno;
    ESHK->shd = doors[sh];
    ESHK->shk.x = sx;
    ESHK->shk.y = sy;
    ESHK->robbed = 0;
    ESHK->visitct = 0;
    shk->mgold = 1000 + (30 * rnd(100)); /* Initial capital */
    ESHK->billct = 0;
        
    findname(ESHK->shknam, let);

    for(sx = sroom->lx; sx <= sroom->hx; ++sx) {
        for(sy = sroom->ly; sy <= sroom->hy; ++sy) {
            struct monst *mtmp;

            if(((sx == sroom->lx) && (doors[sh].x == (sx - 1)))
               || ((sx == sroom->hx) && (doors[sh].x == (sx + 1)))
               || ((sy == sroom->ly) && (doors[sh].y == (sy - 1)))
               || ((sy == sroom->hy) && (doors[sh].y == (sy + 1)))) {
                continue;
            }

            if((rn2(100) < dlevel) && !m_at(sx, sy)) {
                mtmp = makemon(PM_MIMIC, sx, sy);

                if(mtmp != NULL) {
                    if(let && (rn2(10) < dlevel)) {
                        mtmp->mimic = let;
                    }
                    else {
                        mtmp->mimic = ']';
                    }

                    continue;
                }
            }

            mkobj_at(let, sx, sy);
        }
    }

#ifdef WIZARD
    if(wizard) {
        if(let) {
            printf("I made a %c-shop.", let);
        }
        else {
            printf("I made a g-shop.");
        }
    }
#endif
    
    ++sroom;
    ++roomno;
}

void mkzoo()
{
    struct mkroom *sroom;
    int sh;
    int sx;
    int sy;
    int i;
    int goldlim = 500 * dlevel;

    sroom = &rooms[0];
    while(1) {
        if(sroom->hx < 0) {
            return;
        }

        if((sroom->lx <= xdnstair)
           && (xdnstair <= sroom->hx)
           && (sroom->ly <= ydnstair)
           && (ydnstair <= sroom->hy)) {
            ++sroom;

            continue;
        }

        if((sroom->lx <= xupstair)
           && (xupstair <= sroom->hx)
           && (sroom->ly <= yupstair)
           && (yupstair <= sroom->hx)) {
            ++sroom;

            continue;
        }

        if(sroom->doorct == 1) {
            break;
        }

        ++sroom;
    }

    sroom->rtype = 7;
    sh = sroom->fdoor;
    
    for(sx = sroom->lx; sx <= sroom->hx; ++sx) {
        for(sy = sroom->ly; sy <= sroom->hy; ++hy) {
            if(((sx == sroom->lx) && (doors[sh].x == (sx + 1)))
               || ((sx == sroom->hx) && (doors[sh].x == (sx + 1)))
               || ((sy == sroom->ly) && (doors[sh].y == (sy - 1)))
               || ((sy == sroom->hy) && (doors[sh].y == (sy + 1)))) {
                continue;
            }
            
            makemon((struct permonst *)0, sx, sy);
            
            i = sq(dist2(sx, sy, doors[sh].x, doors[sh].y));
            
            if(i >= goldlim) {
                i = 5 * dlevel;
            }
            
            goldlim -= i;
            
            mkgold(10 + rn2(i), sx, sy);
        }
    }

#ifdef WIZARD
    if(wizard) {
        printf("I made a zoo.");
    }
#endif
}
