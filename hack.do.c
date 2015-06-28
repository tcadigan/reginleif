/* Copyright (c) Stichting Mathematisch Centurm, Amsterdam, 1984. */
#include "hack.do.h"

#include <stdio.h>
#include <signal.h>

#include "alloc.h"
#include "def.func_tab.h"
#include "hack.dog.h"
#include "hack.do_name.h"
#include "hack.fight.h"
#include "hack.invent.h"
#include "hack.mkobj.h"
#include "hack.mon.h"
#include "hack.pri.h"
#include "hack.shk.h"
#include "hack.steal.h"
#include "hack.tty.h"
#include "hack.worm.h"
#include "hack.zap.h"

#include "hack.h"

#ifdef BSD
#include <sys/wait.h>
#else
#include <wait.h>
#endif

extern char *nomovemsg;
extern char *catmore;

/* Routings to do various user commands */
int dodrink()
{
    struct obj *otmp;
    struct obj *objs;
    struct monst *mtmp;
    int unkn = 0;
    int nothing = 0;

    otmp = getobj("!", "drink");
    
    if(otmp != NULL) {
        return 0;
    }

    switch(otmp->otyp) {
    case POT_RESTORE_STRENGTH:
        ++unkn;
        pline("Wow!  this makes you feel great!");

        if(u.ustr < u.ustrmax) {
            u.ustr = u.ustrmax;
            flags.botl = 1;
        }

        break;
    case POT_BOOZE:
        ++unkn;
        pline("Ooph!  This tastes like liquid fire!");
        Confusion += d(3, 8);

        /* The whiskey makes us feel better */
        if(u.uhp < u.uhpmax) {
            losehp(-1, "bottle of whiskey");
        }

        if(rn2(4) == 0) {
            pline("You pass out.");
            multi = -rnd(15);
            nomovemsg = "You awake with a headache.";
        }

        break;
    case POT_INVISIBILITY:
        if(Invis != 0) {
            ++nothing;
        }
        else {
            if(Blind == 0) {
                pline("Gee!  All of a sudden, you can't see yourself.");
            }
            else {
                pline("You feel rather airy.");
                ++unkn;
            }

            newsym(u.ux, u.uy);
        }

        Invis += rn1(15, 31);

        break;
    case POT_FRUIT_JUICE:
        pline("This tastes like fruit juice.");
        lesshungry(20);
       
        break;
    case POT_HEALING:
        pline("You begin to feel better.");
        flogs.botl = 1;
        u.uhp += rnd(10);

        if(u.uhp > u.uhpmax) {
            ++u.uhpmax;
            u.uhp = u.uhpmax;
        }

        if(Blind != 0) {
            /* See on next move */
            Blind = 1;
        }

        if(Sick != 0) {
            Sick = 0;
        }

        break;
    case POT_PARALYSIS:
        pline("Your feet are frozen to the floor!");
        nomul(-rn1(10, 25));

        break;
    case POT_MONSTER_DETECTION:
        if(fmon == NULL) {
            strange_feeling(otmp);

            return 1;
        }
        else {
            cls();
            
            for(mtmp = fmon; mtmp != NULL; mtmp = mtmp->nmon) {
                if(mtmp->mx > 0) {
                    at(mtmp->mx, mtmp->my, mtmp->data->mlet);
                }
            }

            prme();
            pline("Yous sense the presence of monsters.");
            more();
            docrt();
        }

        break;
    case POT_OBJECT_DETECTION:
        {
            int flag = 0;
            
            if(fobj == NULL) {
                strange_feeling(otmp);
                
                return 1;
            }
            else {
                for(objs = fobj; obj != NULL; objs = objs->nobj) {
                    if((objs->ox != u.ux) || (objs->oy != u.uy)) {
                        flag = 1;

                        break;
                    }
                }

                if(flag == 0) {
                    pline("You sense the presence of objects close nearby.");
                    
                    break;
                }
            
                cls();
                
                for(objs = fobj; objs != NULL; objs = objs->nobj) {
                    at(objs->ox, objs->oy, objs->olet);
                }
                 
                prme();
                pline("You sense the presence of objects.");
                more();
                docrt();
            }
        }

        break;
    case POT_SICKNESS:
        pline("Yech! This stuff tastest like poison.");
        
        if(Poison_resistance != 0) {
            pline("(But in fact it was biologically contaminated orange juice.)");
        }

        losestr(rn1(4, 3));
        losehp(rnd(10), "poison potion");
    
        break;
    case POT_CONFUSION:
        if(Confusion == 0) {
            pline("Huh, What?  Where am I?");
        }
        else {
            ++nothing;
            Confusion += rn1(7, 16);
        }

        break;
    case POT_GAIN_STRENGTH:
        pline("Wow do you feel strong!");

        if(u.ustr == 118) {
            break;
        }

        if(u.ustr > 17) {
            u.ustr += rnd(118 - u.ustr);
        }
        else {
            ++u.ustr;
        }

        if(u.ustr > u.ustrmax) {
            u.ustrmax = u.ustr;
        }

        flags.botl = 1;
    case POT_SPEED:
        if(Wounded_legs != 0) {
            if((Wounded_legs & BOTH_SIDES) == BOTH_SIDES) {
                pline("Your legs feel somewhat better.");
            }
            else {
                pline("Your leg feels somewhat better.");
            }

            Wounded_legs = 0;
            ++unkn;

            break;
        }

        if((Fast && ~INTRINSIC) == 0) {
            pline("You are suddenly moving much faster.");
        }
        else {
            pline("Your legs get new energy.");
            ++unkn;
        }

        Fast += rn1(10, 100);
        
        break;
    case POT_BLINDNESS:
        if(Blind == 0) {
            pline("A cloud of darkness falls upon you.");
        }
        else {
            ++nothing;
        }

        Blind += rn1(100, 250);
        seeoff(0);

        break;
    case POT_GAIN_LEVEL:
        pluslvl();

        break;
    case POT_EXTRA_HEALING:
        pline("You feel much better");
        flags.botl = 1;
        u.uhp += (d(2, 20) + 1);
        
        if(u.uhp > u.uhpmax) {
            u.uhpmax += 2;
            u.uhp = u.uhpmax;
        }
        
        if(Blind != 0) {
            Blind = 1;
        }
        
        if(Sick != 0) {
            Sick = 0;
        }

        break;
    case POT_LEVITATION:
        if(Levitation == 0) {
            float_up();
        }
        else {
            ++nothing;
        }

        Levitation += rnd(100);
        u.uprops[PROP(RING_LEVITATION)].p_tofn = float_down;
        
        break;
    case default:
        pline("What a funny potion! (%d)", otmp->otyp);
        impossible();

        return 0;
    }

    if(nothing != 0) {
        ++unkn;
        pline("You have a peculiar feeling for a moment, then it passes.");
    }

    if((otmp->dknown != 0) && (objects[otmp->otyp].oc_name_known == 0)) {
        if(unkn == 0) {
            objects[otmp->otyp].oc_name_known = 1;
            u.urexp += 10;
        }
        else if(objects[otmp->otyp].oc_uname == NULL) {
            docall(otmp);
        }
    }

    useup(otmp);

    return 1;
}

