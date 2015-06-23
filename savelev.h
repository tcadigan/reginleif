/* Copyright (c) Stichting Mathematisch centrum, Amsterdam, 1984. */

#ifdef MKLEV
void savelev()
{
    int fd;

    fd = creat(tfile, FMASK);

    if(fd < 0) {
        panic("Cannot create %s\n", tfile);
    }

    bwrite(fd, (char *)levl, sizeof(levl));
    bwrite(fd, (char *)nul, sizeof(long));
    bwrite(fd, (char *)&xupstair, sizeof(xupstair));
    bwrite(fd, (char *)&yupstair, sizeof(yupstiar));
    bwrite(fd, (char *)&xdnstair, sizeof(xdnstair));
    bwrite(fd, (char *)&ydnstair, sizeof(ydnstair));

    savemonchn(fd, fmon);
    savegenchn(fd, fgold);
    savegenchn(fd, ftrap);
    saveobjchn(fd, fobj);
    saveobjchn(fd, (struct obj *)0);

    bwrite(fd, (char *)nul, sizeof(unsigned));
    
#ifndef QUEST
    bwrite(fd, (char *)rooms, sizeof(rooms));
    bwrite(fd, (char *)doors, sizeof(doors));
#endif

    ftrap = 0;
    fgold = 0;
    fobj = 0;
}
#else
extern struct obj *billobjs;

void savelev(int fd)
{
#ifndef NOWORM
    struct wseg *wtmp;
    struct wseg *wtmp2;
    int tmp;
#endif

    if(fd < 0) {
        panic("Save on bad file!");
    }

    bwrite(fd, (char *)levl, sizeof(levl));
    bwrite(fd, (char *)&moves, sizeof(long));

    bwrite(fd, (char *)&xupstair, sizeof(xupstair));
    bwrite(fd, (char *)&yupstair, sizeof(yupstair));
    bwrite(fd, (char *)&xdnstair, sizeof(xdnstair));
    bwrite(fd, (char *)&ydnstair, sizeof(ydnstair));
    
    savemonchn(fd, fmon);
    savegenchn(fd, fgold);
    savegenchn(fd, ftrap);
    saveobjchn(fd, fobj);
    saveobjchn(fd, billobjs);
    
    billobjs = 0;
    save_engravings(fd);

#ifndef QUEST
    bwrite(fd, (char *)rooms, sizeof(rooms));
    bwrite(fd, (char *)doors, sizeof(doors));
#endif

    ftrap = 0;
    fgold = 0;
    fobj = 0;

/*--------------------------------------------------------*/
#ifndef NOWORM
    bwrite(fd, (char *)wsegs, sizeof(wsegs));

    for(tmp = 1; tmp < 32; ++tmp) {
        for(wtmp = wsegs[tmp]; wtmp != 0; wtmp = wtmp2) {
            wtmp2 = wtmp->nseg;
            bwrite(fd, (char *)wtmp, sizeof(struct wseg));
        }

        wsegs[tmp] = 0;
    }

    bwrite(fd, (char *)wgrowtime, sizeof(wgrowtime));
#endif
/*--------------------------------------------------------*/
}
 
void bwrite(int fd, char *loc, unsigned int num)
{
    /* lint wants the 3rd arg of write to be an ine; lint -p an unsigned */
    if(write(fd, loc, (int)num) != num) {
        panic("Cannot write %d bytes to file#%d", num, fd);
    }
}

void saveobjchn(int fd, struct obj *otmp)
{
    struct obj *otmp2;
    unsigned int xl;
    int minusone = -1;
    
    while(otmp) {
        otmp2 = otmp->nobj;
        xl = otmp->onamelth;
        
        bwrite(fd, (char *)&xl, sizeof(int));
        bwrite(fd, (char *)otmp, xl + sizeof(struct obj));
        
        free((char *)otmp);
        
        otmp = otmp2;
    }
    
    bwrite(fd, (char *)&minusone, sizeof(int));
}

void savemonchn(int fd, struct monst *mtmp)
{
    struct monst *mtmp2;
    unsigned int xl;
    int minusone = -1;
    struct permonst *monbegin = &mons[0];
    
    bwrite(fd, (char *)&monbegin, sizeof(monbegin));
    
    while(mtmp) {
        mtmp2 = mtmp->nmon;
        xl = mtmp->mxlth + mtmp->mnamelth;
        
        bwrite(fd, (char *)&xl, sizeof(int));
        bwrite(fd, (char *)mtmp, xl + sizeof(struct monst));

        if(mtmp->minvent) {
            saveobjchn(fd, mtmp->minvent);
        }
        
        free((char *)mtmp);
        mtmp = mtmp2;
    }
    
    bwrite(fd, (char *)&minusone, sizeof(int));
}

void savegenchn(int fd, struct gen *gtmp)
{
    struct gen *gtmp2;
    
    while(gtmp != NULL) {
        gtmp2 = gtmp->ngen;
        
        bwrite(fd, (char *)gtmp, sizeof(struct gen));
        
        free((char *)gtmp);
        
        gtmp = gtmp2;
    }
    
    bwrite(fd, nul, sizeof(struct gen));
}
            
