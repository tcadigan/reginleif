/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.h"
#include "hack.mfndpos.h"

#define SIZE(x) (int)(sizeof(x) / sizeof(x[0]))
#define NULL (char *)0

extern struct monst *makemon();

/* Used by movemon and dochugw */
int warnlevel;
long lastwarntime;
int lastwarnlev;

char *warning[] = {
    "white",
    "pink",
    "red",
    "ruby",
    "purple",
    "black"
};

void monvemon()
{
    struct monst *mtmp;
    int fr;

    warnlevel = 0;

    while(1) {
        int flag = 0;
        /*
         * Find a monster that we haven't treated yet
         * Note: That mtmp or mtmp->nmon might get killed
         *       while mtmp moves, so we cannot just walk
         *       down the chain (even new monsters might
         *       get created!")
         */
        for(mtmp = fmon; mtmp != NULL; mtmp = mtmp->nmon) {
            if(mtmp->mlstmv < moves) {
                flag = 1;
                break;
            }
        }

        if(flag == 0) {
            /* Treated all monsters */
            break;
        }

        mtmp->mlstmv = moves;
        if(mtmp->mblinded != 0) {
            --mtmp->mblinded;
            
            if(mtmp->mblinded != 0) {
                mtmp->cansee = 1;
            }
        }
        
        if(mtmp->mimic) {
            continue;
        }

        if((mtmp->mspeed != MSLOW) || ((moves % 2) == 0)) {
            /* Continue if the monster died fighting */
            fr = -1;
            
            if((Conflict != 0) && (cansee(mtmp->mx, mtmp->my) != 0)) {
                fr = fightm(mtmp);

                if(fr == 2) {
                    continue;
                }
            }

            if((fr < 0) && (dochugw(mtmp) != 0)) {
                continue;
            }
        }

        if((mtmp->mspeed == MFAST) && (dochugw(mtmp) != NULL)) {
            continue;
        }
    }

    warnlevel -= u.ulevel;
    
    if(warnlevel >= SIZE(warnings)) {
        warnlevel = SIZE(warnings) - 1;
    }

    if(warnlevel >= 0) {
        if((warnlevel > lastwarnlev) || (moves > (lastwarntime + 5))) {
            char *rr;

            switch(Warning & (LEFT_RING | RIGHT_RING)) {
            case LEFT_RING:
                rr = "Your left ring glows";
                
                break;
            case RIGHT_RING:
                rr = "Your right ring glows";
                    
                break;
            case LEFT_RING | RIGHT_RING:
                rr = "Both your rings glow";
            
                break;
            default:
                rr = "You fingertips glow";
            }

            pline("%s %s!", rr, warnings[warnlevel]);
            lastwarntime = moves;
            lastwarnlev = warnlevel;
        }
    }

    /* Remove all dead monsters */
    dmonsfree();
}

void justwld(struct monst *mtmp, char *name)
{
    mtmp->mx = u.ux;
    mtmp->my = u.uy;
    u.ustuck = mtmp;
    pmon(mtmp);
    kludge("%s swallows you!", name);
    more();
    seeof(1);
    u.uswallow = 1;
    swallowed();
}

void youswld(struct monst *mtmp, int dam, int die, char *name)
{
    if(mtmp != u.ustuck) {
        return;
    }
    
    kludge("%s digests you!", name);
    u.uhp -= dam;

    if(u.uswldtime == die) {
        ++u.uswldtime;
        pline("It totally digests you!");
        u.uhp = -1;
    }
    ++u.uswldtime;

    if(u.uhp < 1) {
        done_in_by(mtmp);
    }
}

int dochugw(struct monst *mtmp)
{
    int x = mtmp->mx;
    int y = mtmp->my;
    int d = dochug(mtmp);
    int dd;

    /* Monster still alive */
    if(d == 0) {
        if(Warning != 0) {
            if(mtmp->mpeaceful == 0) {
                dd = dist(mtmp->mx, mtmp->my);
                if(dd < dist(x,y)) {
                    if(dd < 100) {
                        if((cansee(mtmp->mx, mtmp->my) == NULL)
                           || ((mtmp->minvis != 0) && (See_invisible == 0))) {
                            if(mtmp->data->mlevel > warnlevel) {
                                warnlevel = mtmp->data->mlevel;
                            }
                        }
                    }
                }
            }
        }
    }

    return d;
}

