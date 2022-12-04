/*
 * pack.c: Rog-O-Matic XIV (CMU) Sat Feb 16 08:58:04 1985 - mlm
 * Copyright (C) by a. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This file contains functions which mess with Rog-O-Matic's pack
 */
#include "pack.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "database.h"
#include "debug.h"
#include "globals.h"
#include "io.h"
#include "things.h"
#include "types.h"
#include "utility.h"
#include "worth.h"

static char *stuffmess[] = {
    "strange object",
    "food",
    "potion",
    "scroll",
    "wand",
    "ring",
    "hitter",
    "thrower",
    "missile",
    "armor",
    "amulet",
    "gold",
    "none"
};

/*
 * itemstr: Print the inventory message for a single item.
 */
char *itemstr(int i)
{
    static char ispace[128];
    char *item = ispace;

    if((i < 0) || (i >= MAXINV)) {
        sprintf(item, "%d out of bounds", i);
    }
    else if(inven[i].count < 1) {
        sprintf(item, "%c)      nothing", LETTER(i));
    }
    else {
        sprintf(item,
                "%c) %4d %d*%s:",
                LETTER(i),
                worth(i),
                inven[i].count,
                stuffmess[(int)inven[i].type]);
    
        if((inven[i].phit != UNKNOWN) && (inven[i].pdam == UNKNOWN)) {
            sprintf(item, "%s (%d)", item, inven[i].phit);
        }
        else if(inven[i].phit != UNKNOWN) {
            sprintf(item, "%s (%d,%d)", item, inven[i].phit, inven[i].pdam);
        }

        if(inven[i].charges != UNKNOWN) {
            sprintf(item, "%s [%d]", item, inven[i].charges);
        }

        /* DR UTexas */
        sprintf(item, "%s %s", item, inven[i].str);

        if(itemis(i, KNOWN)) {
            sprintf(item, "%s", "");
        }
        else {
            sprintf(item, "%s", ", unknown");
        }

        if(used(inven[i].str)) {
            sprintf(item, "%s", ", tried");
        }
        else {
            sprintf(item, "%s", "");
        }

        if(itemis(i, CURSED)) {
            sprintf(item, "%s", ", cursed");
        }
        else {
            sprintf(item, "%s", "");
        }

        if(itemis(i, UNCURSED)) {
            sprintf(item, "%s", ", uncursed");
        }
        else {
            sprintf(item, "%s", "");
        }

        if(itemis(i, ENCHANTED)) {
            sprintf(item, "%s", ", enchanted");
        }
        else {
            sprintf(item, "%s", "");
        }

        if(itemis(i, PROTECTED)) {
            sprintf(item, "%s", ", protected");
        }
        else {
            sprintf(item, "%s", "");
        }

        if(itemis(i, WORTHLESS)) {
            sprintf(item, "%s", ", useless");
        }
        else {
            sprintf(item, "%s", "");
        }

        if(!itemis(i, INUSE)) {
            sprintf(item, "%s", "");
        }
        else if((inven[i].type == armor_obj) || (inven[i].type == ring_obj)) {
            sprintf(item, "%s", ", being worn");
        }
        else {
            sprintf(item, "%s", ", in hand");
        }
    }

    return item;
}

/*
 * dumpinv: Print the inventory. Callse itemstr.
 */
void dumpinv(FILE *f)
{
    int i;
    
    if(f == NULL) {
        at(1, 0);
    }

    for(i = 0; i < MAXINV; ++i) {
        if(inven[i].count == 0) { /* No item here */
        }
        else if(f != NULL) { /* Write to a file */
            fprintf(f, "%s\n", itemstr(i));
        }
        else {
            printw("%s\n", itemstr(i));
        }
    }
}

/*
 * removeinv: Remove an item from the inventory.
 */
void removeinv(int pos)
{
    --inven[pos].count;

    if(inven[pos].count == 0) {
        /* Assure nothing at that spot  DR UT */
        clearpack(pos);
        
        /* Close up the hole */
        rollpackup(pos);
    }

    countpack();
    checkrange = 1;
}

/*
 * deleteinv: Delete an item from the inventory. Note: This function
 * is used when we drop rather than throw or use, since bunches of
 * things can be dropped all at once.
 */
