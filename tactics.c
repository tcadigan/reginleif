/*
 * tactics.c: Rog-O-Matic XIV (CMU) Tue Mar 19 21:52:37 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This file contains all of the 'medium level intelligence' of Rog-O-Matic.
 */
#include "tactics.h"

#include <ctype.h>
#include <curses.h>
#include <stdio.h>

#include "globals.h"
#include "install.h"
#include "types.h"

/*
 * handlearmor: This routine is called to determine whether we should
 * take off or put on armor.
 *
 * Current Strategy: Wear best armor on levels 1..7 or 19 on or
 *                   if protected or have maintain armor.
 *                   Wear 2nd best armor between levels 13..18.
 *                   Wear best leather armor between between levels 8..12
 *                   or 2nd best if no leather armor.  DR UTexas 12/15/83
 *
 * Not that leather armor does not rust.
 */
int handlearmor()
{
    int obj;

    /* Only check when armor status is different */
    if(!newarmor || cursedarmor) {
        return 0;
    }

    /*
     * Pick the armor we want to wear. If we are worried about rust monster
     * we wear the second best armor, but if we won't see any rust monsters,
     * if our armor is too good for a rust monster to hit it, or we have a
     * ring on maintain armor, then we should wear our best armor. On
     * levels 13-18 we wear our second best no matter what.
     */

    /* Get best armor */
    obj = havearmor(1, NOPRINT, ANY);

    if(version < RV52A) {
        if((Level > 8)
           && (Level < 19)
           && (wearing("maintain armor") == NONE)
           && willrust(obj)
           && itemis(obj, KNOWN)) {
            obj = NONE;
            
            if(Level < 13) {
                obj = havearmor(1, NOPRINT, RUSTPROOF);
            }
            
            if((Level < 13) && (obj == NONE)) {
                obj = havearmor(3, NOPRINT, ANY);
            }

            if(obj == NONE) {
                obj = havearmor(2, NOPRINT, ANY);
            }
        }
    }
    else {
        if((Level > 7)
           && (Level < 19)
           && (wearing("maintain armor") == NONE)
           && willrust(obj)
           && itemis(obj, KNOWN)) {
            obj = NONE;

            if(Level < 13) {
                obj = havearmor(1, NOPRINT, RUSTPROOF);
            }

            if((Level < 13) && (obj == NONE)) {
                obj = havearmor(3, NOPRINT, ANY);
            }

            if(obj == NONE) {
                obj = havearmor(2, NOPRINT, ANY);
            }
        }
    }

    /* If the new armor is really bad, then don't bother wearing any */
    if((obj != NONE) && (armorclass(obj) > 9) && itemis(obj, KNOWN)) {
        obj = NONE;
    }

    /* If we are wearing the right armor, then don't bother */
    if(obj == currentarmor) {
        newarmor = 0;
        
        return 0;
    }

    /* Debugging */
    dwait(D_PACK, "handlearmor: obj %d, currentarmor %d", obj, currentarmor);

    /* Take off the wrong armor */
    if((currentarmor != NONE) && takeoff()) {
        return 1;
    }

    /* Put on the right armor, avoid wearing cursed armor */
    if(obj != NONE) {
        return wear(obj);
    }

    /* If we have no armor, then forget it */
    newarmor = 0;

    return 0;
}

/*
 * handleweapon: wield our best weapon. Calls haveweapon.
 *
 * the current strategy is to wield the best weapon from haveweapon.
 */
int handleweapon()
{
    int obj;

    if((!newweapon || cursedweapon) && !wielding(thrower)) {
        return 0;
    }

    /* haveweapon(1) returns the index of the best weapon in the pack */
    obj = haveweapon(1, NOPRINT);

    if(obj == NONE) {
        return 0;
    }

    /* If we are not wielding our best wepaon, do so */
    if(obj == currentweapon) {
        newweapon = 0;

        return 0;
    }
    else if(obj != NONE) {
        return wield(obj);
    }
    else {
        newweapon = 0;

        return 0;
    }
}

/*
 * quaffpotion: check whether we should quaff a potion, and call
 * quaff if so. We quaff add strength, restore strength, healing,
 * extra healing, and raise level here. Potions of seeinvisible
 * are handled in 'fightinvisible'.
 *
 * If we are at or below the exp. level, then experiment with unknown potions.
 */
