/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.engrave.h"

#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "hack.h"
#include "hack.invent.h"
#include "hack.lev.h"
#include "hack.main.h"
#include "hack.objnam.h"
#include "hack.topl.h"
#include "hack.tty.h"
#include "makedefs.h"
#include "rnd.h"
#include "savelev.h"

extern char *nomovemsg;
extern char nul[];

struct engr *head_engr;

struct engr *engr_at(xchar x, xchar y)
{
    struct engr *ep = head_engr;

    while(ep != NULL) {
        if((x = ep->engr_x) && (y == ep->engr_y)) {
            return ep;
        }

        ep = ep->nxt_engr;
    }

    return NULL;
}

int sengr_at(char *s, xchar x, xchar y)
{
    struct engr *ep = engr_at(x, y);
    char *t;
    int n;

    if((ep != NULL) && (ep->engr_time <= moves)) {
        t = ep->engr_txt;

        /*
         * if(strcmp(s, t) == 0) {
         *     return 1;
         * }
         */

        n = strlen(s);

        while(*t != 0) {
            if(strncmp(s, t, n) == 0) {
                return 1;
            }
        }
    }

    return 0;
}

void wipe_engr_at(xchar x, xchar y, xchar cnt)
{
    struct engr *ep = engr_at(x, y);
    int lth;
    int pos;
    char ch;

    if(ep != NULL) {
        if(ep->engr_type != DUST) {
            if(rn2(1 + (50 / (cnt + 1))) != 0) {
                cnt = 0;
            }
            else {
                cnt = 1;
            }
        }

        lth = strlen(ep->engr_txt);

        if((lth != 0) && (cnt > 0)) {
            while(cnt) {
                --cnt;
                pos = rn2(lth);
                ch = ep->engr_txt[pos];

                if(ch == ' ') {
                    continue;
                }

                if(ch != '?') {
                    ep->engr_txt[pos] = '?';
                }
                else {
                    ep->engr_txt[pos] = ' ';
                }
            }
            --cnt;
        }

        while((lth != 0) && (ep->engr_txt[lth - 1] == ' ')) {
            --lth;
            ep->engr_txt[lth] = 0;
        }

        while(ep->engr_txt[0] == ' ') {
            ++ep->engr_txt;
        }

        if(ep->engr_txt[0] == 0) {
            del_engr(ep);
        }
    }
}

void read_engr_at(int x, int y)
{
    struct engr *ep = engr_at(x, y);

    if((ep != NULL) && (ep->engr_txt[0] != 0)) {
        switch(ep->engr_type) {
        case DUST:
            pline("Something is written here in the dust.");
            
            break;
        case ENGRAVE:
            pline("Something is engraved here on the floor");

            break;
        case BURN:
            pline("Some text has been burned here in the floor.");

            break;
        default:
            pline("Something is written in a very strange way.");
            impossible();
        }

        pline("You read: \"%s\".", ep->engr_txt);
    }
}

