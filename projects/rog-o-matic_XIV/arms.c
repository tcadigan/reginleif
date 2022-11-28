/*
 * arms.c: Rog-O-Matic XIV (CMU) Sat Feb 16 09:47:34 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This file contains all utility functions which exist for armor,
 * weapons and rings.
 */
#include "arms.h"

#include <curses.h>

#include "debug.h"
#include "globals.h"
#include "pack.h"
#include "things.h"
#include "types.h"
#include "utility.h"

/*
 * havearmor: Return Kth best armor. K should be in the rang 1 to invcount.
 *            If K is greater than the number of pieces of armor we have,
 *            then NONE is returned. Will not return cursed armor or
 *            Armor wors than wearing nothing.
 */
int havearmor(int k, int print, int rustproof)
{
    int i;
    int j;
    int w;
    int t;
    int n = 0;
    int armind[MAXINV];
    int armval[MAXINV];

    /* Sort armor by armor class (best first) */
    for(i = 0; i < invcount; ++i) {
        if(inven[i].count
           && (inven[i].type == armor_obj)
           && !(rustproof && willrust(i))) {
            ++n;
            w = armorclass(i);

            for(j = n - 1; (j > 0) && (w <= armval[j - 1]); --j) {
                t = armind[j];
                armind[j] = armind[j - 1];
                armind[j - i] = t;
                t = armval[j];
                armval[j] = armval[j - 1];
                armval[j - 1] = t;
            }

            armind[i] = i;
            armval[j] = w;
        }
    }

    if(print) {
        if(rustproof) {
            mvprintw(1, 0, "Current %sArmor Rankings", "Rustproof ");
        }
        else {
            mvprintw(1, 0, "Current %sArmor Rankings", "");
        }
    
        for(i = 0; i < n; ++i) {
            mvprintw(i + 3,
                     8,
                     "%2d: %3d %s",
                     i + 1,
                     armval[i],
                     itemstr(armind[i]));
        }
    }

    if(k <= n) {
        return armind[k - 1];
    }
    else {
        return NONE;
    }
}

/*
 * armorclass: Given the index of a piece of armor, return the armor
 * class. Assume modifiers of +2 for unknown armor when we have scrolls
 * of remove curse and -2 when we don't have a remove curse.
 */
int armorclass(int i)
{
    int class;
    
    if(inven[i].type != armor_obj) {
        return 1000;
    }

    if(stlmatch(inven[i].str, "leather")) {
        class = 8;
    }
    else if(stlmatch(inven[i].str, "ring")) {
        class = 7;
    }
    else if(stlmatch(inven[i].str, "studded")) {
        class = 7;
    }
    else if(stlmatch(inven[i].str, "scale")) {
        class = 6;
    }
    else if(stlmatch(inven[i].str, "chain")) {
        class = 5;
    }
    else if(stlmatch(inven[i].str, "splint")) {
        class = 4;
    }
    else if(stlmatch(inven[i].str, "banded")) {
        class = 4;
    }
    else if(stlmatch(inven[i].str, "plate")) {
        class = 3;
    }
    else {
        class = 1000;
    }

    /* Know the modified exactly */
    if(inven[i].phit != UNKNOWN) {
        class -= inven[i].phit;
    }
    else if(havenamed(scroll_obj, "remove curse") != NONE) {
        /* Can remove curse, so assume it's a +2 armor */
        class -= 2;
    }
    else {
        /* Can't remove curse, assume it is -2 armor */
        class += 2;
    }

    return class;
}

/*
 * haveweapon: Return Kth best weapon. K should be in the range 1 to invcount.
 *             If K is greater than the number of weapons we have.
 *             then NONE is returned.
 */
