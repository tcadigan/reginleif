/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.trap.h"

#include "hack.h"

char vowels[] = "aeiou";

char *traps[] = {
    " bear trap",
    "n arrow trap",
    " dart trap",
    " trapdoor",
    " teleportation trap",
    " pit",
    " sleeping gas trap",
    " piercer",
    " mimic"
};

void dotrap(struct gen *trap)
{
    nomul(0);

    if((trap->gflag & SEEN) && (rn2(5) == 0)) {
        pline("You escape a%s.", traps[trap->gflag & 037]);
    }
    else {
        trap->gflag |= SEEN;
    
        switch(trap->gflag & ~SEEN) {
        case SLP_GAS_TRAP:
            pline("A cloud of gas puts you to sleep!");
            nomul(-rnd(25));

            break;
        case BEAR_TRAP:
            if(Levitation != NULL) {
                pline("You float over a bear trap.");
                
                break;
            }
            
            u.utrap = 4 + rn2(4);
            u.utraptype = TT_BEARTRAP;
            pline("A bear trap closes on your foot");

            break;
        case PIERC:
            deltrap(trap);

            if(makemon(PM_PIERC, u.ux, u.uy) != NULL) {
                pline("A piercer suddenly drops from the ceiling!");
                
                if(uarmh != 0) {
                    pline("Its blow glances off your helmet.");
                }
                else {
                    thitu(3, d(4, 6), "falling piercer");
                }
            }

            break;
        case ARROW_TRAP:
            pline("An arrow shoots out at you!");

            if(thitu(8, rnd(6), "arrow") == NULL) {
                mksobj_at(WEAPON_SYM, ARROW, u.ux, u.uy);

                fobj->quan = 1;
            }

            break;
        case TRAPDOOR:
            if(xdnstair == NULL) {
                pline("A trap door in the ceiling opens and a rock falls on your head!");

                if(uarmh != 0) {
                    pline("Fortunately, you are wearing a helmet!");

                    losehp(2, "falling rock");
                }
                else {
                    losehp(d(2, 10), "falling rock");
                }
            }
            else {
                int newlevel = dlevel + 1;

                while((rn2(4) == 0) && (newlevel < 29)) {
                    ++newlevel;
                }

                pline("A trap door opens up under you!");

                if((Levitation != 0) || (u.ustuck != 0)) {
                    pline("For some reason you don't fall in.");
                
                    break;
                }

                goto_level(newlevel, FALSE);
            }

            break;
        case DART_TRAP:
            pline("A little dart shoots out at you!");

            if(thitu(7, rnd(3), "little dart") != 0) {
                if(rn2(6) == 0) {
                    poisoned("dart", "poison dart");
                }
            }
            else {
                mksobj_at(WEAPON_SYM, DART, u.ux, u.uy);
                fobj->quan = 1;
            }

            break;
        case TELEP_TRAP:
            newsym(u.ux, u.uy);
            tele();

            break;
        case PIT:
            if(Levitation != 0) {
                pline("A pit opens up under you!");
                pline("You don't fall in!");

                break;
            }

            pline("You fall into a pit!");
            u.utrap = rn1(6, 2);
            u.utraptype = TT_PIT;
            losehp(rnd(6), "fall into a pit");
            selftouch("Falling, you");

            break;
        default:
            pline("You hit a trap of type %d", trap->gflag);

            impossible();
        }
    }
}

int mintrap(struct monst *mtmp)
{
    struct gen *gen = g_at(mtmp->mx, mtmp->my, ftrap);
    int wasintrap = mtmp->mtrapped;

    if(gen == NULL) {
        /* Perhaps teleported? */
        mtmp->mtrapped = 0;
    }
    else if(wasintrap != 0) {
        if(rn2(40) == NULL) {
            mtmp->mtrapped = 0;
        }
    }
    else {
        int tt = (gen->gflag & ~SEEN);
        int in_sight = cansee(mtmp->mx, mtmp->my);

        extern char mlarge[];

        if((mtmp->mtrapseen != 0) && (1 << tt)) {
            /* He has been in such a trap - perhaps he escapes */
            if(rn2(4) != 0) {
                return 0;
            }
        }

        mtmp->mtrapseen |= (1 << tt);
        
        switch(tt) {
        case BEAR_TRAP:
            if(index(mlarge, mtmp->data->mlet) != 0) {
                if(in_sight != 0) {
                    pline("%s is caught in a bear trap!", Monnam(mtmp));
                }
                else {
                    if(mtmp->data->mlet == 'o') {
                        pline("You hear the roaring of an angry bear!");
                    }
                }
                
                mtmp->mtrapped = 1;
            }
            
            break;
        case PIT:
            if(index("Eyw", mtmp->data->mlet) == NULL) {
                mtmp->mtrapped = 1;
                if(in_sight != 0) {
                    pline("%s falls int a pit!", Monnam(mtmp));
                }
            }
            
            break;
        case SLP_GAS_TRAP:
            if((mtmp->msleep == 0) && (mtmp->mfroz == NULL)) {
                mtmp->msleep = 1;
                
                if(in_sight != 0) {
                    pline("%s suddenly falls asleep!", Monnam(mtmp));
                }
            }
            
            break;
        case TELEP_TRAP:
            rloc(mtmp);
            
            if((in_sight != 0) && (cansee(mtmp->mx, mtmp->my) == NULL)) {
                pline("%s suddenly disappears!", Monname(mtmp));
            }
            
            break;
        case ARROW_TRAP:
            if(in_sight != 0) {
                pline("%s is hit by an arrow!", Monname(mtmp));
            }
            
            mtmp->mhp -= 3;
            
            break;
        case DART_TRAP:
            if(in_sight != 0) {
                pline("%s is hith by a dart!", Monname(mtmp));
            }
            
            mtmp->mhp -= 2;
            
            /* not mon died here!! */
            break;
        case TRAPDOOR:
            if(xdnstair == NULL) {
                mtmp->mhp -= 10;
                
                if(in_sight != 0) {
                    pline("A trap door in the ceiling opens and a rock hits %s!", monnam(mtmp));
                }
                
                break;
            }
            
            if(mtmp->data->mlet != 'w') {
                fall_down(mtmp);
                
                if(in_sight != 0) {
                    pline("Suddenly, %s disappears out of sight.", monnam(mtmp));
                }
                
                return 2;
            }
            
            break;
        case PIERC:
            
            break;
        default:
            pline("Some monster encountered an impossible trap.");
            
            impossible();
        }
    }

    return mtmp->mtrapped;
}