#define MAXSTR(version < RV52A ? 1900 : 3100)

int quaffpotion()
{
    int obj = NONE;
    int obj2 = NONE;

    /* Take advantage of double haste bug -- assures permanent haste */
    if(!doublehasted && (version < RV52A)) {
        if(hasted) {
            obj = havenamed(potion, "haste self");

            if((obj != NONE) && quaff(obj)) {
                return 1;
            }
        }
        else {
            obj = havemult(potion, "haste self", 2);

            if((obj != NONE) && quaff(obj)) {
                return 1;
            }
        }
    }

    /*
     * Can we use a gain strength to our advantage? Or a restore?
     * If we have a Gain Strength, or our strength is very bad,
     * then we quaff a Regain Strength.
     */

    if(Str == Strmax) {
        obj = havenamed(potion, "gain strength");

        if((obj != NONE) && quaff(obj)) {
            return 1;
        }
    }

    if((Str < 700)
       || ((Str != Strmax) && (havenamed(potion, "gain strength") != NONE))) {
        obj = havenamed(potion, "restore strength");

        if((obj != NONE) && quaff) {
            return 1;
        }
    }

    if((Str < 1600) || (Level > 12)) {
        obj = havemult(potion, "restore strength", 2);

        if((obj != NONE) && quaff(obj)) {
            return 1;
        }
    }

    /* Try to get unblinded by quaffing a potion */
    if(blinded) {
        obj = havenamed(potion, "healing");

        if((obj != NONE) && quaff(obj)) {
            return 1;
        }
        else {
            obj = havenamed(potion, "extra healing");

            if((obj != NONE) && quaff(obj)) {
                return 1;
            }
            else {
                obj = havenamed(potion, "see invisible");

                if((obj != NONE) && quaff(obj)) {
                    return 1;
                }
            }
        }
    }

    /* Try to get uncosmic by quaffing a potion */
    if(cosmic) {
        obj = havenamed(potion, "extra healing");

        if((obj != NONE) && quaff(obj)) {
            return 1;
        }
    }

    if(cosmic && (Str != Strmax)) {
        obj = havenamed(potion, "poison");

        if(obj != NONE) {
            if((wearing("sustain strength") != NONE)
               && quaff(obj)
               || findring("sustain strength")) {
                return 1;
            }
        }
    }

    /*
     * Quaff heading to raise our MaxHp
     * Wait for cosmic known to quaff extra healing.  DR, TG UTexas
     */
    if(Hp == Hpmax) {
        obj = havemult(potion, "healing", 2);

        if(obj != NONE) {
            obj = havenamed(potion, "healing");

            if((obj != NONE)
               || know("blindness")
               && (know("hallucination") || (version < RV53A))
               && (Level < 15)) {
                obj = havenamed("extra healing");

                if((obj != NONE) && quaff(obj)) {
                    return 1;
                }
            }
        }
        else {
            obj = havemult(potion, "extra healing", 2);

            if(obj != NONE) {
                obj = havenamed(potion, "healing");

                if((obj != NONE)
                   || know("blindness")
                   && (know("hallucination") || (version < RV53A))
                   && (Level < 15)) {
                    obj = havenamed("extra healing");

                    if((obj != NONE) && quaff(obj)) {
                        return 1;
                    }
                }
            }
            else {
                if(know("blindness")) {
                    obj = havenamed(potion, "healing");

                    if((obj != NONE)
                       || know(blindness)
                       && (know("hallucination") || (version < RV53A))
                       && (Level < 15)) {
                        obj = havenamed("extra healing");

                        if((obj != NONE) && quaff(obj)) {
                            return 1;
                        }
                    }
                }
            }
        }
    }

    /* Quaff a raise level potion? */
    if((Explev > 8) || (Level > 13)) {
        obj = havenamed(potion, "raise level");

        if((obj != NONE) && quaff(obj)) {
            return 1;
        }
    }

    /* Quaff an unknown potion? */
    if((Level >= (k_expr / 10))
       || (objcount >= maxobj)
       || (Str < 1000)
       || blinded) {
        obj = unknown(potion);

        if(obj != NONE) {
            obj2 = wearing("add strength");

            if((obj2 != NONE) && removering(obj2)) {
                return 1;
            }
            else if(wearing("sustain strength") == NONE) {
                obj2 = havenamed(ring, "sustain strength");

                if((obj2 != NONE) && puton(obj2)) {
                    return 1;
                }
            }
            else if(quaff(obj)) {
                return 1;
            }
        }
    }

    return 0;
}

