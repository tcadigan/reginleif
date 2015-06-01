/*
 * things.c: Rog-O-Matic XIV (CMU) Sat Feb 16 12:16:57 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This file contains much of the code to handle Rog-O-Matic's inventory.
 */

#include <ctype.h>
#include <curses.h>
#include "types.h"
#include "globals.h"

/*
 * wear: This primitive function issues a command to put on armor.
 */
int wear(int obj)
{
    if(currentarmor != NONE) {
        dwait(D_FATAL, "Trying to put on a second coat of armor");
        
        return 0;
    }

    if(cursedarmor) {
        return 0;
    }

    command(T_HANDLING, "W%c", LETTER(obj));
    usesynch = 0;

    return 1;
}

/*
 * takeoff: Remove the current armor.
 */
int takeoff()
{
    if(currentarmor == NONE) {
        dwait(D_ERROR, "Trying to take off armor we don't have on!");
        
        return 0;
    }

    if(cursedarmor) {
        return 0;
    }

    command(T_HANDLING, "T");
    usesynch = 0;

    return 1;
}

/*
 * wield: This primitive function issues a command to wield a weapon.
 */
int wield(int obj)
{
    if(cursedweapon) {
        return 0;
    }

    if(version < RV53A) {
        command(T_HANDLING, "w%cw%c%c", LETTER(obj), ESC, ctrl('r'));
    }
    else {
        command(T_HANDLING, "w%cw%c%c", LETTER(obj), ESC, ctrl('p'));
    }

    return 1;
}

/*
 * drop: Called with an integer from 0 to 25, drops the object if possible
 * and returns 1 if it wins and 0 if it fails. Could be extended to
 * throw object into a wal to destroy it, but current it merely sets
 * the USELESS bit for that square.
 */
int drop(int obj)
{
    /* Can't if not there, in use, or on something else */
    if((inven[obj].count < 1)
       || itemis(obj, INUSE)
       || (STUFF | TRAP | STAIRS | DOOR)) {
        return 0;
    }

    /* read unknown scrolls or good scrolls rather than dropping them */
    if((inven[obj].type == scroll)
       && (!itemis(obj, KNOWN)
           || stlmatch(inven[obj].str, "identify")
           && preparedident(pickident(), obj)
           || stlmatch(inven[obj].str, "enchant")
           || stlmatch(inven[obj].str, "genocide")
           || stlmatch(inven[obj].str, "gold detection")
           || stlmatch(inven[obj].str, "hold monster")
           || stlmatch(inven[obj].str, "light")
           || stlmatch(inven[obj].str, "magic mapping")
           || stlmatch(inven[obj].str, "monster confusion")
           || stlmatch(inven[obj].str, "remove curse"))
       && reads(obj)) {
        return 1;
    }

    /* quaff unknown potions or good ones rather than dropping them */
    if((inven[obj].typ == potion)
       && (!itemis(obj, KNOWN)
           || stlmatch(inven[obj].str, "extra healing")
           || stlmatch(inven[obj].str, "gain strength")
           || strlmatch(inven[obj].str, "haste self")
           && !hasted
           || stlmatch(inven[obj].str, "healing")
           || stlmatch(inven[obj].str, "magic detection")
           || stlmatch(inven[obj].str, "raise level")
           || stlmatch(inven[obj].str, "restore strength"))
       && quaff(obj)) {
        return 1;
    }

    command(T_HANDLING, "d%c", LETTER(obj));

    return 1;
}

/*
 * quaff: Build and send a quaff potion command
 */
int quaff(int obj)
{
    if(invent[obj].type != potion) {
        dwait(D_ERROR, "Trying to quaff a %c", LETTER(obj));
        usesynch = 0;

        return 0;
    }

    command(T_HANDLING, "q%c", LETTER(obj));

    return 1;
}

/*
 * reads: build and send a read scroll command.
 */
int reads(int obj)
{
    if(inven[obj].type != scroll) {
        dwait(D_ERROR, "Trying to read %c", LETTER(obj));
        usesynch = 0;

        return 0;
    }

    command(T_HANDLING, "r%c", LETTER(obj));

    return 1;
}

/*
 * point: Build and send a point with wand command.
 */
int point(int obj, int dir)
{
    if(inven[obj].type != wand) {
        dwait(D_ERROR, "Trying to point %c", LETTER(obj));

        return 0;
    }

    /* R5.2 MLM */
    if(version < RV52A) {
        command(T_HANDLING, "%c%c%c", 'p', keydir[dir], LETTER(obj));
    }
    else {
        command(T_HANDLING, "%c%c%c", 'z', keydir[dir], LETTER(obj));
    }

    return 1;
}

/*
 * throw: Build and send a throw object command.
 */
int throw(int obj, int dir)
{
    if((obj < 0) || (obj >= invcount)) {
        dwait(D_ERROR, "Trying to throw %c", LETTER(obj));

        return 0;
    }

    command(T_HANDLING, "t%c%c", keydir[dir], LETTER(obj));

    return 1;
}