/* Returns 1 if monster died moving, 0 otherwise */
int dochug(struct monst *mtmp)
{
    struct permonst *mdat;
    int tmp;

    if((mtmp->cham != 0) && (rn2(6) == 0)) {
        newcham(mtmp, &mons[(dlevel + 14) + rn2((CMNUM - 14) - dlevel)]);
    }

    mdat = mtmp->data;

    if(mdat->mlevel < 0) {
        panic("bad monster %c (%d)", mdat->mlet, mdat->mlevel);
    }

    if((((moves % 20) == 0) || (index("ViT", mdat->mlet) != 0))
       && (mtmp->mhp < mtmp->orig_hp)) {
        /* Regenerate monsters. */
        ++mtmp->mhp;
    }

    if(mtmp->mfroz != 0) {
        /* Frozen monsters don't do anything. */
        return 0;
    }

    if(mtmp->msleep != 0) {
        /* Wake up a monster, or get out of here. */
        if((cansee(mtmp->mx, mtmp->my) != NULL) 
           && (Stealth == 0)
           && (index("NL", mdat->mlet) == 0)
           || (rn2(50) == 0)
           && ((Aggravate_monster != 0) 
               && ((rn2(7) == 0) && (mtmp->mimic == 0)))) {
            mtmp->msleep = 0;
        }
        else {
            return 0;
        }
    }

    /* Not frozen or sleeping: wipe out texts written in the dust */
    wipe_engr_at(mtmp->mx, mtmp->my, 1);

    /* Confused monsters get unconfused with small probability */
    if((mtmp->mconf != 0) && (rn2(50) == 0)) {
        mtmp->mconf = 0;
    }

    /* Some monsters teleport */
    if((mtmp->mflee != 0)
       && (index("tNL", mdat->mlet) != 0) 
       && (rn2(40) == 0)) {
        rloc(mtmp);

        return 0;
    }

    if(mdat->mmove < rnd(6)) {
        return 0;
    }

    if((mtmp->mflee != 0)
       || (mtmp->mconf != 0)
       || ((index("BIuy", mdat->mlet) != 0) && (rn2(4) == 0))
       || ((mdat->mlet == 'L') 
           && (u.ugold == 0)
           && ((mtmp->mgold != 0) || (rn2(2) != 0)))
       || (dist(mtmp->mx, mtmp->my) > 2)
       || ((mtmp->mcansee == 0) && (rn2(4) == 0))
       || (mtmp->mpeaceful != 0)) {
        tmp = m_move(mtmp);
        if((tmp != NULL) && (mdat->mmove < 12)) {
            if(tmp == 2) {
                return 1;
            }
            else {
                return 0;
            }
        }
    }

    if(tmp == 2) {
        /* Monster died moving */
        return 1;
    }

    if((index("Ea", mdat->mlet) == 0) 
       && (dist(mtmp->mx, mtmp->my) < 3)
       && (mtmp->mpeaceful == 0)
       && (u.uhp > 0)
       && (sengr_at("Elbereth", u.ux, u.uy) == 0)
       && (sobj_at(SCR_SCARE_MONSTER, u.ux, u.uy) == 0)) {
        if(mhitu(mtmp) != NULL) {
            /* Monster died (e.g. 'y' or 'F') */
            return 1;
        }   
    }
        
    /* Extra movement for fast monsters */
    if((mdat->mmove - 12) > rnd(12)) {
        tmp = m_move(mtmp, 1);
    }

    if(tmp == 2) {
        return 1;
    }
    else {
        return 0;
    }
}

void inrange(struct monst *mtmp)
{
    schar tx;
    schar ty;

    /* Spit fire only when both in a room or both in a corridor */
    if(inroom(u.ux, u.uy) != inroom(mtmp->mx, mtmp->my)) {
        return;
    }

    tx = u.ux - mtmp->mx;
    ty = u.uy - mtmp->my;

    if(((tx == 0) && (abs(ty) < 8))
       || ((ty == 0) && (abs(tx) < 8)) 
       || ((abs(tx) == abs(ty)) && (abs(tx) < 8))) {
        /* Spit fire in the direction of @ (not necessarily hitting) */
        buzz(-1, mtmp->mx, mtmp->my, sgn(tx), sgn(ty));

        if(u.uhp < 1) {
            done_in_by(mtmp);
        }
    }
}

