/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.do_name.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "hack.h"
#include "hack.invent.h"
#include "hack.mon.h"
#include "hack.objnam.h"
#include "hack.pri.h"
#include "hack.shk.h"
#include "hack.termcap.h"
#include "hack.topl.h"
#include "hack.tty.h"
#include "hack.vault.h"
#include "hack.worn.h"

coord getpos(int force, char *goal)
{
    int cx;
    int cy;
    int i;
    int c;

    /* Defined in hack.c */
    extern char sdir[];
    extern schar xdir[];
    extern schar ydir[];

    coord cc;
    pline("(For instructions type a ?)");
    cx = u.ux;
    cy = u.uy;
    curs(cx, cy + 2);

    c = readchar();
    
    while(c != '.') {
        int flag = 0;
        for(i = 0; i < 8; ++i) {
            if(sdir[i] == c) {
                if((1 <= (cx + xdir[i])) && ((cx + xdir[i]) <= COLNO)) {
                    cx += xdir[i];
                }

                if((0 <= (cy + ydir[i])) && ((cy + ydir[i]) <= (ROWNO - 1))) {
                    cy += ydir[i];
                }

                flag = 1;

                break;
            }
        }

        if(flag == 1) {
            curs(cx, cy + 2);
            c = readchar();

            continue;
        }

        if(c == '?') {
            pline("Use [hjkl] to move the cursor to %s.", goal);
            pline("Type a . when yo uare at the right place.");
        }
        else {
            if(force != 0) {
                pline("unknown direction: '%s' (%s)",
                      visctrl(c),
                      "use hjkl or .");
            }
            else {
                pline("unknown direction: '%s' (%s)",
                      visctrl(c),
                      "aborted");
            }

            if(force != 0) {
                curs(cx, cy + 2);
                c = readchar();
                
                continue;
            }
            
            cc.x = -1;
            cc.y = 0;
            
            return cc;
        }

        curs(cx, cy + 2);
    }

    cc.x = cx;
    cc.y = cy;

    return cc;
}

int do_mname()
{
    char buf[BUFSZ];
    coord cc;
    int cx;
    int cy;
    int lth;
    int i;

    struct monst *mtmp;
    struct monst *mtmp2;

    cc = getpos(0, "the monster you want to name");
    cx = cc.x;
    cy = cc.y;

    if(cx < 0) {
        return 0;
    }

    mtmp = m_at(cx, cy);

    if(mtmp == NULL) {
        if((cx == u.ux) && (cy == u.uy)) {
            extern char plname[];

            pline("This ugly monster is called %s and cannot be renamed.",
                  plname);
        }
        else {
            pline("There is no monster there.");

            return 1;
        }
    }

    if(mtmp->mimic != 0) {
        pline("I see no monster there.");

        return 1;
    }
    
    if(cansee(cx, cy) == 0) {
        pline("I cannot see a monster there.");
    
        return 1;
    }

    pline("What do you want to call %s? ", lmonnam(mtmp));
    getlin(buf);
    clrlin();

    if(*buf == 0) {
        return 1;
    }

    lth = strlen(buf) + 1;

    if(lth > 63) {
        buf[62] = 0;
        lth = 63;
    }

    mtmp2 = newmonst(mtmp->mxlth + lth);
    *mtmp2 = *mtmp;

    for(i = 0; i < mtmp->mxlth; ++i) {
        ((char *)mtmp2->mextra)[i] = ((char *)mtmp->mextra)[i];
    }

    mtmp2->mnamelth = lth;

    strcpy(NAME(mtmp2), buf);
    replmon(mtmp, mtmp2);

    if(mtmp2->isshk != 0) {
        /* Redefine shopkeeper and bill */
        setshk();
    }

    if(mtmp2->isgd != 0) {
        setgd(/* mtmp2 */);
    }

    return 1;
}

/*
 * This routine changes the address of obj. Be careful not to call it
 * when there might be pointers around in unknown places. For now: only
 * when obj is in the inventory.
 */