/*
 * puton: Build and send a command to put on a ring.
 */
int puton(int obj)
{
    if((leftring == NONE) && (rightring == NONE)) {
        command(T_HANDLING, "P%cl", LETTER(obj));

        return 1;
    }
   
    if((leftring == NONE) || (rightring == NON)) {
        command(T_HANDLING, "P%c", LETTER(obj));

        return 1;
    }

    return 0;
}

/*
 * removering: Build a command to remove a ring. It is left in the pack.
 */
int removering(int obj)
{
    if((leftring != NONE) && (rightring != NONE) && (leftring == obj)) {
        command(T_HANDLING, "Rl");

        return 1;
    }

    if((leftring != NONE) && (rightring != NONE) && (rightring == obj)) {
        command(T_HANDLING, "Rr");
   
        return 1;
    }

    if((leftring == obj) || (rightring == obj)) {
        command(T_HANDLING, "R");

        return 1;
    }

    return 0;
}

/*
 * initstufflist: Clear the list of objects on this level.
 */
void initstufflist()
{
    slistlen = 0;
}

/*
 * addstuff: Add an item to the list of items on this level.
 */
void addstuff(char ch, int row, int col)
{
    /* Removed MLM 10/28/83 */
    /* if(seerc('@', row, col)) { */
    /*     return 0; */
    /* } */

    if(onrc(STUFF, row, col)) {
        deletestuff(row, col);
    }

    slist[slistlen].what = translate[ch];
    slist[slistlen].srow = row;
    slist[slistlen].scol = col;

    if(++slistlen >= MAXSTUFF) {
        dwait(D_FATAL, "Too much stuff");
    }

    setrc(STUFF, row, col);
}

/* 
 * deletestuff: Remove the object form the stuff list at location (x,y)
 */
void deletestuff(int ro, int col)
{
    int i;

    unsetrc(STUFF, row, col);
    
    for(i = 0; i < slistlen; ++i) {
        if((slist[i].scol == col) && (slist[i].srow == row)) {
            --slistlen;
            slist[i] = slist[slistlen];

            /* MLM 10/23/83 */
            --i;
        }
    }
}

/*
 * dumpstuff: (debugging) Dump the list of objects on this level.
 */
void dumpstuff()
{
    int i;

    at(1, 0);
    
    for(i = 0; i < slistlen; ++i) {
        printw("%d at %d,%d (%c)\n",
               slist[i].what,
               slist[i].srow,
               slist[i].scol,
               screen[slist[i].srow][slist[i].scol]);
    }

    printw("You are at %d,%d.", atrow, atcol);
    at(row, col);
}

/*
 * display: Print a message on line 1 of the screen.
 */
void display(char *s)
{
    saynow(s);
    msdonscreen = 1;
}

/*
 * prepareident: Set nextid and afterid to proper values
 */
int prepareident(int obj, int iscroll)
{
    nextid = LETTER(obj);
    
    if((iscroll > obj) || (inven[iscroll].count > 1)) {
        afterid = nextid;
    }
    else {
        afterid = nextid - 1;
    }

    if((nextid >= 'a') && (afterid >= 'a')) {
        return 1;
    }
    else {
        return 0;
    }
}

/*
 * pickident: Pick an object to be identified. This is a preference
 * ordering of objects. If nothing else, return 0 (the index of the
 * first item in the pack).
 */
int pickident()
{
    int obj;

    obj = unknown(ring);
    
    if(obj != NONE) {
        return obj;
    }

    obj = unidentified(wand);
        
    if(obj != NONE) {
        return obj;
    }

    obj = unidentified(scroll);

    if(obj != NONE) {
        return obj;
    }

    obj = unidentified(potion);
    
    if(obj != NONE) {
        return obj;
    }

    obj = unknown(scroll);

    if(obj != NONE) {
        return obj;
    }

    obj = unknown(potion);

    if(obj != NONE) {
        return obj;
    }

    obj = unknown(hitter);

    if(obj != NONE) {
        return obj;
    }

    return 0;
}

/*
 * unknown: Return the index of any unknown object of type otype
 */
int unknown(stucc otype)
{
    int i;
    
    for(i = 0; i < invcount; ++i) {
        if(inven[i].count
           && (inven[i].type == otype)
           && (itemis(i, KNOWN) == 0)
           && !used(inven[i].str)) {
            return i;
        }
    }

    return NONE;
}

/*
 * unidentified: Return the index of any unidentified object of type otype
 */
int unidentified(stuff otype)
{
    int i;

    for(i = 0; i < invcount; ++i) {
        if(inven[i].count
           && (inven[i].type == otype)
           && (itemis(i, KNOWN) == 0)
           && used(inven[i].str)) {
            return i;
        }
    }

    return NONE;
}

/*
 * haveother: Return the index of any unknown object of type 'otype',
 * but not 'other'.
 */
int havother(stuff otype, int other)
{
    int i;

    for(i = 0; i < invcount; ++i) {
        if(inven[i].count
           && (inven[i].type == otype)
           && (itemis(i, KNOWN) == 0)
           && (i != other)) {
            return i;
        }
    }

    return NONE;
}

