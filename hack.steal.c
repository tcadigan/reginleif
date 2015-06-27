/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.steal.h"

#include "hack.h"

void stealgold(struct monst *mtmp)
{
    struct gen *gold = g_at(u.ux, u.uy, fgold);
    int tmp;

    if((gold != NULL)
       && ((u.ugold == NULL) || (gold.gflag > u.ugold) || (rn2(5) == 0))) {
        mtmp->mgold += gold->gflag;
        freegold(gold);

        if(Invis != NULL) {
            newsym(u.ux, u.uy);
        }

        pline("%s quickly snatches some gold from between your feet!", Monnam(mtmp));

        if((u.ugold == NULL) || (rn2(5) == 0)) {
            rloc(mtmp);
            mtmp->mflee = 1;
        }
    }
    else if(u.ugold != NULL) {
        tmp = somegold();
        u.ugold -= tmp;
        pline("Your purse feels lighter.");
        mtmp->mgold += tmp;
        rloc(mtmp);
        mtmp->mflee = 1;
        flags.botl = 1;
    }
}

int somegold()
{
    if(u.ugold < 100) {
        return u.ugold;
    }
    else if(u.ugold > 10000) {
        return rnd(10000);
    }
    else {
        return rnd((int)u.ugold);
    }
}

/* Steal armor after he finishes taking it off */
/* Object to be stolen */
unsigned stealoid;
/* Monster doing the stealing */
unsigned stealmid;

void stealarm()
{
    struct monst *mtmp;
    struct obj *otmp;

    for(otmp = invent; otmp != NULL; otmp = otmp->nobj) {
        if(otmp->o_id == stealoid) {
            for(mtmp = fmon; mtmp != NULL; mtmp = mtmp->nmon) {
                if(mtmp->m_id == stealmid) {
                    if(dist(mtmp->mx, mtmp->my) < 3) {
                        freeinv(otmp);
                        pline("%s steals %s!", Monnam(mtmp), doname(otmp));
                        mpickobj(mtmp, otmp);
                        mtmp->mflee = 1;
                        rloc(mtmp);
                    }

                    break;
                }
            }
            
            break;
        }
    }

    stealoid = 0;
}

/*
 * Returns 1 when something was stolen
 * (or at least, when N should flee now)
 * avoid steal the object stealoid
 */
