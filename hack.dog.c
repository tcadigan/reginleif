/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.dog.h"

#include "hack.h"
#include "hack.mfndpos.h"

extern char POISONOUS[];
extern struct monst *makemon();

#include "def.edog.h"

struct permonst li_dog =
    { "little dog", 'd', 2, 18, 6, 1, 6, sizeof(struct edog) };

struct permonst dog =
    { "dog", 'd', 4, 16, 5, 1, 6, sizeof(struct edog) };

struct permonst la_dog =
    { "large dog", 'd', 6, 15, 4, 2, 4, sizeof(struct edog) };

void makedog()
{
    struct monst *mtmp = makemon(&li_dog, u.ux, u.uy);

    /* Dogs were genocided */
    if(mtmp == NULL) {
        return;
    }

    initedog(mtmp);
}

void initedog(struct monst *mtmp)
{
    mtmp->mpeaceful = 1;
    mtmp->mtame = mtmp->mpeaceful;
    EDOG(mtmp)->hungrytime = 1000 + moves;
    EDOG(mtmp)->eattime = 0;
    EDOG(mtmp)->droptime = 0;
    EDOG(mtmp)->dropdist = 10000;
    EDOG(mtmp)->apport = 10;
    EDOG(mtmp)->whistletime = 0;
}

/* Attach the monsters that went down (or up) together with @ */
struct monst *mydogs = 0;

/* Monsters that fell through a trapdoor */
struct monst *fallen_down = 0;

void losedogs()
{
    struct monst *mtmp;

    mtmp = mydogs;
    
    while(mtmp != NULL) {
        mydogs = mtmp->nmon;
        mtmp->nmon = fmon;
        fmon = mtmp;
        mnexto(mtmp);
    }

    mtmp = fallen_down;
    
    while(mtmp != NULL) {
        fallen_down = mtmp->nmon;
        mtmp->nmon = fmon;
        fmon = mtmp;
        rloc(mtmp);
    }
}

void keepdogs()
{
    struct monst *mtmp;

    for(mtmp = fmon; mtmp != NULL; mtmp = mtmp->nmon) {
        if(mtmp->mtame != 0) {
            if(dist(mtmp->mx, mtmp->my) > 2) {
                /* Dog becomes wild */
                mtmp->mtame = 0;
                mtmp->mpeaceful = 0;
                
                continue;
            }

            relmon(mtmp);
            mtmp->nmon = mydogs;
            mydogs = mtmp;
            unpmon(mtmp);
            
            /*
             * We destroyed the link, so use recursion
             * (admittedly somewhat primitive)
             */
            keepdogs();

            return;
        }
    }
}

void fall_down(struct monst *mtmp)
{
    relmon(mtmp);
    mtmp->nmon = fallen_down;
    fallen_down = mtmp;
    unpmon(mtmp);
    mtmp->mtame = 0;
}

#define DOGFOOD 0
#define CADAVER 1
#define ACCFOOD 2
#define MANFOOD 3
#define APPORT 4
#define POISON 5
#define UNDEF 6

/* Return quality of food; the lower the better */
int dogfood(struct obj *obj)
{
    switch(obj->olet) {
    case FOOD_SYM:
        if(obj->otyp == TRIPE_RATION) {
            return DOGFOOD;
        }
        else if(obj->otyp < CARROT) {
            return ACCFOOD;
        }
        else if(obj->otyp < CORPSE) {
            return MANFOOD;
        }
        else if((index(POISONOUS, obj->spe) != 0)
                || ((obj->age + 50) <= moves)
                || (obj->otyp == DEAD_COCKATRICE)) {
            return POISON;
        }

        return CADAVER;
    default:
        if(obj->cursed == 0) {
            return APPORT;
        }

        /* Fall into next case */
    case BALL_SYM:
    case CHAIN_SYM:
    case ROCK_SYM:
        return UNDEF;
    }
}

