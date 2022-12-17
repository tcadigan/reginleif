/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "mklev.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "alloc.h"
#include "def.trap.h"
#include "mklv.makemon.h"
#include "hack.mkobj.h"
#include "hack.o_init.h"
#include "mklev.h"
#include "mklv.makemaz.h"
#include "mklv.shk.h"
#include "rnd.h"
#include "mklv.savelev.h"

char *tfile;
char *tspe;
char **args;

char nul[40];

#ifdef WIZARD
boolean wizard;
#endif

#define somex() ((rand() % ((croom->hx - croom->lx) + 1)) + croom->lx)
#define somey() ((rand() % ((croom->hy - croom->ly) + 1)) + croom->ly)

struct rm levl[COLNO][ROWNO];
struct monst *fmon;
struct obj *fobj;
struct gen *fgold;
struct gen *ftrap;

char *fut_geno; /* Monsters that should not be created anymore */

struct mkroom rooms[MAXNROFROOMS + 1];
struct mkroom *croom;
struct mkroom *troom;
coord doors[DOORMAX];
int doorindex = 0;

xchar dlevel;

/* For corridors and other things... */
schar nxcor;
schar xx;
schar yy;
schar dx;
schar dy;
schar tx;
schar ty;

boolean goldseen;
int nroom;

xchar xdnstair;
xchar xupstair;
xchar ydnstair;
xchar yupstair;

int main(int argc, char *argv[])
{
    unsigned int tryct;

    if(argc < 6) {
        panic("Too few arguments!");
    }

    args = argv;
    tfile = argv[1];
    tspe = argv[2];
    dlevel = atoi(argv[3]);

    if(dlevel < 1) {
        panic("Bad level");
    }

    fut_geno = argv[4];

#ifdef WIZARD
    if(argv[5][0] == 'w') {
        wizard = 1;
    }
    else {
        wizard = 0;
    }
#endif

    srand(getpid());

    init_objects();

    rooms[0].hx = -1; /* In case we are in a maze */

    /* a: normal; b: maze */
    if(*tspe == 'b') {
        makemaz();

        savelev();

        return 0;
    }

    /* Construct the rooms */
    while(nroom < (MAXNROFROOMS / 3)) {
        croom = rooms;
        nroom = 0;

        makerooms(0); /* Not secret */
    }

    /* For each room: put things inside */
    for(croom = rooms; croom->hx > 0; ++croom) {
        /* Put a sleeing monster inside */
        if(rn2(3) == 0) {
            makemon((struct permonst *)0, somex(), somey());
        }

        /* Put traps and mimics inside */
        goldseen = FALSE;

        while(rn2(8 - (dlevel / 6)) == 0) {
            mktrap(0, 0);
        }

        if((goldseen == 0) && (rn2(3) == 0)) {
            mkgold(0, somex(), somey());
        }

        if(rn2(3) == 0) {
            mkobj_at(0, somex(), somey());
            tryct = 0;

            while(rn2(5) == 0) {
                ++tryct;

                if(tryct > 100) {
                    printf("tryct overflow 4\n");

                    break;
                }

                mkobj_at(0, somex(), somey());
            }
        }
    }

    tryct = 0;

    ++tryct;
    if(tryct > 1000) {
        panic("Cannot make dnstairs\n");
    }

    croom = &rooms[rn2(nroom)];

    xdnstair = somex();
    ydnstair = somey();

    while(((*tspe == 'n') && ((xdnstair % 2 == 0) || (ydnstair % 2 == 0)))
          || g_at(xdnstair, ydnstair, ftrap)) {
        ++tryct;
        if(tryct > 1000) {
            panic("Cannot make dnstair");
        }

        croom = &rooms[rn2(nroom)];

        xdnstair = somex();
        ydnstair = somey();
    }

    levl[(int)xdnstair][(int)ydnstair].scrsym = '>';
    levl[(int)xdnstair][(int)ydnstair].typ = STAIRS;

    troom = croom;

    ++tryct;
    if(tryct > 2000) {
        panic("Cannot make upstairs\n");
    }

    croom = &rooms[rn2(nroom)];

    xupstair = somex();
    yupstair = somey();

    while((croom == troom)
          || m_at(xupstair, yupstair)
          || g_at(xupstair, yupstair, ftrap)) {
        ++tryct;
        if(tryct > 2000) {
            panic("Cannot make upstairs\n");
        }

        croom = &rooms[rn2(nroom)];

        xupstair = somex();
        yupstair = somey();
    }

    levl[(int)xupstair][(int)yupstair].scrsym = '<';
    levl[(int)xupstair][(int)yupstair].typ = STAIRS;

    qsort((char *)rooms, nroom, sizeof(struct mkroom), comp);

    croom = rooms;
    troom = croom + 1;
    nxcor = 0;

    mkpos();

    makecor();

    while((croom->hx > 0) && (troom->hx > 0)) {
        makecor();
    }

    /* Make a secret treasure vault, not connected to the rest */
    if(nroom < ((2 * MAXNROFROOMS) / 3)) {
        if(rn2(3) == 0) {
            int x;
            int y;

            croom = &rooms[nroom];
            troom = croom;

            /* Make secret room */
            if(makerooms(1)) {
                troom->rtype = 6; /* Treasure vault */

                for(x = troom->lx; x <= troom->hx; ++x) {
                    for(y = troom->ly; y <= troom->hy; ++y) {
                        mkgold(rnd(dlevel * 100) + 50, x, y);
                    }
                }
            }
        }
    }

#ifdef WIZARD
    if(wizard) {
        if(rn2(3) != 0) {
            mkshop();
        }
        else {
            mkzoo();
        }
    }
    else if((dlevel > 1) && (dlevel < 20) && (rn2(dlevel) < 2)) {
        mkshop();
    }
    else if((dlevel > 6)
            && ((rn2(7) == 0) || (strcmp("david", getlogin()) == 0))) {
        mkzoo();
    }
#else
    if((dlevel > 1) && (dlevel < 20) && (rn2(dlevel) < 2)) {
        mkshop();
    }
    else if((dlevel > 6)
            && ((rn2(7) == 0) || (strcmp("david", getlogin()) == 0))) {
        mkzoo();
    }
#endif

    savelev();

    return 0;
}