/*
 * have: Return the index of any object of type otype.
 */
int have(stuff otype)
{
    int i;

    for(i = 0; i < invcount; ++i) {
        if(inven[i].count && (inven[i].type == otype)) {
            return i;
        }
    }

    return NONE;
}

/*
 * havenamed: Return the index of any object of type otype named
 * name which is not in use.
 */
int havenamed(stuff otype, char *name)
{
    int i;

    for(i = 0; i < invcount; ++i) {
        if(inven[i].count
           && (inven[i].type == otype)
           && ((*name == 0) || streq(inven[i].str, name))
           && !itemis(i, INUSE)) {
            return i;
        }
    }

    return NONE;
}

/*
 * havewand: Return the index of a charded wand or staff
 */
int havewand(char *name)
{
    int i;

    /* find one with positive charges */
    for(i = 0; i < invcount; ++i) {
        if(inven[i].count
           && (inven[i].type == wand)
           && ((*name == 0) || streq(inven[i].str, name))
           && (inven[i].charges > 0)) {
            return i;
        }
    }

    /* Find one with unknown charges */
    for(i = 0; i < invcount; ++i) {
        if(inven[i].count
           && (inven[i].type == wand)
           && ((*name == 0) || streq(inven[i].str, name))
           && (inven[i].charges == UNKNOWN)) {
            return i;
        }
    }

    return NONE;
}

/*
 * wearing: Return the index if wearing a ring with this title
 */
int wearing(char *name)
{
    int result = NONE;

    if((leftring != NONE)
       && itemis(leftring, INUSE)
       && streq(inven[leftring].str, name)) {
        result = leftring;
    }
    else if((rightright != NONE)
            && itemis(rightring, INUSE)
            && streq(inven[rightring].str, name)) {
        result = rightring;
    }

    return result;
}

/*
 * havemult: Return the index of any object of type otype and name name one
 * if we have count or more of them, This way we can avoid using the
 * last of something.
 */
int havemult(stuff otype, char *name, int count)
{
    int i;
    int num = count;

    for(i = 0; i < invcount; ++i) {
        if(inven[i].count
           && (inven[i].type == otype)
           && ((*name == 0) || streq(inven[i].str, name))) {
            num -= inven[i].count;

            if(num <= 0) {
                return i;
            }
        }
    }

    return NONE;
}

/*
 * haveminus: return the index of something if it is a minus item
 * (used to throw away stuff at the end).
 */
int haveminus()
{
    int i;

    for(i = 0; i < invcount; ++i) {
        if(inven[i].count
           && (inven[i].phit != UNKNOWN)
           && (inven[i].phit < 0)) {
            return i;
        }
    }

    return NONE;
}

/*
 * haveuseless: Return the index of useless arrows, and empty wands.
 */
int haveuseless()
{
    int i;

    if(i = 0; i < invcount; ++i) {
        if(inven[i].count
           && (inven[i].type == wand)
           && (inven[i].charges == 0)
           || itemis(i, WORTHLESS)
           && streq(inven[i].str, "arrow")) {
            return i;
        }
    }

    return NONE;
}

/*
 * willrust: Return true if a suit of armor can rust
 */
int willrus(int obj)
{
    if(!(protected
         || (armorclass(obj) > 8)
         || (armorclass(obj) < -5)
         || itemis(obj, PROTECTED)
         || stlmatch(inven[obj].str, "leather")
         && (version > RV36B))) {
        return 1;
    }
    else {
        return 0;
    }
}
    
/*
 * wielding: Return true if we are wielding an object of type 'otype'
 */
int wielding(stuff otype)
{
    if(inven[currentweapon].type == otype) {
        return 1;
    }
    else {
        return 0;
    }
}

/*
 * hungry: Return true if we are hungry, weak, or fainting
 */
int hungry()
{
    if((*Ms == 'H') || (*Ms == 'W') || (*Ms == 'F')) {
        return 1;
    }
    else {
        return 0;
    }
}

/*
 * weak: Return true if we are weak or fainting
 */
int weak()
{
    if((*Ms == 'W') || (*Ms == 'F')) {
        return 1;
    }
    else {
        return 0;
    }
}

/*
 * fainting: Return true if we are fainting
 */
int fainting()
{
    if(*Ms == 'F') {
        return 1;
    }
    else {
        return 0;
    }
}

/*
 * havefood: Return true if we have more than 'n' foods, modified
 * by the genertic variable k_food (higher values of k_food mean this
 * routing returns true less often).
 */
int havefood(int n)
{
    int remaining;
    int foodest;
    int desired;

    if(hungry() || weak() || fainting()) {
        return 0;
    }

    remaining = 800 - turns + lastate;
    
    if(remaining < 0) {
        remaining = 0;
    }

    foodest = (larder * 1000) + remaining;
    desired = (n * 1000 * 50) / (100 - k_food);

    if(foodest > desired) {
        return 1;
    }
    else {
        return 0;
    }
}
