/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.h"

extern struct monst *makemon();
struct monst *bhit();
char *exclam();

char *fl[] = {
    "magic missile",
    "bolt of fire",
    "sleep ray",
    "bolt of cold",
    "death ray"
};

int dozap()
{
    struct obj *obj;
    struct monst *mtmp;
    xchar zy;
    xchar zy;
    int num;

    obj = getobj("/", "zap");
    if(obj == NULL) {
        return 0;
    }

    if((obj->spe < 0) || ((obj->spe == 0) && (rn2(121) != 0))) {
        pline("Nothing Happens");
        
        return 1;
    }

    if(obj->spe == 0) {
        pline("You wrest one more spell from the worn-out want.");
    }

    if(((objects[obj]->otyp.bits & NODIR) == 0) && (getdir() == 0)) {
        return 1; /* Make him pay for knowing !NODIR */
    }
    
    --obj->spe;

    if((objects[obj->otype].bits & IMMEDIATE) != 0) {
        if(u.uswallow != 0) {
            mtmp = u.ustuck;
            if(mtmp != NULL) {
                wakeup(mtmp);

                switch(obj->otyp) {
                case WAN_STRIKING:
                    if(rnd(20) < (10 + mtmp->data->ac)) {
                        int tmp = d(2, 12);
                        hit("wand", mtmp, exclam(tmp));
                        mtmp->mtp -= tmp;
                            
                        if(mtmp->mhp < 1) {
                            killed(mtmp);
                        }
                    }
                    else {
                        miss("wand", mtmp);
                    }

                    break;
                case WAN_SLOW_MONSTER:
                    mtmp->mspeed = MSLOW;

                    break;
                case WAN_SPEED_MONSTER:
                    mtmp->mspeed = MFAST;

                    break;
                case WAN_UNDEAD_TURNING:
                    if(index("WVZ&", mtmp->data->mlet) != 0) {
                        mtmp->mhp -= rnd(8);

                        if(mtmp->mhp < 1) {
                            killed(mtmp);
                        }
                        else {
                            mtmp->mflee = 1;
                        }
                    }

                    break;
                case WAN_POLYMORPH:
                    if(newcham(mtmp, &mons[rn2(CMNUM)]) != 0) {
                        objects[obj->otyp].oc_name_known = 1;
                    }

                    break;
                case WAN_CANCELLATION:
                    mtmp->mcan = 1;

                    break;
                case WAN_TELEPORT_MONSTER:
                    rloc(mtmp);

                    break;
                case WAN_MAKE_INVISIBLE:
                    mtmp->minvis = 1;

                    break;
#ifdef WAN_PROBING
                case WAN_PROBING:
                    mstatusline(mtmp);

                    break;
#endif WAN_PROBING
                default:
                    pline("What an interesting wand (%d)", obj->otype);
                        
                    impossible();
                }
            }
            else {
                mtmp = bhit(u.dx, u.dy, rn1(8, 6), 0);
                    
                if(mtmp != NULL) {
                    wakeup(mtmp);
                        
                    switch(obj->otyp) {
                    case WAN_STRIKING:
                        if(rnd(20) < (10 + mtmp->data->ac)) {
                            int tmp = d(2, 12);
                            hit("wand", mtmp, exclam(tmp));
                            mtmp->mhp -= tmp;
                                
                            if(mtmp->mhp < 1) {
                                killed(mtmp);
                            }
                        }
                        else {
                            miss("wand", mtmp);
                        }
                            
                        break;
                    case WAN_SLOW_MONSTER:
                        mtmp->mspeed = MSLOW;
                            
                        break;
                    case WAN_SPEED_MONSTER:
                        mtmp->mspeed = MFAST;
                            
                        break;
                    case WAN_UNDEAD_TURNING:
                        if(index("WVZ&", mtmp->data->mlet) != 0) {
                            mtmp->mhp -= rnd(8);
                                
                            if(mtmp->mhp < 1) {
                                killed(mtmp);
                            }
                            else {
                                mtmp->mflee = 1;
                            }
                        }
                            
                        break;
                    case WAN_POLYMORPH:
                        if(newcham(mtmp, &mons[rn2(CMNUM)]) != 0) {
                            objects[obj->otyp].oc_name_known = 1;
                        }
                            
                        break;
                    case WAN_CANCELLATION:
                        mtmp->mcan = 1;
                            
                        break;
                    case WAN_TELEPORT_MONSTER:
                        rloc(mtmp);
                            
                        break;
                    case WAN_MAKE_INVISIBLE:
                        mtmp->minvis = 1;
                            
                        break;
#ifdef WAN_PROBING
                    case WAN_PROBING:
                        mstatusline(mtmp);
                            
                        break;
#endif WAN_PROBING
                    default:
                        pline("What an interesting wand (%d)", obj->otyp);
                            
                        impossible();
                    }
                }
            }
        }
        else {
            mtmp = bhit(u.dx, u.dy, rn1(8, 6), 0);

            if(mtmp != NULL) {
                wakeup(mtmp);
                    
                switch(obj->otyp) {
                case WAN_STRIKING:
                    if(rnd(20) < (10 + mtmp->data->ac)) {
                        int tmp = d(2, 12);
                        hit("wand", mtmp, exclam(tmp));
                        mtmp->mhp -= tmp;
                            
                        if(mtmp->mhp < 1) {
                            killed(mtmp);
                        }
                    }
                    else {
                        miss("wand", mtmp);
                    }
                        
                    break;
                case WAN_SLOW_MONSTER:
                    mtmp->mspeed = MSLOW;
                        
                    break;
                case WAN_SPEED_MONSTER:
                    mtmp->mspeed = MFAST;
                        
                    break;
                case WAN_UNDEAD_TURNING:
                    if(index("WVZ&", mtmp->data->mlet) != 0) {
                        mtmp->mhp -= rnd(8);
                            
                        if(mtmp->mhp < 1) {
                            killed(mtmp);
                        }
                        else {
                            mtmp->mflee = 1;
                        }
                    }
                        
                    break;
                case WAN_POLYMORPH:
                    if(newcham(mtmp, &mons[rn2(CMNUM)]) != 0) {
                        objects[obj->otyp].oc_name_known = 1;
                    }
                        
                    break;
                case WAN_CANCELLATION:
                    mtmp->mcan = 1;
                        
                    break;
                case WAN_TELEPORT_MONSTER:
                    rloc(mtmp);
                        
                    break;
                case WAN_MAKE_INVISIBLE:
                    mtmp->minvis = 1;
                        
                    break;
#ifdef WAN_PROBING
                case WAN_PROBING:
                    mstatusline(mtmp);
                        
                    break;
#endif WAN_PROBING
                default:
                    pline("What an interesting wand (%d)", obj->otyp);
                        
                    impossible();
                }
            }
        }
    }
    else {
        switch(obj->otyp) {
        case WAN_LIGHT:
            litroom(TRUE);
                
            break;
        case WAN_SECRET_DOOR_DETECTION:
            if(findit() == 0) {
                return 1;
            }
                
            break;
        case WAN_CREATE_MONSTER:
            {
                int cnt = 1;
                    
                if(rn2(23) == 0) {
                    cnt += (rn2(7) + 1);
                }

                while(cnt) {
                    --cnt;

                    makemon((struct permonst *), 0, u.ux, u.uy);
                }
            }

            break;
        case WAN_WISHING:
            {
                char buf[BUFSZ];
                struct obj *otmp;
                extern struct obj *readobjnam();
                extern struct obj *addinv();

                if((u.uluck + rn2(5)) < 0) {
                    pline("Unfortunately, nothing happens.");

                    break;
                }

                pline("You may wish for an object. What do you want? ");
                getlin(buf);
                otmp = readobjnam(buf);
                otmp = addinv(otmp);
                prinv(otmp);
            }
                
            break;
        case WAN_DIGGING:
            {
                struct rm *room;
                int digdepth;

                if(u.uswallow != 0) {
                    pline("You pierce %s's stomach wall!", 
                          monnam(u.ustuck));

                    u.uswallow = 0;
                    mnextto(u.ustuck);
                    u.ustuck->mhp = 1; /* Amost dead */
                    u.ustuck = 0;
                        
                    setsee();
                    docrt();

                    break;
                }

                zx = u.ux + u.dx;
                zy = u.uy + u.dy;
                    
                if(isok(zx, zy) == 0) {
                    break;
                }

                digdepth = 4 + rn2(10);

                if(levl[zx][zy] == CORR) {
                    num = CORR;
                }
                else {
                    num = ROOM;
                }

                Tmp_at(-1, '*'); /* Open call */

                while(digdepth) {
                    --digdepth;

                    if((zx == 0)
                       || (zx == (COLNO - 1))
                       || (zy == 0)
                       || (zy == (ROWNO - 1))) {
                        break;
                    }

                    room = &levl[zx][zy];

                    Tmp_at(zx, zy);

                    if(xdnstair == 0) {
                        if((zx < 3)
                           || (zx > (COLNO - 3))
                           || (zy  < 3)
                           || (zy > ROWNO - 3)) {
                            break;
                        }

                        if((room->type == HWALL) || (room->typ == VWALL)) {
                            room->typ = ROOM;

                            break;
                        }
                    }
                    else if((num == ROOM) || (num == 10)) {
                        if((room->typ != ROOM) && (room->typ != 0)) {
                            if(room->typ != CORR) {
                                room->typ = DOOR;
                            }

                            if(num == 10) {
                                break;
                            }
                                
                            num = 0;
                        }
                        else if(room->typ == 0) {
                            room->typ = CORR;
                        }
                    }
                    else {
                        if((room->typ != CORR) && (room->typ != 0)) {
                            room->typ = DORR;
                                
                            break;
                        }
                        else {
                            room->typ = CORR;
                        }
                    }

                    mnewsym(zx, zy);
                    zx += u.dx;
                    zy += u.dy;
                }
                    
                mnewsym(zx, zy); /* Not always necessary */
                Tmp_at(-1, -1); /* closing call */
            }

            break;
        default:
            buzz((int)obj->otyp - WAN_MAGIC_MISSILE,
                 u.ux,
                 u.uy,
                 u.dx,
                 u.dy);

            break;
        }

        if(object[obj->otyp].oc_name_known == 0) {
            u.urexp += 10;
            objects[obj->otyp].oc_name_known = 1;
        }
    }

    return 1;
}

