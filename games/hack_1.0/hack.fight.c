/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.fight.h"

#include <string.h>

#include "hack.h"
#include "hack.do_name.h"
#include "hack.end.h"
#include "hack.invent.h"
#include "hack.mkobj.h"
#include "hack.mon.h"
#include "hack.objnam.h"
#include "hack.pri.h"
#include "hack.search.h"
#include "hack.shk.h"
#include "hack.topl.h"
#include "hack.worn.h"
#include "hack.zap.h"
#include "rnd.h"

extern struct permonst li_dog;
extern struct permonst dog;
extern struct permonst la_dog;

static boolean far_noise;
static long noisetime;

/* hitmm returns 0 (miss), 1 (hit), or 2 (kill) */
int hitmm(struct monst *magr, struct monst *mdef)
{
    struct permonst *pa = magr->data;
    struct permonst *pd = mdef->data;
    int hit;
    schar tmp;
    boolean vis;

    if(index("Eauy", pa->mlet) != 0) {
        return 0;
    }

    tmp = pd->ac + pa->mlevel;

    if((mdef->mconf != 0) || (mdef->mfroz != 0) || (mdef->msleep != 0)) {
        tmp += 4;

        if(mdef->msleep != 0) {
            mdef->msleep = 0;
        }
    }

    hit = 0;

    if(tmp > rnd(20)) {
        hit = 1;
    }

    if(hit != 0) {
        mdef->msleep = 0;
    }

    vis = 0;

    if((cansee(magr->mx, magr->my) != 0) && (cansee(mdef->mx, mdef->my) != 0)) {
        vis = 1;
    }

    if(vis != 0) {
        char buf[BUFSZ];
        
        if(mdef->mimic != 0) {
            seemimic(mdef);
        }

        if(magr->mimic != 0) {
            seemimic(magr);
        }

        if(hit != 0) {
            sprintf(buf, "%s %s", Monnam(magr), "hits");
        }
        else {
            sprintf(buf, "%s %s", Monnam(magr), "misses");
        }

        pline("%s %s.", buf, monnam(mdef));
    }
    else {
        boolean far = 0;

        if(dist(magr->mx, magr->my) > 15) {
            far = 1;
        }

        if((far != far_noise) || ((moves - noisetime) > 10)) {
            far_noise = far;
            noisetime = moves;

            if(far != 0) {
                pline("You hear some noises%s.", " in the distance");
            }
            else {
                pline("You hear some noises%s.", "");
            }
        }
    }

    if(hit != 0) {
        if((magr->data->mlet == 'c') && (magr->cham == 0)) {
            magr->orig_hp += 3;

            if(vis != 0) {
                pline("%s is turned to stone!", Monnam(mdef));
            }
            else if(mdef->mtame != 0) {
                pline("You have a peculiarly sad feeling for a moment, then it passes.");
            }

            monstone(mdef);

            hit = 2;
        }

        mdef->mhp -= d(pa->damn, pa->damd);
        if(mdef->mhp < 1) {
            magr->orig_hp += ((1 + rn2(pd->mlevel)) + 1);

            if((magr->mtame != 0) && (magr->orig_hp > (8 * pa->mlevel))) {
                if(pa == &li_dog) {
                    pa = &dog;
                    magr->data = pa;
                }
                else if(pa == &dog) {
                    pa = &la_dog;
                    magr->data = pa;
                }

                if(vis != 0) {
                    pline("%s is killed!", Monnam(mdef));
                }
                else if(mdef->mtame != 0) {
                    pline("You have a sad feeling for a moment, then it passes.");
                }

                mondied(mdef);

                hit = 2;
            }
        }
    }

    return hit;
}

/* Drop (perhaps) a cadaver and remove monster */
void mondied(struct monst *mdef)
{
    struct permonst *pd = mdef->data;

    if((letter(pd->mlet) != 0) && (rn2(3) != 0)) {
        mksobj_at(pd->mlet, CORPSE, mdef->mx, mdef->my);

        if(cansee(mdef->mx, mdef->my) != 0) {
            unpmon(mdef);
            atl(mdef->mx, mdef->my, fobj->olet);
        }

        stackobj(fobj);
    }

    mondead(mdef);
}

/* Drop a rock and remove monster */
void monstone(struct monst *mdef)
{
    extern char mlarge[];

    if(index(mlarge, mdef->data->mlet) != 0) {
        mksobj_at(ROCK_SYM, ENORMOUS_ROCK, mdef->mx, mdef->my);
    }
    else {
        mksobj_at(WEAPON_SYM, ROCK, mdef->mx, mdef->my);
    }

    if(cansee(mdef->mx, mdef->my) != 0) {
        unpmon(mdef);
        atl(mdef->mx, mdef->my, fobj->olet);
    }

    mondead(mdef);
}

int fightm(struct monst *mtmp)
{
    struct monst *mon;

    for(mon = fmon; mon != NULL; mon = mon->nmon) {
        if(mon != mtmp) {
            if(DIST(mon->mx, mon->my, mtmp->mx, mtmp->my) < 3) {
                if(rn2(4) != 0) {
                    return hitmm(mtmp, mon);
                }
            }
        }
    }

    return -1;
}

