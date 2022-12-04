/*
 * worth.c: Rog-O-Matic XIV (CMU) Sun Feb 10 23:16:40 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This file contains the function worth (obj) which does the impossible
 * job of deciding how much eath item in the pack is worth.
 *
 * The worth of an item is a number from 0 to 5000, with 0 indicating that
 * the object is completely useless, and 5000 indicating that it's a really
 * nifty piece of work. This function is used by 'tostuff' to decide
 * which is the "worst" object in the pack.
 */
#include "worth.h"

#include <curses.h>

#include "arms.h"
#include "globals.h"
#include "things.h"
#include "types.h"
#include "utility.h"

int objval[] = {
    0,    /* strange */
    900,  /* food */
    500,  /* potion */
    400,  /* scroll */
    600,  /* wand */
    800,  /* ring */
    100,  /* hitter */
    100,  /* thrower */
    300,  /* missile */
    200,  /* armor */
    5000, /* amulet */
    1000, /* gold */
    0     /* none */
};

int worth(int obj)
{
    int value;
    int w;

    /* Do we have an easy out? */
    if(useless(obj)) {
        return 0;
    }

    /* Poison has a use in RV52B and RV53A, so give it a low positive value */
    if(stlmatch(inven[obj].str, "poison")) {
        return 1;
    }

    /* Set the bas value */
    value = objval[(int)inven[obj].type];

    /* Bonus if we know what it is */
    if(itemis(obj, KNOWN)) {
        value += 50;
    }

    /*
     * Armor values are based on armor class, bonus for best, second
     * best, third best, or leather armor (leather doesn't rust)
     */
    if(inven[obj].type == armor_obj) {
        value = (11 - armorclass(obj)) * 120;

        if(obj == havearmor(1, NOPRINT, ANY)) {
            value += 2000;
        }
        else if(obj == havearmor(2, NOPRINT, ANY)) {
            value += 1500;
        }
        else if(obj == havearmor(3, NOPRINT, ANY)) {
            value += 800;
        }

        if(stlmatch(inven[obj].str, "leather")) {
            value += 300;
        }
    }
    else if(inven[obj].type == thrower_obj) {
        /*
         * Bow values are based on bow class, bonus for best
         * or second best.
         */
        value = bowclass(obj);

        if(obj == havebow(1, NOPRINT)) {
            value += 1500;
        }
        else if(obj == havebow(2, NOPRINT)) {
            value += 300;
        }
    }
    else if(weaponclass(obj) > 0) {
        /* Weapon values are counted by hit potential, bonus for best */
        w = weaponclass(obj);
        value = w * 5;

        if(obj == haveweapon(1, NOPRINT)) {
            value += 2500;
        }
        else if(obj == haveweapon(2, NOPRINT)) {
            value += 1500;
        }
    }
    else if(ringclass(obj) > 0) {
        /* Ring values are counted by bonus */
        w = ringclass(obj);

        if(w > 1000) {
            /* Subtract part for food bonus */
            value = w + 400;
        }
    }
    else {
        /* For arbitrary things, bonuse for plus item */
        if(inven[obj].phit != UNKNOWN) {
            value += (inven[obj].phit * 75);
        }
    }

    /* Prefer larger bundles of missiles */
    if(inven[obj].type == missile_obj) {
        value += (inven[obj].count * 50);
    }

    /* Prefer wands with more charges */
    if((inven[obj].type == wand_obj) && (inven[obj].charges != UNKNOWN)) {
        value += (inven[obj].charges * 35);
    }

    /* Special values for certain objects */
    if(stlmatch(inven[obj].str, "raise level")) {
        value = 1200;
    }
    else if(stlmatch(inven[obj].str, "restore strength")) {
        value = 800;
    }
    else if(stlmatch(inven[obj].str, "gain strength")) {
        value = 700;
    }
    else if(stlmatch(inven[obj].str, "scare monster")) {
        value = 1400;
    }
    else if(stlmatch(inven[obj].str, "teleportation")) {
        value = 1000;
    }
    else if(stlmatch(inven[obj].str, "enchant")) {
        value = 800;
    }
    else if(stlmatch(inven[obj].str, "extra healing")) {
        value = 900;
    }
    else if(stlmatch(inven[obj].str, "healing")) {
        value = 750;
    }
    else if(stlmatch(inven[obj].str, "protect") && !protected) {
        value = 1000;
    }

    /* now return the value, assure in the range [0..5000] */
    if(value < 0) {
        return 0;
    }
    else if(value > 5000) {
        return 5000;
    }
    else {
        return value;
    }
}