void pluslvl()
{
    int num;
    
    pline("You feel more experienced.");
    num = rnd(10);
    u.uhpmax += num;
    u.uhp += num;
    u.uexp = (10 * pow(u.ulevel - 1)) + 1;
    ++u.ulevel;
    pline("Welcome to level %d", u.ulevel);
    flags.botl = 1;
}

void strange_feeling(struct obj *obj)
{
    pline("You have a strange feeling for a moment, then it passes.");

    if((objects[obj->otyp].oc_name_known == NULL)
       && (objects[obj->otyp.oc_name] == NULL)) {
        docall(obj);
    }

    useup();
}

int dodrop()
{
    struct obj *obj;
    
    obj = getobj("0$#", "drop");
    
    if(obj == NULL) {
        return 0;
    }

    if(obj->olet == '$') {
        if(obj->quan == 0) {
            pline("You didn't drop any gold pieces.");
        }
        else {
            mkgold((int)obj->quan, u.ux, u.uy);
            pline("You dropped %u gold piece%s.", obj->quan, plur(obj->quan));

            if(Invis != 0) {
                newsym(u.ux, u.uy);
            }
        }

        free(obj);

        return 1;
    }

    return drop(obj);
}

int drop(struct obj *obj)
{
    if((obj->owornmask & (W_ARMOR | W_RING)) != 0) {
        pline("You cannot drop something you are wearing.");

        return 0;
    }

    if(obj == uwep) {
        if(uwep->cursed != 0) {
            pline("You weapon is welded to your hand!");

            return 0;
        }

        setuwep((struct obj *)0);
    }

    pline("You dropped %s.", doname(obj));
    dropx(obj);

    return 1;
}