void deleteinv(int pos)
{
    --inven[pos].count;

    if((inven[pos].count == 0) || (inven[pos].type == missile_obj)) {
        /* Assure nothing at that spot  DR UT */
        clearpack(pos);

        /* Close up the hole */
        rollpackup(pos);
    }

    countpack();
    checkrange = 1;
}

/*
 * clearpack: Zero out slot in pack.  DR UTexas 01/05/84
 */
void clearpack(int pos)
{
    if(pos >= MAXINV) {
        return;
    }

    inven[pos].count = 0;
    inven[pos].str[0] = '\0';
    inven[pos].phit = UNKNOWN;
    inven[pos].pdam = UNKNOWN;
    inven[pos].charges = UNKNOWN;

    forget(pos, 
           (KNOWN 
            | CURSED
            | ENCHANTED
            | PROTECTED
            | UNCURSED
            | INUSE
            | WORTHLESS));
}

/*
 * rollpackup: We have deleted an item, move up the objects behind it in
 * the pack.
 */
void rollpackup(int pos)
{
    char *savebuf;
    int i;

    if(version >= RV53A) {
        return;
    }

    if(pos < currentarmor) {
        --currentarmor;
    }
    else if(pos == currentarmor) {
        currentarmor = NONE;
    }

    if(pos < currentweapon) {
        --currentweapon;
    }
    else if(pos == currentweapon) {
        currentweapon = NONE;
    }

    if(pos < leftring) {
        --leftring;
    }
    else if(pos == leftring) {
        leftring = NONE;
    }

    if(pos < rightring) {
        --rightring;
    }
    else if(pos == rightring) {
        rightring = NONE;
    }

    savebuf = inven[pos].str;
    
    for(i = pos; (i + 1) < invcount; ++i) {
        inven[i] = inven[i + 1];
    }

    --invcount;
    inven[invcount].str = savebuf;
}

/*
 * rollpackdown: Open up a new spot in the pack, and move down the
 * objects behind that position.
 */
void rollpackdown(int pos)
{
    char *savebuf;
    int i;

    if(version >= RV53A) {
        return;
    }

    savebuf = inven[invcount].str;
    
    for(i = invcount; i > pos; --i) {
        inven[i] = inven[i - 1];

        if((i - 1) == currentarmor) {
            ++currentarmor;
        }

        if((i - 1) == currentweapon) {
            ++currentweapon;
        }

        if((i - 1) == leftring) {
            ++leftring;
        }

        if((i - 1) == rightring) {
            ++rightring;
        }
    }

    inven[pos].str = savebuf;

    if(++invcount > MAXINV) {
        usesynch = 0;
    }
}

/*
 * resetinv: Send an inventory command. The actual work is done by
 * doresetinv, which is called by a demon in the command handler.
 */
void resetinv()
{
    if(!replaying) {
        command(T_OTHER, "i");
    }
}

/*
 * doresetinv: Reset the inventory.  DR UTexas 01/08/84
 */
void doresetinv()
{
    int i;
    static char space[MAXINV][80];

    usesynch = 1;
    checkrange = 0;

    for(i = 0; i < MAXINV; ++i) {
        inven[i].str = space[i];
        clearpack(i);
    }

    urocnt = 0;
    objcount = urocnt;
    invcount = objcount;

    rightring = NONE;
    leftring = rightring;
    currentweapon = leftring;
    currentarmor = currentweapon;

    if(version >= RV53A) {
        invcount = MAXINV;
    }
}

/*
 * inventory: Parse an item message.
 */
#define xtr(w, b, e, k) \
    what = (w);         \
    xbeg = mess + (b);  \
    xend = mend - (e);  \
    xknow |= (k);