void selftouch(char *arg)
{
    if((uwep != 0) && (uwep->otyp == DEAD_COCKATRICE)) {
        pline("%s touch the dead cockatrice.", arg);
        pline("You turn to stone.");
        
        killer = objects[uwep->otyp].oc_name;
        
        done("died");
    }
}

void float_up()
{
    if(u.utrap != 0) {
        if(u.utraptype == TT_PIT) {
            u.utrap = 0;
            pline("You float up, out of the pit!");
        }
        else {
            pline("You float up, only your leg is still stuck.");
        }
    }
    else {
        pline("You start to float in the air!");
    }
}

void float_down()
{
    struct gen *trap;
    pline("You float gently to the ground.");

    trap = g_at(u.ux, u.uy, ftrap);

    if(trap != NULL) {
        switch(trap->gflag & 037) {
        case PIERC:

            break;
        case TRAPDOOR:
            if((xdnstair == NULL) || (u.ustuck != 0)) {
                break;
            }

            /* Fall into next case */
        default:
            dotrap(trap);
        }
    }

    pickup();
}

void tele()
{
    coord cc;
    int nux;
    int nuy;

    int got_pos = 0;
    if(Teleport_control != 0) {
        pline("To what position do you want to be teleported?");

        /* 1: force valid */
        cc = getpos(1, "the desired position");

        /*
         * Possible extensions: Introduce a small error if
         * magic power is low; Allow trasfer to solid rock 
         */
        if(teleok(cc.x, cc.y) != 0) {
            nux = cc.x;
            nuy = cc.y;

            got_pos = 1;
        }
        else {
            pline("Sorry ...");
        }
    }

    if(got_pos == 0) {
        nux = rnd(COLNO - 1);
        nuy = rn2(ROWNO);
        
        while(teleok(nux, nuy) == NULL) {
            nux = rnd(COLNO - 1);
            nuy = rn2(ROWNO);
        }
    }

    if(Punished != 0) {
        unplacebc();
    }

    unsee();
    u.utrap = 0;
    u.ustuck = 0;
    u.ux = nux;
    u.uy = nuy;
    setsee();

    if(Punished != 0) {
        placebc(1);
    }

    if(u.uswallow != 0) {
        u.uswallow = 0;
        u.uswldtim = u.uswallow;

        docrt();
    }

    nomul(0);
    inshop();
    pickup();

    if(Blind == 0) {
        read_engr_at(u.ux, u.uy);
    }
}

int teleok(int x, int y)
{
    /* Note: gold is permitted (becuase of vaults) */
    return ((isok(x, y) != 0)
            && (levl[x][y].typ > DOOR)
            && (m_at(x, y) == NULL)
            && (sobj(ENORMOUS_ROCK, x, y) == NULL)
            && (g_at(x, y, ftrap) == NULL));
}

void placebc(int attach)
{
    if((uchain == 0) || (uball == 0)) {
        pline("Where are your chain and ball??");
        
        impossible();

        return;
    }

    uchain->ox = u.ux;
    uball->ox = uchain->ox;
    uchain->oy = u.uy;
    uball->oy = uchain->oy;

    if(attach != 0) {
        uchain->nobj = fobj;
        fobj = uchain;

        if(carried(uball) == NULL) {
            uball->nobj = fobj;
            fobj = uball;
        }
    }
}

void unplacebc()
{
    if(carried(uball) == NULL) {
        freeobj(uball);
        unpobj(uball);
    }

    freeobj(uchain);
    unpobj(uchain);
}

void level_tele()
{
    /* 5 - 24 */
    int newlevel = 5 + rn2(20);

    if(dlevel == newlevel) {
        if(xdnstair == NULL) {
            --newlevel;
        }
        else {
            ++newlevel;
        }
    }

    goto_level(newlevel, FALSE);
}
