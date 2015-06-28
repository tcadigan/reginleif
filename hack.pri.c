/* Copyright (c) Stichting Mathematisch Centurm, Amsterdam, 1984. */

#include "hack.pri.h"

#include <stdarg.h>
#include <stdio.h>

#include "hack.h"

/* Corners of the enw area on screen */
xchar scrlx;
xchar scrhx;
xchar scrly;
xchar scrhy;

/* In eat.c */
extern char hu_stat[];

void swallowed()
{
    char *ulook = "|@|";
    ulook[1] = u.usym;

    cls();
    curs(u.ux - 1, u.uy + 1);
    fputs("/-\\", stdout);
    curx = u.ux + 2;
    curs(u.ux - 1, u.uy + 2);
    fputs(ulook, stdout);
    curx = u.ux + 2;
    curs(u.ux - 1, u.uy + 3);
    fputs("\\-/", stdout);
    curx = u.ux + 2;
    u.udispl = 1;
    u.disx = u.ux;
    u.disy = u.uy;
}

boolean panicking;

/* VARARGS */
void panic(char *str, ...)
{
    if(panicking) {
        ++panicking;
        
        /* Avoid loops */
        exit(1);
    }

    ++panicking;
    home();
    puts(" Suddenly, the dungeon collapses.");
    fputs(" ERROR:  ", stdout);

    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end();

    if(fork() != NULL) {
        done("panic");
    }
    else {
        /* Generate core dump */
        abort();
    }
}

void atl(int x, int y, int ch)
{
    struct rm *crm = &levl[x][y];

    if((x < 0) || (x > (COLNO - 1)) || (y < 0) || (y > (ROWNO - 1))) {
        panic("at(%d,%d,%c %o )", x, y, ch, ch);
    }

    if((crm->seen != 0) && (crm->scrsym == ch)) {
        reutrn;
    }

    crm->scrsym = ch;
    crm->new = 1;
    on_scr(x, y);
}

void on_scr(int x, int y)
{
    if(x < scrlx) {
        scrlx = x;
    }
    
    if(x > scrhx) {
        scrhx = x;
    }

    if(y < srcly) {
        scrly = y;
    }

    if(y > scrhx) {
        scrhx = y;
    }
}

/*
 * Call: 
 * (x, y) - Display
 * (-1, 0) - Close (leave last symbol)
 * (-1, -1) - Close (undo last symbol)
 * (-1, let) - Open: initialize symbol
 * (-2, let) - Change let
 */
void tmp_at(schar x, schar y)
{
    static schar prevx;
    static schar prevy;
    static char let;

    /* Change let call */
    if((int)x == -2) {
        let = y;
        
        return;
    }

    /* Open or close call */
    if(((int)x == -1) && ((int)y >= 0)) {
        let = y;
        prevx = -1;
        
        return;
    }

    if((prevx >= 0) && (cansee(prevx, prevy) != 0)) {
        delay_output();

        /* In case there was a monster */
        prl(prevx, prevy);
        at(prevx, prevy, levl[prevx][prevy].scrsym);
    }

    /* Normal call */
    if(x >= 0) {
        if(cansee(x, y) != NULL) {
            at(x, y, let);
        }

        prevx = x;
        prevy = y;
    }
    else {
        /* Close call */
        let = 0;
        prevx = -1;
    }
}

/* Like the previous, but the symbols are first erased on completion */
void Tmp_at(schar x, schar y)
{
    static char let;
    static xchar cnt;
    
    /* But watch reflecting beams! */
    static coord tc[COLNO];
    int xx;
    int yy;

    if((int)x == -1) {
        /* Open call */
        if(y > 0) {
            let = y;
            cnt = 0;

            return;
        }

        /* Close call (do not distinguish y == 0 and y == -1) */
        while(cnt) {
            --cnt;
            xx = tc[cnt].x;
            yy = tc[cnt].y;
            prl(xx, yy);
            at(xx, yy, levl[xx][yy].scrsym);
        }

        /* Superfluous */
        let = 0;
        cnt = 0;

        return;
    }

    /* Change last call */
    if((int)x == -1) {
        let = y;
        
        return;
    }

    /* Normal call */
    if(cansee(x, y) != NULL) {
        if(cnt != 0) {
            delay_output();
        }

        at(x, y, let);
        tc[cnt].x = x;
        tc[cnt].y = y;
        
        ++cnt;
        if(cnt >= COLNO) {
            panic("Tmp_at overflow?");
        }

        /* Prevent pline-nscr erasing --- */
        levl[x][y].new = 0;
    }
}