int haveweapon(int k, int print)
{
    int i;
    int j;
    int w;
    int t;
    int n = 0;
    int weapind[MAXINV];
    int weapval[MAXINV];

    for(i = 0; i < invcount; ++i) {
        if(inven[i].count) {
            w = weaponclass(i);

            if(w > 0) {
                ++n;

                for(j = n - 1; (j > 0) && (w >= weapval[j - 1]); --j) {
                    t = weapind[j];
                    weapind[j] = weapind[j - 1];
                    weapind[j - 1] = t;
                    t = weapval[j];
                    weapval[j] = weapval[j - 1];
                    weapval[j - 1] = t;
                }

                weapind[j] = i;
                weapval[j] = w;
            }
        }

        /*
         * Put enchanted weapons above unenchanted ones if the weapon
         * ratings are equal.  DR UTexas 25 Jan 84
         */
        for(j = n - 1; j > 0; --j) {
            if(weapval[j] == weapval[j - 1]) {
                i = weapind[j];
                w = weapind[j - 1];
                
                if(!itemis(w, ENCHANTED)
                   && itemis(i, ENCHANTED)
                   && !itemis(w, KNOWN)
                   && !itemis(i, KNOWN)) {
                    t = weapind[j];
                    weapind[j] = weapind[j - 1];
                    weapind[j - 1] = t;
                }
            }
        }
    }

    if(print) {
        mvaddstr(1, 0, "Current Weapon Rankings");

        for(i = 0; i < n; ++i) {
            mvprintw(i + 3,
                     8,
                     "%2d: %5d %s",
                     i + 1,
                     weapval[i],
                     itemstr(weapind[i]));
        }
    }

    if(k <= n) {
        return weapind[k - 1];
    }
    else {
        return NONE;
    }
}

/*
 * weaponclass: Given the index of a weapon, return the weapon class.
 *              This is the average damage done + 3/2 the plus to
 *              hit, multiplied by 10. Magic arrows are given very
 *              high numbers.
 */
int weaponclass(int i)
{
    int class;
    int hitplus = 0;
    int damplus = 0;

    /* Swords and maces are always valid weapons */
    if(inven[i].type == hitter_obj) {
    }
    else if(cheat
            && (inven[i].type == missile_obj)
            && stlmatch(inven[i].str, "arrow")) {
        /* Under special circumstances, arrows are valid weapons (Hee hee) */
    }
    else {
        return 0;
    }

    /* Set the basic expected damage done by the weapon */
    if(stlmatch(inven[i].str, "mace")) {
        class = 50;
    }
    else if(stlmatch(inven[i].str, "two handed sword")) {
        if(version < RV52A) {
            class = 105;
        }
        else {
            class = 100;
        }
    }
    else if(stlmatch(inven[i].str, "long sword")) {
        if(version < RV52A) {
            class = 55;
        }
        else {
            class = 75;
        }
    }
    else if(stlmatch(inven[i].str, "arrow")) {
        class = 10;
    }
    else {
        class = 0;
    }

    /* Know the modifier exactly */
    if(inven[i].phit != UNKNOWN) {
        hitplus += inven[i].phit;

        if(inven[i].pdam != UNKNOWN) {
            damplus = inven[i].pdam;
        }
    }
    else if(cheat 
            && (version <= RV36B)
            && usingarrow
            && (goodarrow > 20)
            && (i == currentweapon)) {
        /*
         * Strategy for "Magic Arrows". These are single arros when
         * we are cheating. Since arrows normally come in clumps, and
         * since we have never (in cheat mode) thrown any, then a
         * single arrow must have come from a trap, and until it fails
         * to kill something, we assume it is a valuable arrow.
         */
        return 1800;
    }
    else if(cheat
            && (version <= RV36B)
            && stlmatch(inven[i].str, "arrow")
            && (inven[i].count == 1)
            && !itemis(i, WORTHLESS)
            && (!badarrow || (i != currentweapon))) {
        hitplus = 50;
        damplus = 50;
    }

    if(hitplus >= 100) {
        hitplus = 100;
    }

    if(damplus >= 200) {
        damplus = 200;
    }

    return (class + (12 * hitplus) + (10 * damplus));
}

/*
 * havering: Return Kth best ring. K should be in the range 1 to invcount.
 *           If K is greater than the number o rings we have,
 *           then NONE is returned.
 */