void dropx(struct obj *obj)
{
    if(obj->otyp == CRYSKNIFE) {
        obj->otyp = WORM_TOOTH;
    }

    freeinv(obj);
    obj->ox = u.ux;
    obj->oy = u.uy;
    obj->nobj = fobj;
    fobj = obj;

    if(Invis != 0) {
        newsym(u.ux, u.uy);
    }

    subfrombill(obj);
    stackobj(obj);
}

/* Drop serveral things */
int doddrop()
{
    return ggetobj("drop", drop, 0);
}

void rhack(char *cmd)
{
    struct func_tab *tlist = list;
    boolean firsttime = FALSE;
    int res;

    if(cmd == NULL) {
        firsttime = TRUE;
        flags.nopick = 0;
        cmd = parse();
    }

    if((*cmd == 0) || (*cmd == 0377)) {
        /* Probably we just had an interrupt */
        return;
    }

    if(movecm(cmd) != 0) {
        if(multi != 0) {
            flags.mv = 1;
        }

        domove();

        return;
    }

    if(movecm(lowc(cmd)) != 0) {
        flags.run = 1;

        if(firsttime != 0) {
            if(multi == 0) {
                multi = COLON;
            }

            u.last_str_turn = 0;
        }

        flag.mv = 1;
        
#ifdef QUEST
        if(flags.run >= 4) {
            fiddir();
        }

        if(firsttime != 0) {
            u.ux0 = u.ux + u.dx;
            u.uy0 = u.uy + u.dy;
        }
#endif

        domove();

        return;
    }

    if(((*cmd == 'f') && (movcm(cm + 1) != 0)) || (movecm(unctrl(cmd)) != 0)) {
        flags.run = 2;

        if(firsttime != 0) {
            if(multi == 0) {
                multi = COLNO;
            }

            u.last_str_turn = 0;
        }

        flags.mv = 1;

#ifdef QUEST
        if(flags.run >= 4) {
            finddir();
        }
         
        if(firsttime != 0) {
            u.ux0 = u.ux + u.dx;
            u.uy0 = u.uy + u.dy;
        }
#endif

        doremove();

        return();
    }

    if((*cmd == 'F') && (movecm(lowc(cmd + 1)) != 0)) {
        flags.run = 3;

        if(firsttime != 0) {
            if(multi == 0) {
                multi = COLNO;
            }

            u.last_str_turn = 0;
        }

        flags.mv = 1;

#ifdef QUEST
        if(flags >= 4) {
            finddir();
        }

        if(firsttime != 0) {
            u.ux0 = u.ux + u.dx;
            u.uy0 = u.uy + u.dy;
        }
#endif

        domove();
        
        return;
    }

    if((*cmd == 'm') && (movecm(cmd + 1) != 0)) {
        flags.run = 0;
        flags.nopick = 1;

        if(multi != 0) {
            flags.mv = 1;
        }

        domove();

        return;
    }

    if((*cmd == 'M') && (movcm(lowc(cmd + 1)) != 0)) {
        flags.run = 1;
        flags.nopick = 1;

        if(firsttime != 0) {
            if(multi == 0) {
                multi = COLNO;
            }

            u.last_str_turn = 0;
        }

        flags.mv = 1;

#ifdef QUEST
        if(flags.run >= 4) {
            finddir();
        }

        if(firsttime != 0) {
            u.ux = u.ux + u.dx;
            u.uy = u.uy + u.dy;
        }
#endif

        domove();

        return;
    }

#ifdef QUEST
    if((*cmd == cmd[1]) && ((*cmd == 'f') || (*cmd == 'F'))) {
        flags.run = 4;

        if(*cmd == 'F') {
            flags.run += 2;
        }
            
        if(cmd[2] == '-') {
            flags.run += 1;
        }

        if(firsttime != 0) {
            if(multi == 0) {
                multi = COLNO;
            }

            u.last_str_turn = 0;
        }

        flags.mv = 1;

#ifdef QUEST
        if(flags.run >= 4) {
            finddir();
        }

        if(firsttime != 0) {
            u.ux0 = u.ux + u.dx;
            u.uy0 = u.uy + u.dy;
        }
#endif

        domove();
        
        return;
    }
#endif

    while(tlist->f_char != 0) {
        if(*cmd == tlist->f_char) {
            res = (*(tlist->f_funct))(0);

            if(res == 0) {
                flags.move;
                multi = 0;
            }

            return;
        }

        ++tlist;
    }

    pline("Unknown command '%s'", cmd);
    flags.move = 0;
    multi = flags.move;
}

