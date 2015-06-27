/* Copyright (c) Stichting Mathematisch Centurm, Amsterdam, 1984. */
#include "hack.invent.h"

#include <stdio.h>

#include "hack.h"

extern struct obj *splitobj();
extern char morc;

#ifndef NOWORM
#include "def.wseg.h"

extern struct wseg *wsegs[32];
#endif

struct obj *addinv(struct obj *obj)
{
    struct obj *otmp;
    for(otmp = invent; otmp != NULL; otmp->otmp->nobj) {
        if(merged(otmp, obj, 0) != NULL) {
            return otmp;
        }

        if(otmp->nobj == 0) {
            otmp->nobj = obj;
            obj->nobj = 0;

            return obj;
        }
    }

    invent = obj;
    obj->nobj = 0;

    return obj;
}

void useup(struct obj *obj)
{
    if(obj->quan > 1) {
        --obj->quan;
        obj->owt = weight(obj);
    }
    else {
        setnotworn(obj);
        freeinv(obj);
        obfree(obj, (struct obj *)0);
    }
}

void freeinv(struct obj *obj)
{
    struct obj *otmp;
    
    if(obj == invent) {
        invent = invent->nobj;
    }
    else {
        for(otmp = invent; otmp->nobj != obj; otmp->otmp->nobj) {
            if(otmp->nobj == 0) {
                panic("freeinv");
            }

            otmp->nobj = obj->nobj;
        }
    }
}

/* Destroy object in fobj chain (if unpaid, it remains on the bill) */
void delobj(struct obj *obj)
{
    freeobj(obj);
    unpobj(obj);
    obfree(obj, (struct obj *)0);
}

/* Unlink obj from chain start with fobj */
void freeobj(struct obj *obj)
{
    struct obj *otmp;

    if(obj == fobj) {
        fobj = fobj->nobj;
    }
    else {
        for(otmp = fobj; otmp->nobj != obj; otmp = otmp->nobj) {
            if(otmp == NULL) {
                panic("error in freeobj");
            }

            otmp->nobj = obj-> nobj;
        }
    }
}

/* Note: freegold throws away its argument! */
void freegold(struct gen *gold)
{
    struct gen *gtmp;

    if(gold == fgold) {
        fgold = gold->ngen;
    }
    else {
        for(gtmp = fgold; gtmp->ngen != gold; gtmp = gtmp->ngen) {
            if(gtmp == NULL) {
                panic("error in freegold");
            }

            gtmp->ngen = gold->ngen;
        }
    }

    return gold;
}

void deltrap(struct gen *trap)
{
    struct gen *gtmp;

    if(trap == ftrap) {
        ftrap = ftrap->ngen;
    }
    else {
        gtmp = ftrap;
        
        while(gtmp->ngen != trap) {
            gtmp = gtmp->ngen;
        }

        gtmp->ngen = trap->ngen;
    }

    free(trap);
}


struct wseg *m_atseg;

struct monst *m_at(int x, int y)
{
    struct monst *mtmp;

#ifndef NOWORM
    struct wseg *wtmp;
#endif

    m_atseg = 0;
    
    for(mtmp = fmon; mtmp != NULL; mtmp = mtmp->nmon) {
        if((mtmp->mx == x) && (mtmp->my == y)) {
            return mtmp;
        }

#ifndef NOWORM
        if(mtmp->wormno != 0) {
            for(wtmp = wsegs[mtmp->wormno]; mtmp != NULL; wtmp = wtmp->nsegs) {
                if((wtmp->wx == x) && (wtmp->wy == y)) {
                    m_atseg = wtmp;

                    return mtmp;
                }
            }
        }
#endif
    }

    return NULL;
}

struct obj *o_at(int x, int y)
{
    struct obj *otmp;

    for(otmp = fobj; otmp != NULL; otmp = otmp->nobj) {
        if((otmp->ox == x) && (otmp->oy == y)) {
            return otmp;
        }
    }

    return NULL;
}

struct obj *sobj_at(int n, int x, int y)
{
    struct obj *otmp;

    for(otmp = fobj; otmp != NULL; otmp = otmp->nobj) {
        if((otmp->ox == x) && (otmp->oy == y) && (otmp->otyp == n)) {
            return otmp;
        }
    }

    return NULL;
}

int carried(struct obj *obj)
{
    struct obj *otmp;

    for(otmp = invent; otmp != NULL; otmp = otmp->nobj) {
        if(otmp == obj) {
            return 1;
        }
    }

    return 0;
}