/*
 * readscroll: Check whether we should read a scroll, and call reads
 * to actually read it.
 *
 * Scrolls of identify, remove curse, genocide, enchant weapon,
 * enchant armor, magic mapping are define. Scrolls of scare
 * monster and confuse monster are handled in 'battlestations'.
 *
 * If we are at or below (k_exper / 10), experiment with unknown scrolls.
 * Make certain that we are wearing our best armor when reading
 * enchant armor or an unknown scroll (which could be enchant
 * armor).
 */
int readscroll()
{
    int obj;
    int obj2;

    /* check the item specific identify scrolls first */
    obj = havenamed(scroll, "identify armor");

    if(obj != NONE) {
        obj2 = unknown(armor);

        if(obj2 != NONE) {
            prepareident(obj2, obj);
            
            return reads(obj);
        }
    }

    obj = havenamed(scroll,"identify weapon");

    if(obj != NONE) {
        obj2 = unknown(hitter);

        if(obj2 != NONE) {
            prepareident(obj2, obj);

            return reads(obj);
        }
    }

    obj = havenamed(scroll, "identify potion");
    
    if(obj != NONE) {
        obj2 = unknown(potion);

        if(obj2 != NONE) {
            prepareident(obj2, obj);

            return reads(obj);
        }
    }

    obj = havenamed(scroll, "identify scroll");

    if(obj != NONE) {
        obj2 = unknown(scroll);

        if(obj2 != NONE) {
            prepareident(obj2, obj);

            return reads(obj);
        }
    }

    obj = havenamed(scroll, "identify ring, wand or staff");

    if(obj != NONE) {
        obj2 = unknown(ring);

        if(obj2 != NONE) {
            prepareident(obj2, obj);

            return reads(obj);
        }

        obj2 = unknown(wand);

        if(obj2 != NONE) {
            prepareident(obj2, obj);

            return reads(obj);
        }
    }

    /* In older version, have multiple uses for generic identify scrolls */
    obj = havenamed(scroll, "identify");

    if((obj != NONE)
       && (currentweapon != NONE)
       && (!itemis(currentweapon, KNOWN)
           && (!usingarrow || (goodarrow > 20)))) {
        prepareident(obj2, obj);
        
        return reads(obj);
    }

    if(cursedarmor || cursedweapon) {
        obj = havenamed(scroll, "remove curse");

        if(obj != NONE) {
            return reads(obj);
        }
    }

    obj = havenamed(scroll, "genocide");

    if(obj != NONE) {
        return reads(obj);
    }

    if((currentweapon != NONE)
       && (goodweapon || usingarrow || (MaxLevel > 12))) {
        obj = havenamed(scroll, "enchant weapon");

        if(obj != NONE) {
            return reads(obj);
        }
    }

    if((Level != didreadmap) && (Level > 12)) {
        obj = havenamed(scroll, "magic mapping");

        if(obj != NONE) {
            return reads(obj);
        }
    }

    /*
     * About to read an unknown scroll. We will assure that we have
     * a weapon in hand, and put on our best armor for the occasion
     * We must also prepare to identify something, just in case.
     */
    obj = havenamed(scroll, "enchant armor");

    if((obj != NONE)
       || ((currentweapon != NONE)
           && ((Level >= (k_exper / 10))
               || (objcount >= maxobj)
               || cursedarmor
               || cursedweapon)
           && (exploredlevel
               || (Level > 18)
               || know("aggravate monsters")))) {
        obj = unknown(scroll);

        if(obj != NONE) {
            prepareident(pickident(), obj);

            /* Go to a corner to read the scroll */
            if((version <= RV36B) && !know("create monster") && gotocorner()) {
                return 1;
            }

            /* Must put on our good armor first */
            if(!cursedarmor
               && (!know("enchant armor")
                   || stlmatch(inven[obj].str, "enchant armor")
                   || !know("protect armor")
                   || strlmatch(inven[obj].str, "protect armor"))) {
                /* Pick our best armor */
                int obj 2 = havearmor(1, NOPRINT, ANY);

                if(obj2 == currentarmor) {
                }
                else if((currentarmor != NONE) && takeoff()) {
                    /* Take off the bad stuff */
                    return 1;
                }
                else if((obj2 != NONE) && wear(obj2)) {
                    /* Put on the good stuff */
                    return 1;
                }
            }

            /* No armor handling, so read the scroll */
            return reads(obj);
        }
    }
    else {
        obj = havenamed(scroll, "protect armor");

        if((obj != NONE)
           || ((currentweapon != NONE)
               && ((Level >= (k_exper / 10))
                   || (objcount >= maxobj)
                   || cursedarmor
                   || cursedweapon)
               && (exploredlevel
                   || (Level > 18)
                   || know("aggravate monsters")))) {
            obj = unknown(scroll);

            if(obj != NONE) {
                prepareident(pickident(), obj);

                /* Got to a corner to read the scroll */
                if((version <= RV36B)
                   && !know("create monster")
                   && gotocorner()) {
                    return 1;
                }

                /* Must put on our good armor first */
                if(!cursedarmor
                   && (!know("enchant armor")
                       || stlmatch(inven[obj].str, "enchant armor")
                       || !know("protect armor")
                       || stlmatch(inven[obj].str, "protect armor"))) {
                    /* Pick our best armor */
                    int obj2 = havearmor(1, NOPRINT, ANY);

                    if(obj2 == currentarmor) {
                    }
                    else if((currentarmor != NONE) && takeoff()) {
                        /* Take off the bad stuff */
                        return 1;
                    }
                    else if((obj2 != NONE) && wear(obj2)) {
                        /* Put on the good stuff */
                        return 1;
                    }
                }

                /* No armor handling, so read the scroll */
                return reads(obj);
            }
        }
    }

    return 0;
}