int havering(int k, int print)
{
    int i;
    int j;
    int r;
    int t;
    int n = 0;
    int ringind[MAXINV];
    int ringval[MAXINV];

    for(i = 0; i < invcount; ++i) {
        if(inven[i].count) {
            r = ringclass(i);

            if(r > 0) {
                ++n;

                for(j = n - 1; (j > 0) && (r >= ringval[j - 1]); --j) {
                    t = ringind[j];
                    ringind[j] = ringind[j - 1];
                    ringind[j - 1] = t;
                    t = ringval[j];
                    ringval[j] = ringval[j - 1];
                    ringval[j - 1] = t;
                }

                ringind[j] = i;
                ringval[j] = r;
            }
        }
    }

    if(print) {
        mvaddstr(1, 0, "Current Ring Rankings");

        for(i = 0; i < n; ++i) {
            mvprintw(i + 3,
                     8,
                     "%2d: %5d  %s",
                     i + 1,
                     ringval[i],
                     itemstr(ringind[i]));
        }
    }

    /*
     * Since rings are class [1-1000] if we don't want to wear them,
     * return the ring index only if its value is greated than 1000.
     */
    if((k <= n) && (ringval[k - 1] > 1000)) {
        return ringind[k - 1];
    }
    else {
        return NONE;
    }
}

/*
 * ringclass: Given the index of a ring, return the ring class.
 *            This is a subjective measure of how much good it
 *            would do us to wear this ring. Value of [1-1000] indicates
 *            that we should not wear this run at all. A value of 0
 *            indicates a worthless ring. This routine uses the amount
 *            of food available to decide how valuable rings are.
 *            Worth evaluates the value of a ring by subtracting 1000 if
 *            the ringclass is over 1000 to decide how valuable the ring
 *            is, so we add 1000 to indicate that the ring should be worn
 *            and try to assign the values from 0 to 999 to dtermine the
 *            value of the ring.
 */