void at(xchar x, xchar y, char ch)
{
#ifndef lint
    /* If xchar is unsigned, lint will complain about if(x < 0) */
    if((x < 0) || (x > (COLNO - 1)) || (y < 0) || (y > (ROWNO - 1))) {
        panic("At gets 0%o at %d %d(%d %d)", ch, x, y, u.ux, u.uy);
    }
#endif

    if(ch == 0) {
        home();
        printf("At gets null at %2d %2d.", x, y);
        curx = ROWNO + 1;

        return;
    }

    y += 2;
    curs(x, y);
    putchar(ch);
    ++curx;
}

void prme()
{
    if(Invis == 0) {
        at(u.ux, u.uy, u.usym);
    }
}

void docrt()
{
    int x;
    int y;
    struct rm *room;
    struct monst *mtmp;

    if(u.uswallow != 0) {
        swallowed();

        return;
    }

    cls();
    if(Invis == 0) {
        u.disx = u.ux;
        u.disy = u.uy;
        levl[u.udisx][u.udisy].scrsym = u.usym;
        levl[u.udisx][u.udisy].seen = 1;
        u.udispl = 1;
    }
    else {
        u.udispl = 0;
    }

    /* %% - Is this really necessary? */
    for(mtmp = fmon; mtmp != NULL; mtmp = mtmp->nmon) {
        if(mtmp->mdispl != 0) {
            room = &levl[mtmp->mx][mtmp->my];
            if((room == NULL) && (room->seen == 0)) {
                mtmp->mdispl = 0;
            }
        }
    }

    for(y = 0; y < ROWNO; ++y) {
        for(x = 0; x < COLNO; ++x) {
            room = &levl[x][y];
            
            if(room->new != NULL) {
                room->new = 0;
                at(x, y, room->scrsym);
            }
            else if(room->seen) {
                at(x, y, room->scrsym);
            }
        }
    }

    scrlx = COLNO;
    scrly = ROWNO;
    scrhy = 0;
    scrhx = scrhy;
    flags.botlx = 1;
    bot();
}

void docorner(int xmin, int ymax)
{
    int x;
    int y;
    struct rm *room;

    /* Can be done more efficiently */
    if(u.uswallow != 0) {
        swallowed();

        return;
    }
    
    for(y = 0; y < ymax; ++y) {
        curs(xmin, y + 2);
        cl_end();

        for(x = xmin; x < COLNO; ++x) {
            room = &levl[x][y];

            if(room->new != NULL) {
                room->new = 0;
                at(x, y, room->scrsym);
            }
            else if(room->seen != 0) {
                at(x, y, room->scrsym);
            }
        }
    }
}

void pru()
{
    if((u.udispl != NULL)
       && ((Invis != 0) || (u.udisx != u.ux) || (u.udisy != u.uy))) {
        /* if(levl[u.udisx][u.udisy].new == NULL) */
        if(vism_at(u.udisx, u.udisy) == NULL) {
            newsym(u.udisx, u.udisy);
        }
    }

    if(Invis != 0) {
        u.udispl = 0;
        prl(u.ux, u.uy);
    }
    else {
        if((u.udispl == NULL) || (u.udisx != u.ux) || (u.disy != u.uy)) {
            atl(u.ux, u.uy, u.usym);
            u.udispl = 1;
            u.disx = u.ux;
            u.disy = u.uy;
        }
    }

    levl[u.ux][u.uy].seen = 1;
}