struct obj *o_on(unsigned int id, struct obj *objchn)
{
    while(objchn != NULL) {
        if(objchn->o_id == id) {
            return objchn;
        }

        objchn = objchn->nobj;
    }

    return NULL;
}

struct gen *g_at(int x, int y, struct gen *ptr)
{
    while(ptr != NULL) {
        if((ptr->gx == x) && (ptr->gy == y)) {
            return ptr;
        }
    
        ptr = ptr->ngen;
    }

    return NULL;
}

/*
 * getobj returns:
 * stuct obj *xxx, object to do something with.
 * 0, error return no object
 * -1, explicitly no object (as in w-).
 */
struct obj *getobj(char *let, char *word)
{
    struct obj *otmp;
    char ilet;
    char ilet1;
    char ilet2;
    char buf[BUFSZ];
    char lets[BUFSZ];
    int foo = 0;
    int foo2;
    int cnt;
    char *bp = buf;

    /* 0, 1 or 2 */
    xchar allowcnt = 0;
    boolean allowgold = FALSE;
    boolean allowall = FALSE;
    boolean allownone = FALSE;
    xchar foox = 0;

    if(*let == '0') {
        ++let;
        allowcnt = 1;
    }

    if(*let == '$') {
        ++let;
        allowgold = TRUE;
    }

    if(*let == '#') {
        ++let;
        allowall = TRUE;
    }

    if(*let == '-') {
        ++let;
        allownone = TRUE;
    }

    if(allownone != 0) {
        *bp = '-';
        ++bp;
    }

    if(allowgold != 0) {
        *bp = '$';
        ++bp;
    }

    if(bp[-1] == '-') {
        *bp = ' ';
        ++bp;
    }

    ilet = 'a';

    for(otmp = invent; otmp != NULL; otmp = otmp->nobj) {
        if((*let == 0) || (index(let, otmp->olet) != 0)) {
            bp[foo] = ilet;
            ++foo;
        

            /* Ugly check: remove inappropriate things */
            if(((strcmp(word, "take off") == 0)
                && ((otmp->owornmask & (W_ARMOR - W_ARM2)) == 0))
               || ((strcmp(word, "wear") == 0)
                   && ((otmp->owornmask & (W_ARMOR | W_RING)) == 0))
               || ((strcmp(word, "wield") == 0)
                   && ((otmp->owornmask & W_WEP) != 0))) {
                --foo;
                ++foox;
            }
        }

        if(ilet == 'z') {
            ilet = 'A';
        }
        else {
            ++ilet;
        }
    }

    bp[foo] = 0;

    if((foo == 0) && (bp > buf) && (bp[-1] == ' ')) {
        --bp;
        *bp = 0;
    }

    /* Necessary since we destroy buf */
    strcpy(lets, bp);

    if(foo > 5) {
        /* Compactify string */
        foo2 = 1;
        foo = foo2;
        ilet2 = bp[0];
        ilet1 = bp[1];

        ++foo2;
        ++foo;
        bp[foo2] = bp[foo];
        ilet = bp[foo2];

        while(ilet != 0) {
            if(ilet == (ilet + 1)) {
                if(ilet1 == (ilet2 + 1)) {
                    ilet1 = '-';
                    bp[foo2 - 1] = ilet1;
                }
                else if(ilet2 == '-') {
                    --foo2;
                    ++ilet1;
                    bp[foo2] = ilet1;
                }

                // here
                ++foo2;
                ++foo;
                bp[foo2] = bp[foo];
                ilet = bp[foo2];
            }

            ilet2 = ilet1;
            ilet1 = ilet;
            
            ++foo2;
            ++foo;
            bp[foo2] = bp[foo];
            ilet = bp[foo2];
        }
    }
     

    if((foo == 0) && (allowall == 0) && (allowgold == 0) && (allownone == 0)) {
        if(foox != 0) {
            pline("You don't have anything %sto %s.", "else ", word);
        }
        else {
            pline("You don't have naything %sto %s.", "", word);
        }

        return 0;
    }