int doredraw()
{
    docrt();
    
    return 0;
}

int dohelp()
{
    if(child(1) != 0) {
        execl(catmore, "more", "help", (char *)0);

        exit(1);
    }

    return 0;
}

#ifdef SHELL
int dosh()
{
    char *str;
    
    if(child(0) != 0) {
        chdir(getenv("HOME"));

        str = getenv("SHELL");
        
        if(str != 0) {
            execl(str, str, (char *)0);
        }

        if(strcmp("player", getlogin()) != 0) {
            execl("/bin/sh", "sh", (char *)0);
        }

        pline("sh: cannot execute.");

        exit(1);
    }

    return 0;
}
#endif

int child(int wt)
{
    int f = fork();

    if(f == 0) {
        /* Child */
        setty((char *)0);
        setuid(getuid());
        
        return 1;
    }
    
    if(f == -1) {
        /* Cannot fork */
        pline("Fork failed. Try again.");

        return 0;
    }

    /* Fork succeeded; wait for child to exit */
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    wait((union wait *)0);
    setctty();
    signal(SIGINT, done1);

#ifdef WIZARD
    if(wizard != 0) {
        signal(SIGQUIT, SIG_DFL);
    }
#endif

    if(wt != 0) {
        getret();
    }

    docrt();

    return 0;
}

int dodown()
{
    if((u.ux != xdnstair) || (u.uy != ydnstair)) {
        pline("You can't go down here.");

        return 0;
    }

    if(u.ustuck != 0) {
        pline("You are being held, and cannot go down.");

        return 1;
    }

    if(Levitation != 0) {
        pline("You're floating high above the stairs.");

        return 0;
    }

    goto_level(dlevel + 1, TRUE);

    return 1;
}

int doup()
{
    if((u.ux != xupstair) || (u.uy != yupstai)) {
        pline("You can't do up here.");

        return 0;
    }

    if(u.ustuck != 0) {
        pline("You are being held, and cannot go up.");

        return 1;
    }

    if((inv_wight + 5) > 0) {
        pline("Your load is too heavy to climb the stairs.");

        return 1;
    }

    goto_level(dlevel - 1, TRUE);

    return 1;
}

void goto_level(int newlevel, boolean at_stairs)
{
    int fd;
    boolean up = 0;

    if(newlevel < dlevel) {
        up = 1;
    }

    /* In fact < 0 is impossible */
    if(newlevel <= 0) {
        done("escaped");
    }

    /* This cannot happen either */
    if(newlevel == dlevel) {
        return;
    }

    glo(dlevel);
    fd = creat(lock, FMASK);
    
    if(fd < 0) {
        /* 
         * This is not quite impossible: e.g., we may have
         * exceeded our quota. If that is the case then we
         * cannot leave this level, and cannot save either
         */
        if(up != 0) {
            pline("A mysterious force prevents you from going %d.", "up");
        }
        else {
            pline("A mysterious force prevents you from going %d.", "down");
        }

        return;
    }

    if(Punished != 0) {
        unplacebc();
    }

    keepdogs();
    seeoff(1);
    flags.nscrinh = 1;
    
    /* Hack */
    u.ux = FAR;

    /* Probably was a trapdoor */
    inshop();

    savelev(fd);
    close(fd);

    dlevel = newlevel;

    if(maxdlevel < dlevel) {
        maxdlevel = dlevel;
    }

    glo(dlevel);
    fd = open(lock, 0);

    if(fd < 0) {
        mklev();
    }
    else {
        getlev(fd);
        close(fd);
    }

    if(at_stairs != 0) {
        if(up != 0) {
            u.ux = xdnstair;
            u.uy = ydnstair;

            if(u.ux == 0) {
                /* Entering a maze from below? This will confuse the player! */
                u.ux = xupstair;
                u.uy = yupstair;
            }

            if(Punished != 0) {
                pline("With great effort you climb the stairs.");
                placebc(1);
            }
        }
        else {
            u.ux = xupstair;
            u.uy = yupstair;

            if(((inv_weight() + 5) > 0) || (Punished != 0)) {
                pline("You fall down the stairs.");
                losehp(rnd(3), "fall");

                if(Punished != 0) {
                    if((uwep != uball) && (rn2(3) != 0)) {
                        pline("... and are hit by the iron ball");
                        losehp(rnd(20), "iron ball");
                    }

                    placebc(1);
                }

                selftouch("Falling, you");
            }
        }
    }
    else {
        /* Trapdoor or level_tele */
        u.ux = rnd(COLNO - 1);
        u.uy = rn2(ROWNO);

        while((levl[u.ux][u.uy].typ != ROOM) || (m_at(u.ux, u.uy) != 0)) {
            u.ux = rnd(COLNO - 1);
            u.uy = rn2(ROWNO);
        }
               
        if(Punished != 0) {
            if((uwep != uball) && (/* %% */ up == 0) && (rn2(5) != 0)) {
                pline("The iron ball falls on your head.");
                losehp(rnd(25), "iron ball");
            }

            placebc(1);
        }

        selftouch("Falling, you");
    }

    inshop();

#ifdef TRACK
    inittrack();
#endif

    losedogs();
    flags.nscrinh = 0;
    setsee();
    docrt();
    pickup();
    read_engr_at(u.ux, u.uy);
}

