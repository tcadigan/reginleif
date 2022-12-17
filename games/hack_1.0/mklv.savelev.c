#include "mklv.savelev.h"

#include <stdlib.h>
#include <unistd.h>

extern char nul[];

#include <fcntl.h>

#include "mklev.h"

void savelev()
{
    extern char *tfile;
    
    int fd;

    fd = creat(tfile, FMASK);

    if(fd < 0) {
        panic("Cannot create %s\n", tfile);
    }

    bwrite(fd, (char *)levl, sizeof(levl));
    bwrite(fd, (char *)nul, sizeof(long));
    bwrite(fd, (char *)&xupstair, sizeof(xupstair));
    bwrite(fd, (char *)&yupstair, sizeof(yupstair));
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
 
void bwrite(int fd, char *loc, unsigned int num)
{
    /* lint wants the 3rd arg of write to be an int; lint -p an unsigned */
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