/*
 * handlering: Check whether we should put on a ring, and call
 * puton to wear it. Called 'havering' to find the two best rings
 * and wears them if their evaluations are greater than 1000.
 *
 * 'havering' understands about when different rings are good, and how
 * much food we need to use each ring.
 */
int handlering()
{
    int ring1;
    int ring2;

    if(!newring && !beingstalked) {
        return 0;
    }

    ring1 = havering(1, NOPRINT);
    ring2 = havering(2, NOPRINT);
    
    dwait(D_PACK,
          "Handlering: ring1 %d, ring2 %d, left %d, right %d",
          ring1,
          ring2,
          leftring,
          rightring);

    if(((leftring == ring1) && (rightring == ring2))
       || ((rightring == ring1) && (leftring == ring2))) {
        newring = 0;
        
        return 0;
    }

    if((leftring != NONE)
       && (leftring != ring1)
       && (leftring != ring2)
       && removering(leftring)) {
        return 1;
    }

    if((rightring != NONE)
       && (rightring != ring1)
       && (rightring != ring2)
       && removering(rightring)) {
        return 1;
    }

    if((ring1 != leftring) && (ring1 != rightring) && puton(ring1)) {
        return 1;
    }

    if((ring2 != leftring) && (ring2 != rightring) && puton(ring2)) {
        return 1;
    }

    return 0;
}

/*
 * findring: Called with the name of a ring, attempts to locate such
 * a ring in the pack and wear it. It will remove rings (other than
 * maintain armor) to accomplish this task if we are wearing two rings.
 *
 * Could be extended to have an ordering of the rings to wear.
 */
int findring(char *name)
{
    int obj;

    obj = havenamed(ring, name);

    if((obj == NONE) || (wearing(name) != NONE)) {
        return 0;
    }

    if((leftring != NONE) && (rightring != NONE)) {
        if(stlmatch(inven[leftring].str, "maintain armor")) {
            return removering(rightring);
        }
        else {
            return removering(leftring);
        }
    }

    return puton(obj);
}