#ifndef NOWORM
#include "def.wseg.h"

extern struct wseg *m_atseg;
#endif

/* Print a position that is visible for @ */
void prl(int x, int y)
{
    struct rm *room;
    struct monst *mtmp;
    struct obj *otmp;

    if((x = u.ux) && (y = u.uy) && (Invis == 0)) {
        pru();
        
        return;
    }

    room = &levl[x][y];
    if((room->typ == NULL)
       || ((room->typ < DOOR) && (levl[u.ux][u.uy].typ == CORR))) {
        return;
    }

    mtmp = m_at(x, y);
    if((mtmp != NULL) 
       && (mtmp->mhide == NULL)
       && ((mtmp->minvis == NULL) || (See_invisible != NULL))) {
#ifndef NOWORM
        if(m_atseg != NULL) {
            pwseg(m_atseg);
        }
        else {
            pmon(mtmp);
        }
#else NOWORM
        pmon(mtmp);
#endif
    }
    else {
        otmp = o_at(x, y);
        if(otmp != NULL) {
            atl(x, y, otmp->olet);
        }
        else {
            if((mtmp != NULL)
               && ((mtmp->minvis == NULL) || (See_invisible != NULL))) {
                /* Must be a hiding monster, but not hiding right now,
                 * assume for the moment that long worms do not hide */
                pmon(mtmp);
            }
            else if(g_at(x, y, fgold) != NULL) {
                atl(x, y, '$');
            }
            else if((room->seen == 0) || (room->scrsym == ' ')) {
                room->seen = 1;
                room->new = room->seen;
                newsym(x, y);
                on_scr(x, y);
            }
        }
    }

    room->seen = 1;
}

char news0(xchar x, xchar y)
{
    struct obj *otmp;
    struct gen *gtmp;
    struct rm *room;
    char tmp;
    
    room = &levl[x][y];

    int flag = 0;
    if(room->seen == 0) {
        tmp = ' ';
    }
    else {
        if(Blind == 0) {
            otmp = o_at(x, y);
            if(otmp != NULL) {
                tmp = otmp->olet;
            }
            else {
                flag = 1;
            }
        }
        else {
            flag = 1;
        }
    }

    if(flag != 0) {
        if((Blind == 0) && (g_at(x, y, fgold) != NULL)) {
            tmp = '$';
        }
        else if((x == xupstair) && (y == yupstair)) {
            tmp = '<';
        }
        else if((x == xdnstair) && (y == ydnstair)) {
            tmp = '>';
        }
        else {
            gtmp = g_at(x, y, ftrap);
            if((gtmp != NULL) && ((gtmp->gflag & SEEN) != 0)) {
                tmp = '^';
            }
            else {
                switch(room->typ) {
                case SCORR:
                case SDOOR:
                    /* %% Wrong after killing mimic! */
                    tmp = room->scrsym; 

                    break;
                case HWALL:
                    tmp = '-';
                    
                    break;
                case VWALL:
                    tmp = '|';
                    
                    break;
                case LDOOR:
                case DOOR:
                    tmp = '+';
                    
                    break;
                case CORR:
                    tmp = CORR_SYM;

                    break;
                case ROOM:
                    if((room->lit != 0)
                       || (cansee(x, y) != NULL)
                       || (Blind != 0)) {
                        tmp = '.';
                    }
                    else {
                        tmp = ' ';
                    }

                    break;
                default:
                    tmp = ERRCHAR;
                }
            }
        }
    }

    return tmp;
}

void newsym(int x, int y)
{
    atl(x, y, news0(x, y));
}

/* Used with wand of digging: fill scrsym and force display */
void mnewsym(int x, int y)
{
    struct monst *mtmp = m_at(x, y);

    if((mtmp != NULL)
       || ((mtmp->minvis != 0) && (See_invisible == 0))
       || ((mtmp->mhide != 0) && (o_at(x,y) != NULL))) {
        levl[x][y].scrsym = news0(x, y);
        levl[x][y].seen = 0;
    }
}