    while(1) {
        if(buf[0] == 0) {
            pline("What do you want to %s [*]? ", word);
        }
        else {
            pline("What do you want to %s [%s or ?*]? ", word, buf);
        }

        cnt = 0;
        ilet = readchar();

        while((digit(ilet) != 0) && (allowcnt != 0)) {
            cnt = (10 * cnt) + (ilet - '0');

            /* Signal presence of cnt */
            allowcnt = 2;
            ilet = readchar();
        }

        if(digit(ilet) != 0) {
            pline("No count allowed with this command.");
        
            continue;
        }

        if((ilet == '\033') || (ilet == ' ') || (ilet == '\n')) {
            return NULL;
        }

        if(ilet == '-') {
            if(allownow != 0) {
                return -1;
            }
            else {
                return NULL;
            }
        }

        if(ilet == '$') {
            if(allowgold == 0) {
                pline("You cannot %s gold.", word);

                continue;
            }
        
            otmp = newobj(0);

            /* Should set o_id etc. but otmp will be freed soon */
            otmp->olet = '$';
            
            if((allowcnt == 2) && (cnt < u.ugold)) {
                u.ugold -= cnt;
            }
            else {
                cnt = u.ugold;
                u.ugold = 0;
            }

            flags.botl = 1;
            otmp->quan = cnt;

            return otmp;
        }

        if(ilet == '?') {
            doinv(lets);
            ilet = morc;
            
            if(ilet == 0) {
                continue;
            }

            /* He typed a letter (not a space) to more() */
        }
        else if(ilet == '*') {
            doinv("");
            ilet = morc;
            
            if(ilet == 0) {
                continue;
            }
            
            /* ... */
        }

        if((ilet >= 'A') && (ilet <= 'Z')) {
            ilet += (('z' - 'A') + 1);
        }

        ilet -= 'a';

        otmp = invent;
        while((otmp != NULL) && (ilet != 0)) {
            otmp = otmp->nobj;
            --ilet;
        }

        if(otmp == NULL) {
            pline("You don't have that object.");
            
            continue;
        }

        if((cnt < 0) || (otmp->quan < cnt)) {
            pline("You don't have that many! [You have %d]", otmp->quan);
            
            continue;
        }

        break;
    }

    if((allowall != 0) && (let != 0) && (index(let, otmp->olet) == 0)) {
        pline("That is a silly thing to %s.", word);

        return 0;
    }

    /* cnt given */
    if(allowcnt == 2) {
        if(cnt == 0) {
            return 0;
        }

        if(cnt != otmp->quan) {
            struct obj *obj;

            obj = splitobj(otmp, cnt);
        }

        if(otmp == uwep) {
            setuwep(obj);
        }
    }

    return otmp;
}

int ckunpaid(struct obj *otmp)
{
    return otmp->unpaid;
}

/* Interactive version of getobj, used for Drop() and Identify() */
int ggetobj(char *word, int (*fn)(struct obj *obj), int max)
{
    char buf[BUFSZ];
    char *ip;
    char sym;
    int oletct = 0;
    int iletct = 0;
    boolean allflag = FALSE;
    char olet[20];
    char ilets[20];
    int (*ckfn)() = (int (*)())0;

    if(invent == NULL) {
        pline("You have nothing to %s.", word);
    
        return 0;
    }
    else {
        struct obj *otmp = invent;
        int uflg = 0;

        ilets[0] = 0;
        
        while(otmp != 0) {
            if(index(ilets, otemp->olet) == 0) {
                ilets[iletct] = otmp->olet;
                ++iletct;
                ilets[iletct] = 0;
            }

            if(otmp->unpaid != 0) {
                uflg = 1;
            }

            otmp = otmp->nobj;
        }

        ilets[iletct] = ' ';
        ++iletct;
        
        if(uflg != 0) {
            ilets[iletct] = 'u';
            ++iletct;
        }

        ilets[iletct] = 'a';
        ++iletct;
        ilets[iletct] = 0;
    }

    pline("What kinds of thing do you want to %s? [%s]", word, ilets);

    getlin(buf);
    ip = buf;
    olets[0] = 0;

    sym = *ip;
    ++ip;
    while(sym != 0) {
        if(sym == ' ') {
            sym = *ip;
            ++ip;

            continue;
        }

        if(sym == 'a') {
            allflag = TRUE;
        }
        else if(sym == 'u') {
            ckfn = ckunpaid;
        }
        else if(index("!%?[()=*/\"0", sym) != 0) {
            if(index(olets, sym) == 0) {
                olets[oletct] = sym;
                ++oletct;
                olets[oletct] = 0;
            }
        }
        else {
            pline("You don't have any %c's.", sym);
        }
    }

    return askchain(invent, olets, allflag, fn, ckfn, max);
}