/*
 * grope: Get to a safe square and set and vibrate (move back and forth)
 * and then sleep for 'turns' turns.
 *
 * Problem we need to know which side of us the monster is on. Then
 * we could zap him with wands or staves. This requires some kind of
 * memory and the ability to detect when the motion command (i.e. 'hit'
 * failed to move us).    MLM
 */
int grope(int turns)
{
    int k;
    int moves;

    if((atrow < 2) || (atcol < 1)) {
        if(turns > 0) {
            command(T_GROPING, "%ds", turns);
        }
        else {
            command(T_GROPING, "%ds", 1);
        }
         
        return 1;
    }

    /* Count adjacent CANGO sqaures */
    moves = 0;

    for(k = 0; k < 8; ++k) {
        if(onrc(CANGO, atdrow(k), atdcol(k))) {
            ++moves;
        }
    }

    /* Find a spot with 2 or fewer moves */
    if((moves > 2) && findsafe()) {
        return 1;
    }

    /* blinddir is direction of adjacent CANGO square which is not a trap */
    for(k = 0; k < 4; ++k) {
        if(onrc(CANGO | TRAP, atdrow(blindir), atdcol(blindir)) == CANGO) {
            break;
        }

        blindir = (blindir + 2) % 8;
    }

    if(turns) {
        command(T_GROPING, 
                "%c%c%ds",
                keydir[blindir],
                keydir[(blindir + 4) & 7],
                turns);
    }
    else {
        command(T_GROPING, "%c%c", keydir[blindir], keydir[(blindir + 4) & 7]);
    }

    blindir = (blindir + 2) % 8;

    return 1;
}

/*
 * findarrow: This function tries to run over an arrow trap to get a
 *            magic arrow. Make certain we have some food.
 */
int findarrow()
{
    /* If wrong version, not cheating or must go find food, then forget it */
    if((version > RV36B) || !cheat || hungry()) {
        return 0;
    }
    else if(!usingarrow
            && foundarrowtrap
            && !on(ARROW)
            && gotowards(trapr, trapc, 0)) {
        display("Trying for arrow...");

        return 1;
    }

    return 0;
}

/*
 * checkcango: Verify that a missile fired in direction 'dir' will
 *             travel ;turns' turns.
 *
 * Modified by mlm, 5/31/83: Return false if a monster is in the way.
 * only return true if the missile will travel EXACTLY the distance
 * specified. Also changed it to not check the current square (since
 * we can fire from a door, even if we can't shoot through one).
 */

int checkcango(int dir, int turns)
{
    dr = detlr[dir];
    dc = deltc[dir];
    r = atrow + dr;
    c = atcol + dc;

    while((turns > 0) && (onrc(CANGO | DOOR, r, c) == CANGO)) {
        r += dr;
        c += dc;
        --turns;
    }

    if(turns == 0) {
        return 1;
    }
    else {
        return 0;
    }
}

/*
 * godownstairs: Issues a down command and check for the halftimeshow.
 */

/*
 * running: True ==> Don't do anything fancy
 */