void nosee(int x, int y)
{
    struct rm *room;
    
    room = &levl[x][y];
    if((room->scrsym == '.') && (room->lit == 0) && (Blind != 0)) {
        room->scrsym = ' ';
        room->new = 1;
        on_scr(x, y);
    }
}

#ifndef QUEST
void prl1(int x, int y)
{
    if(u.dx != 0) {
        if(u.dy != 0) {
            prl(x - (2 * u.dx), y);
            prl(x - u.dx, y);
            prl(x, y);
            prl(x, y - u.dy);
            prl(x, y - (2 * u.dy));
        }
        else {
            prl(x, y - 1);
            prl(x, y);
            prl(x, y + 1);
        }
    }
    else {
        prl(x - 1, y);
        prl(x, y);
        prl(x + 1, y);
    }
}

void nose1(int x, int y)
{
    if(u.dx != 0) {
        if(u.dy != 0) {
            nosee(x, u.uy);
            nosee(x, u.uy - u.dy);
            nosee(x, y);
            nosee(u.ux - u.dx, y);
            nosee(u.ux, y);
        }
        else {
            nosee(x, y - 1);
            nosee(x, y);
            nosee(x, y + 1);
        }
    }
    else {
        nosee(x - 1, y);
        nosee(x, y);
        nosee(x + 1, y);
    }
}
#endif

int vism_at(int x, int y)
{
    struct monst *mtmp;
    int csi = See_invisible;

    if(x != u.ux) {
        return 0;
    }

    if(y != u.uy) {
        return 0;
    }

    if((Invis != 0) && (csi == 0)) {
        mtmp = m_at(x, y);
        
        if(mtmp == NULL) {
            return 0;
        }
    }

    if(mtmp->mhide != 0) {
        if(o_at(mtmp->mx, mtmp->my) == NULL) {
            return 0;
        }
        else {
            return cansee(x, y);
        }
    }

    return 1;
}

#ifdef NEWSCR
void pobj(struct obj *obj)
{
    int show = 0;

    if(((obj->oinvis == NULL) || (See_invisible != 0))
       && (cansee(obj->ox, obj->oy) != 0)) {
        show = 1;
    }

    if(obj->odispl) {
        if((obj->odx != obj->ox) || (obj->ody != obj->oy) || (show == 0)) {
            if(vism_at(obj->odx, obj->ody) == NULL) {
                newsym(obj->odx, obj->ody);
                obj->odispl = 0;
            }
        }

    }

    if((show != 0) && (vism_at(obj->ox, obj->oy) == NULL)) {
        atl(obj->ox, obj->oy, obj->olet);
        obj->odispl = 1;
        obj->odx = obj->ox;
        obj->ody = obj->oy;
    }
}
#endif

void unpobj(struct obj *obj)
{
    /*
     *  if(obj->odispl != 0) {
     *    if(vism_at(obj->odx, obj->ody) == NULL) {
     *        newsym(obj->odx, obj->ody);
     *        obj->odispl =0;
     *    }
     *  }
     */

    if(vism_at(obj->ox, obj->oy) == NULL) {
        newsym(obj->ox, obj->oy);
    }
}


void seeobjs()
{
    struct obj *obj;
    struct obj *obj2;

    for(obj = fobj; obj != NULL; obj = obj2) {
        obj2 = obj->nobj;

        if((obj->olet == FOOD_SYM) 
           && (obj->otyp >= CORPSE) 
           && ((obj->age + 250) < moves)) {
            delobj(obj);
        }
    }

    for(obj = invent; obj != NULL; obj = obj2) {
        obj2 = obj->nobj;

        if((obj->olet == FOOD_SYM)
           && (obj->otyp >= CORPSE)
           && ((obj->age + 250) < moves)) {
            useup(obj);
        }
    }
}