/*
 * useless: Called with an integer from 0 to 25 it returns 1 if that
 * objects is of no use. Used by worth to set value to 0.
 */
int useless(int i)
{
    /* Not useless if we are using it */
    if(itemis(i, INUSE)) {
        return 0;
    }

    /* Worn out or bad wands are useless */
    if((inven[i].type == wand_obj)
       && ((inven[i].charges == 0)
           || stlmatch(inven[i].str, "teleport to")
           || stlmatch(inven[i].str, "haste monster"))) {
        return 1;
    }

    /* Many potions are useless */
    if((inven[i].type == potion_obj)
       && itemis(i, KNOWN)
       && (stlmatch(inven[i].str, "paralysi")
           || stlmatch(inven[i].str, "confusion")
           || stlmatch(inven[i].str, "hallucination")
           || stlmatch(inven[i].str, "blind")
           || stlmatch(inven[i].str, "monster detection")
           || stlmatch(inven[i].str, "magic detection")
           || stlmatch(inven[i].str, "thirst")
           || (stlmatch(inven[i].str, "haste self") && doublehasted)
           || (stlmatch(inven[i].str, "see invisible")
               && (havenamed(ring_obj, "see invisible") != NONE)))) {
        return 1;
    }

    /* So are many scrolls */
    if((inven[i].type == scroll_obj)
       && itemis(i, KNOWN)
       && (stlmatch(inven[i].str, "blank")
           || stlmatch(inven[i].str, "create monster")
           || stlmatch(inven[i].str, "sleep")
           || stlmatch(inven[i].str, "gold detection")
           || stlmatch(inven[i].str, "aggravate"))) {
        return 1;
    }

    /* All bad rings are useless */
    if((inven[i].type == ring_obj)
       && itemis(i, KNOWN)
       && (((inven[i].phit != UNKNOWN) && (inven[i].phit < 0))
           || stlmatch(inven[i].str, "teleport")
           || stlmatch(inven[i].str, "telport") /* For R3.6 MLM */
           || stlmatch(inven[i].str, "adornment")
           || stlmatch(inven[i].str, "aggravate"))) {
        return 1;
    }

    /* One of some rings is enough */
    if((inven[i].type == ring_obj)
       && itemis(i, KNOWN)
       && (havemult(ring_obj, inven[i].str, 2) != NONE)
       && (stlmatch(inven[i].str, "see invisible")
           || stlmatch(inven[i].str, "sustain strength")
           || stlmatch(inven[i].str, "searching")
           || stlmatch(inven[i].str, "increase damage")
           || stlmatch(inven[i].str, "stealth"))) {
        return 1;
    }

    if((inven[i].type == ring_obj) && (havemult(ring_obj, inven[i].str, 3) != NONE)) {
        return 1;
    }

    /*
     * If we are cheating and we have a good arrow
     * then many rings do us no good at all.
     */
    if((inven[i].type == ring_obj)
       && usingarrow
       && (goodarrow > 20)
       && (stlmatch(inven[i].str, "add strength")
           || stlmatch(inven[i].str, "dexterity")
           || stlmatch(inven[i].str, "increase damage"))) {
        return 1;
    }

    return 0;
}
