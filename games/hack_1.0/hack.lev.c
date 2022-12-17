/* Copyright (c) Stichting MAthematisch Centrum, Amsterdam, 1984. */

#include "hack.lev.h"

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "alloc.h"
#include "hack.h"
#include "hack.bones.h"
#include "hack.end.h"
#include "hack.engrave.h"
#include "hack.mon.h"
#include "hack.pri.h"
#include "hack.save.h"
#include "hack.shk.h"
#include "hack.topl.h"
#include "hack.tty.h"
#include "hack.vault.h"
#include "rnd.h"
#include "savelev.h"

extern struct obj *billobjs;
extern char nul[];

#ifndef NOWORM
#include "def.wseg.h"

extern struct wseg *wsegs[32];
extern struct wseg *wheads[32];
extern long wgrowtime[32];
#endif

#ifdef BSD
#include <sys/wait.h>
#else
#include <wait.h>
#endif

int getlev(int fd)
{
    struct gen *gtmp;

#ifndef NOWORM
    struct wseg *wtmp;
#endif

    int tmp;
    long omoves;

    if((fd < 0) || (read(fd, (char *)levl, sizeof(levl)) != sizeof(levl))) {
        return 1;
    }

    fgold = 0;
    ftrap = 0;

    /* 0 from MKLEV */
    mread(fd, (char *)&omoves, sizeof(omoves));
    mread(fd, (char *)&xupstair, sizeof(xupstair));
    mread(fd, (char *)&yupstair, sizeof(yupstair));
    mread(fd, (char *)&xdnstair, sizeof(xdnstair));
    mread(fd, (char *)&ydnstair, sizeof(ydnstair));

    fmon = restmonchn(fd);

    if(omoves != 0) {
        /* Regenerate animals on another level */
        long tmoves = 0;
        if(moves > omoves) {
            tmoves = moves - omoves;
        }

        struct monst *mtmp;
        struct monst *mtmp2;
        extern char genocided[];

        for(mtmp = fmon; mtmp != NULL; mtmp = mtmp2) {
            mtmp2 = mtmp->nmon;

            if(index(genocided, mtmp->data->mlet) != 0) {
                mondead(mtmp);

                continue;
            }

            if(index("ViT", mtmp->data->mlet) != 0) {
                mtmp->mhp += tmoves;
            }
            else {
                mtmp->mhp += (tmoves / 20);
            }

            if(mtmp->mhp > mtmp->orig_hp) {
                mtmp->mhp = mtmp->orig_hp;
            }
        }
    }

    setshk();
    setgd();
    gtmp = newgen();
    mread(fd, (char *)gtmp, sizeof(struct gen));

    while(gtmp->gx != 0) {
        gtmp->ngen = fgold;
        fgold = gtmp;
        gtmp = newgen();
        mread(fd, (char *)gtmp, sizeof(struct gen));
    }

    mread(fd, (char *)gtmp, sizeof(struct gen));
    while(gtmp->gx != 0) {
        gtmp->ngen = ftrap;
        ftrap = gtmp;
        gtmp = newgen();
        mread(fd, (char *)gtmp, sizeof(struct gen));
    }

    free(gtmp);
    fobj = restobjchn(fd);
    billobjs = restobjchn(fd);
    rest_engravings(fd);

#ifndef QUEST
    mread(fd, (char *)rooms, sizeof(rooms));
    mread(fd, (char *)doors, sizeof(doors));
#endif

    if(omoves == 0) {
        /* From MKLEV */
        return 0;
    }

#ifndef NOWORM
    mread(fd, (char *)wsegs, sizeof(wsegs));
    for(tmp = 1; tmp < 32; ++tmp) {
        if(wsegs[tmp] != 0) {
            wtmp = newseg();
            wsegs[tmp] = wtmp;
            wheads[tmp] = wsegs[tmp];

            while(1) {
                mread(fd, (char *)wtmp, sizeof(struct wseg));

                if(wtmp->nseg == 0) {
                    break;
                }

                wtmp = newseg();
                wheads[tmp]->nseg = wtmp;
                wheads[tmp] = wtmp;
            }
        }
    }

    mread(fd, (char *)wgrowtime, sizeof(wgrowtime));
#endif

    return 0;
}

void mread(int fd, char *buf, unsigned int len)
{
    int rlen;
    rlen = read(fd, buf, (int)len);

    if(rlen != len) {
        pline("Read %d instead of %d bytes\n", rlen, len);

        panic("Cannot read %d bytes from file #%d\n", len, fd);
    }
}

void mklev()
{
    int fd;
    char type[2];
    int status;
    extern char fut_geno[];

    if(getbones() != 0) {
        return;
    }

    if(dlevel < rn1(3, 26)) {
        /* Normal level */
        type[0] = 'a';
    }
    else {
        /* Maze */
        type[0] = 'b';
    }

    type[1] = 0;

    switch(fork()) {
    case 0:
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);

        if(wizard != 0) {
            execl("./mklev",
                  "mklev",
                  lock,
                  type,
                  itoa(dlevel),
                  fut_geno,
                  "w",
                  (char *)0);
        }
        else {
            execl("./mklev",
                  "mklev",
                  lock,
                  type,
                  itoa(dlevel),
                  fut_geno,
                  "",
                  (char *)0);
        }

        exit(2);
    case -1:
        settty("Cannot fork!\n");

        exit(1);
    default:
        /* You fell into a trap ... */
        fflush(stdout);
        wait(&status);
    }

    if(WCOREDUMP(status)) {
        settty("Mklev dumped core. Exiting...\n");

        exit(1);
    }

    if(WTERMSIG(status)) {
        settty("Mklev killed by a signal. Exiting...\n");

        exit(1);
    }

    if(WIFEXITED(status)) {
        if(WEXITSTATUS(status) == 2) {
            settty("Cannot execl mklev.\n");

            exit(1);
        }

        pline("Mklev failed. Let's try again.");
        mklev();

        return;
    }

    fd = open(lock, 0);
    if(fd < 0) {
        pline("Can't open %s!", lock);
        mklev();

        return;
    }

    getlev(fd);
    close(fd);
}
