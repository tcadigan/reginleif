/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.h"

#include <math.h>
#include <stdio.h>

#include "def.trap.h"
#include "hack.do_name.h"
#include "hack.do_wear.h"
#include "hack.eat.h"
#include "hack.end.h"
#include "hack.engrave.h"
#include "hack.fight.h"
#include "hack.invent.h"
#include "hack.mkobj.h"
#include "hack.mon.h"
#include "hack.objnam.h"
#include "hack.pri.h"
#include "hack.search.h"
#include "hack.shk.h"
#include "hack.topl.h"
#include "hack.trap.h"
#include "hack.wield.h"
#include "hack.worm.h"
#include "rnd.h"

extern char *nomovemsg;

/*
 * Called on movement:
 * 1. When throwing ball + chain far away
 * 2. When teleporting
 * 3. when walking out of a lit room
 */
void unsee()
{
    int x;
    int y;
    struct rm *lev;

    /*
     * if(u.udispl != 0) {
     *     u.udisp = 0;
     *     newsym(u.udisx, u.udisy);
     * }
     */

#ifndef QUEST
    if(seehx !=0) {
        seehx = 0;
    }
    else {
        for(x = u.ux - 1; x < (u.ux + 2); ++x) {
            for(y = u.uy - 1; y < (u.uy + 2); ++y) {
                lev = &levl[x][y];
                
                if((lev->lit == 0) && (lev->scrsym == '.')) {
                    lev->scrsym = ' ';
                    lev->new = 1;
                    on_scr(x, y);
                }
            }
        }
    }
#else
    for(x = u.ux - 1; x < (u.ux + 2); ++ x) {
        for(y = u.uy - 1; y < (u.uy + 2); ++y) {
            lev = &levl[x][y];

            if((lev->lit != 0) && (lev->scrsym == '.')) {
                lev->scrsym = ' ';
                lev->new = 1;
                on_scr(x, y);
            }
        }
    }
#endif

}

/* 
 * Called:
 * in hack.eat.c: seeoff(0) - Blind after eating rotten food
 * in hack.mon.c: seeoff(0) - Blinded by a yellow light
 * in hack.mon.c: seeoff(1) - Swallowed
 * in hack.do.c: seeoff(0) - Blind after drinking potion
 * in hack.do.c: seeoff(1) - Go up or down the stairs
 * in hack.trap.c: seeoff(1) - Fall through trapdoor
 *
 * 1 to redo @, 0 to leave them
 * 1 means misc movement, 0 means blindness
 */
void seeoff(int mode)
{
    int x;
    int y;
    struct rm *lev;

    if((u.udispl != 0) && (mode != 0)) {
        u.udispl = 0;
        levl[(int)u.udisx][(int)u.udisy].scrsym = news0(u.udisx, u.udisy);
    }

#ifdef QUEST
    if(seehx ! 0) {
        seehx = 0;
    }
    else {
        if(mode == 0) {
            for(x = u.ux - 1; x < (u.ux + 2); ++x) {
                for(y = u.uy - 1; y < (u.uy + 2); ++y) {
                    lev = &levl[x][y];

                    if((lev->lit == 0) && (lev->scrsym == '.')) {
                        lev->seen = 0;
                    }
                }
            }
        }
    }
#else
    if(mode == 0) {
        for(x = u.ux - 1; x < (u.ux + 2); ++x) {
            for(y = u.uy - 1; y < (u.uy + 2); ++y) {
                lev = &levl[x][y];

                if((lev->lit == 0) && (lev->scrsym == '.')) {
                    lev->seen = 0;
                }
            }
        }
    }
#endif
}