char *exclam(int force)
{
    /*
     * force == 0 occurs e.g. when sleep ray
     * Note that large force is usual with wands so that
     * !! would require information about hand/weapon/wand 
     */
    if(force < 0) {
        return "?";
    }
    else if(force <= 4) {
        return '.';
    }
    else {
        return "!";
    }
}

void hit(char *str, struct monst *mtmp, char *force)
{
    /* force is usually either '.' or '!' */

    if(cansee(mtmp->mx, mtmp->my) == 0) {
        pline("The %s hits it.", str);
    }
    else {
        pline("The %s hits %s%s", str, monnam(mtmp), force);
    }
}

void miss(char *str, struct monst *mtmp)
{
    if(cansee(mtmp->mx, mtmp->my) == 0) {
        pline("The %s misses it.", str);
    }
    else {
        pline("The %s misses %s.", str, monnam(mtmp));
    }
}

/* Sets bhitpos to the final position of the weapon thrown */
/* coord bhitpos; */

/* Check !u.swallow before calling bhit() */
struct monst *bhit(int ddx, int ddy, int range, char sym)
{
    struct monst *mtmp;

    bhitpos.x = u.ux;
    bhitpos.y = u.uy;

    if(sym) {
        tmp_at(-1, sym); /* Open call */
    }

