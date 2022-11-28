#include "hack.mhitu.h"

#include "hack.h"
#include "hack.do_name.h"
#include "hack.end.h"
#include "hack.fight.h"
#include "hack.makemon.h"
#include "hack.mon.h"
#include "hack.steal.h"
#include "hack.topl.h"
#include "hack.worm.h"
#include "hack.zap.h"
#include "rnd.h"

/*
 * mhitu: Monster hits you
 * returns: 1 if monster dies (e.g. 'y', 'F')
 *          0 otherwise
 */
int mhitu(struct monst *mtmp)
{
    struct permonst *mdat = mtmp->data;
    int tmp;
    int ctmp;

    nomul(0);

    if((index("&DuxynNF", mdat->mlet) == 0) && (u.uswallow == 0)) {
        tmp = hitu(mtmp, d(mdat->damn, mdat->damd));
    }
    else {
        tmp = 0;
    }

    ctmp = 0;

    if((tmp != 0)
       && (mtmp->mcan == 0)
       && ((uarm == 0)
           || (objects[uarm->otyp].a_can < rnd(3)) 
           || (rn2(50) == 0))) {
        ctmp = 1;
    }

    switch(mdat->mlet) {
    case '&':
        if((mtmp->cham == 0) && (mtmp->mcan == 0) && (rn2(15) == 0)) {
            makemon(PM_DEMON, u.ux, u.uy);
        }
        else {
            hitu(mtmp, d(2, 6));
            hitu(mtmp, d(2, 6));
            hitu(mtmp, rnd(3));
            hitu(mtmp, rnd(3));
            hitu(mtmp, rn1(4, 2));
        }

        break;
    case ',':
        if(u.uswallow != 0) {
            youswld(mtmp, 4 + u.uac, 5, "The trapper");
        }
        else if(tmp != 0) {
            justswld(mtmp, "The trapper");
        }

        break;
    case '\'':
        if(u.uswallow != 0) {
            youswld(mtmp, rnd(6), 7, "The lurker above");
        }
        else if(tmp != 0) {
            justswld(mtmp, "The lurker above");
        }

        break;
    case 'A':
        if((ctmp != 0) && (rn2(2) != 0)) {
            pline("You feel weaker!");
            losestr(1);
        }

        break;
    case 'C':
        hitu(mtmp, rnd(6));

        break;
    case 'c':
        if(rn2(5) == 0) {
            pline("You hear %s's hissing!", monnam(mtmp));

            if((ctmp != 0) || (rn2(5) == 0)) {
                pline("You get turned to stone!");
                done_in_by(mtmp);
            }
        }

        break;
    case 'D':
        if((rn2(6) != 0) || (mtmp->mcan != 0)) {
            hitu(mtmp, d(3, 10));
            hitu(mtmp, rnd(8));
            hitu(mtmp, rnd(8));

            break;
        }

        kludge("%s breathes fire!", "The dragon");
        buzz(-1, mtmp->mx, mtmp->my, u.ux - mtmp->mx, u.uy - mtmp->my);
        
        break;
    case 'd':
        hitu(mtmp, d(2, 4));
    
        break;
    case 'e':
        hitu(mtmp, d(3, 6));

        break;
    case 'F':
        if(mtmp->mcan != 0) {
            break;
        }

        kludge("%s explodes!", "The freezing sphere");

        if(Cold_resistance != 0) {
            pline("You don't seem affected by it.");
        }
        else {
            xchar dn;

            if((17 - (u.ulevel / 2)) > rnd(20)) {
                pline("You get blasted!");
                dn = 6;
            }
            else {
                pline("You duck the blast...");
                dn = 3;
            }

            losehp_m(d(dn, 6), mtmp);
        }

        mondead(mtmp);
        
        return 1;
    case 'g':
        if((ctmp != 0) && (multi >= 0) && (rn2(6) == 0)) {
            kludge("You are frozen by %ss juices", "the cube'");
            nomul(-rnd(10));
        }

        break;
    case 'h':
        if((ctmp != 0) && (multi >= 0) && (rn2(5) == 0)) {
            nomul(-rnd(10));
            kludge("You are put to sleep by %ss bite!", "the homunculus'");
        }

        break;
    case 'j':
        tmp = hitu(mtmp, rnd(3));
        tmp &= hitu(mtmp, rnd(3));

        if(tmp != 0) {
            hitu(mtmp, rnd(4));
            hitu(mtmp, rnd(4));
        }

        break;
    case 'k':
        if(((hitu(mtmp, rnd(4)) != 0) || (rn2(3) == 0)) && (ctmp != 0)) {
            poisoned("bee's sting", mdat->mname);
        }

        break;
    case 'L':
        if(tmp != 0) {
            stealgold(mtmp);
        }

        break;
    case 'N':
        if((mtmp->mcan != 0) && (Blind == 0)) {
            pline("%s tries to seduce you, but you seem not interested.",
                  Amonnam(mtmp, "plain"));

            if(rn2(3) != 0) {
                rloc(mtmp);
            }
        }
        else if(steal(mtmp) != 0) {
            rloc(mtmp);
            mtmp->mflee = 1;
        }

        break;
    case 'n':
        if((uwep == 0)
           && (uarm == 0) 
           && (uarmh == 0)
           && (uarms == 0)
           && (uarmg == 0)) {
            pline("%s hits! (I hope you don't mind)", Monnam(mtmp));
            u.uhp += rnd(7);

            if(rn2(7) == 0) {
                ++u.uhpmax;
            }

            if(u.uhp > u.uhpmax) {
                u.uhp = u.uhpmax;
            }

            flags.botl = 1;

            if(rn2(50) != 0) {
                rloc(mtmp);
            }
        }
        else {
            hitu(mtmp, d(2, 6));
            hitu(mtmp, d(2, 6));
        }

        break;
    case 'o':
        tmp = hitu(mtmp, rnd(6));

        if((hitu(mtmp, rnd(6)) != 0)
           && (ctmp != 0)
           && (u.ustuck == 0)
           && (rn2(2) != 0)) {
            u.ustuck = mtmp;
            kludge("%s has grabbed you!", "The owlbear");
            u.uhp -= d(2, 8);
        }
        else if(u.ustuck == mtmp) {
            u.uhp -= d(2, 8);
            pline("You are being crushed.");
        }

        break;
    case 'P':
        if(u.uswallow != 0) {
            youswld(mtmp, d(2, 4), 12, "The purple worm");
        }
        else if((ctmp != 0) && (rn2(4) == 0)) {
            justswld(mtmp, "The purple worm");
        }
        else {
            hitu(mtmp, d(2, 4));
        }

        break;
    case 'Q':
        hitu(mtmp, rnd(2));
        hitu(mtmp, rnd(2));

        break;
    case 'R':
        if((tmp != 0)
           && (uarmh != 0)
           && (uarmh->rustfree == 0)
           && ((int)uarmh->spe >= -1)) {
            pline("You helmet rusts!");
            --uarmh->spe;
        }
        else {
            if((ctmp != 0)
               && (uarm != 0)
               && (uarmh->rustfree == 0)
               && (uarm->otyp < STUDDED_LEATHER_ARMOR)
               && ((int)uarm->spe >= -1)) {
                pline("Your armor rusts!");
                --uarm->spe;
            }
        }

        break;
    case 'S':
        if((ctmp != 0) && (rn2(8) == 0)) {
            poisoned("snake's bite", mdat->mname);
        }

        break;
    case 's':
        if((tmp != 0) && (rn2(8) == 0)) {
            poisoned("scorpion's sting", mdat->mname);
        }

        hitu(mtmp, rnd(8));
        hitu(mtmp, rnd(8));

        break;
    case 'T':
        hitu(mtmp, rnd(6));
        hitu(mtmp, rnd(6));

        break;
    case 't':
        if(rn2(5) == 0) {
            rloc(mtmp);
        }

        break;
    case 'u':
        mtmp->mflee = 1;

        break;
    case 'U':
        hitu(mtmp, d(3, 4));
        hitu(mtmp, d(3, 4));

        break;
    case 'v':
        if((ctmp != 0) && (u.ustuck == 0)) {
            u.ustuck = mtmp;
        }

        break;
    case 'V':
        if(tmp != 0) {
            u.uhp -= 4;
        }

        if((ctmp != 0) && (rn2(3) == 0)) {
            losexp();
        }

        break;
    case 'W':
        if((ctmp != 0) && (rn2(5) == 0)) {
            losexp();
        }

        break;
#ifndef NOWORM
    case 'w':
        if(tmp != 0) {
            wormhit(mtmp);
        }
        
        break;
#endif
    case 'X':
        hitu(mtmp, rnd(3));
        hitu(mtmp, rnd(3));
        hitu(mtmp, rnd(3));

        break;
    case 'x':
        {
            int side = RIGHT_SIDE;

            if(rn2(2) != 0) {
                side = LEFT_SIDE;
            }

            if(side == RIGHT_SIDE) {
                pline("%s pricks in your %s leg!", Monnam(mtmp), "right");
            }
            else {
                pline("%s pricks in your %s leg!", Monnam(mtmp), "left");
            }

            Wounded_legs |= (side + rnd(5));
            losehp_m(2, mtmp);
        }

        break;
    case 'y':
        if(mtmp->mcan != 0) {
            break;
        }

        mondead(mtmp);
        
        if(Blind == 0) {
            pline("You are blinded by a blast of light!");
            Blind = d(4, 12);
            seeoff(0);
        }

        return 1;
    case 'Y':
        hitu(mtmp, rnd(6));

        break;
    }

    if(u.uhp < 1) {
        done_in_by(mtmp);
    }

    return 0;
}
             