int makerooms(int secret)
{
    int lowx;
    int lowy;
    int tryct = 0;

    while((nroom < (MAXNROFROOMS / 2)) || (secret != 0)) {
        for(lowy = rn1(3,3); lowy < (ROWNO - 7); lowy += rn1(2, 4)) {
            for(lowx = rn1(3, 4); lowx < (COLNO - 10); lowx += rn1(2, 7)) {
                if(tryct > 10000) {
                    return 0;
                }

                ++tryct;

                lowy += (rn2(5) - 2);

                if(lowy < 3) {
                    lowy = 3;
                }
                else if(lowy > (ROWNO - 6)) {
                    lowy = ROWNO - 6;
                }


                if(levl[lowx][lowy].typ != 0) {
                    continue;
                }

                if(((secret != 0) && maker(lowx, 1, lowy, 1))
                   || ((secret == 0)
                       && maker(lowx, rn1(9, 2), lowy, rn1(4,2))
                       && ((nroom + 2) > MAXNROFROOMS))) {
                    return 1;
                }
            }
        }
    }

    return 1;
}

int comp(const void *x, const void *y)
{
    struct mkroom *left = (struct mkroom *)x;
    struct mkroom *right = (struct mkroom *)y;

    if(left->lx < right->lx) {
        return -1;
    }

    return (left->lx > right->lx);
}