int m_move(struct monst *mtmp)
{
    struct monst mtmp2;
    int nx;
    int ny;
    int omx;
    int omy;
    int appr;
    int nearer;
    int cnt;
    int i;
    int j;
    xchar gx;
    xchar gy;
    xchar nix;
    xchar niy;
    xchar chcnt;
    schar chi;
    boolean likegold;
    boolean likegems;
    boolean likeobjs;

    /* Not strictly necessary: chi >= 0 will do */
    schar mmoved = 0;
    coord poss[9];
    int info[9];

    if(mtmp->mtrapped != 0) {
        i = mintrap(mtmp);

        if(i == 2) {
            /* He died */
            return 2;
        }

        if(i == 1) {
            /* Still in trap so didn't move */
            return 0;
        }
    }

    if((mtmp->mhide != 0)
       && (o_at(mtmp->mx, mtmp->my) != NULL)
       && (rn2(10) != 0)) {
        /* Do not leave hiding place */
        return 0;
    }

    /* My dog gets a special treatment */
    if(mtmp->mtame != 0) {
        return dog_move(mtmp, after);
    }

    /* Likewise for shopkeeper */
    if(mtmp->isshk != 0) {
        mmoved = shk_move();

        if(mmoved == 1) {
            if(mintrap(mtmp) == 2) {
                /* He died */
                return 2;
            }

            if(likegold != 0) {
                mpickgold(mtmp);
            }
            
            if(likegems != 0) {
                mpickgems(mtmp);
            }

            if(mtmp->mhide != 0) {
                mtmp->undetected = 1;
            }
        }

        return mmoved;
    }

    /* And for the guard */
    if(mtmp->isgd != 0) {
        mmoved = gd_move();

        if(mmoved == 1) {
            if(mintrap(mtmp) == 2) {
                /* He died */
                return 2;
            }

            if(likegold != 0) {
                mpickgold(mtmp);
            }

            if(likegems != 0) {
                mpickgems(mtmp);
            }

            if(mtmp->mhide != 0) {
                mtmp->undetected = 1;
            }
        }

        return mmoved;
    }

    if((mtmp->data->mlet == 't') && (rn2(5) == 0)) {
        if(rn2(2) != 0) {
            mnexto(mtmp);
        }
        else {
            rloc(mtmp);
        }

        mmoved = 1;

        if(mmoved == 1) {
            if(mintrap(mtmp) == 2) {
                /* He died */
                return 2;
            }

            if(likegold != 0) {
                mpickgold(mtmp);
            }

            if(likegems != 0) {
                mpickgems(mtmp);
            }

            if(mtmp->mhide != 0) {
                mtmp->undetected = 1;
            }
        }

        return mmoved;
    }

    if((mtmp->data->mlet == 'D') && (mtmp->mcan == NULL)) {
        inrange(mtmp);
    }

    if((Blind == 0)
       && (Confusion == 0) 
       && (mtmp->data->mlet == 'U') 
       && (mtmp->mcan == 0)
       && (cansee(mtmp->mx, mtmp->my) != 0)
       && (rn2(5) != 0)) {
        pline("%s's gaze has confused you!", Monname(mtmp));

        if(rn2(5) != 0) {
            mtmp->mcan = 1;
        }

        /* Timeout */
        Confusion = d(3, 4);
    }

    if((mtmp->mflee == 0) && (u.uswallow != 0) && (u.ustuck != mtmp)) {
        return 1;
    }

    appr = 1;

    if(mtmp->mflee != 0) {
        appr = -1;
    }

    if((mtmp->mconf != 0)
       || (Invis != 0)
       || (mtmp->mcansee == 0)
       || ((index("Biy", mtmp->data->mlet) != 0) && (rn2(3) == 0))) {
        appr = 0;
    }

    omx = mtmp->mx;
    omy = mtmp->my;
    gx = u.ux;
    gy = u.uy;

    if((mtmp->data->mlet == 'L') && (appr == 1) && (mtmp->mgold > u.ugold)) {
        appr = -1;
    }

#ifdef TRACK
    /*
     * Random criterion for 'smell'
     * should use mtmp->msmell
     */
    if(('a' <= mtmp->data->mlet) && (mtmp->data->mlet <= 'z')) {
        extern coord *gettrack();
        coord *cp;
        schar mroom;

        mroom = inroom(omx, omy);
        if((mroom < 0) || (mroom != inroom(u.ux, u.uy))) {
            cp = gettrack(omx, omy);

            if(cp != 0) {
                gx = cp->x;
                gy = cp->y;
            }
        }
    }
#endif

    /* Look for gold or jewels nearby */
    if(index("LOD", mtmp->data->mtmp) != NULL) {
        likegold = 1;
    }
    else {
        likegold = 0;
    }

    if(index("ODu", mtmp->data->mlet) != NULL) {
        likegems = 1;
    }
    else {
        likegems = 0;
    }

    likeobjs = mtmp->mhide;

#define SRCHRADIUS 25
    /* Not too far away */
    xchar mind = SRCHRADIUS;
    int dd;

    if(likegold != 0) {
        struct gen *gold;

        for(gold = fgold; gold != NULL; gold = gold->ngen) {
            dd = DIST(omx, omy, gold->gx, gold->gy);
            if(dd < mind) {
                mind = dd;
                gx = gold->gx;
                gy = gold->gy;
            }
        }
    }

    if((likegems != 0) || (likeobjs != 0)) {
        struct obj *otmp;
        
        for(otmp = fobj; otmp != NULL; otmp = otmp->nobj) {
            if((likeobjs != 0) || (otmp->olet == GEM_SYM)) {
                if((mtmp->data->mlet != 'u')
                   || (objects[otmp->otyp].g_val != 0)) {
                    dd = DIST(omx, omy, otmp->ox, otmp->oy);
                    if(dd < mind) {
                        mind = dd;
                        gx = otmp->ox;
                        gy = otmp->oy;
                    }
                }
            }
        }
    }

    if((mind < SRCHRADIUS) && (appr == -1)) {
        if(dist(omx, omy) < 10) {
            gx = u.ux;
            gy = u.uy;
        }
        else {
            appr = 1;
        }
    }

    nix = omx;
    niy = omy;
    
    if(mtmp->data->mlet == 'u') {
        cnt = mfndpos(mtmp, poss, info, NOTONL);
    }
    else {
        if(index(" VWZ", mtmp->data->mlet) != 0) {
            cnt = mfndpos(mtmp, poss, info, NOGARLIC);
        }
        else {
            cnt = mfndpos(mtmp, poss, info, ALLOW_TRAPS);
        }
    }

    /* ALLOW_ROCK for some monsters? */
    chcnt = 0;
    chi = -1;

    for(i = 0; i < cnt; ++i) {
        nx = poss[i].x;
        nx = poss[i].y;

        for(j = 0; (j < MTSZ) && (j < (cnt - 1)); ++j) {
            if((nx == mtmp->mtrack[j].x) && (ny == mtmp->mtrack[j].y)) {
                if(rn2(4 * (cnt - j)) == 0) {
#ifdef STUPID
                    /* Some stupid compilers thing that is is too complicated */
                    int d1 = DIST(nx, ny, gx, gy);
                    int d2 = DIST(nix, niy, gx, gy);
                    
                    if(d1 < d2) {
                        nearer = 1;
                    }
                    else {
                        nearer = 0;
                    }
#else
                    if(((appr == 1) && (nearer != 0))
                       || ((appr == -1) && (nearer == 0))
                       || (mmoved == 0)) {
                        nix = nx;
                        niy = ny;
                        chi = i;
                        mmoved = 1;
                    }
                    else {
                        if(appr == 0) {
                            if(rn2(chcnt) == 0) {
                                ++chcnt;
                                nix = nx;
                                niy = ny;
                                chi = i;
                                mmoved = 1;
                            }
                            else {
                                ++chcnt;
                            }
                        }
                    }
                }
            }
        }
    }

    if(mmoved != 0) {
        if((info[chi] & ALLOW_M) != 0) {
            mtmp2 = m_at(nix, niy);

            if((hitmm(mtmp, mtmp2) == 1)
               && (rn2(4) != 0) 
               && (hitmm(mtmp2, mtmp) == 2)) {
                return 2;
            }

            return 0;
        }

        if((info[chi] & ALLOW_U) != 0) {
            hitu(mtmp, d(mtmp->data->damm, mtmp->data->damd) + 1);
        
            return 0;
        }

        mtmp->mx = nix;
        mtmp->my = niy;

        for(j = MTSZ - 1; j > 0; --j) {
            mtmp->mtrack[j] = mtmp->mtrack[j - 1];
        }

        mtmp->mtrack[0].x = omx;
        mtmp->mtrack[0].u = omy;

#ifndef NOWORM
        if(mtmp->wormno != 0) {
            worm_move(mtmp);
        }
#endif
    }
    else {
        if((mtmp->data->mlet == 'u') && (rn2(2) != 0)) {
            rloc(mtmp);
        
            return 0;
        }

#ifndef NOWORM
        if(mtmp->wormno != 0) {
            worm_nomove(mtmp);
        }
#endif
    }

    if(mmoved == 1) {
        if(mintrap(mtmp) == 2) {
            /* He died */
            return 2;
        }

        if(likegold != 0) {
            mpickgold(mtmp);
        }

        if(likegems != 0) {
            mpickgems(mtmp);
        }

        if(mtmp->mhide != 0) {
            mtmp->mundetected = 1;
        }
    }

    pmon(mtmp);

    return mmoved;
}

