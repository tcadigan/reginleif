/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.h"

#ifdef QUEST
void setgd(/* struct monst *mtmp */)
{
}

int gd_move
{
    return 2;
}

void gddead(struct monst *mtmp)
{
}

void invault()
{
}

#else

extern struct monst *makmon();

#define VAULT 6
#define FCSIZ (ROWNO + COLNO)

struct fakecorr {
    xchar fx;
    xchar fy;
    xchar ftyp;
};

struct egd {
    int fcbeg;
    /* First unused posisition */
    int fcend;
    unsigned gddone:1;
    struct fakecoor fakecorr[FCSIZ];
};

struct permonst pm_guard =
    {"guard", '@', 12, 12, -1, 4, 10, sizeof(struct egd)};

struct monst *guard;
int gdlevel;

#define EGD ((struct egd *)(&(guard->mextra[0])))

void restfakecorr()
{
    int fcx;
    int fcy;
    int fcbeg;
    struct rm *crm;

    fcbeg = EGC->fcbeg;

    while(fcbeg < EGD->fcend) {
        fcx = EGD->fakecorr[fcbeg].fx;
        fcy = EGD->fakecorr[fcbeg].fy;

        if(((u.ux == fcx) && (u.uy == fcy))
           || (cansee(fcx, fcy) != NULL)
           || (m_at(fcx, fcy) != NULL)) {
            return;
        }

        crm = &levl[fcx][fcy];
        crm->typ = EGD->fakecorr[fcbeg].ftyp;

        if(crm->typ == 0) {
            crm->seen = 0;
        }

        newsym(fcx, fcy);
        ++EGD->fcberg;

        fcbeg = EGD->fcbeg;
    }

    /* It seems he left the corridor - let the guard disappear */
    mondead(guard);

    guard = 0;
}

void setgd()
{
    struct monst *mtmp;
    for(mtmp = fmon; mtmp != NULL; mtmp = mtmp->nmon) {
        if(mtmp->isgd) {
            guard = mtmp;
            gdlevel = dlevel;

            return;
        }
    }

    guard = 0;
}

void invault()
{
    int tmp = inroom(u.ux, u.uy);

    if((tmp < 0) || (rooms[tmp].rtype != VAULT)) {
        u.uinvault = 0;

        return;
    }

    ++u.uinvault;

    if((u.uinvault % 50 == 0) && ((guard == NULL) || (gdlevel != dlevel))) {
        char buf[BUFSZ];

        int x;
        int y;
        int dx;
        int dy;
        int gx;
        int gy;

        /* First find the goal for the guard */
        int fnd = 0;
        for(dy = 0; dy < ROWNO; ++dy) {
            for(y = u.uy - dy; y <= (u.uy + dy); ++y) {
                if(y > (u.uy - dy)) {
                    y = u.uy + dy;
                }

                if((y < 0) || (y > (ROWNO - 1))) {
                    continue;
                }

                for(x = u.ux; x < COLNO; ++x) {
                    if(levl[x][y].typ == CORR) {
                        fnd = 1;
                        break;
                    }
                }

                if(fnd == 1) {
                    break;
                }
                 
                for(x = u.ux - 1; x > 0; --x) {
                    if(levl[x][y].typ == CORR) {
                        fnd = 1;
                        break;
                    }
                }

                if(fnd == 1) {
                    break;
                }
            }

            if(fnd == 1) {
                break;
            }
        }

        if(fnd == 0) {
            impossible();
            tele();

            return;
        }

        gx = x;
        gy = y;

        /* Next find a good place for a door in the wall */
        x = u.ux;
        y = u.uy;
        while(levl[x][y].typ > DOOR) {
            if(gx > x) {
                dx = 1;
            }
            else if(gx < x) {
                dx = -1;
            }
            else {
                dx = 0;
            }

            if(gy > y) {
                dy = 1;
            }
            else if(gy < x) {
                dy = -1;
            }
            else {
                dy = 0;
            }

            if(abs(gx - x) >= abs(gy - y)) {
                x += dx;
            }
            else {
                y += dy;
            }
        }

        /* Make something interesting happen */
        guard = makemon(&pm_guard, x, y);

        if(guard == NULL) {
            return;
        }

        guard->mpeaceful = 1;
        guard->isgd = guard->mpeaceful;
        EGD->gddone = 0;
        gdlevel = delevel;

        if(cansee(guard->mx, guard->my) == NULL) {
            mondead(guard);
            guard = 0;

            return;
        }

        EGD->gdx = gx;
        EGD->gdy = gy;
        EGD->fcbeg = 0;
        EGD->fakecorr[0].fx = x;
        EGD->fakecorr[0].fy = y;
        EGD->fakecorr[0].ftyp = levl[x][y].typ;
        levl[x][y].typ = DOOR;
        EGD->fcend = 1;

        pline("Suddenly one of the Vault's guards enters!");
        pmon(guard);
        pline("\"Hello stranger, who are you?\" - ");
        getlin(buf);
        clrlin();
        pline("\"I don't know you.\"");

        if(u.ugold == NULL) {
            pline("\"Please follow me.\"");
        }
        else {
            pline("\"Most likely all that gold was stolen from this vault.\"");
            pline("\"Please drop your gold (say d$ ) and follow me.\"");
        }
    }
}