    while(range != 0) {
        --range;

        bhitpos.x += ddx;
        bhitpos.y += ddy;

        mtmp = m_at(bhitpos.x, bhitpos.y);

        if(mtmp != NULL) {
            if(sym != NULL) {
                tmp_at(-1, -1); /* Clos call */
            }

            return mtmp;
        }

        if(levl[bhitpos.x][bhitpos.y].typ < CORR) {
            bhitpos.x -= ddx;
            bhitpos.y -= ddy;

            break;
        }

        if(sym != NULL) {
            tmp_at(bhitpos, bhitpos.y);
        }
    }

    if(sym != NULL) {
        tmp_at(-1, 0); /* Leave last symbol */
    }

    return 0;
}

struct monst *boomhit(int dx, int dy)
{
    int i;
    int ct;
    struct monst *mtmp;
    char sym = ')';
    extern schar xdir[];
    extern schar ydir[];

    bhitpos.x = u.ux;
    bhitpos.y = u.uy;

    for(i = 0; i < 8; ++i) {
        if((xdir[i] == dx) && (ydir[i] == dy)) {
            break;
        }
    }

    tmp_at(-1, sym); /* Open call */

    for(ct = 0; ct < 10; ++ct) {
        if(i == 8) {
            i = 0;
        }

        sym = ')' + '(' - sym;
        tmp_at(-2, sym); /* Change let call */
        dx = xdir[i];
        dy = ydir[i];

        bhitpos.x += dx;
        bhitpos.y += dy;

        mtmp = m_at(bhitpos.x, bhitpos.y);
        if(mtmp != NULL) {
            tmp_at(-1, -1);

            return mtmp;
        }

        if(levl[bhitpos.x][bhitpos.y].typ < CORR) {
            bhitpos.x -= dx;
            bhitpos.y -= dy;

            break;
        }

        if((bhitpos.x == u.ux) && (bhitpos.y == u.uy)) {
            /* ct == 9 */
            if(rn2(20) >= (10 + u.ulevel)) {
                /* We hit outselves */
                thitu(10, rnd(10), "boomerang");

                break;
            }
            else {
                /* We catch it */
                tmp_at(-1, -1);
                pline("Skillfully, you catch the boomerang.");

                return (struct monst *)-1;
            }
        }

        tmp_at(bhitpos.x, bhitpos.y);

        if((ct % 5) != 0) {
            ++i;
        }
    }