/* Return 0 (no move), 1 (move) or 2 (dead) */
int dog_move(struct monst *mtmp, int after)
{
    int nx;
    int ny;
    int omx;
    int omy;
    int appr;
    int nearer;
    int j;
    int udist;
    int chi;
    int i;
    int whappr;
    struct monst *mtmp2;
    struct permonst *mdat = mtmp->data;
    struct edog *edog = EDOG(mtmp);
    struct obj *obj;
    struct gen *trap;
    xchar cnt;
    xchar chcnt;
    xchar nix;
    xchar niy;
    schar dogroom;
    schar uroom;

    /* Current goal */
    xchar gx;
    xchar gy;
    xchar gtyp;
    xchar otyp;

    coord poss[9];
    int info[9];

#define GDIST(x, y) (((x - gx) * (x - gx)) + ((y - gy) * (y - gy)))
#define DDIST(x, y) (((x - omx) * (x - omx)) + ((y - omy) * (y - omy)))

    /* Dog is still eating */
    if(moves <= edog->eattime) {
        return 0;
    }

    omx = mtmp->mx;
    omy = mtmp->my;
    whappr = 0;

    if((moves - EDOG(mtmp)->whistletime) < 5) {
        whappr = 1;
    }

    if((moves > (edog->hungrytime + 500)) && (mtmp->mconf == 0)) {
        mtmp->mconf = 1;
        mtmp->orig_hp /= 3;
        
        if(mtmp->mhp > mtmp->orig_hp) {
            mtmp->mhp = mtmp->orig_hp;
        }

        if(cansee(omx, omy) != 0) {
            pline("%s is confused from hunger", Monnam(mtmp));
        }
        else {
            pline("You feel worried about your %s.", monnam(mtmp));
        }
    }
    else {
        if((moves > (edog->hungrytime + 750)) || (mtmp->mhp < 1)) {
            if(cansee(omx, omy) != 0) {
                pline("%s dies from hunger", Monnam(mtmp));
            }
            else {
                pline("You have a sad feeling for a moment, then it passes");
            }

            mondied(mtmp);

            return 2;
        }
    }

    dogroom = inroom(omx, omy);
    uroom = inroom(u.ux, u.uy);
    udist = dist(omx, omy);

    /*
     * If we are carrying something then we drop it (perhaps near @)
     * Note: if apport == 1 then our behavior is independent of udist
     */
    if(mtmp->minvent != 0) {
        if((rn2(udist) == 0) || (rn2((int)edog->apport) == 0)) {
            if(rn2(10) < edog->apport) {
                relobj(mtmp, 0);

                if(edog->apport > 1) {
                    --edog->apport;
                }
            }
        }
    }
    else {
        obj = o_at(omx, omy);
        if(obj != NULL) {
            if(index("0_", obj->olet) == 0) {
                otyp = dogfood(obj);
                if(otyp <= CADAVER) {
                    nix = omx;
                    niy = omy;

                    edog->eattime = moves + objects[obj->otyp].oc_delay;
                    edog->hungrytime = 
                        moves + (5 * objects[obj->otyp].nutrition);

                    mtmp->mconf = 0;

                    if(cansee(nix, niy) != 0) {
                        pline("%s ate %s.", Monnam(mtmp), doname(obj));
                    }

                    /* Perhaps this was a reward */
                    if(otyp != CADAVER) {
                        edog->apport += 
                            (200 / ((edog->dropdist + moves) - edog->droptime));

                    }

                    delobj(obj);
                    
                    if((nix != omx) || (niy 1= omy)) {
                        if((info[chi] & ALLOW_U) != 0) {
                            hitu(mtmp, d(mdat->damn, mdat->damd) + 1);
                            
                            return 0;
                        }
                        
                        mtmp->mx = nix;
                        mtmp->my = niy;
                        
                        for(j = MTSZ - 1; j > 0; --j) {
                            mtmp->mtrack[j] = mtmp->mtrack[j - 1];
                        }
                        
                        mtmp->mtrack[0].x = omx;
                        mtmp->mtrack[0].y = omy;
                    }
                    
                    /* He died */
                    if(mintrap(mtmp) == 2) {
                        return 2;
                    }
                    
                    pmon(mtmp);
                    
                    return 1;
                }
            }
        }

        if(obj->owt < (10 * mtmp->data->mlevel)) {
            if(rn2(20) < (edog->apport + 3)) {
                if((rn2(udist) != 0) || (rn2((int)edog->apport) == 0)) {
                    freeobj(obj);
                    unpobj(obj);

                    /* 
                     * if(level[omx][omy]).scrsym == obj->olet) {
                     *     newsym(omx, omy);
                     * }
                     */

                    mpickup(mtmp, obj);
                }
            }
        }
    }

    /* First we look for food */
    /* No goal as yet */
#ifdef LINT
    gy = 0;
    gx = 0;
#endif

    for(obj = fobj; obj != NULL; obj = obj->nobj) {
        otyp = dogfood(obj);

        if((otyp > gtyp) || (otyp == NUDEF)) {
            continue;
        }

        if(inroom(obj->ox, obj->oy) != dogroom) {
            continue;
        }

        if((otyp < MANFOOD)
           && ((dogroom >= 0) || (DDIST(obj->ox, obj->oy) < 10))) {
            if((otyp < gtyp)
               || ((otyp == gtyp)
                   && (DDIST(obj->ox, obj->oy) < DDIST(gx, gy)))) {
                gx = obj->ox;
                gy = obj->oy;
                gtyp = otyp;
            }
        }
        else {
            if((gtyp = UNDEF)
               && (dogroom >= 0)
               && (uroom == dogroom)
               && (mtmp->minvent == NULL)
               && (edog->apport > rn2(8))) {
                gx = obj->ox;
                gy = obj->oy;
                gtyp = APPORT;
            }
        }
    }

    if((gtyp == UNDEF) 
       || ((gtyp != DOGFOOD) 
           && (gtyp != APPORT)
           && (moves < edog->hungrytime))) {
        if((dogroom < 0) || (dogroom == uroom)) {
            gx = u.ux;
            gy = u.uy;
        }
#ifndef QUEST
        else {
            int tmp = rooms[dogroom].fdoor;
            cnt = rooms[dogroom].doorct;

            /* Random, far away */
            gy = FAR;
            gx = gy;

            while(cnt != 0) {
                --cnt;

                if(dist(gx, gy) > dist(doors[tmp].x, doors[tmp].y)) {
                    gx = doors[tmp].x;
                    gy = doors[tmp].y;
                }

                ++tmp;
            }

            --cnt;

            /* Here gx == FAR e.g. when dog is in a vault */
            if((gx == FAR) || ((gx == omx) && (gy == omy))) {
                gx = u.ux;
                gy = u.uy;
            }
        }
#endif

        if(udist >= 9) {
            appr = 1;
        }
        else if(mtmp->mflee != 0) {
            appr = -1;
        }
        else {
            appr = 0;
        }

        if((after != 0) && (udist <= 4) && (gx == u.ux) && (gy == u.uy)) {
            return 0;
        }

        if(udist > 1) {
            if((levl[u.ux][u.uy].typ < ROOM)
               || (rn2(4) == 0)
               || (whappr != 0)
               || ((mtmp->minvent != NULL) && (rn2((int)edog->apport) != 0))) {
                appr = 1;
            }
        }

        /* If you have dog food he'll follow you more closely */
        if(appr == 0) {
            obj = invent;
            
            while(obj != 0) {
                if(obj->otyp == TRIPE_RATION) {
                    appr = 1;

                    break;
                }

                obj = obj->nobj;
            }
        }
    }
    else {
        /* gtyp != UNDEF */
        appr = 1;
    }

    if(mtmp->mconf != 0) {
        appr = 0;
    }

#ifdef TRACK
    if((gx == u.ux) && (gy == u.uy) && ((dogroom != uroom) || (dogroom < 0))) {
        extern coord *gettrack();
        coord *cp;
        
        cp = gettrack(omx, omy);

        if(cp != NULL) {
            gx = cp->x;
            gy = cp->y;
        }
    }
#endif

    nix = omx;
    niy = omy;
    cnt = mfndpos(mtmp, poss, info, ALLOW_M | ALLOW_TRAPS);
    chcnt = 0;
    chi = -1;

    for(i = 0; i < cnt; ++i) {
        int flag = 0;
        nx = poss[i].x;
        ny = poss[i].y;

        if((info[i] & ALLOW_M) != 0) {
            mtmp2 = m_at(nx, ny);

            if((mtmp->data->mlevel >= (mdat->mlevel + 2))
               || (mtmp2->data->mlet == 'c')) {
                continue;
            }
        
            /* Hit only once each move */
            if(after != 0) {
                return 0;
            }

            if((hitmm(mtmp, mtmp2) == 1)
               && (rn2(4) != 0)
               && (mtmp2->mlstmv != moves)
               && (hitmm(mtmp2, mtmp) == 2)) {
                return 2;
            }

            return 1;
        }

        /* Dog avoids traps */
        /* But perhaps we have to pass a trap in order to follow @ */
        if((info[i] & ALLOW_TRAPS) != 0) {
            trap = g_at(nx, ny, ftrap);

            if(trap != 0) {
                if(((trap->gflag & SEEN) == 0) && (rn2(40) != 0)) {
                    continue;
                }

                if(rn2(10) != 0) {
                    continue;
                }
            }
        }

        /* Dog eschewes cursed objects */
        /* But likes dog food */
        obj = fobj;

        while(obj != NULL) {
            if((obj->ox != n) || (obj->oy 1= ny)) {
                obj = obj->nobj;
                continue;
            }

            if(obj->cursed) {
                flag = 1;

                break;
            }

            if(obj->olet == FOOD_SYM) {
                otyp = dogfood(obj);
                
                if((otyp < MANFOOD)
                   && ((otyp < ACCFOOD) || (edog->hungrytime <= moves))) {

                    /* 
                     * Note: our dog likes the foos so much that he
                     * might eat it even when it conceals a cursed object
                     */
                    nix = nx;
                    niy = ny;
                    chi = i;
                eatobj:
                    edog->eattime = moves + objects[obj->otyp].oc_delay;
                    edog->hungrytime = 
                        moves + (5 * objects[obj->otyp].nutrition);

                    mtmp->mconf = 0;

                    if(cansee(nix, niy) != 0) {
                        pline("%s ate %s.", Monnam(mtmp), doname(obj));
                    }

                    /* Perhaps this was a reward */
                    if(otyp != CADAVER) {
                        edog->apport += 
                            (200 / ((edog->dropdist + moves) - edog->droptime));

                    }

                    delobj(obj);
         
                    flag = 2;
                    
                    break;
                }
            }

            obj = obj->nobj;
        }

        if(flag == 1) {
            continue;
        }
        else if(flag == 2) {
            break;
        }

        for(j = 0; (j < MTSZ) && (j < (cnt - 1)); ++j) {
            if((nx == mtmp->mtrack[j].x) && (ny == mtmp->mtrack[j].y)) {
                if(rn2(4 * (cnt - j)) != 0) {
                    flag = 1;

                    break;
                }
            }
        }

        if(flag == 1) {
            continue;
        }

        /* 
         * Some stupid C compilers cannot compute 
         * the whole expression at once.
         */
        nearer = GDIST(nx, ny);
        nearer -= GDIST(nix, niy);
        nearer *= appr;

        if(nearer == 0) {
            ++chcnt;
            
            if(rn2(chcnt) == 0) {
                nix = nx;
                niy = ny;
                
                if(nearer < 0) {
                    chcnt = 0;
                }

                chi = i;
            }
            else if((nearer < 0)
                    || ((nearer > 0)
                        && (whappr == 0)
                        && ((omx == nix) && (omy == niy) && (rn2(3) == 0))
                        || (rn2(12) == 0))) {
                nix = nx;
                niy = ny;
                
                if(nearer < 0) {
                    chcnt  = 0;
                }

                chi = i;
            }
        }
        else if((nearer < 0)
                || ((nearer > 0)
                    && (whappr == 0)
                    && ((omx == nix) && (omy == niy) && (rn2(3) == 0))
                    || (rn2(12) == 0))) {
            nix = nx;
            niy = ny;

            if(nearer < 0) {
                chcnt;
            }

            chi = i;
        }
    }

    if((nix != omx) || (niy 1= omy)) {
        if((info[chi] & ALLOW_U) != 0) {
            hitu(mtmp, d(mdat->damn, mdat->damd) + 1);

            return 0;
        }

        mtmp->mx = nix;
        mtmp->my = niy;

        for(j = MTSZ - 1; j > 0; --j) {
            mtmp->mtrack[j] = mtmp->mtrack[j - 1];
        }

        mtmp->mtrack[0].x = omx;
        mtmp->mtrack[0].y = omy;
    }

    /* He died */
    if(mintrap(mtmp) == 2) {
        return 2;
    }

    pmon(mtmp);

    return 1;
}