int godownstairs(int running)
{
    int p;
    int genericinit();
    int downvalue();

    /*
     * We don't want to go down if we have just gotten an arrow, since
     * it is probably bac, and we will want to go back to the trap;
     * Don't go down until we have killed five monsters in one blow.
     * While waiting, run back and forth to look for monsters.
     */
    if(cheat
       && (version <= RV36B)
       && !running
       && foundarrowtrap
       && usingarrow
       && (have(food) != NONE)
       && (goodarrow < 5)
       && waitaround()) {
        saynow("Checking out arrow...");
        
        return 1;
    }

    /* Check for applicability of this rule */
    if(!new_stairs) {
        return 0;
    }

    /* If we are on the stairs perhaps we should rest up some */
    p = between((Explev + larder) & 10, 60, 100);

    if((atrow == stairrow)
       && (atcol == staircol)
       && !running
       && (larder > 0)
       && (Hp < max(10, percent(Hpmax, p)))) {
        command(T_RESTING, "s");
        display("Resting on stairs before next level");

        return 1;
    }

    /* Allow other rules a chance to notice that we are done with the level */
    if(on(STAIRS) && !exploredlevel) {
        exploredlevel = 1;

        return 1;
    }

    /* If we are floating, we can't go down, either rest of fail */
    if(floating && running) {
        saynow("Cannot escape, floating in mid-air!");

        return 0;
    }
    else if(floating) {
        saynow("Floating above stairs...");
        command(T_RESTING, "s");

        return 1;
    }

    /* If we are on the stairs, go down */
    if(on(STAIRS)) {
        halftimeshow(Level);

        /* Start logging at level GOODGAME, if we aren't already */
        if((Level > (GOODGAME - 2)) && !replaying && !logging) {
            toggleecho();
        }

        /* Send the DOWN command and return */
        command(T_MOVING, ">");

        return 1;
    }

    /* If we are running and can run to the next level, do that */
    if(running && makemove(RUNDOWN, genericinit, downvalue, REEVAL)) {
        return 1;
    }

    /* If we see the stairs or a trap door, go there */
    if(!running && makemove(DOWNMOVE, genericinit, downvalue, REUSE)) {
        /* Set a goal (CPU time hack) */
        goalr = targetrow;
        goalc = targetcol;

        return 1;
    }

    newstairs = 0;

    return 0;
}

/*
 * plunge: Shoudl we head down immediately?
 *
 * If we are being teleported too much or
 *    we are on a bad level (19 to 25) or
 *    we want to get past Rust Monsters (level 18) or
 *    we have aggravated all of the monsters then
 *
 * we head down immediately.
 */
int plunge()
{
    /* Check for applicability of this rule */
    if((stairrow == NONE) && !foundtrapdoor) {
        return 0;
    }

    if(have(amulet) != NONE) {
        return 0;
    }

    if((teleported > ((larder + 1) * 5)) && godownstairs(NOTRUNNING)) {
        if(!on(STAIRS)) {
            saynow("Giving up on level, too much teleporting");
        }

        return 1;
    }

    if((Level > 17) && (Level < 26) && godownstairs(NOTRUNNING)) {
        if(!on(STAIRS)) {
            saynow("Plunge mode!!!");
        }

        return 1;
    }

    if(aggravated && godownstairs(NOTRUNNING)) {
        if(!on(STAIRS)) {
            saynow("Running from aggravated monsters");
        }

        return 1;
    }

    if(haveexplored(9) && godownstairs(NOTRUNNING)) {
        if(!on(STAIRS)) {
            saynow("Level explored");
        }

        return 1;
    }

    return 0;
}

/*
 * waitaround: Hang around here waiting for monsters.
 */
struct box {
    int vertstart;
    int vertend;
    int vertdelt;
    int horstart;
    int horend;
    int hordelt;
};

static struct box cb[4] = {
    {  3, 21,  1,  1, 78,  1 }, /* Top left corner */
    {  3, 12,  1, 78,  1, -1 }, /* Top right corner */
    { 21,  3, -1, 78,  1, -1 }, /* Bottom right corner */
    { 21,  3, -1,  1, 78,  1 }  /* Bottom left corner */
};

/* Goal corner from 0..3 */
static int gc = 0;

/*
 * waitaround: Fro some reason we want to stay on this level for a while.
 * Try running to each corner of the level.
 */
int waitaround()
{
    int i;
    int j;

    if(gotowardsgoal()) {
        return 1;
    }

    ++gc;
    gc = gc % 4;

    for(i = cb[gc].verstart; i != cb[gc].vertend; i += cb[gc]vert.delt) {
        for(j = cb[gc].horstart; j != cb[gc].horend; j += cb[gc].hordelt) {
            if(onrc(BEEN | CANGO | ROOM, i, j)
               && !onrc(TRAP, i, j)
               && gotowards(i, j, 0)) {
                goalr = i;
                goalc = j;
                
                return 1;
            }
        }
    }

    return 0;
}

/*
 * goupstairs: If we have the amulet, and our score is good enough, then
 *             go up stairs. This function also checks for the end of the
 *             game, and issues the proper calls to get the score written.
 */