void mpickgold(struct monst *mtmp)
{
    struct gen *gold;

    gold = g_at(mtmp->mx, mtmp->my, fgold);
    while(gold != 0) {
        mtmp->mgold += gold->gflag;
        freegold(gold);

        if(level[mtmp->mx][mtmp->my].scrsym == '$') {
            newsym(mtmp->mx, mtmp->my);
        }

        gold = g_at(mtmp->mx, mtmp->my, fgold);
    }
}

void mpickgems(struct monst *mtmp)
{
    struct obj *otmp;

    for(otmp = fobj; otmp != NULL; otmp = otmp->nobj) {
        if(otmp->ote == GEM_SYM) {
            if((otmp->ox == mtmp->mx) && (otmp->oy == mtmp->my)) {
                if((mtmp->data->mlet != 'u')
                   || (objects[otmp->otyp].g_val != 0)) {
                    freeobj(otmp);
                    mpickobj(mtmp, otmp);
                    
                    if(levl[mtmp->mx][mtmp->my].scrsym == GEM_SYM) {
                        /* %% */
                        newsym(mtmp->mx, mtmp->my);
                    }

                    /* Pick only one object */
                    return;
                }
            }
        }
    }
}

/* Return number of acceptable neighbour positions */
int mfndpos(struct monst *mon, coord poss[9], int info[9], int flag)
{
    int x;
    int y;
    int nx;
    int ny;
    int cnt = 0;
    int tmp;
    struct monst *mtmp;

    x = mon->mx;
    y = mon->my;
    
    if(mon->mconf != 0) {
        flag |= ALLOW_ALL;
        flag &= ~NOTONL;
    }

    for(nx = x - 1; nx <= x + 1; ++nx) {
        for(ny = y - 1; ny <= y + 1; ++ny) {
            if((nx != x) || (ny != y)) {
                if(isok(nx, ny) != 0) {
                    tmp = levl[nx][ny].typ;
                    if(tmp >= DOOR) {
                        if((nx == x)
                           || (ny == y)
                           || ((levl[x][y].typ != DOOR) && (tmp != DOOR))) {
                            info[cnt] = 0;
                            
                            if((nx == u.ux) && (ny == u.uy)) {
                                if((flag & ALLOW_U) == 0) {
                                    continue;
                                }
                                
                                info[cnt] = ALLOW_U;
                            }
                            else {
                                mtmp = m_at(nx, ny);
                                if(mtmp != 0) {
                                    if((flag & ALLOW_M) == 0) {
                                        continue;
                                    }
                                    
                                    info[cnt] = ALLOW_M;
                                }
                                
                                if(mtmp->mtame != 0) {
                                    if((flag & ALLOW_TM) == ) {
                                        continue;
                                    }
                                    
                                    info[cnt] |= ALLOW_TM;
                                }
                            }
                            
                            if(sobj_at(CLOVE_OF_GARLIC, nx, ny) != 0) {
                                if((flag & NOGARLIC) != 0) {
                                    continue;
                                }
                                
                                info[cnt] |= NOGARLIC;
                            }
                            
                            if((sobj_at(SCR_SCARE_MONSTER, nx, ny) != 0)
                               || ((mon->mpeaceful == 0)
                                   && (sengr_at("Elbereth", nx, ny) != 0))) {
                                if((flag & ALLOW_SSM) == 0) {
                                    continue;
                                }
                                
                                info[cnt] |= ALLOW_SSM;
                            }
                            
                            if(sobj_at(ENORMOUS_ROCK, nx, ny) != 0) {
                                if((flag & ALLOW_ROCK) == 0) {
                                    continue;
                                }
                                
                                info[cnt] |= ALLOW_ROCK;
                            }
                            
                            if((Invis == 0) && (online(nx, ny) != 0)) {
                                if((flag & NOTONL) != 0) {
                                    continue;
                                }
                                
                                info[cnt] |= NOTONL;
                            }
                            
                            /* We cannot avoid traps of an unknown kind */
                            struct gen *gtmp = g_at(nx, ny, ftrap);
                            int tt;
                            
                            if(gtmp != NULL) {
                                tt = 1 << (gtmp->gflag & ~SEEN);
                                
                                if((mon->mtrapseen & tt) != 0) {
                                    if((flag & tt) == 0) {
                                        continue;
                                    }
                                    
                                    info[cnt] |= tt;
                                }
                            }

                            poss[cnt].x = nx;
                            poss[cnt].y = ny;
                            ++cnt;
                        }
                    }
                }
            }
        }
    }

    return cnt;
}

