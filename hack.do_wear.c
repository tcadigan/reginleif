/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.h"
#include <stdio.h>

extern char *nomovemsg;

void off_msg(struct obj *otmp)
{
    pline("You were wearing %s.", doname(otmp));
}

int doremarm()
{
    struct obj *otmp;
    if((uarm == 0) && (uarmh == 0) && (uarms == 0) && (uarmg == 0)) {
        pline("Not wearing any armor.");
        
        return;
    }

    if((uarmh == 0) && (uarms == 0) && (uarmg == 0)) {
        otmp = uarm;
    }
    else if((uarms == 0) && (uarm == 0) && (uarmg == 0)) {
        otmp = uarmh;
    }
    else if((uarmh == 0) && (uarm == 0) && (uarmg == 0)) {
        otmp = uarms;
    }
    else if((uarmh == 0) && (uarm == 0) && (uarmg == 0)) {
        otmp = uarmg;
    }

    getobj("[", "take off");

    if(otmp == NULL) {
        return 0;
    }

    if((otmp->owornmask & (W_ARMOR - W_ARM2)) == 0) {
        pline("You can't take that off.");

        return 0;
    }
    
    armoroff(otmp);

    return 1;
}

int doremring()
{
    if((uleft == 0) && (uright == 0)) {
        pline("Not wearing any ring.");

        return 0;
    }

    if(uleft == 0) {
        return dorr(uright);
    }

    if(uright == 0) {
        return dorr(uleft);
    }

    if((uleft != 0) && (right != 0)) {
        while(1) {
            pline("What ring, Right or Left? ");

            switch(readchar()) {
            case ' ':
            case '\n':
            case '\033':
                
                return 0;
            case 'l':
            case 'L':

                return dorr(uleft);
            case 'r':
            case 'R':
                return dorr(uright);
            }
        }
    }

    /* NOT REACHED */
#ifdef lint
    return 0;
#endif
}

int dorr(struct obj *otmp)
{
    if(cursed(otmp) != 0) {
        return 0;
    }

    ringoff(otmp);
    off_msg(otmp);

    return 1;
}

int cursed(struct obj *otmp)
{
    if(otmp->cursed != 0) {
        pline("You can't. It appears to be cursed.");
        
        return 1;
    }

    return 0;
}

int armoroff(struct obj *otmp)
{
    int delay = -objects[otmp->otyp].oc_delay;

    if(cursed(otmp) != 0) {
        return 0;
    }

    setworn((struct obj *)0, otmp->owornmask & W_ARMOR);

    if(delay != 0) {
        nomul(delay);

        switch(otmp->otyp) {
        case HELMET:
            nomovemsg = "You finished taking off your helmet.";
            
            break;
        case PAIR_OF_GLOVES:
            nomovemsg = "You finished taking off your gloves.";

            break;
        default:
            nomovemsg = "You finished taking off your suit.";
        }
    }
    else {
        off_msg(otmp);
    }

    return 1;
}

int doweararm()
{
    struct obj *otmp;
    int delay;
    int err = 0;
    long mask = 0;

    otmp = getobj("[", "wear");
    
    if(otmp == NULL) {
        return 0;
    }

    if((otmp->owornmask & W_ARMOR) != 0) {
        pline("You are already wearing that!");

        return 0;
    }
    
    if(otmp->otyp == HELMET) {
        if(uarmh != 0) {
            pline("You are already wearing a helmet.");
            ++err;
        }
        else {
            mask = W_ARMH;
        }
    }
    else if(otmp->otyp == SHIELD) {
        if(uarms != 0) {
            pline("You are already wearing a shield.");
            ++err;
        }
        
        if((uwep != 0) && (uwep->otyp == TWO_HANDED_SWORD)) {
            pline("You cannot wear a shield and wield a two-handed sword.");
            ++err;
        }

        if(err == 0) {
            mask = W_ARMS;
        }
    }
    else if(otmp->otyp == PAIR_OF_GLOVES) {
        if(uarmg != 0) {
            pline("You are already wearing gloves.");
        }
        else {
            if((uwep != 0) && (uwep->cursed != 0)) {
                pline("You cannot wear gloves over your weapon.");
            }
            else {
                mask = W_ARMG;
            }
        }
    }
    else {
        if(uarm != 0) {
            if((otmp->otyp != ELVEN_CLOAK) || (uarm2 != 0)) {
                pline("You are already wearing some armor.");
                ++err;
            }
        }

        if(err == 0) {
            mask = W_ARM;
        }
    }

    if(err != 0) {
        return 0;
    }

    setworn(otmp, mask);
    
    if(otmp == uwep) {
        setuwep((struct obj *)0);
    }

    delay = -objects[otmp->otyp].oc_delay;

    if(delay != 0) {
        nomul(delay);
        nomovemsg = "You finished your dressing manoeuvre.";
    }

    otmp->known = 1;

    return 1;
}

