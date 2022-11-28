/* Copyright (c) Stichting Matehmatisch Centurm, Amsterdam, 1984. */

#include "hack.read.h"

#include <stdlib.h>
#include <string.h>

#include "hack.h"
#include "hack.do.h"
#include "hack.dog.h"
#include "hack.do_name.h"
#include "hack.do_wear.h"
#include "hack.end.h"
#include "hack.invent.h"
#include "hack.main.h"
#include "hack.makemon.h"
#include "hack.mkobj.h"
#include "hack.mon.h"
#include "hack.pri.h"
#include "hack.trap.h"
#include "hack.topl.h"
#include "hack.tty.h"
#include "hack.wield.h"
#include "hack.worn.h"
#include "rnd.h"

int doread()
{
    struct obj *scroll;
    boolean confused = 0;
    
    if(Confusion != 0) {
        confused = 1;
    }

    boolean known = FALSE;

    scroll = getobj("?", "read");

    if(scroll == NULL) {
        return 0;
    }

    if((scroll->dknown == 0) && (Blind != 0)) {
        pline("Being blind, you cannot read the formula on the scroll.");
        return 0;
    }

    if(Blind != 0) {
        pline("As you pronounce the formula on it, the scroll disappears.");
    }
    else {
        pline("As you read the scroll, it disappears.");
    }

    if(confused != 0) {
        pline("Being confused, you mispronounce the magic words ...");
    }

    switch(scroll->otyp) {
    case SCR_ENCHANT_ARMOR:
        {
            struct obj *otmp = some_armor();

            if(otmp == NULL) {
                strange_feeling(scroll);
                
                return 1;
            }
            
            if(confused != 0) {
                pline("Your %s glows silver for a moment.", 
                      objects[otmp->otyp].oc_name);

                otmp->rustfree = 1;

                break;
            }

            if((((otmp->spe * 2) + objects[otmp->otyp].a_ac) > 23) 
               && (rn2(3) == 0)) {
                pline("Your %s glows violently green for a while, then evaporates.",
                      objects[otmp->otyp].oc_name);

                useup(otmp);
                
                break;
            }

            pline("Your %s glows green for a moment.",
                  objects[otmp->otyp].oc_name);

            otmp->cursed = 0;
            ++otmp->spe;
        }

        break;
    case SCR_DESTROY_ARMOR:
        if(confused != 0) {
            struct obj *otmp = some_armor();
            
            if(otmp != NULL) {
                strange_feeling(scroll);

                return 1;
            }

            pline("Your %s glows purple for a moment.",
                  objects[otmp->otyp].oc_name);

            otmp->rustfree = 0;

            break;
        }

        if(uarm != NULL) {
            pline("Your armor tursn to dust and falls to the floor!");
            useup(uarm);
        }
        else if(uarmh != NULL) {
            pline("Your helmet turns to dust and is blown away!");
            useup(uarmh);
        }
        else if(uarmg != NULL) {
            pline("Your gloves vanish!");
            useup(uarmg);
            selftouch("You");
        }
        else {
            strange_feeling(scroll);

            return 1;
        }

        break;
    case SCR_CONFUSE_MONSTER:
        if(confused != 0) {
            pline("Your hands begin to glow purple.");
            Confusion += rnd(100);
        }
        else {
            pline("You hands begin to glow blue.");
            u.umconf = 1;
        }
        
        break;
    case SCR_SCARE_MONSTER:
        {
            int ct = 0;
            struct monst *mtmp;

            for(mtmp = fmon; mtmp != NULL; mtmp = mtmp->nmon) {
                if(cansee(mtmp->mx, mtmp->my) != 0) {
                    if(confused != 0) {
                        mtmp->msleep = 0;
                        mtmp->mfroz = mtmp->msleep;
                        mtmp->mflee = mtmp->mfroz;
                    }
                    else {
                        mtmp->mflee = 1;
                        ++ct;
                    }
                }
            }

            if(ct != 0) {
                if(confused != 0) {
                    pline("You hear sad wailing in the distance.");
                }
                else {
                    pline("You hear maniacal laughter in the distance.");
                }
            }
        }
        
        break;
    case SCR_BLANK_PAPER:
        if(confused != 0) {
            pline("You see strange patterns on this scroll.");
        }
        else {
            pline("This scroll seems to be blank.");
        }

        break;
    case SCR_REMOVE_CURSE:
        {
            struct obj *obj;

            if(confused != 0) {
                pline("You feel like you need some help.");
            }
            else {
                pline("You fell like someone is helping you.");
            }

            for(obj = invent; obj != NULL; obj = obj->nobj) {
                if(obj->owornmask) {
                    obj->cursed = confused;
                }
            }

            if((Punished != 0) && (confused != 0)) {
                Punished = 0;
                freeobj(uchain);
                unpobj(uchain);
                free(uchain);
                uball->spe = 0;
                uball->owornmask &= ~W_BALL;
                uball = (struct obj *)0;
                uchain = uball;
            }
        }

        break;
    case SCR_CREATE_MONSTER:
        {
            int cnt = 1;
            
            if(rn2(73) == 0) {
                cnt += (rn2(4) + 1);
            }
            
            if(confused != 0) {
                cnt += 12;
            }
            
            while(cnt) {
                --cnt;
                
                if(confused != 0) {
                    makemon(PM_ACIDBLOB, u.ux, u.uy);
                }
                else {
                    makemon((struct permonst *)0, u.ux, u.uy);
                }
            }
        }

        break;
    case SCR_ENCHANT_WEAPON:
        if(uwep == NULL) {
            strange_feeling(scroll);
            
            return 1;
        }

        if(confused != 0) {
            pline("Your %s glows silver for a moment.",
                  objects[uwep->otyp].oc_name);

            uwep->rustfree = 1;
        }
        else {
            if(chwepon(scroll, 1) == 0) {
                return 1;
            }
        }

        break;
    case SCR_DAMAGE_WEAPON:
        if(confused != 0) {
            pline("You %s glows purple for a moment.",
                  objects[uwep->otyp].oc_name);

            uwep->rustfree = 0;
        }
        else {
            if(chwepon(scroll, -1) == 0) {
                return 1;
            }
        }

        break;
    case SCR_TAMING:
        {
            int i;
            int j;
            int bd = 5;
            
            if(confused == 0) {
                bd = 1;
            }

            struct monst *mtmp;

            for(i = -bd; i <= bd; ++i) {
                for(j = -bd; j <= bd; ++j) {
                    mtmp = m_at(u.ux + i, u.uy + j);

                    if(mtmp != NULL) {
                        tamedog(mtmp, (struct obj *)0);
                    }
                }
            }
        }

        break;
    case SCR_GENOCIDE:
        {
            extern char genocided[];
            extern char fut_geno[];
            char buf[BUFSZ];
            struct monst *mtmp;
            struct monst *mtmp2;

            pline("You have found a scroll of genocide!");
            known = TRUE;

            if(confused != 0) {
                *buf = u.usym;
            }
            else {
                pline("What monster do you want to genocide (Type the letter)? ");
                getlin(buf);

                while((strlen(buf) != 1) || (letter(buf[0]) == 0)) {
                    pline("What monster do you want to genocide (Type the letter)? ");
                    getlin(buf);
                }
            }

            if(index(fut_geno, *buf) == NULL) {
                charcat(fut_geno, *buf);
            }

            if(index(genocided, *buf) == NULL) {
                charcat(genocided, *buf);
            }
            else {
                pline("Such monsters do not exist in this world.");
                
                break;
            }

            for(mtmp = fmon; mtmp != NULL; mtmp = mtmp2) {
                mtmp2 = mtmp->nmon;
                
                if(mtmp->data->mlet == *buf) {
                    mondead(mtmp);
                }
            }

            pline("Wiped out all %c's.", *buf);

            if(*buf == u.usym) {
                killer = "scroll of genocide";
                u.uhp = -1;
            }
        }

        break;
    case SCR_LIGHT:
        if(Blind == 0) {
            known = TRUE;
        }

        litroom(!confused);

        break;
    case SCR_TELEPORTATION:
        if(confused != 0) {
            level_tele();
        }
        else {
#ifdef QUEST
            int oux = u.ux;
            int ouy = u.uy;
            tele();

            if(dist(oux, ouy) > 100) {
                known = TRUE;
            }
#else
            int uroom = inroom(u.ux, u.uy);
            tele();

            if(uroom != inroom(u.ux, u.uy)) {
                known = TRUE;
            }
#endif
        }

        break;
    case SCR_GOLD_DETECTION:
        {
            struct gen *head = fgold;
            
            if(confused != 0) {
                head = ftrap;
            }

            struct gen *gtmp;

            if(head == NULL) {
                strange_feeling(scroll);
            
                return 1;
            }
            else {
                known = TRUE;

                int flag = 0;
                for(gtmp = head; gtmp != NULL; gtmp = gtmp->ngen) {
                    if((gtmp->gx != u.ux) || (gtmp->gy != u.uy)) {
                        flag = 1;
                        break;
                    }
                }

                if(flag == 0) {
                    /* Only under me - no separate display required */
                    if(confused != 0) {
                        pline("You feel very giddy!");
                    }
                    else {
                        pline("You notice some gold between your feet.");
                    }
                    
                    break;
                }

                cls();
                
                for(gtmp = head; gtmp != NULL; gtmp = gtmp->ngen) {
                    at(gtmp->gx, gtmp->gy, '$');
                }

                prme();

                if(confused != 0) {
                    pline("You feel very greedy!");
                }
                else {
                    pline("You feel very greedy, and sense gold!");
                }

                more();
                docrt();
            }
        }

        break;
    case SCR_FOOD_DETECTION:
        {
            int ct = 0;
            int ctu = 0;
            struct obj *obj;
            char foodsym = FOOD_SYM;

            if(confused != 0) {
                confused = POTION_SYM;
            }

            for(obj = fobj; obj != NULL; obj = obj->nobj) {
                if(obj->olet == FOOD_SYM) {
                    if((obj->ox == u.ux) && (obj->oy == u.uy)) {
                        ++ctu;
                    }
                    else {
                        ++ct;
                    }
                }
            }

            if((ct == 0) && (ctu == 0)) {
                strange_feeling(scroll);

                return 1;
            }
            else if(ct == 0) {
                known = TRUE;

                if(confused != 0) {
                    pline("You smell %s close nearby.", "something");
                }
                else {
                    pline("You smell %s close nearby.", "food");
                }
            }
            else {
                known = TRUE;
                cls();

                for(obj = fobj; obj != NULL; obj = obj->nobj) {
                    if(obj->olet == foodsym) {
                        at(obj->ox, obj->oy, FOOD_SYM);
                    }
                }

                prme();

                if(confused != 0) {
                    pline("Your nose tingles and you smell %s!", "something");
                }
                else {
                    pline("Your nose tingles and you smell %s!", "food");
                }

                more();
                docrt();
            }
        }

        break;
    case SCR_IDENTIFY:
        /* known = TRUE; */
        if(confused != 0) {
            pline("You identity this as an identify scroll.");
        }
        else {
            pline("This is an identify scroll.");
        }

        useup(scroll);
        objects[SCR_IDENTIFY].oc_name_known = 1;

        if(confused == 0) {
            while((ggetobj("identify", identify, rn2(5) ? 1 : rn2(5)) == 0)
                  && (invent != NULL)) {
            }
        }

        return 1;
    case SCR_MAGIC_MAPPING:
        {
            struct rm *lev;
            int num;
            int zx;
            int zy;

            known = TRUE;

            if(confused != 0) {
                pline("On this scroll %s a map!", "was");
            }
            else {
                pline("On this scroll %s a map!", "is");
            }

            for(zy = 0; zy < ROWNO; ++zy) {
                for(zx = 0; zx < COLNO; ++zx) {
                    if((confused != 0) && (rn2(7) != 0)) {
                        continue;
                    }

                    lev = &(levl[zx][zy]);
                    num = lev->typ;

                    if(num == 0) {
                        continue;
                    }

                    if(num == SCORR) {
                        lev->typ = CORR;
                        lev->scrsym = CORR_SYM;
                    }
                    else {
                        if(num == SDOOR) {
                            lev->typ = DOOR;
                            lev->scrsym = '+';
                            
                            /* Do something in doors? */
                        }
                        else if(lev->seen) {
                            continue;
                        }
                    }

#ifndef QUEST
                    if(num != ROOM) {
                        lev->new = 1;
                        lev->seen = lev->new;
                        on_scr(zx, zy);
                    }
#else
                    lev->new = 1;
                    lev->seen = lev->new;
                    on_scr(zx, zy);
#endif
                }
            }
        }

        break;
    case SCR_AMNESIA:
        {
            int zx;
            int zy;
            
            known = TRUE;

            for(zx = 0; zx < COLNO; ++zx) {
                for(zy = 0; zy < ROWNO; ++zy) {
                    if((confused == 0) || (rn2(7) != 0)) {
                        if(cansee(zx, zy) == 0) {
                            levl[zx][zy].seen = 0;
                        }
                    }
                }
            }

            docrt();
            pline("Thinking of Maud you forget everything else.");
        }

        break;
    case SCR_FIRE:
        {
            int num;

            known = TRUE;

            if(confused != 0) {
                pline("The scroll catches fire and yo uburn your hands.");
                losehp(1, "scroll of fire");
            }
            else {
                pline("the scroll erupts in a tower of flame!");
                
                if(Fire_resistance != 0) {
                    pline("You are uninjured.");
                }
                else {
                    num = rnd(6);
                    u.uhpmax -= num;
                    losehp(num, "scroll of fire");
                }
            }
        }

        break;
    case SCR_PUNISHMENT:
        known = TRUE;

        if(confused != 0) {
            pline("You feel guilty.");

            break;
        }

        pline("You are being punished for your misbahviour!");

        if(Punished != 0) {
            pline("You iron ball gets heavier.");
            uball->owt += 15;
        
            break;
        }

        Punished = INTRINSIC;
        mkobj_at(CHAIN_SYM, u.ux, u.uy);
        setworn(fobj, W_CHAIN);
        mkobj_at(BALL_SYM, u.ux, u.uy);
        setworn(fobj, W_BALL);

        /* Special ball (see save) */
        uball->spe = 1;

        break;
    default:
        pline("What weird language is this written in? (%d)", scroll->otyp);
        impossible();
    }

    if(objects[scroll->otyp].oc_name_known == 0) {
        if((known != 0) && (confused == 0)) {
            objects[scroll->otyp].oc_name_known = 1;
            u.urexp += 10;
        }
        else if(objects[scroll->otyp].oc_uname == 0) {
            docall(scroll);
        }
    }

    useup(scroll);

    return 1;
}

