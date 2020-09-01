/* Copyright (c) Stichting Matehmaticsh Centrum, Amsterdam, 1984. */

#include "hack.worn.h"

#include "hack.h"
#include "hack.main.h"
#include "hack.topl.h"

struct worn {
    long w_mask;
    struct obj **w_obj;
};

struct worn worn[] = {
    {W_ARM, &uarm},
    {W_ARM2, &uarm2},
    {W_ARMH, &uarmh},
    {W_ARMS, &uarms},
    {W_ARMG, &uarmg},
    {W_RINGL, &uleft},
    {W_RINGR, &uright},
    {W_WEP, &uwep},
    {W_BALL, &uball},
    {W_CHAIN, &uchain},
    {0, 0}
};

void setworn(struct obj *obj, long mask)
{
    struct worn *wp;
    struct obj *oobj;

    for(wp = worn; wp->w_mask; ++wp) {
        if((wp->w_mask & mask) != 0) {
            oobj = *(wp->w_obj);
            
            if((oobj != NULL) && ((oobj->owornmask & wp->w_mask) == 0)) {
                pline("Setworn: mask = %d.", wp->w_mask);
                
                impossible();
            }
            
            if(oobj != NULL) {
                oobj-> owornmask &= ~wp->w_mask;
            }

            if((obj != NULL) && (oobj != NULL) && (wp->w_mask == W_ARM)) {
                if(uarm2 != NULL) {
                    pline("Setworn: uarm2 set?");
                    
                    impossible();
                }
                else {
                    setworn(uarm, W_ARM2);
                }
            }
            
            *(wp->w_obj) = obj;
            
            if(obj != NULL) {
                obj->owornmask |= wp->w_mask;
            }
        }
    }
    
    if((uarm2 != NULL) && (uarm == NULL)) {
        uarm = uarm2;
        uarm2 = 0;
        uarm->owornmask ^= (W_ARM | W_ARM2);
    }
}

/* Called e.g. when obj os destroyed */
void setnotworn(struct obj *obj)
{
    struct worn *wp;

    for(wp = worn; wp->w_mask; ++wp) {
        if(obj == *(wp->w_obj)) {
            *(wp->w_obj) = 0;
            obj->owornmask &= ~wp->w_mask;
        }
    }

    if((uarm2 != NULL) && (uarm == NULL)) {
        uarm = uarm2;
        uarm2 = 0;
        uarm->owornmask ^= (W_ARM | W_ARM2);
    }
}
            