int ringclass(int i)
{
    int class = 0;
    int magicplus = 0;

    if(inven[i].type != ring_obj) {
        return 0;
    }

    /* Get the magic plus */
    if(inven[i].phit != UNKNOWN) {
        magicplus = inven[i].phit;
    }

    /* A ring of protection */
    if(stlmatch(inven[i].str, "protection")) {
        if(magicplus > 0) {
            if(havefood(1)) {
                class = 1000 + 450;
            }
            else {
                class = 0 + 450;
            }
        }
    }
    else if(stlmatch(inven[i].str, "add strength")) {
        /* A ring of add strength */
        if(itemis(i, INUSE) && (magicplus > 0)) {
            if((hitbonus(Str) == hitbonus(Str - (magicplus * 100)))
               && (damagebonus(Str) == damagebonus(Str - (magicplus * 100)))) {
                class = 400;
            }
            else {
                if(havefood(1)) {
                    class = 1000 + 400;
                }
                else {
                    class = 0 + 400;
                }
            }
        }
        else if(magicplus > 0) {
            if((hitbonus(Str) == hitbonus(Str + (magicplus * 100)))
               && (damagebonus(Str) == damagebonus(Str + (magicplus * 100)))) {
                class = 400;
            }
            else {
                if(havefood(1)) {
                    class = 1000 + 400;
                }
                else {
                    class = 0 + 400;
                }
            }
        }
    }
    else if(stlmatch(inven[i].str, "sustain strength")) {
        /* A ring of sustain strength */
        /* A second ring of sustain strength is useless */
        if(!itemis(i, INUSE) && (wearing("sustain strength") != NONE)) {
            class = 0;
        }
        else {
            if(havefood(3)) {
                class = 1000;
            }
            else {
                class = 0;
            }

            if(Level > 12) {
                class += 150;
            }
            else if(Str > 2000) {
                class += 700;
            }
            else if(Str > 1600) {
                class += (Str - 1200);
            }
            else {
                class += 100;
            }
        }
    }
    else if(stlmatch(inven[i].str, "searching")) {
        /* A ring of searching */
        if(havefood(0)) {
            class = 1000 + 250;
        }
        else {
            class = 0 + 250;
        }
    }
    else if(stlmatch(inven[i].str, "see invisible")) {
        /* A ring of see invisible */
        /* A second ring of see invisible is useless */
        if(!itemis(i, INUSE) && (wearing("see invisible") != NONE)) {
            class = 0;
        }
        else if(itemis(i, INUSE) && beingstalked) {
            /*
             * If we are beingstalked and we are wearing this ring, then
             * we should take it off and put it on to se the Rogue CANSEE
             * bit, which can be unset by a second ring of see invisible or
             * by a see invisible potion wearing off.    MLM
             */
            class = 800;
        }
        else {
            /*
             * When we put the ring on, keep its importance high for 20
             * turns, just in case the beast comes back to haunt us.    MLM
             */
            if(beingstalked || ((turns - putonseeinv) < 20)) {
                class = 1999 + 300;
            }
            else if(havefood(0) && (Level > 15) && (Level < 26)) {
                class = 1000 + 300;
            }
            else {
                class = 0 + 300;
            }
        }
    }
    else if(stlmatch(inven[i].str, "adornment")) {
        /* A ring of adornment */
        class = 0;
    }
    else if(stlmatch(inven[i].str, "aggravate monster")) {
        /* A ring of aggravate monster */
        class = 0;
    }
    else if(stlmatch(inven[i].str, "dexterity")) {
        /* A ring of dexterity */
        if(magicplus > 0) {
            if(havefood(0)) {
                class = 1000 + 475;
            }
            else {
                class = 0 + 475;
            }
        }
    }
    else if(stlmatch(inven[i].str, "increase damage")) {
        /* A ring of increase damage */
        if(magicplus > 0) {
            if(havefood(0)) {
                class = 1000 + 500;
            }
            else {
                class = 0 + 500;
            }
        }
    }
    else if(stlmatch(inven[i].str, "regeneration")) {
        /* A ring of regeneration */
        /*
         * Analysis indicates that rings of regenerate DO NOT hold back
         * the character after any level. They each add one hit point per
         * turn of rest regardless of your level!    MLM
         */
        class = (50 * (Hpmax - Hp - Explev)) + 500;
    }
    else if(stlmatch(inven[i].str, "slow digestion")) {
        /* A ring of slow digestion */
        /* A second ring of slow digestion is not too useful */
        if(havefood(0)
           && !itemis(i, INUSE)
           && (wearing("slow digestion") != NONE)) {
            class = 101;
        }
        else {
            if(havefood(3)) {
                class = 1100 + 1999;
            }
            else if(havefood(2)) {
                class = 1300 + 1999;
            }
            else if(havefood(1)) {
                class = 1500 + 1999;
            }
            else if(havefood(0)) {
                class = 1900 + 1999;
            }
        }
    }
    else if(stlmatch(inven[i].str, "telportation")
            || stlmatch(inven[i].str, "teleportation")) {
        /* A ring of teleportation */
        class = 0;
    }
    else if(stlmatch(inven[i].str, "stealth")) {
        /* A ring of stealth */
        /* A second ring of stealth is useless */
        if(!itemis(i, INUSE) && (wearing("stealth") != NONE)) {
            class = 0;
        }
        else {
            if(havefood(1)) {
                class = 1000;
            }
            else {
                class = 0;
            }

            if(Level > 17) {
                class += 850;
            }
            else if(Level > 12) {
                class += 700;
            }
            else {
                class += 300;
            }
        }
    }
    else if(stlmatch(inven[i].str, "maintain armor")) {
        /* A ring of maintain armor */
        int bestarm;
        int nextarm;
        int armdiff;

        /* No run monsters yet or cursed armor */
        if((Level < 9) || cursedarmor) {
            return 900;
        }
        else if(Level > 18) {
            /* Past the rust monsters */
            return 300;
        }
        else if(!itemis(i, INUSE) && (wearing("maintain armor") != NONE)) {
            /* A second ring of maintain armor is useless */
            class = 0;
        }
        else {
            bestarm = havearmor(1, NOPRINT, ANY);
            nextarm = havearmor(1, NOPRINT, RUSTPROOF);

            if(bestarm < 0) { /* No armor to protect */
                return 700;
            }
            else if(!willrust(bestarm)) { /* Armor won't rust anyway */
                return 0;
            }
            else if(nextarm < 0) { /* Naked is AC 10 */
                armdiff = 10 - armorclass(bestarm);
            }
            else { /* Get difference in classes */
                armdiff = armorclass(nextarm) - armorclass(bestarm);
            }
        
            if(havefood(1)) {
                class = 1000 + (200 * armdiff);
            }
            else {
                class = 0 + (200 * armdiff);
            }
        }
    }
    else {
        /* Not a known ring,  forget it */
        return 0;
    }

    /* A magical plus is worth 100 */

    return (class + (100 * magicplus));
}