int identify(struct obj *otmp)
{
    objects[otmp->otyp].oc_name_known = 1;
    otmp->dknown = 1;
    otmp->known = otmp->dknown;
    prinv(otmp);

    return 1;
}

void litroom(boolean on)
{
    int num;
    int zx;
    int zy;

    /* First produce the text (provided he is not blind) */
    if(Blind != 0) {
        if(on == 0) {
            if((u.uswallow != 0)
               || (xdnstair == 0)
               || (levl[(int)u.ux][(int)u.uy].typ == CORR)
               || (levl[(int)u.ux][(int)u.uy].lit == 0)) {
                pline("It seems even darker in here than before.");

                return;
            }
            else {
                pline("It suddenly becomes dark in here.");
            }
        }
        else {
            if(u.uswallow != 0) {
                pline("%s's stmoach is lit.", Monnam(u.ustuck));

                return;
            }
        }

        if(xdnstair == 0) {
            pline("Nothing Happens");

            return;
        }

#ifdef QUEST
        pline("The cave lights up around you, then fades.");

        return;
#endif
        if(levl[(int)u.ux][(int)u.uy].typ == CORR) {
            pline("The corridor lights up around you, then fades.");

            return;
        }
        else if(levl[(int)u.ux][(int)u.uy].lit) {
            pline("The light here seems better now.");

            return;
        }
        else {
            pline("The room is lit.");
        }
    }

#ifdef QUEST
    return;
#else
    if(levl[(int)u.ux][(int)u.uy].lit == on) {
        return;
    }

    if(levl[(int)u.ux][(int)u.uy].typ == DOOR) {
        if(levl[(int)u.ux][(int)(u.uy + 1)].typ >= ROOM) {
            zy = u.uy + 1;
        }
        else if(levl[(int)u.ux][(int)(u.uy - 1)].typ >= ROOM) {
            zy = u.uy - 1;
        }
        else {
            zy = u.uy;
        }

        if(levl[(int)(u.ux + 1)][(int)u.uy].typ >= ROOM) {
            zx = u.ux + 1;
        }
        else if(levl[(int)(u.ux - 1)][(int)u.uy].typ >= ROOM) {
            zx = u.ux - 1;
        }
        else {
            zx = u.ux;
        }
    }
    else {
        zx = u.ux;
        zy = u.uy;
    }

    seelx = u.ux;
    num = levl[seelx - 1][zy].typ;
    while((num != CORR) && (num != 0)) {
        --seelx;
        num = levl[seelx - 1][zy].typ;
    }

    seehx = u.ux;
    num = levl[seehx + 1][zy].typ;
    while((num != CORR) && (num != 0)) {
        ++seehx;
        num = levl[seehx + 1][zy].typ;
    }

    seely = u.uy;
    num = levl[zx][seely - 1].typ;
    while((num != CORR) && (num != 0)) {
        --seely;
        num = levl[zx][seely - 1].typ;
    }

    seehy = u.uy;
    num = levl[zx][seehy + 1].typ;
    while((num != CORR) && (num != 0)) {
        ++seehy;
        num = levl[zx][seehx + 1].typ;
    }

    for(zy = seely; zy <= seehy; ++zy) {
        for(zx = seelx; zx <= seehx; ++zx) {
            levl[zx][zy].lit = on;

            if((Blind == 0) && (dist(zx, zy) > 2)) {
                if(on != 0) {
                    prl(zx, zy);
                }   
            }
            else {
                nosee(zx, zy);
            }
        }
    }

    if(on == 0) {
        seehx = 0;
    }
#endif
}