int goupstairs(int running)
{
    int obj;

    /* Check for applicability of this rule */
    if((stairrow == NONE)
       || (have(amulet) == NONE)
       || (!running && (quitat < BOGUS) && (Gold <= quitat))) {
        return 0;
    }

    /* If we are on the stairs, then check for win, else go up */
    if((atrow == stairrow) && (atcol == staircol)) {
        /* If we are about to win, dump any magic arrows or minus things */
        if(Level == 1) {
            obj = havearrow();

            if((obj != NONE) && throw(obj, 0)) {
                return 1;
            }
            else {
                obj = haveminus();

                if((obj != NONE) && throw(obj, 0)) {
                    return 1;
                }
            }
        }

        /* No magic arrows, time to leave */
        if(Level == 1) {
            /* Send an up command and a space to clear the 'You Made It' */
            sendnow("< ");

            /*
             * Now read char until we have the end of the inventory.
             * Not misspelling in Rogue 'Peices', so don't assume anything
             */
            waitfor("Gold P");

            /* Note that quitrogue sends a '\n' to get the score */
            quitrogue("total winner", Gold, 0);

            return 1;
        }
        else {
            /* Not at the top yet, keep on trucking */
            command(T_MOVING, "<");

            return 1;
        }
    }
    else {
        /* If we know the stairs are, go there */
        goalr = stairrow;

        if(goalr > 0) {
            goalc = staircol;

            if((goalc > 0) && gotowards(goalr, goalc, running)) {
                return 1;
            }
        }
    }

    return 0;
}

/*
 * restup: If we are long on hit points, sit for a while. Since handlering
 * was called first, we will be wearing a ring of regeneration if need be.
 *
 * First we find a good place to rest (we will move into a room, but not
 * out of one). In lit rooms, stand far from doors so we can shoot
 * arros at things coming in. In dark rooms, stand diagonally away
 * from doors (so we get a one turn warning of monsters coming in that
 * door). In either case, stand on stairs or next to trap doors and
 * teleport traps).
 *
 * Then rest by searching 's'. If one blow would not kill us, and we
 * don't blan to shoot arrows, then rest up so as to heal one hit point.
 * If we are critically low, rest up one turn at a time.
 *
 * Other consideration: Don't move if confused or cosmic.
 *                      Drink healing potions if really low.
 *                      Don't rest when hungry (and no food).
 */
int restup()
{
    int obj;
    int turns;

    /* If we are confused, sit still so we don't bump into anything bad */
    if(confused) {
        command(T_RESTING, "s");
        
        return 1;
    }

    /* If cosmic and plenty of hit points and food, rest for long periods */
    if(cosmic && ((Hp >= percent(Hpmax, 80)) && (larder > 2))) {
        display("Oh wow man, I'm ocntemplating my navel!") ;
        command(T_RESTING, "100s");

        return 1;
    }

    /* If we are well, return */
    if(Hp >= max(8, percent(Hpmax, between((Explev * 10) + k_rest - 50, 40, 80)))) {
        unrest();

        return 0;
    }

    /*
     * If we are really ill then try a healing potion (save a healing
     * potion for blindness, extra healing for hallucination).
     */
    if((Hp < (Level + 10)) && (Hp < (Hpmax / 3))) {
        obj = havemult(potion, "extra healing", 2);
        
        if((obj != NONE) && quaff(obj)) {
            return 1;
        }

        obj = havemult(potion, "healing", 2);

        if((obj != NONE) && quaff(obj)) {
            return 1;
        }

        if(know("hallucination")) {
            obj = havenamed(potion, "extra healing");
            
            if((obj != NONE) && quaff(obj)) {
                return 1;
            }
        }

        if(know("blindness")) {
            obj = havenamed(option, "healing");
            
            if((obj != NONE) && quaff(obj)) {
                return 1;
            }
        }
    }

    /* Don't rest when we haven't enough to eat */
    if(hungry()) {
        return 0;
    }

    display("Resting up...");

    /* Look for a good place to rest */
    if(movetorest()) {
        return 1;
    }

    /*
     * If we are very ill, or we are very deep, or we are in a lit room
     * an can shoot at things as they come at us, rest only one turn so
     * monsters don't get the first shot. Otherwise rest enough turns
     * to heal one step.
     */
    if(Level < 8) {
        turns = 20 - (Explev * 2);
    }
    else {
        turns = 3;
    }

    if((!darkroom() && ammo) || (Hp < ((Level * 2) + 8)) || (Level > 15)) {
        turns = 1;
    }

    command(T_RESTING, "%ds", turns);

    return 1;
}