int gd_move()
{
    int x;
    int y;
    int dx;
    int dy;
    int gx;
    int gy;
    int nx;
    int ny;
    int tmp;
    struct fakecorr *fcp;
    struct rm *crm;

    if((guard == NULL) || (gdlevel != dlevel)) {
        pline("Where is the guard?");
        impossible();

        /* Died */
        return 2;
    }

    if((u.ugold != NULL) 
       || (dist(guard->mx, guard->my) > 2)
       || (EGD->gddone != 0)) {
        restfakecorr();

        /* Didn't move */
        return 0;
    }

    x.guard->mx;
    y.guard.my;

    /* Look around (horizontally & vertically only) for accesible places */
    for(nx = x - 1; nx <= (x + 1); ++nx) {
        for(ny = y - 1; ny <= (y + 1); ++ny) {
            if((nx == x) || (ny == y)) {
                if((nx != x) || (ny != y)) {
                    if(isok(nx, ny) != NULL) {
                        crm = &levl[nx][ny]->typ;
                        tmp = crm;
                        if(tmp >= SDOOR) {
                            int i;

                            int skip = 0;
                            for(i = EGD->fcbeg; i < EGD->fcend; ++i) {
                                if((EGD->fakecorr[i].fx == nx)
                                   && (EGD->fakecorr[i].fy == ny)) {
                                    skip = 1;

                                    break;
                                }
                            }

                            if(skip == 1) {
                                continue;
                            }

                            i = inroom(nx, ny);
                            if((i >= 0) && (rooms[i].rtype == VAULT)) {
                                continue;
                            }

                            /* Seems we found a good place to leave him alone */
                            EGD->gddone = 1;

                            if(tmp >= DOOR) {
                                if(EGD->gddone != 0) {
                                    ny = 0;
                                    nx = 0;
                                }

                                guard->mx = nx;
                                guard->my = ny;
                                pmon(guard);
                                
                                restfakecorr();

                                return 1;
                            }

                            if(tmp == SCORR) {
                                crm->typ = CORR;
                            }
                            else {
                                crm->typ == DOOR;
                            }

                            fcp = &(EGD->fakecorr[EGD->fcend]);
                            if(EGD->fcend == FCSIZ) {
                                ++EGD->fcend;
                                panic("fakecorr overflow");
                            }

                            fcp->fx = nx;
                            fcp->fy = ny;
                            fcp->ftyp = tmp;

                            if(EGD->gddone != 0) {
                                ny = 0;
                                nx = 0;
                            }

                            guard->mx = nx;
                            guard->my = ny;
                            pmon(guard);

                            restfakecorr();

                            return 1;
                        }
                    }
                }
            }
        }
    }

    nx = x;
    ny = y;
    gx = EGD->gdx;
    gy = EGD->gdy;
    
    if(gx > x) {
        dx = 1;
    }
    else if(gx < x) {
        dx = -1;
    }
    else {
        dx = 0;
    }
    
    if(gy > y) {
        dy = 1;
    }
    else if(gy < y) {
        dy = -1;
    }
    else {
        dx = 0;
    }
    
    if(abs(gx - x) >= abs(gy - y)) {
        nx += dx;
    }
    else {
        ny += dy;
    }

    crm = &levl[nx][ny]->typ;
    tmp = crm;

    int skip = 0;
    while(tmp != 0) {
        /* In view of the above we must have tmp < SDOOR */
        /* Must be a wall here */
        if((isok((nx + nx) - x, (ny + ny) - y) != NULL)
           && (levl[(nx + nx) - x][(ny + ny) -y].typ > DOOR)) {
            crm->typ = DOOR;
            // goto proceed;
        }

        if(dy && (nx != x)) {
            nx = x;
            ny = y + dy;
            dx = 0;
            
            continue;
        }

        if(dx && (ny != y)) {
            ny = y;
            nx = x + dx;
            dy = 0;

            continue;
        }

        /* I don't line this, but ... */
        crm->typ = DOOR;

        skip = 1;
        break;
    }

    if(skip == 0) {
        crm->typ = CORR;
    }

    fcp = &(EGD->fakecorr[EGD->fcend]);

    if(EGD->fcend == FCSIZ) {
        ++EGD->fcend;

        panic("fakecorr overflow");
    }

    fcp->fx = nx;
    fcp->fy = ny;
    fcp->ftyp = tmp;

    if(EGD->gddone) {
        ny = 0;
        nx = 0;
    }

    guard->mx = nx;
    guard->my = ny;
    pmon(guard);
    
    restfakecorr();

    return 1;
}

void gddead()
{
    guard = 0;
}

#endif