void do_oname(struct obj *obj)
{
    struct obj *otmp;
    struct obj *otmp2;
    int lth;
    char buf[BUFSZ];

    pline("What do you want to name %s? ", doname(obj));
    getlin(buf);
    clrlin();

    if(*buf == 0) {
        return;
    }

    lth = strlen(buf) + 1;

    if(lth > 63) {
        buf[62] = 0;
        lth = 63;
    }

    otmp2 = newobj(lth);
    *otmp2 = *obj;

    otmp2->onamelth = lth;
    strcpy(ONAME(otmp2), buf);

    setworn((struct obj *)0, obj->owornmask);
    setworn(otmp2, otmp2->owornmask);

    /* 
     * Do freeinv(obj); by hand in order to preserve
     * the position of this object in the inventory
     */
    if(obj == invent) {
        invent = otmp2;
    }
    else {
        otmp = invent;
        
        while(1) {
            if(otmp == NULL) {
                panic("Do_oname: cannot find obj.");
            }

            if(otmp->nobj == obj) {
                otmp->nobj = otmp2;
            
                break;
            }

            otmp = otmp->nobj;
        }
    }

    /* Now unnecessary: no points on bill */
    /* obfree(obj, otmp2); */

    /* Let us hope nobody else saved a pointer */
    free(obj);
}

int ddocall()
{
    struct obj *obj;

    pline("Do you want to name an individual object? [yn] ");
    
    if(readchar() == 'y') {
        obj = getobj("#", "name");

        if(obj != NULL) {
            do_oname(obj);
        }
    }
    else {
        obj = getobj("?!=/", "call");

        if(obj != 0) {
            docall(obj);
        }
    }

    return 0;
}

void docall(struct obj *obj)
{
    char buf[BUFSZ];
    char **str1;
    char *str = xname(obj);

    if(index(vowels, *str) != 0) {
        pline("Call %s %s: ", "an", str);
    }
    else {
        pline("Call %s %s: ", "a", str);
    }

    getlin(buf);
    clrlin();
    
    if(*buf == 0) {
        return;
    }

    str = (char *)alloc((unsigned int)(strlen(buf) + 1));
    strcpy(str, buf);
    str1 = &(objects[obj->otyp].oc_uname);
    
    if(*str1 != NULL) {
        free(*str1);
    }

    str1 = &str;
}

char *xmonnam(struct monst *mtmp, int vb)
{
    /* %% */
    static char buf[BUFSZ];

    if((mtmp->mnamelth != 0) && (vb == 0)) {
        return NAME(mtmp);
    }

    switch(mtmp->data->mlet) {
    case ' ':
        sprintf(buf, "%s's ghost", (char *)mtmp->mextra);
        
        break;
    case '@':
        if(mtmp->isshk != 0) {
            strcpy(buf, shkname());

            break;
        }

        /* Fall int next case */
    default:
        if(mtmp->minvis != 0) {
            sprintf(buf, "the %s%s", "invisible ", mtmp->data->mname);
        }
        else {
            sprintf(buf, "the %s%s", "", mtmp->data->mname);
        }
    }

    if((vb != 0) && (mtmp->mnamelth != 0)) {
        strcat(buf, " called ");
        strcat(buf, NAME(mtmp));
    }

    return buf;
}

char *lmonnam(struct monst *mtmp)
{
    return xmonnam(mtmp, 1);
}

char *monnam(struct monst *mtmp)
{
    return xmonnam(mtmp, 0);
}

char *Monnam(struct monst *mtmp)
{
    char *bp = monnam(mtmp);

    if(('a' <= *bp) && (*bp <= 'z')) {
        *bp += ('A' - 'a');
    }

    return bp;
}

char *amonnam(struct monst *mtmp, char *adj)
{
    char *bp = monnam(mtmp);

    /* %% */
    static char buf[BUFSZ];

    if(strncmp(bp, "the ", 4) == 0) {
        bp += 4;
    }

    sprintf(buf, "the %s %s", adj, bp);

    return buf;
}

char *Amonnam(struct monst *mtmp, char *adj)
{
    char *bp = amonnam(mtmp, adj);

    *bp = 'T';

    return bp;
}

char *Xmonnam(struct monst *mtmp)
{
    char *bp = Monnam(mtmp);

    if(strncmp(bp, "The ", 4) == 0) {
        bp += 2;
        *bp = 'A';
    }

    return bp;
}

char *visctrl(char c)
{
    static char ccc[3];
    
    if(c < 040) {
        ccc[0] = '^';
        ccc[1] = c + 0100;
        ccc[2] = 0;
    }
    else {
        ccc[0] = c;
        ccc[1] = 0;
    }

    return ccc;
}