int hitu(struct monst *mtmp, int dam)
{
    int tmp;

    if((mtmp->mhide != 0) && (mtmp->mundetected != 0)) {
        mtmp->mundetected = 0;

        if(Blind == 0) {
            struct obj *obj;

            obj = o_at(mtmp->mx, mtmp->my);

            if(obj != NULL) {
                pline("%s was hidden under %s!", Xmonnam(mtmp), doname(obj));
            }
        }
    }

    tmp = u.uac;

    /* Give people with AC = -10 at least some vulnerability */
    if(tmp < 0) {
        /* Decrease damage */
        dam += tmp;

        if(dam <= 0) {
            dam = 1;
        }

        tmp = -rn2(-tmp);
    }

    if(multi < 0) {
        tmp += 4;
    }

    if((Invis != 0) || (mtmp->mcansee != 0)) {
        tmp -= 2;
    }

    if(mtmp->mtrapped != 0) {
        tmp -= 2;
    }

    if(tmp <= rnd(20)) {
        if(Blind != 0) {
            pline("It misses.");
        }
        else {
            pline("%s misses.", Monnam(mtmp));
        }

        return 0;
    }

    if(Blind != 0) {
        pline("It hits!");
    }
    else {
        pline("%s hits!", Monnam(mtmp));
    }

    losehp_m(dam, mtmp);

    return 1;
}

/* u is hit by something, but not a monster */
int thitu(int tlev, int dam, char *name)
{
    char buf[BUFSZ];
    
    setan(name, buf);

    if((u.uac + tlev) <= rnd(20)) {
        if(Blind != 0) {
            pline("It misses.");
        }
        else {
            pline("You are almost hit by %s!", buf);
        }

        return 0;
    }
    else {
        if(Blind != 0) {
            pline("You are hit!");
        }
        else {
            pline("You are hit by %s!", buf);
        }

        losehp(dam, name);

        return 1;
    }
}


char mlarge[] = "bCDdegIlmnoPSsTUwY~,&";

/* Return TRUE if mon is still alive */
boolean hmon(struct monst *mon, struct obj *obj, int thrown)
{
    int tmp;

    if(obj == NULL) {
        /* Attack with bare hands */
        tmp = rnd(2);

        if((mon->data->mlet == 'c') && (uarmg == NULL)) {
            pline("You hit the cockatrice with your bare hands");
            pline("You turn to stone ...");
            done_in_by(mon);
        }
    }
    else if(obj->olet == WEAPON_SYM) {
        if((obj == uwep) && ((obj->otyp > SPEAR) || (obj->otyp < BOOMERANG))) {
            tmp = rnd(2);
        }
        else {
            if(index(mlarge, mon->data->mlet) != 0) {
                tmp = rnd(objects[obj->otyp].wldam);

                if(obj->otyp == TWO_HANDED_SWORD) {
                    tmp += d(2, 6);
                }
                else if(obj->otyp == FLAIL) {
                    tmp += rnd(4);
                }
            }
            else {
                tmp = rnd(objects[obj->otyp].wsdam);
            }

            tmp += obj->spe;
            
            if((thrown == 0)
               && (obj == uwep)
               && (obj->otyp == BOOMERANG) 
               && (rn2(3) == 0)) {
                pline("As you hit %s, the boomerang breaks into splinters.",
                      monnam(mon));
                
                freeinv(obj);
                setworn((struct obj *)0, obj->owornmask);
                obfree(obj, (struct obj *)0);
                
                ++tmp;
            }
        }

        if((mon->data->mlet == 'O') && (strcmp(ONAME(obj), "Orcist") == 0)) {
            tmp += rnd(10);
        }
    }
    else {
        switch(obj->otyp) {
        case HEAVY_IRON_BALL:
            tmp = rnd(25);

            break;
        case EXPENSIVE_CAMERA:
            pline("You succeed in destroying your camera. Congratulations!");
            freeinv(obj);

            if(obj->owornmask != 0) {
                setworn((struct obj *)0, obj->owornmask);
            }

            obfree(obj, (struct obj *)0);
            
            return TRUE;
        case DEAD_COCKATRICE:
            pline("You hit %s with the cockatrice corpse", monnam(mon));
            pline("%s is turned to stone!", Monnam(mon));
            killed(mon);

            return FALSE;
        case CLOVE_OF_GARLIC:
            if(index(" VWZ", mon->data->mlet) != 0) {
                mon->mflee = 1;
            }

            tmp = 1;

            break;
        default:
            /* 
             * Non-weapons can damage because of their weight
             * (but not too much)
             */
            tmp = obj->owt / 10;

            if(tmp < 1) {
                tmp = 1;
            }
            else {
                tmp = rnd(tmp);
            }

            if(tmp > 6) {
                tmp = 6;
            }
        }
    }

    /******* NOTE: Perhaps obj is undefined!! (if (thrown == 0) && (BOOMERANG != 0)) */

    tmp += (u.udaminc + dbon());
    
    if(u.uswallow != 0) {
        if(mon->data->mlet == 'P') {
            tmp -= u.uswldtim;
            if(tmp <= 0) {
                pline("Your arms are no longer able to hit.");
                
                return TRUE;
            }
        }
    }
    
    if(tmp < 1) {
        tmp = 1;
    }
    
    mon->mhp -= tmp;
    
    if(mon->mhp < 1) {
        killed(mon);
        
        return FALSE;
    }
    
    /* This assumes that we cannot throw plural things */
    if(thrown != 0) {
        /* Or: objects[obj->otyp].oc_name */
        hit(xname(obj), mon, exclam(tmp));
        
        return TRUE;
    }
    
    if(Blind != 0) {
        pline("You hit it.");
    }
    else {
        pline("You hit %s%s", monnam(mon), exclam(tmp));
    }
    
    if(u.umconf != 0) {
        if(Blind == 0) {
            pline("Your hands stop glowing blue.");
            
            if((mon->mfroz == 0) && (mon->msleep == 0)) {
                pline("%s appears confused.", Monnam(mon));
            }
        }
        
        mon->mconf = 1;
        u.umconf = 0;
    }
    
    /* mon still alive */
    return TRUE;
}
