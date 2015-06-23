/* Copyright (C) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#ifdef MKLEV
#include "mklev.h"

extern char *fut_geno;
#else
#include "hack.h"

extern char fut_geno[];
#endif

extern char *index();

struct monst zeromonst;

/*
 * Called with [x, y] = coordinates;
 * [0, 0] means any place
 * [u.ux, u.uy] meands call mnexto (not in MKLEV)
 *
 * In case we make an Orc of killer bee, we make an entire
 * horde (swarm).
 *
 * Note: That in this case we return only one of them
 *       (the one at [x, y])
 */
struct monst *makemon(struct permonst *ptr, int x, int y)
{
    struct monst *mtmp;
    int tmp;
    int ct;
    boolean anything = 0;

    if(ptr != 0) {
        anything = 1;
    }

    if((x != 0) || (y != 0)) {
        if(m_at(x, y) != 0) {
            return (struct monst *)0;
        }
    }

    if(ptr != NULL) {
        if(index(fut_geno, ptr->mlet) != 0) {
            return (struct monst *)0;
        }
    }
    else {
        ct = CNUM - strlen(fut_geno);
        
        if(index(fut_geno, 'm') != 0) {
            /* Make only 1 minotaur */
            ++ct;
        }

        if(index(fut_geno, '@') != 0) {
            ++ct;
        }

        if(ct <= 0) {
            /* No more monsters! */
            return 0;
        }

        tmp = rn2(((ct * dlevel) / 24) + 7);

        if(tmp < (dlevel - 4)) {
            tmp = rn2(((ct * dlevel) / 24) + 12);
        }

        if(tmp >= ct) {
            tmp = rn1(ct - (ct / 2), (ct / 2));
        }

        int flag = 0;
        for(ct = 0; ct < CMNUM; ++ct) {
            ptr = &mons[ct];

            if(index(fut_geno, ptr->mlet) != 0) {
                continue;
            }

            if(tmp == 0) {
                --tmp;
                flag = 1;

                break;
            }
            
            --tmp;
        }

        if(flag == 0) {
            panic("makemon?");
        }
    }

    mtmp = newmonst(ptr->pxlth);

    /* Clear all entries in structure */
    *mtmp = zeromonst;

    for(ct = 0; ct < ptr->pxlth; ++ct) {
        ((char *)&(mtmp->mextra[0]))[ct] = 0;
    }

    mtmp->nmon = fmon;
    fmon = mtmp;

#ifdef MKLEV
    mtmp->m_id = flags.ident;
    ++flags.ident;
#endif

    mtmp->data = ptr;
    mtmp->mxlth = ptr->pxlth;

    if(ptr->mlet == 'D') {
        mtmp->mhp = 80;
        mtmp->orig_hp = mtmp->mhp;
    }
    else if(ptr->mlevel == 0) {
        mtmp->mhp = rnd(4);
        mtmp->orig_hp = mtmp->mhp;
    }
    else {
        mtmp->mhp = d(ptr->mlevel, 8);
        mtmp->orig_hp = mtmp->mhp;
    }

    mtmp->mx = x;
    mtmp->my = y;
    mtmp->mcansee = 1;

    if(ptr->mlet == 'M') {
        mtmp->mimic = ']';
    }

#ifndef MKLEV
    if((x == u.ux) && (y == u.uy)) {
        mnexto(mtmp);
    }

    if((x == 0) && (y == 0)) {
        rloc(mtmp);
    }
#endif

    if((ptr->mlet == 's') || (ptr->mlet == 'S')) {
        mtmp->mundetected = 1;
        mtmp->mhide = mtmp->mundetected;
        
#ifdef MKLEV
        if((mtmp->mx != 0) && (mtmp->my != 0)) {
            mkobj_at(0, mtmp->mx, mtmp->my);
        }
#endif
    }

    if(ptr->mlet == ':') {
        mtmp->cham = 1;

#ifndef MKLEV
        newcham(mtmp, &mons[(dlevel + 14) + rn2((CMNUM - 14) - dlevel)]);
#endif
    }

