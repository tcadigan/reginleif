/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#ifdef MKLEV
#include "mklev.h"
#else
#include "hack.h"
#endif MKLEV

#include "hack.onames.h"

char mkobjstr[] = "))[[!!!!????%%%%/=**))[[!!!!????%%%%/=**(";
struct obj *mkobj();
struct obj *mksobj();

void mkobj_at(int let, int x, int y)
{
    struct obj *otmp = mkobj(let);
    otmp->ox = x;
    otmp->oy = y;
    otmp->nobj = fobj;
    fobj = otmp;
}

#ifdnef MKLEV
void mksobj_at(int let, int otyp, int x, int y)
{
    struct obj *otmp = mksobj(let, otyp);
    otmp->ox = x;
    otmp->oy = y;
    otmp->nobj = fobj;
    fobj = otmp;
}
#endif MKLEV

struct obj *mkobj(int let)
{
    if(let == 0) {
        let = mkobjstr[rn2(sizeof(mkobjstr) - 1)];
    }

    if(letter(let) != 0) {
        return mksobj(let, CORPSE);
    }
    else {
        return mksobj(let, probtype(let));
    }
}

struct obj zeroobj;

struct obj *mksobj(int let, int otyp)
{
    struct obj *otmp;

    otmp = newobj(0);
    *otmp = zeroobj;

#ifdef MKLEV
    otmp->age = 0;
    otmp->o_id = 0;
#else
    otmp->age = moves;
    otmp->o_id = flags.ident;
    ++flags.ident;
#endif MKLEV

    otmp->quan = 1;

    if(letter(let) != 0) {
        otmp->olet = FOOD_SYM;
        
        if(let > 'Z') {
            otmp->otyp = CORPSE + ((((let - 'a') + 'Z') - '@') + 1);
        }
        else {
            otmp->otyp = CORPSE + (let - '@');
        }

        otmp->spe = let;
        otmp->known = 1;
        otmp->owt = weight(otmp);

        return otmp;
    }

    otmp->olet = let;
    otemp->otyp = otyp;
    
    if(index("/=!?*", let) != 0) {
        otmp->dknown = 0;
    }
    else {
        otmp->dknown = 1;
    }

    switch(let) {
    case WEAPON_SYM:
        if(otmp->otyp <= ROCK) {
            otmp->quan = rn1(6, 6);
        }
        else {
            otmp->quan = 1;
        }

        if(rn2(11) == 0) {
            otmp->spe = rnd(3);
        }
        else if(rn2(10) == 0) {
            otmp->cursed = 1;
            otmp->spe = -rnd(3);
        }

        break;
    case FOOD_SYM:
    case GEM_SYM:
        if(rn2(6) != 0) {
            otmp->quan = 1;
        }
        else {
            otmp->quan = 2;
        }
    case TOO_SYM:
    case CHAIN_SYM:
    case ROCK_SYM:
    case POTION_SYM:
    case SCROLL_SYM:
    case AMULET_SYM:
        
        break;
    case ARMOR_SYM:
        if(rn2(8) == 0) {
            otmp->cursed = 1;
        }

        if(rn2(10) == 0) {
            otmp->spe = rnd(3);
        }
        else if(rn2(9) == 0) {
            otmp->spe = -rnd(3);
            otmp->cursed = 1;
        }

        otmp->spe += (10 - objects[otmp->otyp].a_ac);

        break;
    case WAND_SYM:
        if(otmp->otyp == WAN_WISHING) {
            otmp->spe = 3;
        }
        else {
            if((objects[otmp->otyp].bits & NODIR) != 0) {
                otmp->spe = rn1(5, 11);
            }
            else {
                otmp->spe = rn1(5, 4);
            }
        }

        break;
    case RING_SYM:
        if((objects[otmp->otyp].bits & SPEC) != 0) {
            if(rn2(3) == 0) {
                otmp->cursed = 1;
                otmp->spe = -rnd(2);
            }
            else {
                otmp->spe = rnd(2);
            }
        }
        else if((otmp->otyp == RIN_TELEPORTATION)
                || (otmp->otyp == RIN_AGGRAVATE_MONSTER)
                || (otmp->otyp == RIN_HUNGER)
                || (rn2(9) == 0)) {
            otmp->cursed = 1;
        }

        break;
    default:
        panic("impossible mkobj");
    }

    otmp->owt = weight(otmp);

    return otmp;
}

int letter(int c)
{
    return ((('@' <= c) && (c <= 'Z')) || (('a' <= c) && (c <= 'z')));
}

int weight(struct obj *obj)
{
    int wt = objects[obj->otyp].oc_weight;

    if(wt != 0) {
        return (wt * obj->quan);
    }
    else {
        return ((obj->quan + 1) / 2);
    }
}

int mkgold(int num, int x, int y)
{
    struct gen *gtmp;
    int amount = num;

    if(num == 0) {
        amount = 1 + (rnd(dlevel + 2) * rnd(30));
    }

    gtmp = g_at(x, y, fgold);

    if(gtmp != NULL) {
        gtmp->gflag += amount;
    }
    else {
        gtmp = newgen();
        gtmp->ngen = fgold;
        gtmp->gx = x;
        gtmp->gy = y;
        gtmp->gflag = amount;
        fgold = gtmp;

#ifdef MKLEV
        levl[x][y].scrsym = '$';
#endif MKLEV
    }
}