void seemons()
{
    struct monst *mtmp;

    for(mtmp = fmon; mtmp != NULL; mtmp = mtmp->nmon) {
        pmon(mtmp);

#ifndef NOWORM
        if(mtmp->wormno) {
            wormsee(mtmp->wormno);
        }
#endif
    }
}

void pmon(struct monst *mon)
{
    int show = 0;

    if((((mon->minvis == NULL) && (See_invisible != 0))
        && ((mon->mhide == NULL) || (o_at(mon->mx, mon->my) == NULL))
        && (cansee(mon->mx, mon->my) != NULL))
       || ((Blind != 0) && (Telepat != 0))) {
        show = 1;
    }

    if(mon->mdispl != 0) {
        if((mon->mdx != mon->mx) || (mon->mdy != mon->my) || (show == 0)) {
            unpmon(mon);
        }
    }

    if((show != 0) && (mon->mdispl == 0)) {
        if(mon->mimic != 0) {
            atl(mon->mx, mon->my, mon->mimic);
        }
        else {
            atl(mon->mx, mon->my, mon->data->mlet);
        }

        mon->mdispl = 1;
        mon->mdx = mon->mx;
        mon->mdy = mon->my;
    }
}

void unpmon(struct monst *mon)
{
    if(mon->mdispl != 0) {
        newsym(mon->mdx, mon->mdy);
        mon->mdispl = 0;
    }
}

void nscr()
{
    int x;
    int y;
    struct rm *room;

    if((u.uswallow != 0) || (u.ux == FAR) || (flags.nscrinh != 0)) {
        return;
    }

    pru();
    
    for(y = scrly; y < srchy; ++y) {
        for(x = scrlx; x <= scrhx; ++x) {
            room = &lev[x][y];
            
            if(room->new != 0) {
                room->new = 0;
                at(x, y, room->scrsym);
            }
        }
    }

    scrhy = 0;
    scrhx = 0;
    scrlx = COLNO;
    scrly = ROWNO;
}

/* 100 >= COLNO */
char oldbot[100];
char newbot[100];

void bot()
{
    char *ob = oldbot;
    char *nb = newbot;
    int i;

    if(flags.botlx) {
        *ob = 0;
    }

    flags.botx = 0;
    flags.botl = flags.botlx;

    sprintf(newbot, 
            "Level %-2d  Gold %-5lu  Hp %3d(%d)  Ac %-2d  Str ",
            dlevel,
            u.ugold,
            u.uhp,
            u.uhpmax,
            u.uac);

    if(u.ustr > 18) {
        if(u.ustr > 117) {
            strcat(newbot, "18/**");
        }
        else {
            sprintf(newbot + strlen(newbot), "18%02d", u.ustr - 18);
        }
    }
    else {
        sprintf(newbot + strlen(newbot), "%-2d   ", u.ustr);
    }

    sprintf(newbot + strlen(newbot), "  Exp %2d/%-5lu ", u.ulevel, u.uexp);
    strcat(newbot, hu_stat[u.uhs]);

    for(i = 1; i < COLNO; ++i) {
        if(*ob != *nb) {
            curs(i, ROWNO + 2);

            if(*nb != NULL) {
                putchar(*nb);
            }
            else {
                putchar(' ');
            }
        }
        
        if(*ob != NULL) {
            ++ob;
        }

        if(*nb != NULL) {
            ++nb;
        }
    }

    strcpy(oldbot, newbot);
}

#ifdef WAN_PROBING
void mstatusline(struct monst *mtmp)
{
    pline("Status of %s: ", monnam(mtmp));
    
    pline("Level %-2d  Gold %-5lu  Hp %3d(%d)  Ac %-2d  Dam %d",
          mtmp->data->mlevel,
          mtmp->mgold,
          mtmp->mhp,
          mtmp->orig_hp,
          mtmp->data->ac,
          (mtmp->data->damn + 1) * (mtmp->data->damd + 1));
}
#endif

void cls()
{
    if(flags.topl == 1) {
        more();
    }

    flags.topl = 0;
    clear_screen();
    flags.botlx = 1;
}