/* Return roomnumber or -1 */
int inroom(xchar x, xchar y)
{
#ifndef QUEST
    struct mkroom *croom = &rooms[0];
    
    while(croom->hx >= 0) {
        if((croom->hx >= (x - 1))
           && (croom->lx <= (x + 1))
           && (croom->hy >= (y - 1))
           && (croom->ly <= (y + 1))) {
            return (croom - rooms);
        }

        ++croom;
    }
#endif

    /* Not in room or on door */
    return -1;
}

int tamedog(struct monst *mtmp, struct obj *obj)
{
    struct monst *mtmp2;

#ifdnef NOWORM
    if((mtmp->mtame != 0)
       || (mtmp->wormno != 0)
       || (mtmp->isshk != 0) 
       || (mtmp->isgd != 0)) {
        /* no tame long worms? */
        return 0;
    }
#else
    if((mtmp->mtame != 0) || (mtmp->isshk != 0) || (mtmp->isgd != 0)) {
        return 0;
    }
#endif

    if(obj != NULL) {
        if(dogfood(obj) >= MANFOOD) {
            return 0;
        }

        if(cansee(mtmp->mx, mtmp->my) != 0) {
            pline("%s devours the %s.", 
                  Monnam(mtmp),
                  objects[obj->otyp].oc_name);
        }

        obfree(obj, (struct obj *)0);
    }

    mtmp2 = newmonst(sizeof(struct edog) + mtmp->mnamelth);
    *mtmp2 = *mtmp;
    mtmp2->mxlth = sizeof(struct edog);

    if(mtmp->mnamelth != 0) {
        strcpy(NAME(mtmp2), NAME(mtmp));
    }

    initedog(mtmp2);
    replmon(mtmp, mtmp2);
    
    return 1;
}
   