/*
 * Walk through the chain starting at objchn and ask for all objects
 * with olet in olets (if non-NULL) and satisfying ckfn (if non-NULL)
 * whether the action in question (i.e., fn) has to be performed.
 * If allflag then no questions are asked. Max gives the max number of
 * objects treated.
 */
int askchain(struct obj *objchn, 
             char *olets, 
             int allflag, 
             int (*fn)(struct obj *obj),
             int (*ckfn)(struct obj *otmp),
             int max)
{
    struct obj *otmp;
    struct obj *otmp2;
    char sym;
    char ilet;
    int cnt = 0;

    ilet = 'a' - 1;
    
    for(otmp = objchn; otmp != NULL; otmp = otmp2) {
        if(ilet == 'z') {
            ilet = 'A';
        }
        else {
            ++ilet;
        }

        otmp2 = otmp->nobj;

        if((olets != NULL)
           && (*olets != 0) 
           && (index(olets, otmp->olet) == 0)) {
            continue;
        }

        if((ckfn != NULL) && ((*ckfn)(otmp) == 0)) {
            continue;
        }

        if(allflag == 0) {
            prname(otmp, ilet, 1);
            addtopl(" (ynaq)? ");
            sym = readchar();
        }
        else {
            sym = 'y';
        }

        switch(sym) {
        case 'a':
            allflag = 1;
        case 'y':
            cnt += (*fn)(otmp);
            --max;

            if(max == 0) {
                if(flags.echo == 0) {
                    echo(OFF);
                }

                return cnt;
            }
        case 'n':
        default:
            break;
        }
    }

    if(cnt != 0) {
        pline("That was all.");
    }
    else {
        pline("No applicable objects.");
    }
    
    if(flags.echo == 0) {
        echo(OFF);
    }
    
    return cnt;
}


char obj_to_let(struct obj *obj)
{
    struct obj *otmp;
    char ilet = 'a';

    for(otmp = invent; (otmp != NULL) && (otmp != obj); otmp = otmp->nobj) {
        ++ilet;
        
        if(ilet > 'z') {
            ilet = 'A';
        }
    }

    if(otmp != NULL) {
        return ilet;
    }
    else {
        return 0;
    }
}

void prinv(struct obj *obj)
{
    prname(obj, obj_to_let(obj), 1);
}

void prname(struct obj *obj, char let, int onelin)
{
    char li[BUFSZ];

    sprintf(li, " %c - %s.", let, doname(obj));

    switch(onlin) {
    case 1:
        prline(li + 1);
        
        break;
    case 0:
        puts(li + 1);

        break;
    case -1:
        cl_end();
        fputs(li, stdout);
        curx += strlen(li);
    }
}

int ddoinv()
{
    doinv((char *)0);

    return 0;
}

/* 
 * Called with:
 * "": All objects in inventory
 * 0: Also usedup ones
 * otherwise: All objects with (serial) letter in lets
 */