int donull()
{
    /* Do nothing, but let other things happen */

    return 1;
}

int dothrow()
{
    struct obj *obj;
    struct monst *mon;
    int tmp;

    /* It is also possible to throw food (or jewels, or iron balls ...) */
    obj = getobj("#)", "throw");

    if((obj == NULL) || (getdir() == 0)) {
        return 0;
    }

    if((obj->owornmask & (W_ARMOR | W_RING)) != 0) {
        pline("You can't throw something you are wearing");
        
        return 0;
    }

    if(obj == uwep) {
        if(obj->cursed != 0) {
            pline("You weapon is welded to your hand");

            return 1;
        }

        if(obj->quan > 1) {
            setuwep(splitobj(obj, 1));
        }
        else {
            setuwep((struct obj *)0);
        }
    }
    else if(obj->quan > 1) {
        splitobj(obj, 1);
    }

    freeinv(obj);
    
    if(u.uswallow != 0) {
        mon = u.ustruck;
        bhitpos.x = mon->mx;
        bhitpos.y = mon->my;
    }
    else if(obj->otyp == BOOMERANG) {
        mon = boomhit(u.dx, u.dy);
        
        /* boomhit() delivers -1 if the thing was caught */
        if((int)mon == -1) {
            addinv(obj);
            
            return 1;
        }
    }
    else {
        if((Punished == 0) || (obj != uball)) {
            mon = bhit(u.dx, u.dy, 8, obj->olet);
        }
        else if(u.ustuck == 0) {
            mon = bhit(u.dx, u.dy, 5, obj->olet);
        }
        else {
            mon = bhit(u.dx, u.dy, 1, obj->olet);
        }
    }

    if(mon != 0) {
        /* Awake monster if sleeping */
        wakeup(mon);

        if(obj->olet == WEAPON_SYM) {
            tmp = ((-1 + u.ulevel) + mon->data->ac) + abon();

            if(obj->otyp < ROCK) {
                if((uwep == 0) || (uwep->otyp != (obj->otyp + (BOW - ARROW)))) {
                    tmp -= 4;
                }
                else {
                    tmp += uwep->spe;
                }
            }
            else {
                if(obj->otyp == BOOMERANG) {
                    tmp += 4;
                }
            }

            tmp += obj->spe;
            
            if((u.uswallow != 0) || (tmp >= rnd(20))) {
#ifndef NOWORM
                if(hmon(mon, obj, 1) == TRUE) {
                    /* mon still alive */
                    cutworm(mon, bhitpos.x, bhitpos.y, obj->otyp);
                }
                else {
                    mon = 0;
                }
#else
                if(hmon(mon, obj, 1) != TRUE) {
                    mon = 0;
                }
#endif
                
                /* Weapons thrown disappear sometimes */
                if((obj->otyp < BOOMERANG) && (rn2(3) != 0)) {
                    /* Check bill; free */
                    obfree(obj, (struct obj *)0);
                    
                    return 1;
                }
            }
            else {
                miss(objects[obj->otyp].oc_name, mon);
            }
        }
        else if(obj->otyp == HEAVY_IRON_BALL) {
            tmp = ((-1 + u.ulevel) + mon->data->ac) + abon();

            if((Punished == 0) || (obj != uball)) {
                tmp += 2;
            }

            if(u.utrap != 0) {
                tmp -= 2;
            }

            if((u.uswallow != 0) || (tmp >= rnd(20))) {
                if(hmon(mon, obj, 1) == FALSE) {
                    /* He died */
                    mon = 0;
                }
            }
            else {
                miss("iron ball", mon);
            }
        }
        else {
            if(cansee(bhitpos.x, bhitpos.y) != 0) {
                pline("You miss %s.", monnam(mon));
            }
            else {
                pline("You miss it.");
            }

            if((obj->olet == FOOD_SYM) && (mon->data->mlet == 'd')) {
                if(tamedog(mon, obj) != 0) {
                    return 1;
                }
            }

            if((obj->olet == GEM_SYM) && (mon->data->mlet == 'u')) {
                if((obj->dknown != 0) && (objects[obj->otyp].oc_name_known != 0)) {
                    if(objects[obj->otyp].g_val > 0) {
                        u.uluck += 5;
                        pline("%s graciously accepts your gift.", Monnam(mon));
                        mpickobj(mon, obj);
                        rloc(mon);
                        
                        return 1;
                    }
                    else {
                        pline("%s is not interested in your junk.", Monnam(mon));
                    }
                }
                else {
                    /* Value unknown to @ */
                    ++u.uluck;
                    pline("%s graciously accepts your gift.", Monnam(mon));
                    mpickobj(mon, obj);
                    rloc(mon);
                    
                    return 1;
                }
            }
        }
    }

    obj->ox = bhitpos.x;
    obj->oy = bhitpos.y;
    obj->nobj = fobj;
    fobj = obj;

    /* Prevent him from throwing articles to the exit and escaping */
    /* subfrombill(obj); */

    stackobj(obj);

    if((Punished != 0)
       && (obj == uball)
       && ((bhitpos.x != u.ux) || (bhitpos.y != u.uy))) {
        freeobj(uchain);
        unpobj(uchain);

        if(u.utrap != 0) {
            if(u.utraptype == TT_PIT) {
                pline("The ball pulls you out of the pit!");
            }
            else {
                int side = RIGHT_SIDE;

                if(rn2(3) != 0) {
                    side = LEFT_SIDE;
                }

                pline("The ball pulls you out of the bear trap.");

                if(side == LEFT_SIDE) {
                    pline("Your %s leg is severely damaged.", "left");
                }
                else {
                    pline("Your %s leg is severely damaged.", "right");
                }

                Wounded_legs |= (side + rnd(1000));
                losehp(2, "thrown ball");
            }

            u.utrap = 0;
        }
   
        unsee();
        uchain->nobj = fobj;
        fobj = uchain;
        uchain->ox = bhitpos.x - u.dx;
        u.ux = uchain->ox;
        uchain->oy = bhitpos.y - u.dy;
        u.uy = uchain->oy;
        setsee();
        inshop();
    }

    if(cansee(bhitpos.x, bhitpos.y) != 0) {
        prl(bhitpos.x, bhitpos.y);
    }

    return 1;
}