coord finddpos(int xl, int yl, int xh, int yh)
{
    coord ff;
    int x;
    int y;

    if(xl == xh) {
        ff.x = xl;
    }
    else {
        ff.x = xl + rn2((xh - xl) + 1);
    }

    if(yl == yh) {
        ff.y = yl;
    }
    else {
        ff.y = yl + rn2((yh - yl) + 1);
    }

    if(okdoor(ff.x, ff.y) != 0) {
        return ff;
    }

    if(xl < xh) {
        for(x = xl; x < xh; ++x) {
            if(okdoor(x, ff.y) != 0) {
                ff.x = x;

                return ff;
            }
        }
    }

    if(yl < yh) {
        for(y = yl; y <= yh; ++y) {
            if(okdoor(ff.x, y) != 0) {
                ff.y = y;

                return ff;
            }
        }
    }

    return ff;
}

/*
 * When croom and troom exist, find position for a door in croom
 * and direction for a corridor towards position [tx, ty] in the wall
 * of troom
 */
void mkpos()
{
    coord cc;
    coord tt;

    if((troom->hx < 0) || (croom->hx < 0) || (doorindex >= DOORMAX)) {
        return;
    }

    if(troom->lx > croom->hx) {
        dx = 1;
        dy = 0;
        xx = croom->hx + 1;
        tx = troom->lx - 1;
        cc = finddpos(xx, croom->ly, xx, croom->hy);
        tt = finddpos(tx, troom->ly, tx, troom->hy);
    }
    else if(troom->hy < croom->ly) {
        dy = -1;
        dx = 0;
        yy = croom->ly - 1;
        cc = finddpos(croom->lx, yy, croom->hx, yy);
        ty = troom->hy + 1;
        tt = finddpos(troom->lx, ty, troom->hx, ty);
    }
    else if(troom->hx < croom->lx) {
        dx = -1;
        dy = 0;
        xx = croom->lx - 1;
        tx = troom->hx + 1;
        cc = finddpos(xx, croom->ly, xx, croom->hy);
        tt = finddpos(tx, troom->ly, tx, troom->hy);
    }
    else {
        dy = 1;
        dx = 1;
        yy = croom->hy + 1;
        tx = troom->ly - 1;
        cc = finddpos(croom->lx, yy, croom->hx, yy);
        tt = finddpos(troom->lx, ty, troom->hx, ty);
    }

    xx = cc.x;
    yy = cc.y;
    tx = tt.x;
    ty = tt.y;

    if(levl[xx + dx][yy + dy].typ != 0) {
        if(nxcor != 0) {
            newloc();
        }
        else {
            dodoor(xx, yy, croom);

            xx += dx;
            yy += dy;
        }

        return;
    }

    dodoor(xx, yy, croom);
}

/* If allowable, create a door at [x, y] */
int okdoor(int x, int y)
{
    if((levl[x - 1][y].typ == DOOR)
       || (levl[x + 1][y].typ == DOOR)
       || (levl[x][y + 1].typ == DOOR)
       || (levl[x][y - 1].typ == DOOR)
       || (levl[x - 1][y].typ == SDOOR)
       || (levl[x + 1][y].typ == SDOOR)
       || (levl[x][y - 1].typ == SDOOR)
       || (levl[x][y + 1].typ == SDOOR)
       || ((levl[x][y].typ != HWALL) && (levl[x][y].typ != VWALL))
       || (doorindex >= DOORMAX)) {
        return 0;
    }

    return 1;
}

void dodoor(int x, int y, struct mkroom *aroom)
{
    struct mkroom *broom;
    int tmp;

    if(doorindex >= DOORMAX) {
        panic("DOORMAX exceeded?");
    }

    if((okdoor(x, y) == 0) && (nxcor != 0)) {
        return;
    }

    if(rn2(8) == 0) {
        levl[x][y].typ = SDOOR;
    }
    else {
        levl[x][y].scrsym = '+';
        levl[x][y].typ = DOOR;
    }

    ++aroom->doorct;
    broom = aroom + 1;

    if(broom->hx < 0) {
        tmp = doorindex;
    }
    else {
        for(tmp = doorindex; tmp > broom->fdoor; --tmp) {
            doors[tmp] = doors[tmp - 1];
        }
    }

    ++doorindex;

    doors[tmp].x = x;
    doors[tmp].y = y;

    while(broom->hx >= 0) {
        ++broom->fdoor;

        ++broom;
    }
}