/*
 * If goalr and goalc are set (not -1,-1) then attempt to move towards
 * that square. Called gotowards which calls bfsearch.
 */
int gotowardsgoal()
{
    if((goalr > 0) && (golarc > 0)) { /* Keep on trucking */
        if((goalr == atrow) && (goalc == atcol)) {
            goalr = NONE;
            goalc = NONE;
        }
        else if(gotowards(goalr, goalc, 0)) {
            return 1;
        }
        else {
            goalr = NONE;
            goalc = NONE;
        }
    }

    return 0;
};

/*
 * gotocorner: Find a corner using downright and try to go there.
 * this is done so we can destroy old wands by throwing 
 * them into the corner (which destroys them).
 */
int gotocorner()
{
    int r;
    int c;

    if(!downright(&r, &c)) {
        return 0;
    }

    if(debug(D_SCREEN)) {
        saynow("Gotocorner called:");
        mvaddch(r, c, 'T');
        at(row, col);
    }

    if(gotwards(r, c, 0)) {
        goalr = r;
        goalc = c;

        return 1;
    }

    return 0;
}

/*
 * lightroom: Try to light up the room if we are below level 17.
 */
int light()
{
    if(Level < 17) {
        return 0;
    }
    
    return lightroom();
}

/*
 * shootindark: If we are arching at an old monster, fire another arrow.
 */
int shootindark()
{
    int obj;
    int bow;

    /* If no longer arching in the dark, fail */
    if((darkturns < 1) || (darkdir == NONE) || !darkroom()) {
        return 0;
    }

    /* Count off turns till he reaches us */
    --darkturns;

    /* If he is one turn away, switch back to our sword */
    if(!cursedweapon
       && wielding(thrower)
       && (darkturns == 0)
       && handleweapon()) {
        dwait(D_BATTLE, "Switching to sword [4]");

        return 1;
    }

    /* If we have room, switch to our bow */
    if(!cursedweapon && !wielding(thrower) && (darkturns > 3)) {
        bow = havebow(1, NOPRINT);

        if((bow != NONE) && wield(bow)) {
            return 1;
        }
    }

    /* Fail if we have run out of arrows */
    obj = havemissile();

    if(obj == NONE) {
        return 0;
    }

    /* Throw the arrow in the arching direction */
    return throw(obj, darkdir);
}

/*
 * dinnertime: Eat if we are hungry or if we have a surplus of food
 */
int dinnertime()
{
    if((havefood(5) && (objcount == maxobj) && !droppedscare)
       || ((larder > 0) && hungry())) {
        return eat();
    }

    return 0;
}

/*
 * trywand: Zap a blank wall with an unknown and unused want in an attempt
 * to generate a message which identifies the wand.
 */
int trywand()
{
    int obj;
    int dir;
    int r;
    int c;
    int count;

    /*
     * If we aren't in a room, if there are monsters around,
     * or we are in the dark, then we can't try this strategy 
     */
    if(!on(ROOM) || mlistlen || darkroom()) {
        return 0;
    }

    /* Have a wand to identify? */
    for(dir = 0; dir < 8; dir += 2) {
        count = 0;
        r = atrow;
        c = atcol;

        while(onrc(CANGO | DOOR, r, c) == CANGO) {
            ++count;

            r += deltr[dir];
            c += deltc[dir];
        }

        /* Found a likely wall */
        if((count == 4) || (count == 5)) {
            break;
        }
    }

    /* If we couldn't find room, then fail */
    if(dir > 7) {
        return 0;
    }

    /* Set to do a reset inventory (usesynch) and point the wand */
    usesynch = 0;

    return point(obj, dir);
}

/*
 * eat: If we have food, eat it.
 */
int eat()
{
    int obj;

    obj = have(food);

    if(obj != NONE) {
        command(T_HANDLING, "e%c", LETTER(obj));
        
        return 1;
    }

    return 0;
}