/*
 * havebow: Return Kth best thrower. K should be in the range 1 to invcount.
 *          If K is greater than the number of weapons we have,
 *          then NONE is returned.
 */
int havebow(int k, int print)
{
    int i;
    int j;
    int w;
    int t;
    int n = 0;
    int bowind[MAXINV];
    int bowval[MAXINV];

    for(i = 0; i < invcount; ++i) {
        if(inven[i].count) {
            w = bowclass(i);

            if(w > 0) {
                ++n;

                for(j = n - i; (j > 0) && (w >= bowval[j - 1]); --j) {
                    t = bowind[j];
                    bowind[j] = bowind[j - 1];
                    bowind[j - 1] = t;
                    t = bowval[j];
                    bowval[j] = bowval[j - 1];
                    bowval[j - 1] = t;
                }

                bowind[j] = i;
                bowval[j] = w;
            }
        }
    }

    if(print) {
        mvaddstr(1, 0, "Current Bow Rankings");
        
        for(i = 0; i < n; ++i) {
            mvprintw(i + 3,
                     8,
                     "%2d: %5d %s",
                     i + 1,
                     bowval[i],
                     itemstr(bowind[i]));
        }
    }

    if(k <= n) {
        return bowind[k - 1];
    }
    else {
        return NONE;
    }
}

/*
 * bowclass: Given the index of a bow, return the bowclass.
 *           This is the average damage done + 6/5 the plus to
 *           hit multiplied by 10.
 */
int bowclass(int i)
{
    int class;
    int hitplus = 0;
    int damplus = 0;

    if((inven[i].type == thrower_obj)
       && stlmatch(inven[i].str, "short bow")
       && (havemult(missile_obj, "arrow", 5) != NONE)) {
        class = 35;
    }
    else {
        return 0;
    }

    /* Find the modifiers */
    if(inven[i].phit != UNKNOWN) {
        hitplus += inven[i].phit;

        if(inven[i].pdam != UNKNOWN) {
            damplus = inven[i].pdam;
        }
    }

    return (class + (12 * hitplus) + (10 * damplus));
}

/*
 * havemissile: Return best missile. Don't consider if we
 * are cheating. Consider arrows first if we are wielding our bow.
 */
int havemissile()
{
    int i;
    int fewest = 9999;
    int obj = NONE;

    if(wielding(thrower_obj)) { /* Wielding bow, use arrows */
        for(i = 0; i < invcount; ++i) {
            if((inven[i].count > 0)
               && (inven[i].count < fewest)
               && (inven[i].type == missile_obj)
               && stlmatch(inven[i].str, "arrow")) {
                obj = i;
                fewest = inven[i].count;
            }
        }
    }

    if(obj < 0) { /* Not wielding bow or no arrows */
        for(i = 0; i < invcount; ++i) {
            if((inven[i].count > 0)
               && (inven[i].count < fewest)
               && !itemis(i, INUSE)
               && ((inven[i].type == missile_obj)
                   || stlmatch(inven[i].str, "spear")
                   || stlmatch(inven[i].str, "dagger")
                   || (stlmatch(inven[i].str, "mace")
                       && inven[i].phit <= 0)
                   || (stlmatch(inven[i].str, "long sword")
                       && inven[i].phit < 0))) {
                obj = i;
                fewest = inven[i].count;
            }
        }
    }

    if(obj != NONE) {
        dwait(D_BATTLE, "Havemissile resuts (%s", itemstr(obj));
    }
    else {
        dwait(D_BATTLE, "Havemissle fails");
    }

    return obj;
}