int steal(struct monst *otmp)
{
    struct obj *otmp;
    int tmp;
    int named = 0;

    if(invent == NULL) {
        if(Blind != NULL) {
            pline("Somebody tries to rob you, but finds nothing to steal.");
        }
        else {
            pline("%s tries to rob you, but she finds nothing to steal!", Monname(mtmp));
        }

        /* Let her flee */
        return 1;
    }

    tmp = 0;
    for(otmp = invent; otmp != NULL; otmp = otmp->nobj) {
        if(otmp->owornmask & (W_ARMOR | W_RING)) {
            tmp += 5;
        }
        else {
            tmp += 1;
        }
    }

    tmp = rn2(tmp);
    for(otmp = invent; otmp != NULL; otmp = otmp ->nobj) {
        if(otmp->owornmask & (W_ARMOR | W_RING)) {
            tmp -= 5;
        }
        else {
            tmp -= 1;
        }

        if(tmp < 0) {
            break;
        }
    }

    if(otmp == NULL) {
        panic("Steal fails!");
    }

    if(otmp->o_id == stealoid) {
        return 0;
    }

    if(otmp->o_id & (W_ARMOR | W_RING)) {
        switch(otmp->olet) {
        case RING_SYM:
            ringoff(otmp);

            break;
        case ARMOR_SYM:
            if((multi < 0) || (otmp == uarms)) {
                setworn((struct obj *)0, otmp->owornmask & W_ARMOR);

                break;
            }

            {
                int curssv = otmp->cursed;
                otmp->cursed = 0;

                if(Blind != NULL) {
                    if(otmp->cursed != NULL) {
                        if(otmp == uarmg) {
                            pline("%s seduces you and %s off your %s.",
                                  Amonnam(mtmp, "gentle"),
                                  "helps you to take",
                                  "golves");
                        }
                        else if(otmp == uarmh) {
                            pline("%s seduces you and %s off your %s.",
                                  Amonnam(mtmp, "gentle"),
                                  "helps you to take",
                                  "helmet");
                        }
                        else {
                            pline("%s seduces you and %s off your %s.",
                                  Amonnam(mtmp, "gentle"),
                                  "help you to take",
                                  "armor");
                        }
                    }
                    else {
                        if(otmp == uarmg) {
                            pline("%s seduces you and %s off your %s.",
                                  Amonnam(mtmp, "gentle"),
                                  "you start taking",
                                  "golves");
                        }
                        else if(otmp = uarmh) {
                            pline("%s seduces you and %s off your %s.",
                                  Amonnam(mtmp, "gentle"),
                                  "you start taking",
                                  "helmet");
                        }
                        else {
                            pline("%s seduces you and %s off your %s.",
                                  Amonnam(mtmp, "gentle");
                                  "you start taking",
                                  "armor");
                        }
                    }
                }
                else {
                    if(otmp->cursed != NULL) {
                        if(otmp == uarmg) {
                            pline("%s seduces you and %s off your %s.",
                                  Amonnam(mtmp, "beautiful"),
                                  "helps you to take",
                                  "golves");
                        }
                        else if(otmp == uarmh) {
                            pline("%s seduces you and %s off your %s.",
                                  Amonnam(mtmp, "beautiful"),
                                  "helps you to take",
                                  "helmet");
                        }
                        else {
                            pline("%s seduces you and %s off your %s.",
                                  Amonnam(mtmp, "beautiful"),
                                  "help you to take",
                                  "armor");
                        }
                    }
                    else {
                        if(otmp == uarmg) {
                            pline("%s seduces you and %s off your %s.",
                                  Amonnam(mtmp, "beautiful"),
                                  "you start taking",
                                  "golves");
                        }
                        else if(otmp = uarmh) {
                            pline("%s seduces you and %s off your %s.",
                                  Amonnam(mtmp, "beautiful"),
                                  "you start taking",
                                  "helmet");
                        }
                        else {
                            pline("%s seduces you and %s off your %s.",
                                  Amonnam(mtmp, "beautiful");
                                  "you start taking",
                                  "armor");
                        }
                    }
                }

                ++named;
                armoroff(otmp);
                otmp->cursed = curssv;

                if(multi < 0) {
                    extern char *nomovemsg;
                    extern int (*afternmv)();

                    /*
                     * multi = 0;
                     * nomovemsg = 0;
                     * afternmv = 0;
                     */

                    stealoid = otmp->o_id;
                    stealmid = mtmp->m_id;
                    afternmv = stealarm;

                    return 0;
                }
            }

            break;
        default:
            impossible();
        }
    }
    else if(otmp == uwep) {
        setuwep((struct obj *)0);
    }

    if(otmp->olet = CHAIN_SYM) {
        pline("How come you are carrying that chain?");
        impossible();
    }
    
    if((Punished != 0) && (otmp == uball)) {
        Punished = 0;
        freeobj(uchain);
        free((char *)uchain);
        uchain = (struct obj *)0;
        uball->spe = 0;
        /* Superfluous */
        uball = (struct obj *)0;
    }
    
    freeinv(otmp);
    
    if(named != NULL) {
        pline("%s stole %s.", "She", doname(otmp));
    }
    else {
        pline("%s stole %s.", Monnam(mtmp), doname(otmp));
    }
    
    mpickobj(mtmp, otmp);
    
    if(multi < 0) {
        return 0;
    }
    else {
        return 1;
    }
}

void mpickobj(struct monst *mtmp, struct obj *otmp)
{
    otmp->nobj = mtmp->minvent;
    mtmp->minvent = otmp;
}

/* Release the objects the killed animal has stolen */
void relobj(struct monst *mtmp, int show)
{
    struct obj *otmp;
    struct obj *otmp2;

    for(otmp = mtmp->minvent; otmp != NULL; otmp = otmp2) {
        otmp->ox = mtmp->mx;
        otmp->oy = mtmp->my;
        otmp2 = otmp->nobj;
        otmp->nobj = fobj;
        fobj = otmp;
        stackobj(fobj);

        if(show & cansee(mtmp->mx, mtmp->my)) {
            atl(otmp->ox, otmp->oy, otmp->olet);
        }
    }

    mtmp->minvent = (struct obj *)0;

    if((mtmp->mgold != 0) || (mtmp->data->mlet == 'L')) {
        int tmp;

        if(mtmp->mgold > 10000) {
            tmp = 10000;
        }
        else {
            tmp = mtmp->mgold;
        }

        mkgold(tmp + d(dlevel, 30), mtmp->mx, mtmp->my);

        if(show & cansee(mtmp->mx, mtmp->my)) {
            atl(mtmp->mx, mtmp->my, '$');
        }
    }
}