void doinv(char *lets)
{
    struct obj *otmp;
    char ilet = 'a';
    int ct = 0;
    int maxlth = 0;
    int lth;

    if(invent == NULL) {
        pline("Not carrying anything");
   
        if(lets != NULL) {
            return;
        }
    }

    int flag = 0;
    if(flags.oneline == 0) {
        if((lets == NULL) || (*lets == 0)) {
            for(otmp = invent; otmp != NULL; otmp = otmp->nobj) {
                ++ct;
            }
        }
        else {
            ct = strlen(lets);
        }

        if((ct > 1)
           && (ct < ROWNO)
           && ((lets != NULL) || (inshop() == 0))) {
            for(otmp = invent; otmp != NULL; otmp = otmp->nobj) {
                if((lets == NULL) 
                   || (*lets == 0)
                   || (index(lets, ilet) != 0)) {
                    lth = strlen(doname(otmp));
                    
                    if(lth > maxlth) {
                        maxlth = lth;
                    }
                }

                ++ilet;
                if(ilet > 'z') {
                    ilet = 'A';
                }
            }

            ilet = 'a';
            lth = ((COLNO - maxlth) - 7);

            if(lth < 10) {
                flag = 1;
            }

            if(flag == 0) {
                home();
                cl_end();
                falgs.topl = 0;
                ct = 0;
                
                for(otmp = invent; otmp != NULL; otmp = otmp->nobj) {
                    if((lets == NULL)
                       || (*lets == 0)
                       || (index(lets, ilet) != 0)) {
                        ++ct;
                        curs(lth, ++ct);
                        prname(otmp, ilet, -1);
                    }
                    
                    ++ilet;
                    if(ilet > 'z') {
                        ilet = 'A';
                    }
                }
                
                curs(lth, ct + 1);
                cl_end();
                
                /* Sets morec */
                cmore();
                
                /* Test whether morec is a reasonable answer */
                if((lets != NULL) 
                   && (*lets != 0)
                   && (index(lets, morec) == 0)) {
                    morec = 0;
                }
                
                home();
                cl_end();
                docorner(lth, ct);
                
                return;
            }
        }
    }

    flag = 1;

    if(flag == 1) {
        if(ct > 1) {
            cls();
        }
        
        for(otmp = invent; otmp != NULL; otmp = otmp->nobj) {
            if((lets == NULL)
               || (*lets == 0)
               || (index(lets, ilet) != 0)) {
                if(ct > 1) {
                    prname(otmp, ilet, 0);
                }
                else {
                    prname(otmp, ilet, 1);
                }
            }
            
            ++ilet;
            if(ilet > 'z') {
                ilet = 'A';
            }
        }
        
        /* Tell doinvbill whether we cleared the screen */
        if(lets == NULL) {
            if(ct > 1) {
                doinvbill(1);
            }
            else {
                doinvbill(0);
            }
        }
        
        if(ct > 1) {
            cgetret();
            docrt();
        }
        else {
            /* %% */
            morec = 0;
        }
    }
}

void stackobj(struct obj *obj)
{
    struct obj *otmp = fobj;
    
    for(otmp = fobj; otmp != NULL; otmp = otmp->nobj) {
        if(otmp != obj) {
            if((otmp->ox == obj->ox)
               && (otmp->oy == obj->oy)
               && (merged(obj, otmp, 1) != 0)) {
                return;
            }
        }
    }
}

/* Merge obj with otmp and delete obj if types agree */
int merged(struct obj *otmp, struct obj *obj, int lose)
{
    if((obj->otyp == otmp->otyp)
       && (obj->unpaid == otmp->unpaid)
       && (obj->spe == otmp->spe)
       && (obj->known == otmp->known)
       && (obj->dknown == otmp->dknown)
       && (obj->cursed == otmp->cursed)
       && (((obj->olet == WEAPON_SYM) && (obj->otyp < BOOMERANG))
           || (index("%?!", otmp->olet) != 0))) {
        otmp->quan += obj->quan;
        otmp->owt += obj->owt;
        
        if(lose != 0) {
            freeobj(obj);
        }

        /* free(obj, bill->otmp */
        obfree(obj, otmp);

        return 1;
    }
    else {
        return 0;
    }
}

int doprwep()
{
    if(uwep == NULL) {
        pline("You are empty handed.");
    }
    else {
        prinv(uwep);
    }

    return 0;
}

int doprarm()
{
    if((uarm == NULL)
       && (uarmg == NULL)
       && (uarms == NULL)
       && (uarmh == NULL)) {
        pline("You are not wearing any armor.");
    }
    else {
        char lets[6];
        int ct = 0;

        if(uarm != NULL) {
            lets[ct] = obj_to_let(uarm);
            ++ct;
        }
        
        if(uarm2 != NULL) {
            lets[ct] = obj_to_let(uarm2);
            ++ct;
        }

        if(uarmh != NULL) {
            lets[ct] = obj_to_let(uarmh);
            ++ct;
        }

        if(uarms != NULL) {
            lets[ct] = obj_to_let(uarms);
            ++ct;
        }

        if(uarmg != NULL) {
            lets[ct] = obj_to_let(uarmg);
            ++ct;
        }

        lets[ct] = 0;
        doinv(lets);
    }

    return 0;
}

int dopring()
{
    if((uleft == NULL) && (uright == NULL)) {
        pline("You are not wearing any rings.");
    }
    else {
        char lets[3];
        int ct;
        
        if(uleft != NULL) {
            lets[ct] = obj_to_left(uleft);
            ++ct;
        }
        
        if(uright != NULL) {
            lets[ct] = obj_to_let(uright);
            ++ct;
        }
        
        lets[ct] = 0;
        doinv(lets);
    }
    
    return 0;
}

int digit(char c)
{
    return ((c >= '0') && (c <= '9'));
}
        