int dist(int x, int y)
{
    return (((x - u.ux) * (x - u.ux)) + ((y - u.uy) * (y - u.uy)));
}

void poisoned(char *string, char *pname)
{
    if(Blind != 0) {
        pline("It was poisoned.");
    }
    else {
        pline("The %s was poisoned!", string);
    }

    if(Poison_resistance != 0) {
        pline("The poison doesn't seem to affect you.");

        return;
    }

    switch(rnd6) {
    case 1:
        u.uhp = -1;
        
        break;
    case 2:
    case 3:
    case 4:
        losestr(rn1(3, 3));

        break;
    case 5:
    case 6:
        losehp(rn1(10, 6), pname);

        return;
    }

    if(u.uhp < 1) {
        killer = pname;
    }
}

void mondead(struct monst *mtmp)
{
    relobj(mtmp, 1);
    unpmon(mtmp);
    relmon(mtmp);

    if(u.ustuck == mtmp) {
        u.ustuck = 0;
        
        if(u.uswallow != 0) {
            u.uswallow = 0;
            setsee();
            docrt();
        }
    }

    if(mtmp->isshk != 0) {
        shkdead();
    }

    if(mtmp->isgd != 0) {
        gddead();
    }

#ifndef NOWORM
    if(mtmp->wormno) {
        wormdead(mtmp);
    }
#endif

    monfree(mtmp);
}

