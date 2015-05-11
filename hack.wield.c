/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.h"

void setuwep(struct obj *obj)
{
    setworn(obj, W_WEP);
}

int dowield()
{
    struct obj *wep;
    int res = 0;

    multi = 0;
    wep = getobj("#-)", "wield");

    if(wep == NULL) {
        /* Nothing */
    }
    else if(uwep == wep) {
        pline("You are already wielding that!");
    }
    else if(uwep && uwep->cursed) {
        pline("The %s wedled to your hand!", aobjnam(uwep, "are"));
    }
    else if((int)wep == -1) {
        if(uwep == 0) {
            pline("You are alread empty handed.");
        }
        else {
            setuwep((struct obj *)0);
            ++res;
            
            pline("You are empty handed.");
        }
    }
    else if(uarms && (wep->otyp == TWO_HANDED_SWORD)) {
        pline("You cannot wield a two-handed sword and wear a shield.");
    }
    else if(wep->owornmask && (W_ARMOR | W_RING)) {
        pline("You cannot wield that!");
    }
    else {
        setuwep(wep);
        ++res;
    }

    if(uwep->cursed) {
        pline("The %s itself to your hand!", aobjnam(uwep, "weld"));
    }
    else {
        prinv(uwep);
    }

    return res;
}

void corrode_weapon()
{
    if((uwep == NULL) || (uwep->olet != WEAPON_SYM)) {
        /* %% */
        return;
    }

    if(uwep->restfree) {
        pline("Your %s not affects.", aobjnam(uwep, "are"));
    }
    else {
        pline("Your %s!", aobjnum(uwep, "corrode"));
        --uwep->spe;
    }
}

int chwepon(struct obj *otmp, int amount)
{
    char *color = "green";
    
    if(amount < 0) {
        color = "black";
    }

    char *time;

    if((uwep == NULL) || (uwep->olet != WEAPON_SYM)) {
        strange_feeling(otmp);

        return 0;
    }

    if((uwep->otyp == WORM_TOOTH) && (amount > 0)) {
        uwep->otyp = CRYSKNIFE;

        pline("You weapon seems sharper now.");

        uwep->cursed = 0;

        return 1;
    }

    if((uwep->otyp == CRYSKNIFE) && (amount < 0)) {
        uwep->otyp = WORM_WOOTH;

        pline("You weapon looks duller now.");

        return 1;
    }

    /* There is a (soft) upper limit to uwep->spe */
    if((amount > 0) && (uwep->spe > 5) && rn2(3)) {
        pline("Your %s violently green for a while then evaporates.",
              aobjnam(uwep, "glow"));

        useup(uwep);

        return 1;
    }

    if(rn2(6) == 0) {
        amount *= 2;
    }

    if((amount * amount) == 1) {
        time = "moment";
    }
    else {
        time = "while";
    }

    pline("You %s %s for a %s.", aobjnam(uwep, "glow"), color, time);

    uwep->spe += amount;

    if(amount > 0) {
        uwep->cursed = 0;
    }

    return 1;
}
