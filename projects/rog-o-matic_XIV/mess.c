/*
 * mess.c: Rog-O-Matic XIV (CMU) Tue Mar 19 21:31:30 1985 - mlm
 * Copyright (C) 1985 by A. Apple, G. Jacobson, L. Hamey, M. Mauldin
 *
 * mess.c: Thie file contains all of the functions which parse the
 * message line.
 */
#include "mess.h"

#include <ctype.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "arms.h"
#include "database.h"
#include "debug.h"
#include "globals.h"
#include "io.h"
#include "ltm.h"
#include "monsters.h"
#include "pack.h"
#include "rooms.h"
#include "search.h"
#include "stats.h"
#include "tactics.h"
#include "things.h"
#include "types.h"
#include "utility.h"

/* Matching macros */
#define MATCH(p) smatch(mess, p, result)

/* Local data recording statistics */
static int monkilled[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int totalkilled = 0;
static int timeshit = 0;
static int timesmissed = 0;
static int hits = 0;
static int misses = 0;
static int sumgold = 0;
static int sumsqgold = 0;
static int numgold = 0;

static int mhit = 0;
static int mmiss = 0;
static int mtarget = NONE;

/* Other local data */
int identifying = 0; /* Next message is from identify scroll */
static int justreadid = 0; /* True if just read identify scroll */
static int gushed = 0; /* True ==> water on head msg recently */
static int echoit; /* True ==> echo this message to the user */

/* Results from start matcher */
static char res1[NAMESIZ];
static char res2[NAMESIZ];
static char res3[NAMESIZ];
static char res4[NAMESIZ];

static char *result[] = {
    res1,
    res2,
    res3,
    res4
};

/*
 * terpmes: Called when a message from Rogue is on the top line
 * this function parses the message and notes the information.
 * Note that the messages are all lower cased, to help with
 * compatability between 3.6 and 5.2, since 5.2 capitalizes more
 * messages than 3.6. Trailing punctuation is also ignored.
 *
 * As of Rogue 5.3, multiple messages are broken into single
 * messages before being passed to parsemsg. Periods separate
 * multiple messages.
 */
void terpmes()
{
    char mess[128];
    char *m;
    char *mend;
    char *s = screen[0];
    char *t;

    /* Set 't' to the tail of the message, skip bckward over blank and dot */
    t = s + 79;

    while((*t == ' ') || (*t == '.')) { /* Find last non-blank */
        --t;
    }

    ++t; /* t -> beyond string */

    /*
     * Loop through each message, finding the beginning and end, and
     * copying it to mess, lower-casing it as we go. Then call parsemsg.
     */
    while(s < t) { /* While more chars in msg */
        while((*s == ' ') && (s < t)) { /* Skip leading blanks */
            ++s;
        }

        /* Copy text */
        for(m = mess; (s < t) && ((version < RV53A) || (*s != '.') || s[1] != ' '); ++s) {
            /* Lower case the char */
            if(isupper(*s)) {
                *m = tolower(*s);
            }
            else {
                *m = *s;
            }

            ++m;
        }

        /* Skip the period, if any */
        ++s;

        /* Add trailing NULL */
        mend = m;
        *mend = '\0';

        if(mess != mend) {
            /* Parse it */
            parsemsg(mess, mend);
        }
    }
}

/*
 * parsemsg: Parse a single message, and if necessary set variables
 * or call functions.
 */
void parsemsg(char *mess, char *mend)
{
    int unknown = 0;
    echoit = 1;

    /* Take action based on type of message */
    if(MATCH("was wearing *")) {
    }
    else if((mend[-1] == ')') && (mend[-3] == '(')) {
        /* Message indicates we picked up a new item */
        echoit = !inventory(mess, mend);
        justreadid = 0;
        identifying = 0;
    }
    else if(mess[1] == ')') {
        /* Message describes an old item already in our pack */
        echoit = identifying;
        justreadid = 0;
        identifying = justreadid;
        inventory(mess, mend);
    }
    else {
        /* A random message, switch off first char to save some time... */
        switch(mess[0]) {
        case 'a':
            if(MATCH("as you read the scroll, it vanishes")) {
                echoit = 0;
            }
            else if(MATCH("a cloak of darkness falls around you")) {
                infer("blindness");
                blinded = 1;
            }
            else if(MATCH("a teleport trap")) {
                nametrap(TELTRAP, NEAR);
            }
            else if(MATCH("a trapdoor")) {
                nametrap(TRAPDOR, NEAR);
            }
            else if(MATCH("an arror shoots *")) {
                arrowshot = 1;
                nametrap(ARROW, HERE);
            }
            else if(MATCH("an arrow trap")) {
                nametrap(ARROW, NEAR);
            }
            else if(MATCH("a beartrap")) {
                nametrap(BEARTRP, NEAR);
            }
            else if(MATCH("a strange white mist *")) {
                nametrap(GASTRAP, HERE);
            }
            else if(MATCH("a sleeping gas trap")) {
                nametrap(GASTRAP, NEAR);
            }
            else if(MATCH("a small dart")) {
                nametrap(DARTRAP, HERE);
            }
            else if(MATCH("a dart trap")) {
                nametrap(DARTRAP, NEAR);
            }
            else if(MATCH("a poison dart trap")) {
                nametrap(DARTRAP, NEAR);
            }
            else if(MATCH("a rust trap")) {
                nametrap(WATERAP, NEAR);
            }
            else if(MATCH("A gush of water hits you on the head")) {
                ++gushed;
            }
            else if(MATCH("a sting has weakened you")) {
            }
            else if(MATCH("a bite has weakened you")) {
            }
            else if(MATCH("a ring *")) {
            }
            else if(MATCH("a wand *")) {
            }
            else if(MATCH("a staff *")) {
            }
            else if(MATCH("a scroll *")) {
            }
            else if(MATCH("a potion *")) {
            }
            else if(MATCH("a +*")) {
            }
            else if(MATCH("a -*")) {
            }
            else {
                ++unknown;
            }

            break;
        case 'b':
            if(MATCH("bolt bounces")) {
                infer("lightning");
            }
            else if(MATCH("bolt hits")) {
                infer("lightning");
            }
            else if(MATCH("bolt misses")) {
                infer("lightning");
            }
            else if(MATCH("bummer, this food tastes awful")) {
            }
            else if(MATCH("bummer! you've hit the ground")) {
                floating = 0;
            }
            else if(MATCH("bite has no effect")) {
            }
            else {
                ++unknown;
            }

            break;
        case 'c':
            if(MATCH("call it*")) { /* Handled in getrogue() */
                echoit = 0;
            }
            else {
                ++unknown;
            }

            break;
        case 'd':
            if(MATCH("defeated the *")) {
                echoit = 0;
                killed(res1);
            }
            else if(MATCH("defeated it")) {
                echoit = 0;
                killed("it");
            }
            else if(MATCH("defeated *")) {
                echoit = 0;
                killed(res1);
            }
            else if(MATCH("drop what*")) {
                echoit = 0;
            }
            else if(MATCH("dropped *")) {
            }
            else {
                ++unknown;
            }

        case 'e':
            if(MATCH("eat what*")) {
                echoit = 0;
            }
            else if(MATCH("everything looks so boring now")) {
                cosmic = 0;
            }
            else {
                ++unknown;
            }

            break;
        case 'f':
            if(MATCH("flame *")) {
            }
            else if(MATCH("far out!  everything is all cosmic again")) {
                blinded = 0;
            }
            else {
                ++unknown;
            }

            break;
        case 'g':
            if(MATCH("getting hungry")) {
                echoit = 0;
            }
            else if(MATCH("getting the munchies")) {
                echoit = 0;
            }
            else {
                ++unknown;
            }

            break;
        case 'h':
            if(MATCH("hey, this tastes great*")) {
                infer("restore strength");
            }
            else if(MATCH("huh? what? who?")) {
            }
            else if(MATCH("heavy!  that's a nasty critter!")) {
            }
            else {
                ++unknown;
            }

            break;
        case 'i':
            if(MATCH("it hit")) {
                washit("it");
                echoit = 0;
            }
            else if(MATCH("it misses")) {
                wasmissed("it");
                echoit = 0;
            }
            else if(MATCH("it appears confused")) {
            }
            else if(MATCH("ice *")) {
            }
            else if(MATCH("identify what*")) {
                echoit = 0;
            }
            else if(MATCH("illegal command*")) {
                echoit = 0;
            }
            else if(MATCH("i see no way*")) {
                unset(STAIRS);
                findstairs(atrow, atcol);
            }
            else if(MATCH("it appears to be cursed")) {
                curseditem();
            }
            else if(MATCH("it make*")) {
            }
            else {
                ++unknown;
            }

            break;
        case 'j':
        case 'k':
            ++unknown;

            break;
        case 'l':
            if(MATCH("left or*")) {
                echoit = 0;
            }
            else {
                ++unknown;
            }

            break;
        case 'm':
            if(MATCH("missile vanishes")) {
                infer("magic missile");
            }
            else if(MATCH("missle vanishes")) {
                infer("magic missile");
            }
            else if(MATCH("my, that was a yummy *")) {
            }
            else if(MATCH("moved onto *")) {
                set(STUFF);
            }
            else {
                ++unknown;
            }

            break;
        case 'n':
            if(MATCH("nothing happens")) {
                inven[lastwand].charges = 0;
            }
            else if(MATCH("no more *")) {
            }
            else if(MATCH("nothing appropriate")) {
            }
            else if(MATCH("no room")) {
            }
            else {
                ++unknown;
            }

            break;
        case 'o':
            if(MATCH("oh no! an arrow shot *")) {
                arrowshot = 1;
                nametrap(ARROW, HERE);
            }
            else if(MATCH("oh, now this scroll has a map *")) {
                infer("magic mapping");
                didreadmap = Level;
            }
            else if(MATCH("oh, bummer!  everything is dark!  help!")) {
                infer("blindness");
                blinded = 1;
            }
            else if(MATCH("oh, wow!  everything seems so cosmic!")) {
                infer("hallucination");
                cosmic = 1;
            }
            else if(MATCH("oh, wow!  you're floating in the air!")) {
                infer("levitation");
                floating = 1;
            }
            else if(MATCH("oh, wow, that tasted good")) {
            }
            else {
                ++unknown;
            }

            break;
        case 'p':
            if(MATCH("please spec*")) {
                echoit = 0;
            }
            else if(MATCH("put on what*")) {
                echoit = 0;
            }
            else {
                ++unknown;
            }

            break;
        case 'q':
            if(MATCH("quaff what*")) {
                echoit = 0;
            }
            else {
                ++unknown;
            }

            break;
        case 'r':
            if(MATCH("range is 'a' or '*'")) {
                echoit = 0;

                if((*res1 - 'a' + 1) != invcount) {
                    dwait(D_INFORM, "Range check failed...");
                    usesynch = 0;
                }
            }
            else if(MATCH("read what*")) {
                echoit = 0;
            }
            else if(MATCH("rogue version *")) {
                echoit = 0;
            }
            else {
                ++unknown;
            }

            break;
        case 's':
            if(MATCH("she stole *")) {
                usesynch = 0;
            }
            else if(MATCH("sting has no effect")) {
            }
            else {
                ++unknown;
            }

            break;
        case 't':
            if(MATCH("throw what*")) {
                echoit = 0;
            }
            else if(MATCH("the * bounces")) {
            }
            else if(MATCH("the bolt *")) {
            }
            else if(MATCH("the flame *")) {
            }
            else if(MATCH("the ice hits")) {
            }
            else if(MATCH("the ice misses")) {
            }
            else if(MATCH("the ice whizzes by you")) {
                wasmissed("ice monster");
            }
            else if(MATCH("the * hits it")) {
                echoit = 0;
                mshit("it");
            }
            else if(MATCH("the * misses it")) {
                echoit = 0;
                msmiss("it");
            }
            else if(MATCH("the * hits the *")) {
                echoit = 0;
                mshit(res2);
            }
            else if(MATCH("the * misses the *")) {
                echoit = 0;
                msmiss(res2);
            }
            else if(MATCH("the * hit")) {
                washit(res1);
                gushed = 0;
                echoit = 0;
            }
            else if(MATCH("the * misses")) {
                wasmissed(res1);
                echoit = 0;
            }
            else if(MATCH("the * appears confused")) {
            }
            else if(MATCH("the rust vanishes instantly")) {
                if(gushed) {
                    gushed = 0;
                    nametrap(WATERAP, HERE);
                }
            }
            else if(MATCH("the room is lit")) {
                setnewgoal();
                infer("light");
            }
            else if(MATCH("the corridor glows")) {
                infer("light");
            }
            else if(MATCH("the * has confused you")) {
                confused = 1;
            }
            else if(MATCH("this scroll is an * scroll")) {
                if(stlmatch(res1, "identify")) {
                    readident(res1);
                }
            }
            else if(MATCH("that's not a valid item")) {
                if(justreadid < 1) {
                    echoit = 1;
                }
                else {
                    echoit = 0;
                }

                if(justreadid-- == 0) {
                    sendnow(" *");
                }

                if(justreadid < -50) {
                    dwait(D_FATAL, "Caught in invali item loop");
                }
            }
            else if(MATCH("the veil of darkness lifts")) {
                blinded = 0;
            }
            else if(MATCH("the scroll turns to dust*")) {
                deletestuff(atrow, atcol);
                unset(SCAREM | STUFF);
                --droppedscare;
            }
            else if(MATCH("this potion tastes * dull")) {
                infer("thirst quenching");
            }
            else if(MATCH("this potion tests pretty")) {
                infer("thirst quenching");
            }
            else if(MATCH("this potion tests like apricot juice")) {
                infer("see invisible");

                if(version == RV36A) {
                    sendnow("%c", ESC);
                }
            }
            else if(MATCH("this scroll seems to be blank")) {
                infer("blank paper");
            }
            else if(MATCH("the * bounces")) {
            }
            else if(MATCH("the * vanishes as it hits the ground")) {
                darkturns = 0;
                darkdir = NONE;
                targetmonster = 0;
                echoit = 0;
            }
            else if(MATCH("there is something here*")) {
                usesynch = 0;
                set(STUFF);
            }
            else if(MATCH("the munchies are interfering")) {
            }
            else if(MATCH("the monsters around you freeze")) {
                holdmonsters();
            }
            else if(MATCH("the monster freezes")) {
                holdmonsters();
            }
            else if(MATCH("that's inedible")) {
                usesynch = 0;
            }
            else {
                ++unknown;
            }

            break;
        case 'u':
        case 'v':
            if(MATCH("version *")) {
                echoit = 0;
            }
            else {
                ++unknown;
            }

            break;
        case 'w':
            if(MATCH("what do you want*")) {
                echoit = 0;
            }
            else if(MATCH("wield what*")) {
                echoit = 0;
            }
            else if(MATCH("wielding a*")) {
                echoit = 0;
            }
            else if(MATCH("wear what*")) {
                echoit = 0;
            }
            else if(MATCH("what monster?")) {
                echoit = 0;
            }
            else if(MATCH("wait, what's going*")) {
                infer("confusion");
                confused = 1;
            }
            else if(MATCH("wait*that's a *")) {
            }
            else if(MATCH("what a*feeling")) {
                infer("confusion");
                confused = 1;
            }
            else if(MATCH("what a*piece of paper")) {
                infer("blank paper");
            }
            else if(MATCH("welcome to level *")) {
            }
            else if(MATCH("was wearing*")) {
            }
            else if(MATCH("what bulging muscles*")) {
            }
            else if(MATCH("wearing *")) {
            }
            else {
                ++unknown;
            }

            break;
        case 'x':
            ++unknown;

            break;
        case 'y':
            if(MATCH("you hit*")) {
                echoit = 0;
                didhit();
            }
            else if(MATCH("you miss")) {
                echoit = 0;
                didmiss();
            }
            else if(MATCH("you are starting to feel weak")) {
                echoit = 0;
            }
            else if(MATCH("you are weak from hunger")) {
                echoit = 0;
                eat();
            }
            else if(MATCH("you are being held")) {
                beingheld = 30;
            }
            else if(MATCH("you can move again")) {
                echoit = 0;
            }
            else if(MATCH("you are still stuck *")) {
                nametrap(BEARTRP, HERE);
            }
            else if(MATCH("you can't move")) {
                echoit = 0;
            }
            else if(MATCH("you can't carry anything else")) {
                echoit = 0;
                set(STUFF);
                maxobj = objcount;
            }
            else if(MATCH("you*cursed*")) {
                echoit = 0;
                curseditem();
            }
            else if(MATCH("you can't")) {
                echoit = 0;
            }
            else if(MATCH("you begin to feel greedy")) {
                infer("gold detection");
            }
            else if(MATCH("you begin to feel better")) {
                infer("healing");
            }
            else if(MATCH("you begin to feel much better")) {
                infer("healing");
            }
            else if(MATCH("you begin to sense the presence of monsters")) {
                infer("monster detection");
            }
            else if(MATCH("you feel a strange sense of loss")) {
            }
            else if(MATCH("you feel a wrenching sensation in your gut")) {
            }
            else if(MATCH("you feel stronger, now*")) {
                infer("gain strength");
            }
            else if(MATCH("you feel very sick now")) {
                infer("poison");
            }
            else if(MATCH("you feel momentarily sick")) {
                infer("poison");
            }
            else if(MATCH("you suddenly feel much more skillful")) {
                infer("raise level");
            }
            else if(MATCH("your nose tingles")) {
                infer("food detection");
            }
            else if(MATCH("you start to float in the air")) {
                infer("levitation");
                floating = 1;
            }
            else if(MATCH("you're floating off the ground!")) {
                floating = 1;
            }
            else if(MATCH("you float gently to the ground")) {
                floating = 0;
            }
            else if(MATCH("you feel yourself moving much faster")) {
                infer("haste self");
                hasted = 1;
            }
            else if(MATCH("you feel yourself slowing down")) {
                hasted = 0;
                doublehasted = 0;
            }
            else if(MATCH("you faint from exhaustion")) {
                if(version < RV52A) {
                    doublehasted = 1;
                }
                else {
                    hasted = 0;
                }
            }
            else if(MATCH("you feel less confused now")) {
                confused = 0;
            }
            else if(MATCH("you feel less trip*")) {
                confused = 0;
            }
            else if(MATCH("your * vanishes as it hits the ground")) {
                darkturns = 0;
                darkdir = NONE;
                echoit = 0;
            }
            else if(MATCH("your hands begin to glow *")) {
                infer("monster confusion");
                redhands = 1;
            }
            else if(MATCH("you hands stop glowing *")) {
                redhands = 0;
            }
            else if(MATCH("you feel as if somebody is watching over you")
                    || MATCH("you feel in touch with the universal ones")) {
                infer("remove curse");

                if(cursedarmor) {
                    forget(currentarmor, CURSED);
                    cursedarmor = 0;
                }

                if(cursedweapon) {
                    forget(currentweapon, CURSED);
                    cursedweapon = 0;
                }

                newweapon = 1;
                newarmor = newweapon;
            }
            else if(MATCH("your armor weakens")) {
                --inven[currentarmor].phit;

                if(gushed) {
                    gushed = 0;
                    nametrap(WATERAP, HERE);
                }
            }
            else if(MATCH("your armor is covered by a shimmering * shield")) {
                infer("protect armor");
                ++protected;
                remember(currentarmor, PROTECTED);
            }
            else if(MATCH("your armor glows * for a moment")) {
                infer("enchant armor");
                ++inven[currentarmor].phit;
                cursedarmor = 0;
                newarmor = 1;
            }
            else if(MATCH("your * glows * for a momen")) {
                infer("enchant weapon");
                plusweapon();
                newweapon = 1;
            }
            else if(MATCH("You hear a high pitched humming noise")) {
                infer("aggravate monsters");
                wakemonster(9);
                aggravated = 1;
            }
            else if(MATCH("you hear maniacal laughter*")) {
                infer("scare monster");
            }
            else if(MATCH("you hear a faint cry")) {
                infer("create monster");
            }
            else if(MATCH("you fall asleep")) {
                infer("sleep");
            }
            else if(MATCH("you have been granted the boon of genocide")) {
                infer("genocide");
                echoit = 0;
                rampage();
            }
            else if(MATCH("you have a tingling feeling")) {
                infer("drain life");
            }
            else if(MATCH("you are too weak to use it")) {
                infer("drain life");
            }
            else if(MATCH("you begin to feel greedy")) {
                infer("gold detection");
            }
            else if(MATCH("you feel a pull downward")) {
                infer("gold detection");
            }
            else if(MATCH("you begin to feel a pull downward")) {
                infer("gold detection");
            }
            else if(MATCH("you are caught *")) {
                nametrap(BEARTRP, HERE);
            }
            else if(MATCH("your purse feels lighter")) {
            }
            else if(MATCH("you suddenly feel weaker")) {
            }
            else if(MATCH("you must identify something")) {
            }
            else if(MATCH("you have a * feeling for a moment, then it passes")) {
            }
            else if(MATCH("you are transfixed")) {
            }
            else if(MATCH("you are frozen")) {
                washit("ice monster");
            }
            else if(MATCH("you faint")) {
                echoit = 0;

                if(version < RV36B) {
                    eat();
                }
            }
            else if(MATCH("you freak out")) {
                echoit = 0;
            }
            else if(MATCH("you fell into a trap!")) {
            }
            else if(MATCH("yum*")) {
                echoit = 0;
            }
            else if(MATCH("yuk*")) {
                echoit = 0;
            }
            else if(MATCH("you sense the presence of magic*")) {
                echoit = 0;
            }
            else {
                ++unknown;
            }

            break;
        case 'z':
            if(MATCH("zap with what*")) {
                echoit = 0;
            }
            else {
                ++unknown;
            }

            break;
        default:
            if(MATCH("* gold pieces")) {
                echoit = 0;
                countgold(res1);
            }
            else if(MATCH("(mctesq was here)")) {
                echoit = 0;
            }
            else if(MATCH("'*'*: *")) {
                echoit = 0;
                mapcharacter(*res1, res3);
            }
            else if((*mess == '+') || (*mess == '-') || ISDIGIT(*mess)) {
            }
            else {
                ++unknown;
            }
        }
    }

    /* Log unknown or troublesome messages */
    if(morecount > 50) {
        dwait(D_WARNING, "More loop msg '%s'", mess);
    }
    else if(unknown) {
        dwait(D_WARNING, "Unknown message '%s'", mess);
    }

    /* Send it to dwait; if dwait doesn't print it (and echo is on) echo it */
    if(echoit & !dwait(D_MESSAGE, mess)) {
        saynow(mess);
    }
}

/*
 * smatch: Given a data string and a pattern containing one or more
 * embedded stars (*) (which match any number of characters)
 * return true if the match success, and set res[i] to the
 * characters matched by the 'i'th *.
 */
int smatch(char *dat, char *pat, char **res)
{
    char *star = 0;
    char *starend;
    char *resp;
    int nres = 0;

    while(1) {
        if(*pat == '*') {
            /* Pattern after * */
            star = ++pat;

            /* Data after * match */
            starend = dat;

            /* Result string */
            resp = res[nres];
            ++nres;

            /* Initially NULL */
            *resp = '\0';
        }
        else if(*dat == *pat) { /* Characters match */
            if(*pat == '\0') { /* Pattern matches */
                return 1;
            }

            /* Try next position */
            ++pat;
            ++dat;
        }
        else {
            /* Pattern fails - no more data */
            if(*dat == '\0') {
                return 0;
            }

            /* Pattern fails - no * to adjust */
            if(star == 0) {
                return 0;
            }

            /* Restart pattern after * */
            pat = star;

            /* Copy character to resul */
            *resp++ = *starend;

            /* Null terminate */
            *resp = '\0';

            /* Rescan after copied char */
            dat = ++starend;
        }
    }
}

/*
 * readident: Read an identify scroll.
 */
void readident(char *name)
{
    int obj;

    /* Default is "* for a list" */
    char id = '*';

    if(!replaying
       && (version < RV53A)
       && ((nextid < LETTER(0)) || (nextid > LETTER(invcount)))) {
        dwait(D_FATAL,
              "Readident: nextid %d, afterid %d, invcount %d.",
              nextid,
              afterid,
              invcount);
    }

    /* Record what kind a scroll this is */
    infer(name);

    if(version < RV53A) { /* Rogue 3.6, Rogue 5.2 */
        /* Assume object is gone */
        deleteinv(OBJECT(afterid));

        /* Identify it */
        sendnow(" %c", nextid);

        /* Generate a message about it */
        send("I%c", afterid);

        /* Set variables */
        identifying = 1;
        knowident = identifying;
    }
    else { /* Rogue 5.3 */
        if(streq(name, "identify scroll")) {
            obj = unknown(scroll_obj);

            if(obj != NONE) {
                id = LETTER(obj);
            }
            else {
                obj = have(scroll_obj);

                if(obj != NONE) {
                    id = LETTER(obj);
                }
            }
        }
        else if(streq(name, "identify potion")) {
            obj = unknown(potion_obj);

            if(obj != NONE) {
                id = LETTER(obj);
            }
            else {
                obj = have(potion_obj);

                if(obj != NONE) {
                    id = LETTER(obj);
                }
            }
        }
        else if(streq(name, "identify armor")) {
            obj = unknown(armor_obj);

            if(obj != NONE) {
                id = LETTER(obj);
            }
            else {
                obj = have(armor_obj);

                if(obj != NONE) {
                    id = LETTER(obj);
                }
            }
        }
        else if(streq(name, "identify weapon")) {
            obj = unknown(hitter_obj);

            if(obj != NONE) {
                id = LETTER(obj);
            }
            else {
                obj = unknown(thrower_obj);

                if(obj != NONE) {
                    id = LETTER(obj);
                }
                else {
                    obj = unknown(missile_obj);

                    if(obj != NONE) {
                        id = LETTER(obj);
                    }
                    else {
                        obj = have(hitter_obj);

                        if(obj != NONE) {
                            id = LETTER(obj);
                        }
                        else {
                            obj = have(thrower_obj);

                            if(obj != NONE) {
                                id = LETTER(obj);
                            }
                            else {
                                obj = have(missile_obj);

                                if(obj != NONE) {
                                    id = LETTER(obj);
                                }
                            }
                        }
                    }
                }
            }
        }
        else if(streq(name, "identify ring, wand or staff")) {
            obj = unknown(ring_obj);

            if(obj != NONE) {
                id = LETTER(obj);
            }
            else {
                obj = unknown(wand_obj);

                if(obj != NONE) {
                    id = LETTER(obj);
                }
                else {
                    obj = have(ring_obj);

                    if(obj != NONE) {
                        id = LETTER(obj);
                    }
                    else {
                        obj = have(wand_obj);

                        if(obj != NONE) {
                            id = LETTER(obj);
                        }
                    }
                }
            }
        }
        else {
            dwait(D_FATAL, "Unkown identify scroll '%s'", name);
        }

        waitfor("not a valid item");
        waitfor("--More--");

        /* Pick an object to identify */
        sendnow(" %c", id);

        /* Must reset inventory */
        usesynch = 0;
        justreadid = 1;
    }

    newweapon = 1;
    newring = newweapon;
    nextid = '\0';
    afterid = nextid;
}

/*
 * rampage: Read a scroll of genocide.
 */
void rampage()
{
    char monc;

    /* Check the next monster in the list, we may not fear him */
    monc = *genocide;
    while(monc) {
        /* Do not waste genocide on stalkers if we have the right ring */
        if((streq(monname(monc), "invisible stalker")
            || streq(monname(monc), "phantom"))
           && (havenamed(ring_obj, "see invisible") != NONE)) {
            ++genocide;
        }
        else if((streq(monname(monc), "rust monster")
                 || streq(monname(monc), "aquator"))
                && (havenamed(ring_obj, "maintain armor") != NONE)) {
            /* Do not waste genocide on rusties if we have the right ring */
            ++genocide;
        }
        else {
            /* No fancy magic for this monster, use the genocide scroll */
            break;
        }

        monc = *genocide;
    }

    /* If we found a monster, send this character, else send ESC */
    if(monc) {
        saynow("About to rampage against %s", monname(monc));

        /* Send the monster */
        sendnow(" %c;", monc);

        /* Add to the list of 'gone' monsters */
        sprintf(genocided, "%s%c", genocided, monc);
        ++genocide;
    }
    else {
        dwait(D_ERROR, "Out of monsters to genocide!");

        /* Cancel the command */
        sendnow(" %c;", ESC);
    }
}

/*
 * curseditem: The last object we tried to drop (unwield, etc.) was cursed.
 *
 * Note that cursed rings are not a problem since we only put on
 * good rings we have identified, so don't bother marking rings.
 */
void curseditem()
{
    /* Force a reset inventory */
    usesynch = 0;

    /* lastdrop is index of last item we tried to use which could be cursed */
    if((lastdrop != NONE) && (lastdrop < invcount)) {
        remember(lastdrop, CURSED);

        /* Is our armor cursed? */
        if(inven[lastdrop].type == armor_obj) {
            currentarmor = lastdrop;
            cursedarmor = 1;

            return;
        }
        else if((inven[lastdrop].type == hitter_obj)
                || (inven[lastdrop].type == missile_obj)) {
            /*
             * Is it our wepaon (may be wielding a hitter
             * or a bogus magic arrow)?
             */
            currentweapon = lastdrop;
            cursedweapon = 1;

            return;
        }
    }

    /* Don't know what was cursed so assume the worst */
    cursedarmor = 1;
    cursedweapon = 1;
}

/*
 * first copy the title of the last scroll into the appropriate slot
 * then find the real name of the object by looking through the data
 * base, and then zap that name into all of the same objects
 */
void infer(char *objname)
{
    int i;

    if(*lastname && *objname && !stlmatch(objname, lastname)) {
        infername(lastname, objname);

        for(i = 0; i < MAXINV; ++i) {
            if(stlmatch(inven[i].str, lastname)) {
                strcpy(inven[i].str, objname);
                remember(i, KNOWN);
            }
        }
    }
}

/*
 * killed: Called whenever we defeat a monster.
 */
void killed(char *monster)
{
    int m = 0;
    int mh = 0;

    /* Find out what we really killed */
    if(!cosmic && !blinded && (targetmonster > 0) && streq(monster, "it")) {
        monster = monname(targetmonster);
    }

    mh = getmonhist(monster, 0);

    if(mh != NONE) {
        monster = monhist[mh].m_name;
        m = monsternum(monster);
    }

    /* Tell the user what we killed */
    dwait(D_BATTLE | D_MONSTER, "Killed '%s'", monster);

    /* If cheating against Rogue 3.6, check out our arrow */
    if((version < RV52A) && cheat) {
        if(usingarrow
           && (hitstokill > 1)
           && !beingstalked
           && (goodarrow < 20)) {
            saynow("Oops, bad arrow...");
            badarrow = 1;
            newweapon = badarrow;
            remember(currentweapon, WORTHLESS);
        }
        else if(usingarrow) {
            ++goodarrow;
        }
    }

    /* Echo the number of arrows we pumped into him */
    if((mh >= 0) && ((mhit + mmiss) > 0) && (mtarget == mh)) {
        dwait(D_BATTLE | D_MONSTER,
              "%d out of %d missiles hit the %s",
              mhit,
              mhit + mmiss,
              monster);
    }

    /* If we killed it by hacking, add the result to long term memory */
    if((hitstokill > 0) && (mh != NONE)) {
        addstat(&monhist[mh].htokill, hitstokill);
    }

    /* If we killed it with arrows, add that fact to long term memory */
    if((mhit > 0) && (mh != NONE)) {
        addstat(&monhist[mh].atokill, mhit);
    }

    /* Stop shooting arrows if we killed the right monster */
    if(targetmonster == (m + 'A' - 1)) {
        darkturns = 0;
        darkdir = NONE;
        targetmonster = 0;
    }

    /* Clear old movement goal */
    goalc = NONE;
    goalr = goalc;

    /* Bump kill count */
    ++monkilled[m];
    ++totalkilled;

    /* Clear individual monster stats */
    mmiss = 0;
    mhit = mmiss;
    hitstokill = mhit;

    /* Clear target */
    mtarget = NONE;

    /* Clear flags */
    cancelled = 0;
    beingheld = cancelled;

    /* If we killed an invisible, assume no more invisible around */
    if(!cosmic
       && !blinded
       && (streq(monster, "invisible stalker") || streq(monster, "phantom"))) {
        beingstalked = 0;
    }
}

/*
 * washit: Record being hit by a monster.
 */
void washit(char *monster)
{
    int mh = 0;
    int m = 0;

    /* Find out what really hit us */
    mh = getmonhist(monster, 1);

    if(mh != NONE) {
        monster = monhist[mh].m_name;
        m = monsternum(monster);
    }

    dwait(D_MONSTER, "was hit by a '%s'", monster);

    /* Bump global count */
    ++timeshit;

    if(m > 0) {
        /* Wake him up */
        wakemonster(-m);
    }

    /* hit points changed, read bottom */
    terpbot();

    /* Add data bout the event to long term memory */
    if(mh != NONE) {
        addprob(&monhist[mh].theyhit, SUCCESS);
        addstat(&monhist[mh].damage, lastdamage);
        analyzeltm();
    }
}

/*
 * wasmissed: Record being missed by a monster.
 */
void wasmissed(char *monster)
{
    int mh = 0;
    int m = 0;

    /* Find out what really missed us */
    mh = getmonhist(monster, 1);

    if(mh != NONE) {
        monster = monhist[mh].m_name;
        m = monsternum(monster);
    }

    dwait(D_MONSTER, "was missed by a '%s'", monster);

    /* Bump global count */
    ++timesmissed;

    if(m > 0) {
        /* Wake him up */
        wakemonster(-m);
    }

    /* Add data to long term memory */
    if(mh != NONE) {
        addprob(&monhist[mh].theyhit, FAILURE);
        analyzeltm();
    }
}

/*
 * didhit: Record hitting a monster.
 */
void didhit()
{
    int m = 0;

    /* Record our hit */
    if(!cosmic) {
        m = lastmonster;
    }

    ++hits;
    ++hitstokill;
    addprob(&monhist[monindex[m]].wehit, SUCCESS);

    if(wielding(wand_obj)) {
        --inven[currentweapon].charges;
        ++newweapon;
    }
}

/*
 * didmiss: Record missing monster.
 */
void didmiss()
{
    int m = 0;

    /* Record our miss */
    if(!cosmic) {
        m = lastmonster;
    }

    ++misses;
    addprob(&monhist[monindex[m]].wehit, FAILURE);

    if(usingarrow && (goodarrow < 20)) {
        badarrow = 1;
        newweapon = badarrow;
        remember(currentweapon, WORTHLESS);
    }
}

/*
 * mshit: Record a monster with a missile.
 */
void mshit(char *monster)
{
    int mh;

    /* Arching in a dark room? */
    if(!cosmic && !blinded && (targetmonster > 0) && streq(monster, "it")) {
        monster = monname(targetmonster);
    }

    /* Add data about the enven to long term memory */
    mh = getmonhist(monster, 0);

    if(mh < 0) {
        return;
    }

    addprob(&monhist[monindex[mh]].arrowhit, SUCCESS);

    if(mh == mtarget) {
        ++mhit;
    }
    else {
        mhit = 1;
        mmiss = 0;
        mtarget = mh;
    }
}

/*
 * msmiss: Record missing a monster with a missile.
 */
void msmiss(char *monster)
{
    int mh;

    /* Arching in a dark room? */
    if(!cosmic && !blinded && (targetmonster > 0) && streq(monster, "it")) {
        monster = monname(targetmonster);
    }

    /* Add data about the event to long term memory */
    mh = getmonhist(monster, 0);

    if(mh < 0) {
        return;
    }

    addprob(&monhist[monindex[mh]].arrowhit, FAILURE);

    if(mh == mtarget) {
        ++mmiss;
    }
    else {
        mmiss = 1;
        mhit = 0;
        mtarget = mh;
    }
}

/*
 * countgold: Called whenever msg contains a message about the number
 *            of gold pieces we just picked up. This routine keeps
 *            statistics about the amount of gold picked up.
 */
void countgold(char *amount)
{
    int pot;

    pot = atoi(amount);

    if(pot > 0) {
        sumgold += pot;
        sumsqgold += (pot * pot);
        ++numgold;
    }
}

/*
 * Summary: print a summary of the game.
 */
void summary(FILE *f, char sep)
{
    int m;
    char s[1024];

    sprintf(s, "Monsters killed:%c%c", sep, sep);

    for(m = 0; m <= 26; ++m) {
        if(monkilled[m] > 0) {
            sprintf(s,
                    "%s\t%d %s%s%c",
                    s,
                    monkilled[m],
                    monname(m + 'A' - 1),
                    plural(monkilled[m]),
                    sep);
        }
    }

    sprintf(s, "%s%cTotal: %d%c%c", s, sep, totalkilled, sep, sep);

    sprintf(s,
            "%sHit %d out of %d times, whas hit %d out of %d times.%c",
            s,
            hits,
            misses + hits,
            timeshit,
            timesmissed + timeshit,
            sep);

    if(numgold > 0) {
        sprintf(s,
                "%sGold %d total, %d pots, %d average.%c",
                s,
                sumgold,
                numgold,
                ((sumgold * 10) + 5) / (numgold * 10),
                sep);
    }

    if(f == NULL) {
        addstr(s);
    }
    else {
        fprintf(f, "%s", s);
    }
}

/*
 * versiondep: Set version dependent variables.
 */
void versiondep()
{
    if(version >= RV53A) {
        genocide = "DMJGU";
    }
    else if(version >= RV52A) {
        genocide = "UDVPX";
    }
    else {
        genocide = "UXDPW";
    }

    analyzeltm();
}

/*
 * getmonhist: Retrieve the index in the history array of a monster,
 * taking out status into account. This code is responsible for determining
 * when we are being stalked by an invisible monster.
 */
int getmonhist(char *monster, int hitormiss)
{
    if(cosmic || blinded) {
        return findmonster("it");
    }
    else {
        if(streq(monster, "it") && hitormiss) {
            if(version < RV53A) {
                if(!seemonster("invisible stalker")) {
                    beingstalked = INVHIT;
                }

                return findmonster("invisible stalker");
            }
            else {
                if(!seemonster("phantom")) {
                    beingstalked = INVHIT;
                }

                return findmonster("phantom");
            }
        }
        else {
            if((version < RV52B)
               && streq(monster, "invisible stalker")
               && !seemonster(monster)) {
                beingstalked = INVHIT;
            }

            return findmonster(monster);
        }
    }
}