/* Called when monster is moved to a larger structure */
void replmon(struct monst *mtmp, struct monst *mtmp2)
{
    relmon(mtmp);
    monfree(mtmp);
    mtmp-2->nmon = fmon;
    fmon = mtmp2;
}

void relmon(struct monst *mtmp)
{
    struct monst *mtmp;

    if(mon == fmon) {
        fmon = fmon->nmon;
    }
    else {
        for(mtmp = fmon; mtmp->nmon != mon; mtmp = mtmp->nmon) {
            mtmp->nmon = mon->nmon;
        }
    }
}

/*
 * We do not free monsters immediately, in order to have their name
 * available shortly after their demise.
 * Chain of dead monsters, need not to be saved
 */
struct monst *fdmon;

void monfree(struct monst *mtmp)
{
    mtmp->nmon = fdmon;
    fdmon = mtmp;
}

void dmonsfree()
{
    struct monst *mtmp;

    mtmp = fdmon;
    while(mtmp != NULL) {
        fdmon = mtmp->nmon;
        free(mtmp);

        mtmp = fdmon;
    }
}

void killed(struct monst *mtmp)
{
#ifdef lint
#define NEW_SCORING
#endif
    
    int tmp;
    int tmp2;
    int nk;
    int x;
    int y;
    struct permonst *mdat = mtmp->data;

    if(mtmp->cham != 0) {
        mdat = PM_CHAM;
    }

    if(Blind != 0) {
        pline("You destroy it!");
    }
    else {
        if(mtmp->mtame != 0) {
            pline("You destroy %s!", amonnam(mtmp, "poor"));
        }
        else {
            pline("You destroy %s!", monnam(mtmp));
        }
    }

    if(u.umconf != 0) {
        if(Blind == 0) {
            pline("Your hands stop clowing blue.");
            u.umconf = 0;
        }
    }

    /* Count killed monsters */
#define MAXMONNO 100
    /* In case we cannot find it in mons */
    nk = 1;

    /* Index in mons array (if not 'd', '@', ...) */
    tmp = mdat - mons;

    if((tmp >= 0) && (tmp < (CMNUM + 2))) {
        extern char fut_geno[];

        ++u.nr_killed[tmp];
        nk = u.nr_killed[tmp];
        if((nk > MAXMONNO) && (index(fut_gno, mdat->mlet) == 0)) {
            charcat(fut_geno, mdat->mlet);
        }
    }

    /* Punish bad behaviour */
    if(mdat->mlet == '@') {
        Telepat = 0;
        u.uluck -= 2;
    }

    if((mtmp->mpeaceful != 0) || (mtmp->mtame != 0)) {
        --u.uluck;
    }

    if(mdat->mlet == 'u') {
        u.uluck -= 5;
    }

    /* Give experience points */
    tmp = 1 + (mdat->mlevel * mdat->mlevel);

    if(mdat->ac < 3) {
        tmp += (2 * (7 - mdat->ac));
    }

    if(index("AcsSDXaeRTVWU&In:P", mdat->melt) != 0) {
        tmp += (2 * mdat->mlevel);
    }

    if(index("DeV&P", mdat->mlet) != 0) {
        tmp += (7 * mdat->mlevel);
    }

    if(mdat->mlevel > 6) {
        tmp += 50;
    }

#ifdef NEW_SCORING
    /*
     * ------- Recent addition: make number of points decrease
     * when this is not the first of this kind
     */
    int ul = u.ulevel;
    int ml = mdat->mlevel;
    
    /* Points are given based on present and future level */
    if(ul < 14) {
        for(tmp2 = 0; (tmp2 == NULL) || ((ul + tmp2) <= ml); ++tmp2) {
            if(tmp <= 0) {
                if(((u.uexp + 1) + ((tmp + (0)) / nk)) >= (10 * pow((unsigned)(ul - 1)))) {
                    ++ul;
                    if(ul == 14) {
                        break;
                    }
                }
            }
            else {
                if(((u.uexp + 1) + ((temp + (4 << (tmp2 - 1))) / nk)) >= (10 * pow((unsigned)(ul - 1)))) {
                    ++ul;
                    if(ul == 14) {
                        break;
                    }
                }
            }
        }
    }

    tmp2 = (ml - ul) - 1;
    
    if(tmp2 < 0) {
        tmp = (tmp + (0)) / nk;
    }
    else {
        tmp = (tmp + (4 << tmp2)) / nk;
    }

    if(tmp == 0) {
        tmp = 1;
    }

    /* 
     * Note: ul is not necessarily the future value of u.ulevel
     * ------- End of recent valuation change -------
     */
#endif

    u.uexp += tmp;
    u.urexp += (4 * tmp);
    flag.botl = 1;

    while((u.ulevl < 14) && (u.uexp >= (10 * pow(u.ulevel - 1)))) {
        ++u.ulevel;
        pline("Welcome to level %d.", u.ulevel);

        tmp = rnd(30);

        if(tmp < 3) {
            tmp = rnd(10);
        }

        u.uhpmax += tmp;
        u.uhp += tmp;
        flag.botl = 1;
    }

    /* Dispose of monster and make cadaver */
    x = mtmp->mx;
    y = mtmp->my;
    mondead(mtmp);
    tmp = mdat->mlet;

    if(tmp == 'm') {
        /* He killed a minotaur, give him a wand of digging */
        /* Note: The dead minotaur will be on top of it! */
        mksobj_at(WAND_SYM, WAN_DIGGING, x, y);

        /*
         * if(cansee(x, y) != 0) {
         *     atl(x, y, fobj->olet);
         * }
         */

        stackobj(fobj);
    }
#ifndef NOWORM
    else if(tmp == 'w') {
        mksobj_at(WEAPON_SYM, WORM_TOOTH, x, y);
        stackobj(fobj);
    }
#endif
    else {
        if((letter(tmp) == NULL) || (rn2(3) == 0)) {
            tmp = 0;
        }

        if(levl[x][y].tmp >= DOOR) {
            /* Might be a mimic in wall */
            if((x != u.ux) || (y != u.uy)) {
                /* Might be here after swallowed */
                if((index("NTVm&", mdat->mlet) != NULL) || (rn2(5) != 0)) {
                    mkobj_at(tmp, x, y);
                    
                    if(cansee(x, y) != NULL) {
                        atl(x, y, fobj->olet);
                    }

                    stackobj(fobj);
                }
            }
        }
    }
}