void newloc()
{
    int a;
    int b;
    int tryct = 0;

    ++croom;
    ++troom;

    if((nxcor != 0) || (croom->hx < 0) || (troom->hx < 0)) {
        if(nxcor > rn1(nroom, 4)) {
            ++nxcor;

            croom = &rooms[nroom];

            return;
        }

        ++nxcor;

        ++tryct;
        if(tryct > 100) {
            printf("tryct overflow 5\n");

            croom = &rooms[nroom];

            return;
        }

        a = rn2(nroom);
        b = rn2(nroom);

        croom = &rooms[a];
        troom = &rooms[b];

        while((croom == troom) || ((troom == (croom + 1)) && (rn2(3) == 0))) {
            ++tryct;

            if(tryct > 100) {
                printf("tryct overflow 5\n");

                croom = &rooms[nroom];

                return;
            }

            a = rn2(nroom);
            b = rn2(nroom);

            croom = &rooms[a];
            troom = &rooms[b];
        }
    }

    mkpos();
}

/* Make a trap somewhere (in croom if mazeflag = 0) */
void mktrap(int num, int mazeflag)
{
    struct gen *gtmp;
    int kind;
    int nopierc;
    int nomimic;
    int fakedoor;
    int fakegold;
    int tryct = 0;
    xchar mx;
    xchar my;

    if((num == 0) || (num >= TRAPNUM)) {
        if(dlevel < 4) {
            nopierc = 1;
        }
        else {
            nopierc = 0;
        }

        if((dlevel < 9) || (goldseen != 0)) {
            nomimic = 1;
        }
        else {
            nomimic = 0;
        }

        if(index(fut_geno, 'M') != 0) {
            nomimic = 1;
        }

        kind = rn2((TRAPNUM - nopierc) - nomimic);
        /* Note: PIERC = 7, MIMIC = 8, TRAPNUM = 9 */
    }
    else {
        kind = num;
    }

    if(kind == MIMIC) {
        struct monst *mtmp;

        if((rn2(3) == 0)  && (mazeflag == 0)) {
            fakedoor = 1;
        }
        else {
            fakedoor = 0;
        }

        if((fakedoor == 0) || (rn2(2) == 0)) {
            fakegold = 1;
        }
        else {
            fakegold = 0;
        }

        if(fakegold != 0) {
            goldseen = TRUE;
        }

        ++tryct;

        if(tryct > 200) {
            return;
        }

        if(fakedoor != 0) {
            /* Note: fakedoor maybe on actual door */
            if(rn2(2) != 0) {
                if(rn2(2) != 0) {
                    mx = croom->hx + 1;
                }
                else {
                    mx = croom->lx - 1;
                }

                my = somey();
            }
            else {
                if(rn2(2) != 0) {
                    my = croom->hy + 1;
                }
                else {
                    my = croom->ly - 1;
                }

                mx = somex();
            }
        }
        else if(mazeflag != 0) {
            coord mm;

            mm = mazexy();
            mx = mm.x;
            my = mm.y;
        }
        else {
            mx = somex();
            my = somey();
        }

        while(m_at(mx, my)) {
            ++tryct;

            if(tryct > 200) {
                return;
            }

            if(fakedoor != 0) {
                /* Note fakedoor maybe on actual door */
                if(rn2(2) != 0) {
                    if(rn2(2) != 0) {
                        mx = croom->hx + 1;
                    }
                    else {
                        mx = croom->lx - 1;
                    }

                    my = somey();
                }
                else {
                    if(rn2(2) != 0) {
                        my = croom->hy + 1;
                    }
                    else {
                        my = croom->ly - 1;
                    }

                    mx = somex();
                }
            }
            else if(mazeflag != 0) {
                coord mm;

                mm = mazexy();
                mx = mm.x;
                my = mm.y;
            }
            else {
                my = somex();
                my = somey();
            }
        }

        mtmp = makemon(PM_MIMIC, mx, my);

        if(mtmp != 0) {
            if(fakegold != 0) {
                mtmp->mimic = '$';
            }
            else if(fakedoor != 0) {
                mtmp->mimic = '+';
            }
            else if((mazeflag != 0) && (rn2(2) != 0)) {
                mtmp->mimic = AMULET_SYM;
            }
            else {
                mtmp->mimic = "=/%?![<>"[rn2(9)];
            }
        }

        return;
    }

    gtmp = newgen();
    gtmp->gflag = kind;

    ++tryct;

    if(tryct > 200) {
        printf("tryct overflow 7\n");

        free((char *)gtmp);

        return;
    }

    if(mazeflag != 0) {
        coord mm;

        mm = mazexy();
        gtmp->gx = mm.x;
        gtmp->gy = mm.y;
    }
    else {
        gtmp->gx = somex();
        gtmp->gy = somey();
    }

    while(g_at(gtmp->gx, gtmp->gy, ftrap) != 0) {
        ++tryct;

        if(tryct > 200) {
            printf("tryct overflow 7\n");

            free((char *)gtmp);

            return;
        }

        if(mazeflag != 0) {
            coord mm;

            mm = mazexy();
            gtmp->gx = mm.x;
            gtmp->gy = mm.y;
        }
        else {
            gtmp->gx = somex();
            gtmp->gy = somey();
        }
    }

    gtmp->ngen = ftrap;
    ftrap = gtmp;

    if((mazeflag != 0) && (rn2(10) == 0) && (gtmp->gflag < PIERC)) {
        gtmp->gflag |= SEEN;
    }
}