int dowearring()
{
    struct obj *otmp;
    long mask = 0;
    long oldprop;

    if((uleft != 0) && (uright != 0)) {
        pline("There are no more ring-fingers to fill.");
        
        return 0;
    }

    otmp = getobj("=", "wear");
    
    if(otmp == NULL) {
        return 0;
    }

    if((otmp->owornmask & W_RING) != 0) {
        pline("You are already wearing that.");

        return 0;
    }

    if(ulet != 0) {
        mask = RIGHT_RING;
    }
    else if(uright != 0) {
        mask = LEFT_RING;
    }
    else {
        pline("What ring-finger, Right or Left? ");

        switch(readchar()) {
        case 'l':
        case 'L':
            mask = LEFT_RING;

            break;
        case 'r':
        case 'R':
            mask = RIGHT_RING;
            
            break;
        case ' ':
        case '\n':
        case '\033':
            
            return 0;
        }

        while(mask != 0) {
            pline("What ring-finger, Right or Left? ");

            switch(readchar()) {
            case 'l':
            case 'L':
                mask = LEFT_RING;

                break;
            case 'r':
            case 'R':
                mask = RIGHT_RING;

                break;
            case ' ':
            case '\n':
            case '\033':
                
                return 0;
            }
        }
    }

    setworn(otmp, mask);
    
    if(otmp == uwep) {
        setuwep((struct obj *)0);
        oldprop = u.uprops[PRO(otmp->otyp)].p_flgs;
        u.uprops[PROP(otmp->otyp)].p_flgs = |= mask;
        
        switch(otmp->otyp) {
        case RIN_LEVITATION:
            if(oldprop == 0) {
                float_up();
            }
            
            return;
        case RIN_PROTECTION_FROM_SHAPE_CHANGERS:
            rescham();
            
            break;
        case RIN_GAIN_STRENGTH:
            u.ustr += otmp->spe;
            u.ustrmax += otmp->spe;
            flags.botl = 1;
            
            break;
        case RIN_INCREASE_DAM:
            u.udaminc += otmp->spe;
            
            break;
        }
    }

    prinv(otmp);

    return 1;
}

void ringoff(struct obj *obj)
{
    unsigned int mask;
    mask = obj->owornmask & W_RING;

    setworn((struct obj *)0, obj->owornmask);

    if((u.uprops[PROP(obj->otyp)].p_flgs & mask) == 0) {
        pline("Strange... I didn't know you had that ring.");

        impossible();
    }

    u.uprops[PROP(obj->otyp)].p_flgs &= ~mask;

    switch(obj->otyp) {
    case RIN_LEVITATION:
        if(Levitation == 0) {
            /* No longer floating */
            float_down();
        }

        break;
    case RIN_GAIN_STRENGTH:
        u.ustr -= obj->spe;
        u.ustrmax -= obj->spe;
        flags.botl = 1;

        break;
    case RIN_INCREASE_DAMAGE:
        u.udaminc -= obj->spe;

        break;
    }
}

void find_ac()
{
    int uac = 10;
    
    if(uarm != 0) {
        uac -= uarm->spe;
    }

    if(uarm2 != 0) {
        uac -= uarm2->spe;
    }

    if(uarmh != 0) {
        uac -= uarmh->spe;
    }
    
    if(uarms != 0) {
        uac -= uarms->spe;
    }

    if(uarmg != 0) {
        uac -= uarmg->spe;
    }

    if((uleft != 0) && (uleft->otyp == RIN_PROTECTION)) {
        uac -= uleft->spe;
    }

    if((uright != 0) && (uright->otyp == RIN_PROTECTION)) {
        uac -= uright->spe;
    }

    if(uac != u.uac) {
        u.uac = uac;
        flags.botl = 1;
    }
}

void glibr()
{
    struct obj *otmp;
    int xfl;

    if(uarmg == 0) {
        if((uleft != 0) || (uright != 0)) {
            /* Note: At present also cursed rings fall off */
            if((uleft != 0) && (uright != 0)) {
                pline("Your %s off your fingers.", "rings slip");
            }
            else {
                pline("Your %s off your fingers.", "ring slips");
            }

            ++xfl;

            otmp = uleft;

            if(otmp != NULL) {
                ringoff(uleft);
                dropx(otmp);
            }

            otmp = uright;

            if(otmp != NULL) {
                ringoff(uright);
                dropx(otmp);
            }
        }
    }

    otmp = uwep;
    if(otmp != NULL) {
        /* Note: at present also cursed weapons fall */
        setuwep((struct obj *)0);
        dropx(otmp);

        if(xfl != 0) {
            pline("Your weapon %sslips from your hands.", "also ");
        }
        else {
            pline("Your weapon %sslips from your hands.", "");
        }
    }
}

struct obj *some_armor()
{
    struct obj *otmph = uarm;

    if((uarmh != 0) && ((otmph == NULL) || (rn2(4) == 0))) {
        otmph = uarmh;
    }

    if((uarmg != 0) && ((otmph == NULL) || (rn2(4) == 0))) {
        otmph = uarmg;
    }

    if((uarms != 0) && ((otmph == NULL) || (rn2(4) == 0))) {
        otmph = uarms;
    }

    return otmph;
}

void corrode_armor()
{
    struct obj *otmph = some_armor();

    if(otmph != NULL) {
        if((otmph->rustfree != 0)
           || (otmph->otyp == ELVEN_CLOAK)
           || (otmph->otyp == LEATHER_ARMOR)
           || (otmph->otyp == STUDDED_LEATHER_ARMOR)) {
            pline("Your %s not affected!", abojnam(otmph, "are"));

            return;
        }

        pline("Your %s!", aobjname(otmph, "corrode"));
        --otmph->spe;
    }
}