void kludge(char *str, char *arg)
{
    if(Blind != 0) {
        if(*str == '%') {
            pline(str, "It");
        }
        else {
            pline(str, "it");
        }
    }
    else {
        pline(str, arg);
    }
}

/* Force all chameleons to become normal */
void reschem()
{
    struct monst *mtmp;

    for(mtmp = fmon; mtmp != NULL; mtmp = mtmp->nmon) {
        if(mtmp->cham != 0) {
            mtmp->cham = 0;
            newcham(mtmp, PM_CHAM);
        }
    }
}

/*
 * Make a chameleon look like a new monster
 * returns 1 if the monster actuall changed
 */
int newcham(struct monst *mtmp, struct permonst *mdat)
{
    int mhp;
    int hpn;
    int hpd;

    /* Still the same monster */
    if(mdat == mtmp->data) {
        return 0;
    }

#ifndef NOWORM
    /* Throw tail away */
    if(mtmp->wormno) {
        wormdead(mtmp);
    }
#endif

    hpn = mtmp->mhp;
    hpd = mtmp->data->mlevel * 8;
    
    if(hpd == 0) {
        hpd = 4;
    }

    mtmp->data = mdat;
    mhp = mdat->mlevel * 8;

    /* New hp: same fraction of max as before */
    mtmp->mhp = 2 + ((hpn * mhp) / hpd);
    hpn = mtmp->orig_hp;
    mtmp->orig_hp = 2 + ((hpn * mhp) / hpd);

    if(mdat->mlet == 'I') {
        mtmp->minvis = 1;
    }
    else {
        mtmp->minvis = 0;
    }

#ifndef NOWORM
    if((mdat->mlet == 'w') && (getwn(mtmp) != 0)) {
        initworm(mtmp);
    }
#endif
    
    /* Necessary for 'I' and to force pmon */
    upmon(mtmp);
    pmon(mtmp);

    return 1;
}