/* VARARGS1 */
void panic(char *str, ...)
{
    char bufr[BUFSZ];

    va_list args;
    va_start(args, str);
    vsprintf(bufr, str, args);
    va_end(args);
    puts(bufr);

    if(write(1, "\nMKLEV ERROR:  ", 15) != -1) {
	fflush(stdout);
    }

    exit(1);
}

int maker(schar lowx, schar ddx, schar lowy, schar ddy)
{
    int x;
    int y;
    int hix = lowx + ddx;
    int hiy = lowy + ddy;

    if(nroom >= MAXNROFROOMS) {
        return 0;
    }

    if(hix > (COLNO - 5)) {
        hix = COLNO - 5;
    }

    if(hiy > (ROWNO - 4)) {
        hiy = ROWNO - 4;
    }

    while(1) {
        if((hix <= lowx) || (hiy <= lowy)) {
            return 0;
        }

        /* Check area around room (and make room smaller if necessary) */
        for(x = lowx - 4; x <= (hix + 4); ++x) {
            for(y = lowy - 3; y <= (hiy + 3); ++y) {
                if(levl[x][y].typ != 0) {
                    if(rn2(3) != 0) {
                        return 0;
                    }

                    lowx = x + 5;
                    lowy = y + 4;

                    continue;
                }
            }
        }
    }

    /*
     * On low levels the room is lit (usually)
     * secret vaults are always lit
     */
    if(((rnd(dlevel) < 10) && (rn2(77) != 0)) || ((ddx == 1) && (ddy == 1))) {
        for(x = lowx - 1; x <= (hix + 1); ++x) {
            for(y = lowy - 1; y <= (hiy + 1); ++y) {
                levl[x][y].lit = 1;
            }
        }
    }

    croom->lx = lowx;
    croom->hx = hix;
    croom->ly = lowy;
    croom->hy = hiy;
    croom->fdoor = 0;
    croom->doorct = croom->fdoor;
    croom->rtype = croom->doorct;

    for(x = lowx - 1; x <= (hix + 1); ++x) {
        for(y = lowy - 1; y <= (hiy + 1); y += ((hiy - lowy) + 2)) {
            levl[x][y].scrsym = '-';
            levl[x][y].typ = HWALL;
        }
    }

    for(x = lowx; x < (hix + 1); x += ((hix - lowx) + 2)) {
        for(y = lowy; y <= hiy; ++y) {
            levl[x][y].scrsym = '|';
            levl[x][y].typ = VWALL;
        }
    }

    for(x = lowx; x <= hix; ++x) {
        for(y = lowy; y <= hiy; ++y) {
            levl[x][y].scrsym = '.';
            levl[x][y].typ = ROOM;
        }
    }

    ++croom;
    croom->hx = -1;
    ++nroom;

    return 1;
}