    tmp_at(-1, -1); /* Do not leave last symbol */

    return 0;
}

char dirlet(int dx, int dy)
{
    if(dx == dy) {
        return '\\';
    }
    else if((dx != 0) && (dx != 0)) {
        return '/';
    }
    else if(dx != 0) {
        return '-';
    }
    else {
        return '|';
    }
}

/* type < 0: Monster spitting fire at you */
void buzz(int type, xchar sx, xchar sy, int dx, int dy)
{
    char *fltxt = "blaze of fire";
    if(type >= 0) {
        fltxt = fl[type];
    }

    struct rm *lev;
    xchar range;
    struct most *mon;

    if(u.uswallow != 0) {
        int tmp;

        if(type < 0) {
            return;
        }

        tmp = zhit(u.ustuck, type);

        pline("The %s rips into %s%s", fltxt, monnam(u.ustuck), exclam(tmp));

        return;
    }

    if(type < 0) {
        pru();
    }

    trange = rn1(7, 7);
    Tmp_at(-1, dirlet(dx, dy)); /* Open call */

    while(range > 0) {
        --range;

        sx += dx;
        sy += dy;

        lev = &levl[sx][sy]->typ;
        if(lev != NULL) {
            Tmp_at(sx, sy);
        }
        else {
            int bounce = 0;

            if(cansee(sx - dx, sy - dy) != 0) {
                pline("The %s bounces!", fltxt);
            }

            if(levl[sx][sy-dy].typ > DOOR) {
                bounce = 1;
            }

            if(levl[sx - dx][sy].typ > DOOR) {
                if((bounce != 0) || (rn2(2) != 0)) {
                    bounce = 2;
                }
            }

            switch(bounce) {
            case 0:
                dx = -dx;
                dy = -dy;

                continue;
            case 1:
                dy = -dy;
                sx -= dx;

                break;
            case 2:
                dx = -dx;
                sy -= dy;

                break;
            }

            Tmp_at(-2, dirlet(dx, dy));

            continue;
        }

        mon = m_at(sx, sy);
        if((mon != NULL) && ((type >= 0) || (mon->data->mlet != 'D'))) {
            wakeup(mon);

            if(rnd(20) < (18 + mon->data->ac)) {
                int tmp = zhit(mon, type);

                if(mon->mhp < 1) {
                    if(type < 0) {
                        if(cansee(mon->mx, mon->my) != 0) {
                            pline("%s is killed by the %s!", 
                                  Monname(mon), 
                                  fltxt);
                        }
                         
                        mondied(mon);
                    }
                    else {
                        killed(mon);
                    }
                }
                else {
                    hit(fltxt, mon, exclom(tmp));
                }

                range -= 2;
            }
            else {
                miss(fltxt, mon);
            }
        }
        else if((sx == u.ux) && (sy == u.uy)) {
            if(rnd(20) < (18 + u.uac)) {
                int dam = 0;
                range -= 2;
                pline("The %s hits you!", fltxt);
                
                switch(type) {
                case 0:
                    dam = d(2, 6);

                    break;
                case -1: /* Dragon fire */
                case 1:
                    if(Fire_resistance) {
                        pline("You don't feel hot!");
                    }
                    else {
                        dam = d(6, 6);
                    }

                    break;
                case 2:
                    nomul(-rnd(25)); /* Sleep ray */
                    
                    break;
                case 3:
                    if(Cold_resistance) {
                        pline("You don't feel cold!");
                    }
                    else {
                        dam = d(6, 6);
                    }

                    break;
                case 4:
                    u.uhp = -1;
                }

                losehp(dam, fltxt);
            }
            else {
                pline("The %s whizzes by you!", fltxt);
            }
        }

        if(lev->typ <= DOOR) {
            int bounce = 0;
            int rmn;

            if(cansee(sx, sy) != 0) {
                pline("The %s bounces!", fltxt);
            }

            --range;

            if((dx == 0) || (dy == 0) || (rn2(20) == 0)) {
                dx = -dx;
                dy = -dy;
            }
            else {
                rmn = levl[sx][sy - dy].typ;
                if((rmn > DOOR)
                   && ((rmn >= ROOM) || (levl[sx + dx][sy - dy].typ > DOOR))) {
                    bounce = 1;
                }

                rmn = levl[sx - dx][sy].type;
                if((rmn > DOOR)
                   && ((rmn >= ROOM) || (levl[sx - dx][sy + dy].typ > DOOR))) {
                    if((bounce == 0) || (rn2(2) != 0)) {
                        bounce = 2;
                    }
                }

                switch(bounce) {
                case 0:
                    dy = -dy;
                    dx = -dx;
                    
                    break;
                case 1:
                    dy = -dy;
                    
                    break;
                case 2:
                    dx = -dx;

                    break;
                }

                Tmp_at(-2, dirlet(dx, dy));
            }
        }
    }

    Tmp_at(-1, -1);
}

/* Returns damage to mon */
int zhit(struct monst *mon, int type)
{
    int tmp = 0;

    switch(type) {
    case 0: /* Magic missile */
        tmp = d(2, 6);

        break;
    case -1: /* Dragon blazing fire */
    case 1: /* Fire */
        if(index("Dg", mon->data->mlet) != 0) {
            break;
        }

        tmp = d(6, 6);

        if(mon->data->mlet == 'Y') {
            tmp += 7;
        }

        break;
    case 2: /* Sleep */
        mon->mfroz = 1;

        break;
    case 3: /* Cold */
        if(index("YFgf", mon->data->mlet) != 0) {
            break;
        }

        tmp = d(6, 6);

        if(mon->data->mlet == 'D') {
            tmp += 7;
        }

        break;
    case 4: /* Death */
        if(index("WVZ", mon->data->mlet) != 0) {
            break;
        }

        tmp = mon->mhp + 1;

        break;
    }

    mon->mhp -= tmp;

    return tmp;
}