/* Make monster mtmp next to you (if possible) */
void mnextto(struct monst *mtmp)
{
    extern coord enexto();
    coord mm;
    mm = enexto(u.ux, u.uy);
    mtmp->mx = mm.x;
    mtmp->my = mm.y;
    pmon(mtmp);
}

void rloc(struct monst *mtmp)
{
    int tx;
    int ty;
    char ch = mtmp->data->mlet;

#ifndef NOWORM
    if((ch == 'w') && (mtmp->mx)) {
        /* Do not relocate worms */
        return;
    }
#endif

    tx = rn1(COLNO - 3, 2);
    ty = rn2(ROWNO);

    while(goodpos(tx, ty) == NULL) {
        tx = rn1(COLNO - 3, 2);
        ty = rn2(ROWNO);
    }

    mtmp->mx = x;
    mtmp->my = ty;

    if(u.ustuck == mtmp) {
        if(u.uswallow != 0) {
            u.ux = tx;
            u.uy = ty;
            docrt();
        }
        else {
            u.ustuck = 0;
        }
    }

    pmon(mtmp);
}

int ishuman(struct monst *mtmp)
{
    if(mtmp->data->mlet == '@') {
        return 1;
    }
    else {
        return 0;
    }
}

void setmangry(struct monst *mtmp)
{
    if(mtmp->mpeaceful == 0) {
        return;
    }

    if(mtmp->mtame != 0) {
        return;
    }

    mtmp->mpeaceful = 0;

    if(ishuman(mtmp) != 0) {
        pline("%s get angry!", Monnam(mtmp));
    }
}
  