int getdir()
{
    char buf[2];
    
    pline("What direction?");
    buf[0] = readchar();
    buf[1] = 0;

    return movecm(buf);
}

/* 
 * Split obj so that it gets size num
 * remainder is put in the object structure delivered by this call
 */
struct obj *splitobj(struct obj *obj, int num)
{
    struct obj *otmp;

    otmp = newobj(0);
    
    /* Copies whole structure */
    *otmp = *obj;
    otmp->o_id = flags.ident;
    ++flags.ident;
    otmp->onamelth = 0;
    obj->quan = num;
    obj->owt = weight(obj);
    otmp->quan -= num;

    /* -= obj->owt ? */
    otmp->owt = weight(otmp);
    
    obj->nobj = otmp;

    if(obj->unpaid != 0) {
        splitbill(obj, otmp);
    }

    return otmp;
}

char *lowc(char *str)
{
    static char buf[2];

    if((*str >= 'A') && (*str <= 'Z')) {
        *buf = (*str + 'a') - 'A';
    }
    else {
        *buf = *str;
    }

    buf[1] = 0;

    return buf;
}

char *unctrl(char *str)
{
    static char buf[2];

    if((*str >= ('A' & 037)) && (*str <= ('Z' & 037))) {
        *buf = *str + 0140;
    }
    else {
        *buf = *str;
    }

    buf[1] = 0;

    return buf;
}