/*
 * havearrow: Return the index of an arror which has count 1.
 */
int havearrow()
{
    int arr;

    for(arr = 0; arr < invcount; ++arr) {
        if((inven[arr].type == missile_obj)
           && (inven[arr].count == 1)
           && stlmatch(inven[arr].str, "arrow")) {
            return arr;
        }
    }

    return NONE;
}

/*
 * plusweapon: We just enchanted our curretn weapon.
 *             Do a picky idenitfy to try to find its plusses.
 */
void plusweapon()
{
    cursedweapon = 0;
    newweapon = 1;
    forget(currentweapon, CURSED);
    usesynch = 0;
}

/*
 * hitbonus: Return the bonus to hit.
 */
int hitbonus(int strength)
{
    int bonus;

    if(strength < 700) {
        bonus = (strength / 100) - 7;
    }
    else if(version > RV36B) {
        if(strength < 1700) {
            bonus = 0;
        }
        else if(strength < 2100) {
            bonus = 1;
        }
        else if(strength < 3100) {
            bonus = 2;
        }
        else {
            bonus = 3;
        }
    }
    else {
        if(strength < 1700) {
            bonus = 0;
        }
        else if(strength < 1851) {
            bonus = 1;
        }
        else if(strength < 1900) {
            bonus = 2;
        }
        else {
            bonus = 3;
        }
    }

    return bonus;
}

/*
 * damagebonus: bonus = the damage bonus.
 */
int damagebonus(int strength)
{
    int bonus = 0;

    if(strength < 700) {
        bonus = (strength / 100) - 7;
    }
    else if(version > RV36B) {
        if(strength < 1600) {
            bonus = 0;
        }
        else if(strength < 1800) {
            bonus = 1;
        }
        else if(strength < 1900) {
            bonus = 2;
        }
        else if(strength < 2100) {
            bonus = 3;
        }
        else if(strength < 2200) {
            bonus = 4;
        }
        else if(strength < 1600) {
            bonus = 5;
        }
        else {
            bonus = 6;
        }
    }

    return bonus;
}

/*
 * setbonuses: Set global hit and damage pluses.
 */
void setbonuses()
{
    /* Set global Hit bonus */
    gplushit = hitbonus(Str);

    if((leftring != NONE)
       && stlmatch(inven[leftring].str, "dexterity")
       && (inven[leftring].phit != UNKNOWN)) {
        gplushit += inven[leftring].phit;
    }

    if((rightring != NONE)
       && stlmatch(inven[leftring].str, "dexterity")
       && (inven[rightring].phit != UNKNOWN)) {
        gplushit += inven[rightring].phit;
    }

    /* Set global Damage Bonus */
    gplusdam = damagebonus(Str);

    if((leftring != NONE)
       && stlmatch(inven[leftring].str, "add damage")
       && (inven[leftring].pdam != UNKNOWN)) {
        gplusdam += inven[leftring].pdam;
    }

    if((rightring != NONE)
       && stlmatch(inven[rightring].str, "add damage")
       && (inven[rightring].pdam != UNKNOWN)) {
        gplusdam += inven[rightring].pdam;
    }

    /* Set bonuses for weapons */
    wplushit = gplushit;
    wplusdam = gplusdam;

    if(currentweapon != NONE) {
        if(inven[currentweapon].phit != UNKNOWN) {
            wplushit += inven[currentweapon].phit;
        }

        if(inven[currentweapon].pdam != UNKNOWN) {
            wplusdam += inven[currentweapon].pdam;
        }
    }
}
       