int inventory(char *msgstart, char *msgend)
{
    char *p;
    char *q;
    char *mess = msgstart;
    char *mend = msgend;
    char objname[100];
    int n;
    int ipos;
    int xknow = 0;
    int newitem = 0;
    int inuse = 0;
    int printed = 0;
    int plushit = UNKNOWN;
    int plusdam = UNKNOWN;
    int charges = UNKNOWN;
    stuff what;
    char *xbeg;
    char *xend;

    xend = "";
    xbeg = xend;
    dwait(D_PACK, "inventory: message %s", mess);

    /* Rip surrounding garbage from the message */
    if(mess[1] == ')') {
        ipos = DIGIT(*mess);
        mess += 3;
    }
    else {
        ipos = DIGIT(mend[-2]);
        mend -= 4;
        deletestuff(atrow, atcol);
        unsetrc(USELESS, atrow, atcol);
        newitem = 1;
    }

    if(ISDIGIT(*mess)) {
        n = atoi(mess);

        if(n > 9) {
            mess += (2 + 1);
        }
        else {
            mess += (2 + 0);
        }
    }
    else {
        n = 1;

        if(*mess == 'a') { /* Eat the determiner A/An/The */
            ++mess;
        }
        
        if(*mess == 'n') {
            ++mess;
        }

        if(*mess == 't') {
            ++mess;
        }

        if(*mess == 'h') {
            ++mess;
        }

        if(*mess == 'e') {
            ++mess;
        }

        if(*mess == ' ') { /* Eat the space after the determiner */
            ++mess;
        }
    }

    /* Read the plus to hit */
    if((*mess == '+') || (*mess == '-')) {
        plushit = atoi(mess);
        ++mess;

        while(ISDIGIT(*mess)) {
            ++mess;
        }

        xknow = KNOWN;
    }

    /* Eat any comma separating two modifiers */
    if(*mess == ',') {
        ++mess;
    }

    /* Read the plus damage */
    if((*mess == '+') || (*mess == '-')) {
        plusdam = atoi(mess);
        ++mess;

        while(ISDIGIT(*mess)) {
            ++mess;
        }

        xknow = KNOWN;
    }

    while(*mess == ' ') { /* Eat any separating spaces */
        ++mess;
    }

    while(mend[-1] == ' ') { /* Remove trailing blanks */
        --mend;
    }

    while(mend[-1] == '.') { /* Remove trailing periods */
        --mend;
    }

    /* Read any parenthesized strings at the end of the message */
    while(mend[-1] == ')') {
        --mend;

        while(*mend != '(') { /* On exit mend -> '(' */
            --mend;
        }
        
        if(stlmatch(mend, "(being worn)")) {
            currentarmor = ipos;
            inuse = INUSE;
        }
        else if(stlmatch(mend, "(weapon in hand)")) {
            currentweapon = ipos;
            inuse = INUSE;
        }
        else if(stlmatch(mend, "(on left hand)")) {
            leftring = ipos;
            inuse = INUSE;
        }
        else if(stlmatch(mend, "(on right hand)")) {
            rightring = ipos;
            inuse = INUSE;
        }

        while(mend[-1] == ' ') {
            --mend;
        }
    }

    /* Read the charges on a wand (or armor class or ring bonus) */
    if(mend[-1] == ']') {
        --mend;

        while(*mend != '[') { /* On exit mend -> '[' */
            --mend;
        }

        if(mend[1] == '+') {
            charges = atoi(mend + 2);
        }
        else {
            charges = atoi(mend + 1);
        }

        xknow = KNOWN;
    }

    /* Undo plurals by removing trailing 's' */
    while(mend[-1] == ' ') {
        --mend;
    }

    if(mend[-1] == 's') {
        --mend;
    }

    /* Now find what we picked up: */
    if(stlmatch(mend - 4, "food")) {
        what = food_obj;
        xknow = KNOWN;
    }
    else if(stlmatch(mess, "amulet")) {
        xtr(amulet_obj, 0, 0, KNOWN);
    }
    else if(stlmatch(mess, "potion of ")) {
        xtr(potion_obj, 10, 0, KNOWN);
    }
    else if(stlmatch(mess, "potions of ")) {
        xtr(potion_obj, 11, 0, KNOWN);
    }
    else if(stlmatch(mess, "scroll of ")) {
        xtr(scroll_obj, 10, 0, KNOWN);
    }
    else if(stlmatch(mess, "scrolls of ")) {
        xtr(scroll_obj, 11, 0, KNOWN);
    }
    else if(stlmatch(mess, "staff of ")) {
        xtr(wand_obj, 9, 0, KNOWN);
    }
    else if(stlmatch(mess, "wand of ")) {
        xtr(wand_obj, 8, 0, KNOWN);
    }
    else if(stlmatch(mess, "ring of ")) {
        xtr(ring_obj, 8, 0, KNOWN);
    }
    else if(stlmatch(mend - 4, "mail")) {
        xtr(armor_obj, 0, 0, 0);
    }
    else if(stlmatch(mend - 6, "potion")) {
        xtr(potion_obj, 0, 7, 0);
    }
    else if(stlmatch(mess, "scroll titled '")) {
        xtr(scroll_obj, 15, 1, 0);
    }
    else if(stlmatch(mess, "scrolls titled '")) {
        xtr(scroll_obj, 16, 1, 0);
    }
    else if(stlmatch(mend - 5, "staff")) {
        xtr(wand_obj, 0, 6, 0);
    }
    else if(stlmatch(mend - 4, "wand")) {
        xtr(wand_obj, 0, 5, 0);
    }
    else if(stlmatch(mend - 4, "ring")) {
        xtr(ring_obj, 0, 5, 0);
    }
    else if(stlmatch(mess, "apricot")) {
        xtr(food_obj, 0, 0, KNOWN);
    }
    else if(stlmatch(mend - 5, "sword")) {
        xtr(hitter_obj, 0, 0, 0);
    }
    else if(stlmatch(mend - 4, "mace")) {
        xtr(hitter_obj, 0, 0, 0);
    }
    else if(stlmatch(mend - 6, "dagger")) {
        xtr(missile_obj, 0, 0, 0);
    }
    else if(stlmatch(mend - 5, "spear")) {
        xtr(missile_obj, 0, 0, 0);
    }
    else if(stlmatch(mend - 5, "armor")) {
        xtr(armor_obj, 0, 0, 0);
    }
    else if(stlmatch(mend - 3, "arm")) {
        xtr(armor_obj, 0, 0, 0);
    }
    else if(stlmatch(mend - 3, "bow")) {
        xtr(thrower_obj, 0, 0, 0);
    }
    else if(stlmatch(mend - 5, "sling")) {
        xtr(thrower_obj, 0, 0, 0);
    }
    else if(stlmatch(mend - 5, "arrow")) {
        xtr(missile_obj, 0, 0, 0);
    }
    else if(stlmatch(mend - 4, "dart")) {
        xtr(missile_obj, 0, 0, 0);
    }
    else if(stlmatch(mend - 4, "rock")) {
        xtr(missile_obj, 0, 0, 0);
    }
    else if(stlmatch(mend - 4, "bolt")) {
        xtr(missile_obj, 0, 0, 0);
    }
    else if(stlmatch(mend - 8, "shuriken")) {
        xtr(missile_obj, 0, 0, 0);
    }
    else {
        xtr(strange_obj, 0, 0, 0);
    }

    /* Copy the name of the objet into a string */
    p = objname;
    
    for(q = xbeg; q < xend; ++q) {
        *p = *q;

        ++p;
    }

    *p = '\0';

    dwait(D_PACK,
          "inv: %s '%s', hit %d, dam %d, chg %d, knw %d",
          stuffmess[(int)what],
          objname,
          plushit,
          plusdam,
          charges,
          xknow);

    /* Ring bonus is printed differently in Rogue 5.3 */
    if((version >= RV53A) && (what == ring_obj) && (charges != UNKNOWN)) {
        plushit = charges;
        charges = UNKNOWN;
    }

    /*
     * If the name of the object matches something in the database,
     * slap the real name into the slot and mark it as known
     */
    if(((what == potion_obj)
        || (what == scroll_obj)
        || (what == wand_obj))
       && !xknow) {
        char *dbname = realname(objname);

        if(*dbname) {
            strcpy(objname, dbname);
            xknow = KNOWN;
            
            if(newitem) {
                at(0, 0);

                if(n == 1) {
                    printw(" a");
                }
                else {
                    printw("%d ", n);
                }

                if(what == potion_obj) {
                    if(n == 1) {
                        printw("%s%s of %s (%c)",
                               "potion",
                               "", 
                               objname,
                               LETTER(ipos));
                    }
                    else {
                        printw("%s%s of %s (%c)",
                               "potion",
                               "s",
                               objname,
                               LETTER(ipos));
                    }
                }
                else if(what == scroll_obj) {
                    if(n == 1) {
                        printw("%s%s of %s (%c)",
                               "scroll",
                               "",
                               objname,
                               LETTER(ipos));
                    }
                    else {
                        printw("%s%s of %s (%c)",
                               "scroll",
                               "s",
                               objname,
                               LETTER(ipos));
                    }
                }
                else if(what == ring_obj) {
                    if(n == 1) {
                        printw("%s%s of %s (%c)",
                               "ring",
                               "",
                               objname,
                               LETTER(ipos));
                    }
                    else {
                        printw("%s%s of %s (%c)",
                               "ring",
                               "s",
                               objname,
                               LETTER(ipos));
                    }
                }

                clrtoeol();
                at(row, col);
                refresh();
                ++printed;
            }
        }
    }

    /* If new item, record the change */
    if(newitem && (what == armor_obj)) {
        newarmor = 1;
    }
    else if(newitem && (what == ring_obj)) {
        newring = 1;
    }
    else if(newitem && (what == food_obj)) {
        newring = 1;
        lastfoodlevel = Level;
    }
    else if(newitem 
            && ((what == hitter_obj)
                || (what == missile_obj)
                || (what == wand_obj))) {
        newweapon = 1;
    }

    /*
     * If the object is an old object, set its count, else allocate
     * a new object and roll the other objects down
     */
    if((n > 1)
       && (ipos < invcount)
       && (inven[ipos].type == what)
       && (n == (inven[ipos].count + 1))
       && (inven[ipos].phit == plushit)
       && (inven[ipos].pdam == plusdam)) {
        inven[ipos].count = n;
    }
    else {
        /* New item, in older Rogues, open up a spot in the pack */
        if(version < RV53A) {
            rollpackdown(ipos);
        }

        inven[ipos].type = what;
        inven[ipos].count = n;
        inven[ipos].phit = plushit;
        inven[ipos].pdam = plusdam;
        inven[ipos].charges = charges;
        remember(ipos, inuse | xknow);

        if(!xknow) {
            ++urocnt;
        }
    }

    /* Forget enchanted status if item known.  DR UTexas 31 Jan 84 */
    if(itemis(ipos, KNOWN)) {
        forget(ipos, ENCHANTED);
    }

    /* Set the name of the object */
    if(inven[ipos].str != NULL) {
        strcpy(inven[ipos].str, objname);
    }
    else if(!replaying) {
        dwait(D_ERROR,
              "termpass: null inven[%d].str, invcount %d.",
              ipos,
              invcount);
    }

    /* Set cursed attribute for weapon and armor */
    if(cursedarmor && (ipos == currentarmor)) {
        remember(ipos, CURSED);
    }
    
    if(cursedweapon && (ipos == currentweapon)) {
        remember(ipos, CURSED);
    }

    /* Keep track of whether we are wielding a trap arrow */
    if(ipos == currentweapon) {
        if(what == missile_obj) {
            usingarrow = 1;
        }
        else {
            usingarrow = 0;
        }
    }

    countpack();

    /* If we picked up a useless thing, note that fact */
    if(newitem && on(USELESS)) {
        remember(ipos, WORTHLESS);
    }
    else if(newitem) {
        forget(ipos, WORTHLESS);
    }

    checkrange = 1;

    return printed;
}

/*
 * countpack: Count objects, missiles, and food in the pack.
 */
void countpack()
{
    int i;
    int cnt;

    objcount = 0;
    larder = 0;
    ammo = 0;

    for(i = 0; i < invcount; ++i) {
        cnt = inven[i].count;

        if(!cnt) { /* No object here */
        }
        else if(inven[i].type == missile_obj) {
            ++objcount;
            ammo += cnt;
        }
        else if(inven[i].type == food_obj) {
            objcount += cnt;
            larder += cnt;
        }
        else {
            objcount += cnt;
        }
    }
}