    if(ptr->mlet == 'I') {
        mtmp->minvis = 1;
    }

#ifdef MKLEV
    if((ptr->mlet == 'L') || (ptr->mlet == 'N') || (rn2(5) != 0)) {
        mtmp->msleep = 1;
    }
#else
    if((ptr->mlet == 'L') || (ptr->mlet == 'N')) {
        mtmp->msleep = 1;
    }
#endif

#ifndef NOWORM
#ifndef MKLEV
    if((ptr->mlet == 'w') && (getwn(mtmp) != NULL)) {
        initworm(mtmp);
    }
#endif
#endif

    if(anything != 0) {
        if((ptr->mlet == 'O') || (ptr->mlet == 'k')) {
            coord enexto();
            coord mm;
            int cnt = rnd(10);

            mm.x = x;
            mmy.y = y;
            
            while(cnt != 0) {
                --cnt;
                mm = enexto(mm.x, mm.y);
                makemon(ptr, mm.x, mm.y);
            }

            --cnt;
        }
    }

    return mtmp;
}

coord enexto(xchar xx, xchar yy)
{
    xchar x;
    xchar y;
    coord foo[15];
    coord *tfoo;
    int range;

    tfoo = foo;
    range = 1;

    /* Full kludge action */

    for(x = xx - range; x <= (xx + range); ++x) {
        if(goodpos(x, yy - range) != 0) {
            tfoo->x = x;
            tfoo->y = yy - range;
            ++tfoo;

            if(tfoo == &foo[15]) {
                return foo[rn2(tfoo - foo)];
            }
        }
    }

    for(x = xx - range; x <= (xx + range); ++x) {
        if(goodpos(x, yy + range) != 0) {
            tfoo->x = x;
            tfoo->y = yy + range;
            ++tfoo;

            if(tfoo == &foo[15]) {
                return foo[rn2(tfoo - foo)];
            }
        }
    }

    for(y = (yy + 1) - range; y < (yy + range); ++y) {
        if(goodpos(xx - range, y) != 0) {
            tfoo->x = xx - range;
            tfoo->y = y;
            ++tfoo;

            if(tfoo == &foo[15]) {
                return foo[rn2(tfoo - foo)];
            }
        }
    }

    for(y = (yy + 1) - range; y < (yy + range); ++y) {
        if(goodpos(xx + range, y) != 0) {
            tfoo->x = xx + range;
            tfoo->y = y;
            ++tfoo;

            if(tfoo == &foo[15]) {
                return foo[rn2(tfoo - foo)];
            }
        }
    }

    while(tfoo == foo) {
        for(x = xx - range; x <= (xx + range); ++x) {
            if(goodpos(x, yy - range) != 0) {
                tfoo->x = x;
                tfoo->y = yy - range;
                ++tfoo;

                if(tfoo == &foo[15]) {
                    return foo[rn2(tfoo - foo)];
                }
            }
        }

        for(x = xx - range; x <= (xx + range); ++x) {
            if(goodpos(x, yy + range) != 0) {
                tfoo->x = x;
                tfoo->y = yy + range;
                ++tfoo;

                if(tfoo == &foo[15]) {
                    return foo[rn2(tfoo - foo)];
                }
            }
        }

        for(y = (yy + 1) - range; y < (yy + range); ++y) {
            if(goodpos(xx - range, y) != 0) {
                tfoo->x = xx - range;
                tfoo->y = y;
                ++tfoo;
                
                if(tfoo == &foo[15]) {
                    return foo[rn2(tfoo - foo)];
                }
            }
        }

        for(x = (yy + 1) - range; y < (yy + range); ++y) {
            if(goodpos(xx + range, y) != 0) {
                tfoo->x = xx + range;
                tfoo->y = y;
                ++tfoo;

                if(tfoo = &foo[15]) {
                    return foo[rn2(tfoo - foo)];
                }
            }
        }
    }

    return foo[rn2(tfoo - foo)];
}

/* Used on in mnexto and rloc */
int goodpos(int x, int y)
{
#ifndef MKLEV
    return ((x >= 1)
            && (x <= (COLNO - 2))
            && (y >= 1)
            && (y <= (ROWNO - 2))
            && (m_at(x, y) == 0)
            && (levl[x][y].typ >= DOOR)
            && ((x != u.ux) || (y != u.uy))
            && (sobj_at(ENORMOUS_ROCK, x, y) == 0));
#else
    return ((x >= 1)
            && (x <= (COLNO - 2))
            && (y >= 1)
            && (y <= (ROWNO - 2))
            && (m_at(x, y) == 0)
            && (levl[x][y].typ >= DOOR));
#endif
}
