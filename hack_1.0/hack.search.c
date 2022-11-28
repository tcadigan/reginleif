/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.search.h"

#include "def.trap.h"
#include "hack.h"
#include "hack.do.h"
#include "hack.invent.h"
#include "hack.makemon.h"
#include "hack.mon.h"
#include "hack.pri.h"
#include "hack.topl.h"
#include "rnd.h"

/* Returns number of things found */
int findit()
{
    int num;
    xchar zx;
    xchar zy;
    struct gen *gtmp;
    struct monst *mtmp;
    xchar lx;
    xchar hx;
    xchar ly;
    xchar hy;

    if(u.uswallow != 0) {
        return 0;
    }

    lx = u.ux;
    num = levl[(int)(lx - 1)][(int)u.uy].typ;
    while((num != 0) && (num != CORR)) {
        --lx;
        num = levl[(int)(lx - 1)][(int)u.uy].typ;
    }

    hx = u.ux;
    num = levl[(int)(hx + 1)][(int)u.uy].typ;
    while((num != 0) && (num != CORR)) {
        ++hx;
        num = levl[(int)(hx + 1)][(int)u.uy].typ;
    }

    ly = u.uy;
    num = levl[(int)u.ux][(int)(ly - 1)].typ;
    while((num != 0) && (num != CORR)) {
        --ly;
        num = levl[(int)u.ux][(int)(ly - 1)].typ;
    }

    hy = u.uy;
    num = levl[(int)u.ux][(int)(hy + 1)].typ;
    while((num != 0) && (num != CORR)) {
        ++hy;
        num = levl[(int)u.ux][(int)(hy + 1)].typ;
    }

    num = 0;

    for(zy = ly; zy <= hy; ++zy) {
        for(zx = lx; zx <= hx; ++zx) {
            if(levl[(int)zx][(int)zy].typ == SDOOR) {
                levl[(int)zx][(int)zy].typ = DOOR;
                atl(zx, zy, '+');
                ++num;
            }
            else if(levl[(int)zx][(int)zy].typ == SCORR) {
                levl[(int)zx][(int)zy].typ = CORR;
                atl(zx, zy, CORR_SYM);
            }
            else {
                gtmp = g_at(zx, zy, ftrap);
                if(gtmp != NULL) {
                    if(gtmp->gflag == PIERC) {
                        makemon(PM_PIERC, zx, zy);
                        ++num;
                        deltrap(gtmp);
                    }
                    else if(!(gtmp->gflag & SEEN)) {
                        gtmp->gflag |= SEEN;

                        if(vism_at(zx, zy) == 0) {
                            atl(zx, zy, '^');
                        }

                        ++num;
                    }
                }
                else {
                    mtmp = m_at(zx, zy);
                    if(mtmp != NULL) {
                        if(mtmp->mimic) {
                            seemimic(mtmp);

                            ++num;
                        }
                    }
                }
            }
        }
    }

    return num;
}

int dosearch()
{
    xchar x;
    xchar y;
    struct gen *tgen;
    struct monst *mtmp;

    for(x = u.ux - 1; x < (u.ux + 2); ++x) {
        for(y = u.uy - 1; y < (u.uy + 2); ++y) {
            if((x != u.ux) || (y != u.uy)) {
                if((levl[(int)x][(int)y].typ == SDOOR) && (rn2(7) == 0)) {
                    levl[(int)x][(int)y].typ = DOOR;
                    levl[(int)x][(int)y].seen = 0;

                    /* Force prl */
                    prl(x, y);
                    nomul(0);
                }
                else if((levl[(int)x][(int)y].typ == SCORR) && (rn2(7) == 0)) {
                    levl[(int)x][(int)y].typ = CORR;
                    levl[(int)x][(int)y].seen = 0;

                    /* Force prl */
                    prl(x, y);
                    nomul(0);
                }
                else {
                    mtmp = m_at(x, y);
                    if(mtmp != NULL) {
                        if(mtmp->mimic) {
                            seemimic(mtmp);
                            pline("You find a mimic.");
                            
                            return 1;
                        }
                    }

                    for(tgen = ftrap; tgen != NULL; tgen = tgen->ngen) {
                        if((tgen->gx == x)
                           && (tgen->gy == y)
                           && ((tgen->gflag & SEEN) == 0)
                           && (rn2(8) == 0)) {
                            nomul(0);
                            
                            pline("You find a%s.", traps[tgen->gflag]);

                            if(tgen->gflag == PIERC) {
                                deltrap(tgen);
                                makemon(PM_PIERC, x, y);

                                return 1;
                            }

                            tgen->gflag |= SEEN;

                            if(vism_at(x, y) == 0) {
                                atl(x, y, '^');
                            }
                        }
                    }
                }
            }
        }
    }

    return 1;
}

/* ARGSUSED */
int doidtrap()
{
    struct gen *tgen;
    int x;
    int y;

    if(getdir() == 0) {
        return 0;
    }

    x = u.ux + u.dx;
    y = u.uy + u.dy;

    for(tgen = ftrap; tgen != NULL; tgen = tgen->ngen) {
        if((tgen->gx == x) && (tgen->gy == y) && (tgen->gflag & SEEN))  {
            pline("That is a%s.", traps[tgen->gflag & ~SEEN]);

            return 0;
        }
    }

    pline("I can't see a trap there.");
    
    return 0;
}

void wakeup(struct monst *mtmp)
{
    mtmp->msleep = 0;
    setmangry(mtmp);

    if(mtmp->mimic) {
        seemimic(mtmp);
    }
}

/* NOTE: We must check if(mtmp->mimic) before calling this routine */
void seemimic(struct monst *mtmp)
{
    mtmp->mimic = 0;
    unpmon(mtmp);
    pmon(mtmp);
}