void makecor()
{
    int nx;
    int ny;
    struct rm *crm;
    int dix;
    int diy;
    int secondtry = 0;

    while(1) {
        nx = xx + dx;
        ny = yy + dy;

        if((nxcor != 0) && (rn2(35) == 0)) {
            newloc();

            return;
        }

        if((nx == (COLNO - 1))
           || (nx == 0)
           || (ny == 0)
           || (ny == (ROWNO - 1))) {
            if(nxcor != 0) {
                newloc();

                return;
            }
            else {
                printf("Something went wrong. We try again...\n");
                execl("./mklev",
                      args[0],
                      tfile,
                      tspe,
                      args[3],
                      args[4],
                      args[5],
                      NULL);
                panic("Cannont execute ./mklev\n");
            }
        }

        dix = abs(nx - tx);
        diy = abs(ny - ty);

        if((dy != 0) && (dix > diy)) {
            dy = 0;

            if(nx > tx) {
                dx = -1;
            }
            else {
                dx = 1;
            }
        }
        else if((dx != 0) && (diy > dix)) {
            dx = 0;

            if(ny > ty) {
                dy = -1;
            }
            else {
                dy = 1;
            }
        }

        crm = &levl[nx][ny];

        if(crm->typ == 0) {
            if(rn2(100) != 0) {
                crm->typ = CORR;
                crm->scrsym = CORR_SYM;
            }
            else {
                crm->typ = SCORR;
                crm->scrsym = ' ';
            }

            xx = nx;
            yy = ny;

            if((nxcor != 0) && (rn2(50) == 0)) {
                mkobj_at(ROCK_SYM, nx, ny);
            }

            return;
        }

        if((crm->typ == CORR) || (crm->typ == SCORR)) {
            xx = nx;
            yy = ny;

            return;
        }

        if((nx == tx) && (ny = ty)) {
            dodoor(nx, ny, troom);

            newloc();

            return;
        }

        if((secondtry == 0) && ((nx != (xx + dx)) || (ny != (yy + dy)))) {
            ++secondtry;

            continue;
        }
        else {
            ++secondtry;

            break;
        }
    }

    if(dx != 0) {
        if(ty < ny) {
            dy = -1;
        }
        else {
            if(levl[nx + dy][ny - 1].typ == ROOM) {
                dy = 1;
            }
            else {
                dy = -1;
            }
        }

        dx = 0;
    }
    else {
        if(tx < nx) {
            dx = -1;
        }
        else {
            if(levl[nx - 1][ny + dy].typ == ROOM) {
                dx = 1;
            }
            else {
                dx = -1;
            }
        }

        dy = 0;
    }
}

struct monst *m_at(int x, int y)
{
    struct monst *mtmp;

    for(mtmp = fmon; mtmp != NULL; mtmp = mtmp->nmon) {
        if((mtmp->mx == x) && (mtmp->my == y)) {
            return mtmp;
        }
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