/* 'rogue'-like direction commands */
char sdir[] = "hykulnjb";
schar xdir[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
schar ydir[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };

int movecm(unsigned char const *cmd)
{
    char *dp;

    dp = index(sdir, *cmd);

    if(dp == NULL) {
        return 0;
    }

    u.dx = xdir[dp - sdir];
    u.dy = ydir[dp - sdir];

    return 1;
}

#ifdef QUEST
int finddir()
{
    int i;
    int ui = u.di;
    
    for(i = 0; i <= 8; ++i) {
        if((flags.run & 1) != 0) {
            ++ui;
        }
        else {
            ui += 7;
        }

        ui %= 8;

        if(i == 8) {
            pline("Not near a wall.");
            multi = 0;
            flags.move = multi;

            return 0;
        }

        if(isroom(u.ux + xdir[ui], u.uy + ydir[ui]) != 0) {
            break;
        }
    }

    for(i = 0; i <= 8; ++i) {
        if((flags.run & 1) != 0) {
            ui += 7;
        }
        else {
            ++ui;
        }

        ui %= 8;

        if(i == 8) {
            pline("Not near a room.");
            multi = 0;
            falgs.move = multi;

            return 0;
        }

        if(isroom(u.ux + xdir[ui], u.uy + ydir[ui]) != 0) {
            break;
        }
    }

    u.di = ui;
    u.dx = xdir[ui];
    u.dy = ydir[ui];
}

int isroom(int x, int y)
{
    return ((isok(x, y) != 0)
            && ((levl[x][y].typ == ROOM)
                || ((levl[x][y].typ >= LDOOR) && (flags.run >= 6))));
}
#endif

int isok(int x, int y)
{
    return ((x >= 0)
            && (x <= (COLNO - 1))
            && (y >= 0)
            && (y <= (ROWNO - 1)));
}

void domove()
{
    xchar oldx;
    xchar oldy;
    struct monst *mtmp;
    struct rm *tmpr;
    struct rm *ust;
    struct gen *trap = NULL;
    struct obj *otmp;

    wipe_engr_at(u.ux, u.uy, rnd(5));

    if(inv_weight() > 0) {
        pline("You collase under your load.");
        nomul(0);

        return;
    }

    if(Confusion != 0) {
        u.dx = rn1(3, -1);
        u.dy = rn1(3, -1);
        tmpr = &levl[u.ux + u.dx][u.dy + u.dy];

        while(((u.dx == 0) && (u.dy == 0))
              || (isok(u.ux + u.dx, u.uy + u.dy) == 0)
              || (tmpr->typ < DOOR)) {
            u.dx = rn1(3, -1);
            u.dy = rn1(3, -1);
            tmpr = &levl[u.ux + u.dx][u.uy + u.dy];
        }
    }
    else {
        tmpr = &levl[u.ux + u.dx][u.uy + u.dy];
    }

    if(isok(u.ux + u.dx, u.uy + u.dy) == 0) {
        nomul(0);

        return;
    }

    ust = &levl[(int)u.ux][(int)u.uy];
    oldx = u.ux;
    oldy = u.uy;

    if(u.uswallow == 0) {
        trap = g_at(u.ux + u.dx, u.uy + u.dy, ftrap);
        
        if(trap != 0) {
            if((trap->gflag & SEEN) != 0) {
                nomul(0);
            }
        }
    }

    if((u.ustuck != 0)
       && (u.uswallow == 0)
       && (((u.ux + u.dx) != u.ustuck->mx) || ((u.uy + u.dy) != u.ustuck->my))) {
        if(dist(u.ustuck->mx, u.ustuck->my) > 2) {
            /* Perhaps it fled (or was teleported or ... ) */
            u.ustuck = 0;
        }
        else {
            if(Blind != 0) {
                pline("You cannot escape from it!");
            }
            else {
                pline("You cannot escape from %s!", monnam(u.ustuck));
            }

            nomul(0);

            return;
        }
    }

    mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
    if((mtmp != NULL) || (u.uswallow != 0)) {
        /* Attack monster */
        schar tmp;
        boolean malive = TRUE;
        struct permonst *mdat;

        nomul(0);
        gethungry();

        if(multi < 0) {
            /* We just fainted */
            return;
        }

        if(u.uswallow != 0) {
            mtmp = u.ustuck;
        }

        mdat = mtmp->data;

        int flag = 0;
        if((mdat->mlet == 'L') 
           && (mtmp->mfroz == 0)
           && (mtmp->msleep == 0)
           && (mtmp->mconf == 0)
           && (mtmp->mcansee != 0)
           && (rn2(7) == 0)
           && ((/* He died */ m_move(mtmp, 0) == 2)
               || (/* He moved */ mtmp->mx != (u.ux + u.dx))
               || (mtmp->my != (u.uy + u.dy)))) {
            flag = 1;
        }

        if(flag == 0) {
            if(mtmp->mimic != 0) {
                if((u.ustuck == 0) && (mtmp->mflee == 0)) {
                    u.ustuck = mtmp;
                }

                switch(levl[u.ux + u.dx][u.uy + u.dy].scrsym) {
                case '+':
                    pline("The door was actually a Mimic.");

                    break;
                case '$':
                    pline("The chest was a Mimic!");
                    
                    break;
                default:
                    pline("Wait! That's a Mimic!");
                }

                /* Clears mtmp->mimic */
                wakeup(mtmp);

                return;
            }

            /* Clears mtmp->mimic */
            wakeup(mtmp);

            if((mtmp->mhide != 0) && (mtmp->mundetected != 0)) {
                struct obj *obj;
                
                mtmp->mundetected = 0;
                obj = o_at(mtmp->mx, mtmp->my);

                if((obj != NULL) && (Blind == 0)) {
                    pline("Wait! There's a %s hiding under %s!", 
                          mdat->mname, 
                          doname(obj));
                }

                return;
            }

            tmp = ((u.uluck + u.ulevel) + mdat->ac) + abon();
        
            if(uwep != 0) {
                if(uwep->olet == WEAPON_SYM) {
                    tmp += uwep->spe;
                }

                if(uwep->otyp == TWO_HANDED_SWORD) {
                    tmp -= 1;
                }
                else if(uwep->otyp == DAGGER) {
                    tmp += 2;
                }
                else if(uwep->otyp == CRYSKNIFE) {
                    tmp += 3;
                }
                else if((uwep->otyp == SPEAR) 
                        && (index("XDne", mdat->mlet) != NULL)) {
                    tmp += 2;
                }
            }

            if(mtmp->msleep != 0) {
                mtmp->msleep = 0;
                tmp += 2;
            }

            if(mtmp->mfroz != 0) {
                tmp += 4;
                
                if(rn2(10) == 0) {
                    mtmp->mfroz = 0;
                }
            }

            if(mtmp->mflee != 0) {
                tmp += 2;
            }

            if(u.utrap != 0) {
                tmp -= 3;
            }

            if((tmp <= rnd(20)) && (u.uswallow != 0)) {
                if(Blind != 0) {
                    pline("You miss it.");
                }
                else {
                    pline("You miss %s.", monnam(mtmp));
                }
            }
            else {
                /* We hit the monster; be careful, it might die! */
                malive = hmon(mtmp, uwep, 0);
                
                if(malive == TRUE) {
                    /* Monster still alive */
                    if((rn2(25) == 0) && (mtmp->mhp < (mtmp->orig_hp / 2))) {
                        mtmp->mflee = 1;
                    
                        if((u.ustuck == mtmp) && (u.uswallow == 0)) {
                            u.ustuck = 0;
                        }
                    }

#ifndef NOWORM
                    if(mtmp->wormno != 0) {
                        if(uwep != 0) {
                            cutworm(mtmp, u.ux + u.dx, u.uy + u.dy, uwep->otyp);
                        }
                        else {
                            cutworm(mtmp, u.ux + u.dy, u.uy + u.dy, 0);
                        }
                    }
#endif
                }

                if(mdat->mlet == 'a') {
                    if(rn2(2) != 0) {
                        pline("You are splashed by the blob's acid!");
                        losehp_m(rnd(6), mtmp);
                    }

                    if(rn2(6) == 0) {
                        corrode_weapon();
                    }
                    else if(rn2(60) == 0) {
                        corrode_armor();
                    }
                }
            }

            if((malive != 0)
               && (Blind == 0)
               && (mdat->mlet == 'E')
               && (rn2(3) != 0)) {
                if(mtmp->mcansee != 0) {
                    pline("You are frozen by the floating eye's gaze!");

                    if((u.ulevel > 6) || (rn2(4) != 0)) {
                        nomul(rn1(10, -21));
                    }
                    else {
                        nomul(-200);
                    }
                }
                else {
                    pline("The blinded floating eye cannot defend itself.");

                    if(rn2(500) == 0) {
                        --u.uluck;
                    }
                }
            }

            return;
        }
    }
    
    /* Not attacking an animal, so we try to move */
    if(u.utrap != 0) {
        if(u.utraptype == TT_PIT) {
            pline("You are still in a pit.");

            --u.utrap;
        }
        else {
            pline("You are caught in a beartrap.");

            if(((u.dx != 0) && (u.dy != 0)) || (rn2(5) == 0)) {
                --u.utrap;
            }
        }

        return;
    }

    if((tmpr->typ < DOOR)
       || ((u.dx != 0)
           && (u.dy != 0)
           && ((tmpr->typ == DOOR) || (ust->typ == DOOR)))) {
        flags.move = 0;
        nomul(0);

        return;
    }

    otmp = sobj_at(ENORMOUS_ROCK, u.ux + u.dx, u.uy + u.dy);

    while(otmp != NULL) {
        xchar rx = u.ux + (2 * u.dx);
        xchar ry = u.uy + (2 * u.dy);
        struct gen *gtmp;
        nomul(0);

        if((isok(rx, ry) != 0)
           && ((levl[(int)rx][(int)ry].typ > DOOR)
               || ((levl[(int)rx][(int)ry].typ == DOOR)
                   && ((u.dx == 0) && (u.dy == 0))))) {
            if(m_at(rx, ry) != 0) {
                pline("You hear a monster behind the rock.");
                pline("Perhaps that's why you cannot move it.");

                return;
            }

            gtmp = g_at(rx, ry, ftrap);
        
            if(gtmp != NULL) {
                switch(gtmp->gflag & ~SEEN) {
                case PIT:
                    pline("You push the rock into a pit!");
                    deltrap(gtmp);
                    delobj(otmp);
                    pline("It completely fills the pit!");
                    
                    continue;
                case TELEP_TRAP:
                    pline("You push the rock and suddenly it disappears!");
                    delobj(otmp);
                    
                    continue;
                }
            }

            otmp->ox = rx;
            otmp->oy = ry;
            /* pobj(otmp); */
            
            if(cansee(rx, ry) != 0) {
                atl(rx, ry, otmp->olet);
            }
            
            if(Invis != 0) {
                newsym(u.ux + u.dy, u.uy + u.dy);
            }
            
            /* 
             * Note: This variable contains garbage initially 
             * and after a restore 
             */
            static int lastmovetime;
            
            if((moves > (lastmovetime + 2)) || (moves < lastmovetime)) {
                pline("With great effort you move the enormous rock.");
            }
            
            lastmovetime = moves;
        }
        else {
            pline("You try to move the enormous rock, but in vain.");

            return;
        }
    }

    if((u.dx != 0)
       && (u.dy != 0)
       && (levl[(int)u.ux][(int)(u.uy + u.dy)].typ < DOOR)
       && (levl[(int)(u.ux + u.dx)][(int)u.dy].typ < DOOR)
       && (invent != NULL)
       && ((inv_weight() + 40) > 0)) {
        pline("You are carrying too much to get through.");
        nomul(0);

        return;
    }

    if((Punished != 0)
       && (DIST(u.ux + u.dx, u.uy + u.dy, uchain->ox, uchain->oy) > 2)) {
        if(carried(uball) == 0) {
            if(DIST(u.ux + u.dx, u.uy + u.dy, uball->ox, uball->oy) >= 3) {
                if((inv_weight() + ((int)uball->owt / 2)) > 0) {
                    if(invent != NULL) {
                        pline("You cannot %sdrag the heavy iron ball.",
                              "carry all that and also ");
                    }
                    else {
                        pline("You cannot%sdrap the heavy iron ball.", "");
                    }
                    
                    nomul(0);
                    
                    return;
                }
                
                movobj(uball, uchain->ox, uchain->oy);
                
                /* BAH %% */
                unpobj(uball);
                
                uchain->ox = u.ux;
                uchain->oy = u.uy;
                nomul(-2);
                nomovemsg = "";
            }
            else {
                /* Leave ball, move chain under/over ball */
                movobj(uchain, uball->ox, uball->oy);
            }
        }
        else {
            movobj(uchain, u.ux, u.uy);
        }
    }

    u.ux += u.dx;
    u.uy += u.dy;
    
    if(flags.run != 0) {
        if((tmpr->typ == DOOR)
           || ((xupstair == u.ux) && (yupstair == u.uy))
           || ((xdnstair == u.ux) && (ydnstair == u.uy))) {
            nomul(0);
        }
    }

    /*
     * if(u.udispl != 0) {
     *     u.udispl = 0;
     *     newsym(oldx, oldy);
     * }
     */

    if(Blind == 0) {
#ifdef QUEST
        setsee();
#else
        if(ust->lit != 0) {
            if(tmpr->lit != 0) {
                if(tmpr->typ == DOOR) {
                    prl1(u.ux + u.dx, u.uy + u.dy);
                }
                else if(ust->typ == DOOR) {
                    nose1(oldx - u.dx, oldy - u.dy);
                }
            }
            else {
                unsee();
                prl1(u.ux + u.dx, u.uy + u.dy);
            }
        }
        else {
            if(tmpr->lit != 0) {
                setsee();
            }
            else {
                prl1(u.ux + u.dx, u.uy + u.dy);

                if(tmpr->typ == DOOR) {
                    if(u.dy != 0) {
                        prl(u.ux - 1, u.uy);
                        prl(u.ux + 1, u.uy);
                    }
                    else {
                        prl(u.ux, u.uy - 1);
                        prl(u.ux, u.uy + 1);
                    }
                }
            }

            nose1(oldx - u.dx, oldy - u.dy);
        }
#endif 
    }
    else {
        pru();
    }

    if(flags.nopick == 0) {
        pickup();
    }
    
    if(trap != 0) {
        /* Fall into pit, arrow trap, etc. */
        dotrap(trap);
    }

    inshop();

    if(Blind == 0) {
        read_engr_at(u.ux, u.uy);
    }
}

void movobj(struct obj *obj, int ox, int oy)
{
    /* Some dirty programming to get display right */
    freeobj(obj);
    unpobj(obj);
    obj->nobj = fobj;
    fobj = obj;
    obj->ox = ox;
    obj->oy = oy;
}

void pickup()
{
    struct gen *gold;
    struct obj *obj;
    struct obj *obj2;
    int wt;

    if(Levitation != 0) {
        return;
    }

    gold = g_at(u.ux, u.uy, fgold);

    while(gold != NULL) {
        pline("%u gold piece%s.", gold->gflag, plur(gold->gflag));
        u.ugold += gold->gflag;
        flags.botl = 1;
        freegold(gold);

        if(flags.run != 0) {
            nomul(0);
        }

        if(Invis != 0) {
            newsym(u.ux, u.uy);
        }

        gold = g_at(u.ux, u.uy, fgold);
    }

    for(obj = fobj; obj != NULL; obj = obj2) {
        /* Perhaps obj will be picked up */
        obj2 = obj->nobj;

        if((obj->ox == u.ux) && (obj->oy == u.uy)) {
            if(flags.run != 0) {
                nomul(0);
            }

#define DEAD_CORPSE CORPSE + (((('c' - 'a') + 'Z') - '@') + 1)

            if((obj->otyp == DEAD_COCKATRICE) && (uarmg == 0)) {
                pline("Touching the dead cockatrice is a fatal mistake.");
                pline("You turn to stone.");
                killer = "cockatrice cadaver";
                done("died");
            }

            if(obj->otyp == SCR_SCARE_MONSTER) {
                if(obj->spe == 0) {
                    obj->spe = 1;
                }
                else {
                    /*
                     * Note: Perhaps the first pickup failed: you cannot
                     * carry anymore, and so we never dropped it -
                     * let's assume that treading on it twice also
                     * destroys the scroll
                     */
                    pline("The scroll turns to dust as you pick it up.");
                    delobj(obj);

                    continue;
                }
            }

            /* Do not pick up uchain */
            if((Punished != 0) && (obj == uchain)) {
                continue;
            }

            wt = inv_weight() + obj->owt;

            if(wt > 0) {
                if(obj->quan > 1) {
                    /* See how many we can lift */
                    extern struct obj *splitobj();
                    
                    int savequan = obj->quan;
                    int iw = inv_weight();
                    int qq;

                    for(qq = 1; qq < savequan; ++qq) {
                        obj->quan = qq;
                        
                        if((iw + weight(obj)) > 0) {
                            break;
                        }
                    }

                    obj->quan = savequan;
                    --qq;

                    /* We can carry qq of them */
                    if(qq == 0) {
                        if(obj->quan == 1) {
                            if(invent == NULL) {
                                pline("There %s %s here, but %s.",
                                      "is", 
                                      doname(obj),
                                      "it is too heavy for you to lift");
                            }
                            else {
                                pline("There %s %s here, but %s.",
                                      "is",
                                      doname(obj),
                                      "you cannot carry anymore");
                            }
                        }
                        else {
                            if(invent == NULL) {
                                pline("There %s %s here, but %s.",
                                      "are",
                                      doname(obj),
                                      "it is too heavy for you to lift");
                            }
                            else {
                                pline("There %s %s here, but %s.",
                                      "are",
                                      doname(obj),
                                      "you cannot carry anymore.");
                            }
                        }

                        break;
                    }

                    if(qq == 1) {
                        pline("You can only carry %s of the %s lying here.",
                              "one",
                              doname(obj));
                    }
                    else {
                        pline("You can only carry %s of the %s lying here.",
                              "some",
                              doname(obj));
                    }

                    splitobj(obj, qq);

                    /* 
                     * Note: obj2 is set already, so we'll never
                     * encounter the other half; if it should be otherwise then write
                     * obj2 = splitobj(obj, qq);
                     */
                    if(inv_cnt() >= 52) {
                        pline("Your knapsack cannot accomodate anymore items.");
                        
                        break;
                    }

                    if(wt > -5) {
                        pline("You have little trouble lifting");
                    }

                    freeobj(obj);

                    if(Invis != 0) {
                        newsym(u.ux, u.uy);
                    }

                    /* Sets obj->unpaid if necessary */
                    addtobill(obj);
                    int pickquan = obj->quan;
                    int mergquan;
                    
                    if(Blind == 0) {
                        /* 
                         * This is done by prinv() but addinv() needs it already
                         * for merging, might merge with other objects
                         */
                        obj->dknown = 1;
                    }

                    obj = addinv(obj);
                    mergquan = obj->quan;

                    /* To fool prinv() */
                    obj->quan = pickquan;

                    prinv(obj);
                    obj->quan = mergquan;

                    continue;
                }

                if(obj->quan == 1) {
                    if(invent == NULL) {
                        pline("There %s %s here, but %s.",
                              "is",
                              doname(obj),
                              "it is too heavy for you to lift");
                    }
                    else {
                        pline("There %s %s here, but %s.",
                              "is",
                              doname(obj),
                              "you cannot carry anymore");
                    }
                }
                else {
                    if(invent == NULL) {
                        pline("There %s %s here, but %s.",
                              "are",
                              doname(obj),
                              "it is too heavy for you to lift");
                    }
                    else {
                        pline("There %s %s here, but %s.",
                              "are",
                              doname(obj),
                              "you cannot carry anymore");
                    }
                }

                break;
            }

            if(inv_cnt() >= 52) {
                pline("Your knapsack cannot accomodate anymore items.");

                break;
            }

            if(wt > -5) {
                pline("You have little trouble lifting");
            }

            freeobj(obj);
            
            if(Invis != 0) {
                newsym(u.ux, u.uy);
            }

            /* Sets obj->unpaid if necessary */
            addtobill(obj);

            int pickquan = obj->quan;
            int mergquan;
            
            if(Blind == 0) {
                /* 
                 * This is done by prinv() but addinv() needs it already
                 * for merging might merge it with other objects
                 */
                obj->dknown = 1;
            }

            obj = addinv(obj);
            mergquan = obj->quan;

            /* To fool prinv() */
            obj->quan = pickquan;

            prinv(obj);
            obj->quan = mergquan;
        }
    }
}

/*
 * Stop running if we see something interesting
 * turn around a corner if that is the only way we can proceed
 * do not durn let or right twice
 */
void lookaround()
{
    int x;
    int y;
    int i;
    int x0 = 0;
    int y0 = 0;
    int m0 = 0;
    int i0 = 9;
    int corrct = 0;
    int noturn = 0;
    struct monst *mtmp;

    if((Blind != 0) || (flags.run == 0)) {
        return;
    }

    if((flags.run == 1) && (levl[(int)u.ux][(int)u.uy].typ > ROOM)) {
        return;
    }

#ifdef QUEST
    if((u.ux0 == (u.ux + u.dx)) && (u.uy0 == (u.uy + u.dy))) {
        nomul(0);
        
        return;
    }
#endif

    for(x = u.ux - 1; x <= (u.ux + 1); ++x) {
        for(y = u.uy - 1; y <= (u.uy + 1); ++y) {
            if((x == u.ux) && (y = u.uy)) {
                continue;
            }

            if(levl[x][y].typ == 0) {
                continue;
            }

            mtmp = m_at(x, y);

            if((mtmp != NULL)
               && (mtmp->mimic == 0)
               && ((mtmp->minvis == 0) || (See_invisible != 0))) {
                if((mtmp->mtame == 0) || ((x == (u.ux + u.dx)) && (y == (u.uy + u.dy)))) {
                    nomul(0);

                    return;
                }
            }
            else {
                /* Invisible M cannot influence us */
                mtmp = 0;
            }

            if((x == (u.ux - u.dx)) && (y == (u.uy - u.dy))) {
                continue;
            }

            switch(levl[x][y].scrsym) {
            case '|':
            case '-':
            case '.':
            case ' ':
                
                break;
            case '+':
                if((x != u.ux) && (y != u.uy)) {
                    break;
                }

                if(flags.run != 1) {
                    nomul(0);
                    
                    return;
                }

                /* Fall into next case */
            case CORR_SYM:
                if((flags.run == 1) || (flags.run == 3)) {
                    i = DIST(x, y, u.ux + u.dx, u.uy + u.dy);

                    if(i > 2) {
                        break;
                    }

                    if((corrct == 1) && (DIST(x, y, x0, y0) != 1)) {
                        noturn = 1;
                    }

                    if(i < i0) {
                        i0 = i;
                        x0 = x;
                        y0 = y;
                        
                        if(mtmp != 0) {
                            m0 = 1;
                        }
                        else {
                            m0 = 0;
                        }
                    }
                }
                
                ++corrct;

                break;
            case '^':
                if(flags.run == 1) {
                    /* If you must */
                    if((flags.run == 1) || (flags.run == 3)) {
                        i = DIST(x, y, u.ux + u.dx, u.uy + u.dy);

                        if(i > 2) {
                            break;
                        }

                        if((corrct == 1) && (DIST(x, y, x0, y0) != 1)) {
                            noturn = 1;
                        }

                        if(i < i0) {
                            i0 = i;
                            x0 = x;
                            y0 = y;
                            
                            if(mtmp != NULL) {
                                m0 = 1;
                            }
                            else {
                                m0 = 0;
                            }
                        }
                    }

                    ++corrct;

                    break;
                }

                if((x == (u.ux + u.dx)) && (y == (u.uy + u.dy))) {
                    nomul(0);
                    
                    return;
                }

                break;
            default:
                /* e.g. objects or trap or stairs */
                if(flags.run == 1) {
                    if((flags.run == 1) || (flags.run == 3)) {
                        i = DIST(x, y, u.ux + u.dy, u.uy + u.dy);

                        if(i > 2) {
                            break;
                        }

                        if((corrct == 1) && (DIST(x, y, x0, y0) != 1)) {
                            noturn = 0;
                        }

                        if(i < i0) {
                            i0 = i;
                            x0 = x;
                            y0 = y;

                            if(mtmp != NULL) {
                                m0 = 1;
                            }
                            else {
                                m0 = 0;
                            }
                        }
                    }

                    ++corrct;

                    break;
                }

                if(mtmp != NULL) {
                    /* d */
                    break;
                }

                nomul(0);
                
                return;
            }
        }
    }

#ifdef QUEST
    if((corrct > 0) && ((flags.run == 4) || (flags.run == 5))) {
        nomul(0);

        return;
    }
#endif

    if((corrct > 1) && (flags.run == 2)) {
        nomul(0);

        return;
    }

    if(((flags.run == 1) || (flags.run == 3))
       && (noturn == 0)
       && (m0 == 0)
       && (i0 != 0)
       && ((corrct == 1) || ((corrct == 2) && (i0 == 1)))) {
        /* Make sure that we do not turn too far */
        if(i0 == 2) {
            if((u.dx == (y0 - u.uy)) && (u.dy == (u.ux - x0))) {
                /* Straight turn right */
                i = 2;
            }
            else {
                /* Straight turn left */
                i = -2;
            }
        }
        else if((u.dx != 0) && (u.dy != 0)) {
            if(((u.dx == u.dy) && (y0 != u.uy))
               || ((u.dx != u.dy) && (y0 != u.uy))) {
                /* Half turn left */
                i = -1;
            }
            else {
                /* Half turn right */
                i = 1;
            }
        }
        else {
            if((((x0 - u.ux) == (y0 - u.uy)) && (u.dy == 0))
               || (((x0 - u.ux) != (y0 - u.uy)) && (u.dy != 0))) {
                /* Half turn right */
                i = 1;
            }
            else {
                /* Half turn left */
                i = -1;
            }
        }

        i += u.last_str_turn;

        if((i <= 2) && (i >= 2)) {
            u.last_str_turn = i;
            u.dx = x0 - u.ux;
            u.dy = y0 - u.uy;
        }
    }
}

#ifdef QUEST
int cansee(xchar x, xchar y)
{
    int dx;
    int dy;
    int adx;
    int ady;
    int sdx;
    int sdy;
    int dmax;
    int d;

    if(Blind != 0) {
        return 0;
    }

    if(isok(x, y) == 0) {
        return 0;
    }

    d = dist(x, y);

    if(d < 3) {
        return 1;
    }

    if(d > (u.uhorizon * u.uhorizon)) {
        return 0;
    }

    if(levl[x][y].lit == 0) {
        return 0;
    }

    dx = x - u.ux;
    adx = abs(dx);
    sdx = sgn(dx);
    dy = y - u.uy;
    ady = abs(dy);
    sdy = sgn(dy);

    if((dx == 0) || (dy == 0) || (adx == ady)) {
        if(dx == 0) {
            dmax = ady;
        }
        else {
            dmax = adx;
        }

        for(d = 1; d <= dmax; ++d) {
            if(rroom(sdx * d, sdy * d) == 0) {
                rturen 0;
            }
        }
         
        return 1;
    }
    else if(ady > adx) {
        for(d = 1; d <= ady; ++d) {
            if((rroom(sdx * ((d * adx) / ady), sdy * d) == 0)
               || (rroom(sdx * (((d * adx) - 1) / (ady + 1)), sdy * d) == 0)) {
                return 0;
            }
        }

        return 1;
    }
    else {
        for(d = 1; d < adx; ++d) {
            if((rroom(sdx * d, sdy * ((d * ady) / adx)) == 0)
               || (rroom(sdx * d, sdy * (((d * ady) - 1) / (adx + 1))) == 0)) {
                return 0;
            }
        }

        return 1;
    }
}

int rroom(int x, int y)
{
    if(levl[u.ux + x][u.uy + y] >= ROOM) {
        return 1;
    }
    else {
        return 0;
    }
}

#else

int cansee(xchar x, xchar y)
{
    if((Blind != 0) || (u.uswallow != 0)) {
        return 0;
    }

    if(dist(x, y) < 3) {
        return 1;
    }

    if((levl[(int)x][(int)y].lit != 0)
       && (seelx <= x)
       && (x <= seehx)
       && (seely <= y)
       && (y <= seehy)) {
        return 1;
    }

    return 0;
}
#endif

int sgn(int a)
{
    if(a > 0) {
        return 1;
    }
    else if(a == 0) {
        return 0;
    }
    else {
        return -1;
    }
}

#ifdef QUEST
void setsee()
{
    int x;
    int y;

    if(Blind != 0) {
        pru();
        
        return;
    }

    for(y = u.uy - u.uhorizon; y < (u.uy + u.uhorizon); ++y) {
        for(x = u.ux - u.uhorizon; x <= (u.ux + u.uhorizon); ++x) {
            if(cansee(x, y) != 0) {
                prl(x, y);
            }
        }
    }
}

#else

void setsee()
{
    int x;
    int y;
    
    if(Blind != 0) {
        pru();
        
        return;
    }

    if(levl[(int)u.ux][(int)u.uy].lit == 0) {
        seelx = u.ux - 1;
        seehx = u.ux + 1;
        seely = u.uy - 1;
        seehy = u.uy + 1;
    }
    else {
        seelx = u.ux;
        
        while(levl[(int)(seelx - 1)][(int)u.uy].lit != 0) {
            --seelx;
        }

        seehx = u.ux;
        
        while(levl[(int)(seehx + 1)][(int)u.uy].lit != 0) {
            ++seehx;
        }

        seely = u.uy;

        while(levl[(int)u.ux][(int)(seely - 1)].lit != 0) {
            --seely;
        }

        seehy = u.uy;

        while(levl[(int)u.ux][(int)(seely - 1)].lit != 0) {
            ++seehy;
        }
    }

    for(y = seely; y <= seehy; ++y) {
        for(x = seelx; x <= seehx; ++x) {
            prl(x, y);
        }
    }

    if(levl[(int)u.ux][(int)u.uy].lit != 0) {
        /* Seems necessary elsewhere */
        seehx = 0;
    }
    else {
        if(seely == u.uy) {
            for(x = u.ux - 1; x <= (u.ux + 1); ++x) {
                prl(x, seely - 1);
            }
        }

        if(seehy == u.uy) {
            for(x = u.ux - 1; x <= (u.ux + 1); ++x) {
                prl(x, seehy + 1);
            }
        }

        if(seelx == u.ux) {
            for(y = u.uy - 1; y <= (u.uy + 1); ++y) {
                prl(seelx - 1, y);
            }
        }

        if(seehx == u.ux) {
            for(y = u.uy - 1; y <= (u.uy + 1); ++y) {
                prl(seehx + 1, y);
            }
        }
    }
}
#endif

void nomul(int nval)
{
    if(multi < 0) {
        return;
    }

    multi = nval;
    flags.run = 0;
    flags.mv = flags.run;
}

int abon()
{
    if(u.ustr == 3) {
        return -3;
    }
    else if(u.ustr < 6) {
        return -2;
    }
    else if(u.ustr < 8) {
        return -1;
    }
    else if(u.ustr < 17) {
        return 0;
    }
    else if(u.ustr < 69) {
        /* Up to 18/50 */
        return 1;
    }
    else if(u.ustr < 118) {
        return 2;
    }
    else {
        return 3;
    }
}

int dbon()
{
    if(u.ustr < 6) {
        return -1;
    }
    else if(u.ustr < 16) {
        return 0;
    }
    else if(u.ustr < 18) {
        return 1;
    }
    else if(u.ustr == 18) {
        /* Up to 18 */
        return 2;
    }
    else if(u.ustr < 94) {
        /* Up to 18/75 */
        return 3;
    }
    else if(u.ustr < 109) {
        /* Up to 18/90 */
        return 4;
    }
    else if(u.ustr < 118) {
        /* Up to 18/99 */
        return 5;
    }
    else {
        return 6;
    }
}

void losestr(int num)
{
    u.ustr -= num;

    while(u.ustr < 3) {
        ++u.ustr;
        u.uhp -= 6;
        u.uhpmax -= 6;
    }

    flags.botl = 1;
}

void losehp(int n, char *knam)
{
    u.uhp -= n;
    
    if(u.uhp > u.uhpmax) {
        /* Perhaps n was negative */
        u.uhpmax = u.uhp;
    }

    flags.botl = 1;
    
    if(u.uhp < 1) {
        /* The thing that killed you */
        killer = knam;
    }
}

void losehp_m(int n, struct monst *mtmp)
{
    u.uhp -= n;
    flags.botl = 1;

    if(u.uhp < 1) {
        done_in_by(mtmp);
    }
}

/* Hit by 'V' or 'W' */
void losexp()
{
    int num;

    if(u.ulevel > 1) {
        pline("Goodbye level %d.", u.ulevel);
        --u.ulevel;
    }
    else {
        u.uhp = -1;
        num = rnd(10);
        u.uhp -= num;
        u.uhpmax -= num;
        u.uexp = 10 * pow(2, u.ulevel - 1);
        flags.botl = 1;
    }
}

int inv_weight()
{
    struct obj *otmp = invent;
    int wt = 0;
    
    int carrcap = (5 * u.ustr) + u.ulevel;

    if(u.ustr > 18) {
        carrcap = (5 * 20) + u.ulevel;
    }

    if(carrcap > MAX_CARR_CAP) {
        carrcap = MAX_CARR_CAP;
    }

    if((Wounded_legs & LEFT_SIDE) != 0) {
        carrcap -= 10;
    }
   
    if((Wounded_legs & RIGHT_SIDE) != 0) {
        carrcap -= 10;
    }

    while(otmp != NULL) {
        wt += otmp->owt;
        otmp = otmp->nobj;
    }

    return (wt - carrcap);
}

int inv_cnt()
{
    struct obj *otmp = invent;
    int ct = 0;

    while(otmp != NULL) {
        ++ct;
        otmp = otmp->nobj;
    }

    return ct;
}
