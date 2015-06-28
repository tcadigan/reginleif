/* Copyright (c) Stichting Matehmatisch Centrum, Amsterdam, 1984. */

#include "hack.apply.h"

#include "hack.h"
#include "hack.apply.h"
#include "hack.do.h"
#include "hack.do_name.h"
#include "hack.invent.h"
#include "hack.mon.h"
#include "hack.pri.h"
#include "hack.topl.h"
#include "hack.tty.h"
#include "hack.wield.h"
#include "rnd.h"

extern char pl_character[];

int doapply()
{
    struct obj *obj;

    obj = getobj("(", "use or apply");

    if(obj == NULL) {
        return 0;
    }

    switch(obj->otyp) {
    case EXPENSIVE_CAMERA:
        use_camera(obj);
        break;
    case ICE_BOX:
        use_ice_box(obj);
        break;
    case MAGIC_WHISTLE:
        if((pl_character[0] == 'W') || (u.ulevel > 9)) {
            use_magic_whistle(obj);
            break;
        }
        /* Fall into next case */
    case WHISTLE:
        use_whistle(obj);
        break;
    default:
        pline("Sorry, I don't know how to use that.");

        return 0;
    }

    return 1;
}

/* ARGSUSED */
void use_camera(struct obj *obj)
{
    struct monst *mtmp;

    if(!getdir()) {
        multi = 0;
        flags.move = multi;
        return;
    }

    mtmp = bchit(u.dx, u.dy, COLNO, '!');

    if(mtmp != NULL) {
        if(mtmp->msleep) {
            mtmp->msleep = 0;

            pline("The flash awakens the %s.", monnam(mtmp));
        }
        else {
            if(mtmp->data->mlet != 'y') {
                if(mtmp->mcansee || mtmp->mblinded) {
                    int tmp = dist(mtmp->mx, mtmp->my);
                    int tmp2;

                    /* if(cansee(mtmp->mx, mtmp->my)) */
                    pline("%s is blinded by the flash!", Monnam(mtmp));

                    setmangry(mtmp);

                    if((tmp < 9) && !mtmp->isshk && !rn2(4)) {
                        mtmp->mflee = 1;
                    }

                    if(tmp < 3) {
                        mtmp->mblinded = 0;
                        mtmp->mcansee = mtmp->mblinded;
                    }
                    else {
                        tmp2 = mtmp->mblinded;
                        tmp2 += rnd(1 + (50 / tmp));

                        if(tmp2 > 127) {
                            tmp2 = 127;
                        }

                        mtmp->mblinded = tmp2;
                        mtmp->mcansee = 0;
                    }
                }
            }
        }
    }
}

/*
 * A local variable of use_ice_box, to be used by its 
 * local procedures in/ck_ice_box
 */
struct obj *current_ice_box;

int in_ice_box(struct obj *obj)
{
    if((obj == current_ice_box)
       || (Punished && ((obj == uball) || (obj == uchain)))) {
        pline("You must be kidding.");

        return 0;
    }

    if(obj->owornmask & (W_ARMOR | W_RING)) {
        pline("You cannot refrigerate something you are wearing.");
       
        return 0;
    }

    if((obj->owt + current_ice_box->owt) > 70) {
        pline("It won't fit.");

        return 1; /* Be careful! */
    }

    if(obj == uwep) {
        if(uwep->cursed) {
            pline("Your weapon is welded to your hand!");

            return 0;
        }

        setuwep((struct obj *)0);
    }

    current_ice_box->owt += obj->owt;
    freeinv(obj);
    obj->o_cnt_id = current_ice_box->o_id;
    obj->nobj = fcobj;
    fcobj = obj;
    obj->age = moves - obj->age; /* Actual age */

    return 1;
}

int ck_ice_box(struct obj *obj)
{
    return (obj->o_cnt_id == current_ice_box->o_id);
}

int out_ice_box(struct obj *obj)
{
    struct obj *otmp;

    if(obj == fcobj) {
        fcobj = fcobj->nobj;
    }
    else {
        for(otmp = fcobj; otmp->nobj != obj; otmp = otmp->nobj) {
            if(!otmp->nobj) {
                panic("out_ice_box");
            }
        }

        otmp->nobj = obj->nobj;
    }

    current_ice_box->owt -= obj->owt;
    obj->age = moves - obj->age; /* Simulated point of time */
    
    addinv(obj);

    return 0; /* Needed to be function pointer for askchain() */
}

void use_ice_box(struct obj *obj)
{
    int cnt = 0;
    struct obj *otmp;

    current_ice_box = obj; /* For use by in/out_ice_box */

    for(otmp = fcobj; otmp != NULL; otmp = otmp->nobj) {
        if(otmp->o_cnt_id == obj->o_id) {
            ++cnt;
        }
    }

    if(cnt != 0) {
        pline("Your ice-box is empty.");
    }
    else {
        pline("Do you want to take something out of the ice-box? [yn] ");
        
        if(readchar() == 'y') {
            if(askchain(fcobj, (char *)0, 0, out_ice_box, ck_ice_box, 0)) {
                return;
            }
        }

        pline("That was all. Do you wish to put something in? [yn] ");

        if(readchar() != 'y') {
            return;
        }
    }

    /* Call getobj: 0: allow cnt; #: allow all types; %: expect food */
    otmp = getobj("0#%", "put in");

    if((otmp != NULL) || !in_ice_box(otmp)) {
        multi = 0;
        flags.move = multi;
    }
}

struct monst *bchit(int ddx, int ddy, int range, char sym)
{
    struct monst *mtmp = (struct monst *)0;
    int bchx = u.ux;
    int bchy = u.uy;

    if(sym != 0) {
        Tmp_at(-1, sym); /* Open call */
    }

    while(range) {
        --range;
        bchx += ddx;
        bchy += ddy;

        mtmp = m_at(bchx, bchy);

        if(mtmp != 0) {
            break;
        }

        if(levl[bchx][bchy].typ < CORR) {
            bchx -= ddx;
            bchy -= ddy;
            break;
        }

        if(sym != 0) {
            Tmp_at(bchx, bchy);
        }
    }

    if(sym != 0) {
        Tmp_at(-1, -1);
    }

    return mtmp;
}

#include "def.edog.h"

/* ARGSUSED */
void use_whistle(struct obj *obj)
{
    struct monst *mtmp = fmon;

    pline("You produce a high whistling sound.");

    while(mtmp != NULL) {
        if(dist(mtmp->mx, mtmp->my) < (u.ulevel * 10)) {
            if(mtmp->msleep) {
                mtmp->msleep = 0;
            }

            if(mtmp->mtame) {
                EDOG(mtmp)->whistletime = moves;
            }
        }

        mtmp = mtmp->nmon;
    }
}

/* ARGSUSED */
void use_magic_whistle(struct obj *obj)
{
    struct monst *mtmp = fmon;

    pline("You produce a strange whistling sound.");

    while(mtmp != NULL) {
        if(mtmp->mtame) {
            mnexto(mtmp);
        }

        mtmp = mtmp->nmon;
    }
}
        