int doengrave()
{
    int len;
    char *sp;
    struct engr *ep;
    struct engr *oep = engr_at(u.ux, u.uy);
    char buf[BUFSZ];
    xchar type;

    /* Number of leading spaces */
    int spct;
    struct obj *otmp;
    
    multi = 0;

    /* One may write with finger, weapon, or wand */
    otmp = getobj("#-)/", "write with");

    if(otmp == NULL) {
        return 0;
    }

    if(otmp == (struct obj *)-1) {
        type = DUST;
    }
    else if((otmp->otyp == WAN_FIRE) && (otmp->spe != 0)) {
        type = BURN;
        --otmp->spe;
    }
    else if((otmp->otyp == DAGGER)
            || (otmp->otyp == TWO_HANDED_SWORD)
            || (otmp->otyp == CRYSKNIFE)
            || (otmp->otyp == LONG_SWORD)
            || (otmp->otyp == AXE)) {
        type = ENGRAVE;

        if((int)otmp->spe <= -3) {
            type = DUST;

            pline("You %s too dull for engraving.", aobjnam(otmp, "are"));

            if((oep != NULL) && (oep->engr_type != DUST)) {
                return 1;
            }
        }
    }
    else {
        type = DUST;
    }

    if((oep != NULL) && (oep->engr_type == DUST)) {
        pline("You wipe out the message that was written here.");
        del_engr(oep);
        oep = 0;
    }

    if((type == DUST) && (oep != NULL)) {
        if(oep->engr_type == BURN) {
            pline("You cannot wipe out the message that is %s in the rock.",
                  "burned");
        }
        else {
            pline("You cannot wipe out the message that is %s in the rock.",
                  "engraved");
        }

        return 1;
    }

    if(type == ENGRAVE) {
        pline("What do you want to %s on the floor here? ", "engrave");
    }
    else if(type == BURN) {
        pline("What do you want to %s on the floor here? ", "burn");
    }
    else {
        pline("What do you want to %s on the floor here? ", "write");
    }

    getlin(buf);
    clrlin();
    spct = 0;
    sp = buf;
    
    while(*sp == ' ') {
        ++spct;
        ++sp;
    }

    len = strlen(sp);

    if(len == 0) {
        if(type == BURN) {
            ++otmp->spe;
        }

        return 0;
    }

    switch(type) {
    case DUST:
    case BURN:
        if(len > 15) {
            multi = -(len / 10);

            nomovemsg = "You finished writing.";
        }

        break;
    case ENGRAVE:
        {
            int len2 = ((otmp->spe + 3) * 2) + 1;
            char *bufp = doname(otmp);

            if(digit(*bufp) != 0) {
                pline("Your %s get dull.", bufp);
            }
            else {
                if(strncmp(bufp, "a ", 2) == 0) {
                    bufp += 2;
                }
                else if(strncmp(bufp, "an ", 3) == 0) {
                    bufp += 3;
                }
                 
                pline("You %s gets dull.", bufp);
            }

            if(len2 < len) {
                len = len2;
                sp[len] = 0;
                otmp->spe = -3;
                nomovemsg = "You cannot engrave more.";
            }
            else {
                otmp->spe -= (len / 2);
                nomovemsg = "You finished engraving.";
            }

            multi = -len;
        }

        break;
    }

    if(oep != NULL) {
        len += (strlen(oep->engr_txt) + spct);
    }

    ep = (struct engr *)alloc((unsigned int)((sizeof(struct engr) + len) + 1));
    ep->nxt_engr = head_engr;
    head_engr = ep;
    ep->engr_x = u.ux;
    ep->engr_y = u.uy;

    /* (char *)ep + sizeof(struct engr) */
    sp = (char *)(ep + 1);
    ep->engr_txt = sp;

    if(oep != NULL) {
        strcpy(sp, oep->engr_txt);
        strcpy(sp, buf);
        del_engr(oep);
    }
    else {
        strcpy(sp, buf);
    }

    ep->engr_lth = len + 1;
    ep->engr_type = type;
    ep->engr_time = moves - multi;
        
    /* Kludge to protect pline against excessively long texts */
    if(len > (BUFSZ - 20)) {
        sp[BUFSZ] = 0;
    }

    return 1;
}

void save_engravings(int fd)
{
    struct engr *ep = head_engr;

    while(ep != NULL) {
        if((ep->engr_lth == 0) || (ep->engr_txt[0] == 0)) {
            ep = ep->nxt_engr;

            continue;
        }

        bwrite(fd, (char *)&(ep->engr_lth), sizeof(ep->engr_lth));
        bwrite(fd, (char *)ep, sizeof(struct engr) + ep->engr_lth);

        ep = ep->nxt_engr;
    }

    bwrite(fd, (char *)nul, sizeof(unsigned int));
}

void rest_engravins(int fd)
{
    struct engr *ep;
    unsigned int lth;
    head_engr = 0;
    
    while(1) {
        mread(fd, (char *)&lth, sizeof(unsigned int));

        if(lth == 0) {
            return;
        }

        ep = (struct engr *)alloc(sizeof(struct engr) + lth);
        mread(fd, (char *)ep, sizeof(struct engr) + lth);
        ep->nxt_engr = head_engr;
        head_engr = ep;
    }
}

void del_engr(struct engr *ep)
{
    struct engr *ept;

    if(ep == head_engr) {
        head_engr = ep->nxt_engr;
    }
    else {
        int flag = 0;
        for(ept = head_engr; ept != NULL; ept = ept->nxt_engr) {
            if(ept->nxt_engr == ep) {
                ept->nxt_engr = ep->nxt_engr;

                flag = 1;
                break;
            }
        }

        if(flag == 0) {
            pline("Error in del_engr?");
            impossible();
        }
    }

    free(ep);
}

    

                
              
        
            
