/*
 * strategy.c: Rog-O-Matic XIV (CMU) Tue Mar 19 21:49:33 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This file contains all of the 'high level intelligence' of Rog-O-Matic.
 */
#include "strategy.h"

#include <ctype.h>
#include <curses.h>
#include <stdio.h>
#include <string.h>

#include "arms.h"
#include "command.h"
#include "debug.h"
#include "explore.h"
#include "globals.h"
#include "install.h"
#include "io.h"
#include "monsters.h"
#include "rooms.h"
#include "search.h"
#include "survival.h"
#include "tactics.h"
#include "things.h"
#include "types.h"
#include "worth.h"

/*
 * foughtmonster recrods whether we engaged in battle recently. This
 * information is used to tell whether we should sit still, waiting for a
 * confused monster to come back, or to go on about our business.
 * DIDFIGHT is the number of turns to sit still after a battle.
 */
#define DIDFIGHT 3

/*
 * strategize: Run through each rule until something fires. Return 1 if an
 * action was taken, otherwise return 0 (and then play will read a command
 * from the user).
 */
int strategize()
{
    dwait(D_CONTROL, "Strategizing...");

    /* If replaying, instead of making an action, return the old one. */
    if(replaying) {
        return replaycommand();
    }

    /* Clear an messages we printed last turn */
    if(msgonscreen) {
        at(0, 0);
        clrtoeol();
        msgonscreen = 0;
        at(row, col);
    }

    /* Production Rules */
    if(fightmonster()) { /* We are under attack! */
        return 1;
    }

    if(fightinvisible()) { /* Claude Raines! */
        return 1;
    }

    if(tomonster()) { /* go play with the pretty monster */
        return 1;
    }

    if(shootindark()) { /* Shoot arrows in dark rooms */
        return 1;
    }

    if(handleweapon()) { /* Play with the nice sword */
        dwait(D_BATTLE, "Switching the sword [1]");
        
        return 1;
    }

    if(light()) { /* Fait lux! Especially if we lost a monster from view */
        return 1;
    }

    if(dinnertime()) { /* Soups on! */
        return 1;
    }

    /*
     * These variables are short term memory. Slowed and
     * cancelled are fuses which are disabled after a small
     * number of turns.
     */
    lyinginwait = 0; /* No more monsters to wait for */
    
    if(foughtmonster) {
        /* Turns since fought monster */
        --foughtmonster;
    }

    if(slowed) {
        /* Turns since we slowed a monster */
        --slowed;
    }

    if(cancelled) {
        /* Turns since we zapped 'cancel' */
        --cancelled;
    }

    if(beingheld) {
        /* Turns since held by a fungus */
        --beingheld;
    }

    /* End of short term memory modification */
    
    if(dropjunk()) { /* Send it back */
        return 1;
    }

    if(readscroll()) { 
        /* Get out the reading glasses. Must come before handlearmor() */
        return 1;
    }

    if(handlearmor()) { /* Play dressup */
        return 1;
    }

    if(quaffpotion()) {
        /* Glug glug glug... Must come before handlering() */
        return 1;
    }

    if(handlering()) { /* We are engaged! */
        return 1;
    }

    if(blinded && grope(50)) { /* Who turned out the lights */
        display("Blinded, groping...");

        return 1;
    }

    if(aftermelee()) { /* Wait for lingering monsters */
        return 1;
    }

    if(tostuff()) { /* Pick up the play pretty */
        return 1;
    }

    if(restup()) { /* Yawn! */
        return 1;
    }

    if(goupstairs(NOTRUNNING)) {
        /* Up we go! Make sure we get a better rank on the board */
        return 1;
    }

    if(trywand()) { /* Try to use a wand */
        return 1;
    }

    if(gotowardsgoal()) { /* Keep on trucking */
        return 1;
    }

    if(exploreroom()) { /* Search the room */
        return 1;
    }

    if(archery()) { /* Try to position for fight */
        return 1;
    }

    if(pickupafter()) { /* Look for stuff dropped by arched mon */
        return 1;
    }

    if(plunge()) { /* Plunge mode */
        return 1;
    }

    if(findarrow()) { /* Do we have an uninitialized arrow? */
        return 1;
    }

    if(findroom()) { /* Look for another room */
        return 1;
    }

    /*
     * 'attempt' records the number of times we have completely search
     * this level for secret doors. If attempt is greater than 0, then we
     * have failed once to find the stairs and go down. If this happens
     * three times, there could be a monster sleeping on the stairs. We set
     * the SLEEPER bit for each square with a sleeping monster. Go find
     * such a monster and kill it to see whether (s)he was on the stairs.
     */
    if((attempt > 4) && makemove(ATTACKSLEEP, genericinit, sleepvalue, REUSE)) {
        display("No stairs, attacking sleeping monster...");
        
        return 1;
    }

    if((Level > 1) && (larder > 0) && doorexplore()) { /* Grub around */
        return 1;
    }

    if(godownstairs(NOTRUNNING)) { /* Down we go! */
        return 1;
    }

    if(((Level < 2) || (larder < 1)) && doorexplore()) { /* Grub around anyway */
        return 1;
    }

    /*
     * If we think we are on the stairs, but aren't, maybe they were moved
     * (i.e. we were hallucinating when we saw them last time).
     */
    if(on(STAIRS) && ((atrow != stairrow) || (atcol != staircol))) {
        dwait(D_ERROR, "Stairs moved!");
        findstairs(NONE, NONE);
        
        return 1;
    }

    /*
     * If we failed to find the stairs, explore each possible secret door
     * another ten times.
     */
    while(attempt++ < MAXATTEMPTS) {
        timestosearch += max(3, k_door / 5);
        foundnew();
        
        if(doorexplore()) {
            return 1;
        }
    }

    /* Don't give up, start all over! */
    newlevel();
    display("I would give up, but I am too stubborn, starting over...");
    
    return grope(10);
}

/*
 * fightmonster: looks for adjacent monsters. If found, it calls
 * battlestations to prepare for battle otherwise hacks with the
 * weapon already in hand.
 */
int fightmonster()
{
    int i;
    int rr;
    int cc;
    int mdir = NONE;
    int mbad = NONE;
    int danger = 0;
    int melee = 0;
    int adjacent = 0;
    int alertmonster = 0;
    int wanddir = NONE;
    int m = NONE;
    int howmean;
    char mon;
    char monc = ':';
    char *monster;

    /* Check for adjacent monsters */
    for(i = 0; i < mlistlen; ++i) {
        rr = mlist[i].mrow;
        cc = mlist[i].mcol;

        if(max(abs(atrow - rr), abs(atcol - cc)) == 1) {
            if(mlist[i].q != ASLEEP) {
                if((mlist[i].q != HELD) || (Hp >= Hpmax) || !havefood(1)) {
                    melee = 1;
                    
                    if(mlist[i].q == AWAKE) {
                        alertmonster = 1;
                    }
                }
            }
        }
    }

    if(!melee) {
        /* No one to fight */
        return 0;
    }

    /* Loop to find worst monster and tally danger and number adjacent */
    for(i = 0; i < mlistlen; ++i) {
        /* Monster position */
        rr = mlist[i].mrow;
        cc = mlist[i].mcol;

        /*
         * If the mosnter is adjacent and is either awake or
         * we don't know yet whether he is asleep, but we haven't
         * see any alert monsters yet.
         */
        /* DR UTexas 26 Jan 24 */
        if(alertmonster) {
            if((max(abs(atrow - rr), abs(atcol - cc)) == 1)
               && (mlist[i].q == AWAKE)) {
                /* Record the monster type */
                mon = mlist[i].chr;

                /* Record the monster name */
                monster = monname(mon);

                /* Add to the danger */
                danger += maxhitchar(mon);

                /* If he is adjacent, add to the adj count */
                if(onrc(CANGO, rr, atcol) && onrc(CANGO, atrow, cc)) {
                    ++adjacent;
                    
                    if(isholder(monster)) {
                        howmean = 10000;
                    }
                    else {
                        howmean = avghit(i);
                    }

                    /* If he adjacent and the worst monster yet, save him */
                    if(howmean > mbad) {
                        mdir = direc(rr - atrow, cc - atcol);
                        wanddir = mdir;
                        monc = mon;
                        m = i;
                        mbad = howmean;
                    }
                }
                else if(wanddir == NONE) {
                    /* If we haven't yet a line of sight, check this guy out */
                    wanddir = direc(rr - atrow, cc - atcol);
                }

                /* Deubugging breakpoint */
                dwait(D_BATTLE,
                      "%c (%d,%d), danger %d, worst %c(%d,%d), total %d",
                      screen[rr][cc],
                      rr - atrow,
                      cc - atcol,
                      danger,
                      monc,
                      mdir,
                      mbad,
                      adjacent);
            }
        }
        else {
            if((max(abs(atrow - rr), abs(atcol - cc)) == 1)
               && mlist[i].q != ASLEEP) {
                /* Record the monster type */
                mon = mlist[i].chr;

                /* Record the monster name */
                monster = monname(mon);

                /* Add to the danger */
                danger += maxhitchar(mon);

                /* If he is adjacent, add to the adj count */
                if(onrc(CANGO, rr, atcol) && onrc(CANGO, atrow, cc)) {
                    ++adjacent;
                    
                    if(isholder(monster)) {
                        howmean = 10000;
                    }
                    else {
                        howmean = avghit(i);
                    }
                }
                else if(wanddir == NONE) {
                    /* If we haven't yet a line of sight, check this guy out */
                    wanddir = direc(rr - atrow, cc - atcol);
                }

                /* Debugging breakpoint */
                dwait(D_BATTLE,
                      "%c (%d,%d), danger %d, worst %c(%d,%d), total %d",
                      screen[rr][cc],
                      rr - atrow,
                      cc - atcol,
                      danger,
                      monc,
                      mdir,
                      mbad,
                      adjacent);
            }
        }
    }

    /* 
     * The folling variables have now been set:
     *
     * monc:     The letter of the worst monster we can hit
     * mbad:     Relative scale 0 to 26, how bad is (s)he
     * mdir:     Which direction to him/her
     * danger:   How many hit points can (s)he/they do this round?
     * wanddir:  Direction of worst monster, even if we can't move to it.
     */

    /* Check whether the battlestations expert has a suggested action. */
    monster = monname(monc);

    if(adjacent) {
        if(battlestations(m,
                          monster,
                          mbad,
                          danger,
                          mdir,
                          1,
                          alertmonster,
                          max(1, adjacent))) {
            foughtmonster = DIDFIGHT;
            
            return 1;
        }
    }
    else {
        if(battlestations(m,
                          monster,
                          mbad,
                          danger,
                          wanddir,
                          2,
                          alertmonster,
                          max(1, adjacent))) {
            foughtmonster = DIDFIGHT;

            return 1;
        }
    }

    /*
     * If we did not wait for him last turn, he is not adjacent,
     * let him move to us (otherwise, he gits to hit us first).
     */
    if(!lyinginwait && !adjacent) {
        command(T_FIGHTING, "s");
        dwait(D_BATTLE, "Lying in wait...");
        lyinginwait = 1;
        foughtmonster = DIDFIGHT;

        return 1;
    }

    /* If we are here but have no direction, there was a bug somewhere */
    if(mdir < 0) {
        dwait(D_BATTLE, "Adjacent, but no direction known!");
        
        return 0;
    }

    /* If we could die this round, tell the user about it */
    if(danger >= Hp) {
        display("In trouble...");
    }

    /* Well, nothing better than to hit the beast! Tell dwait about it */
    dwait(D_BATTLE,
          "Attacking %s(%d) direction %d (total danger %d)...",
          monster,
          mbad,
          mdir,
          danger);

    /* Record the monster type */
    lastmonster = monc - 'A' + 1;

    /* Move towards the monster (this causes us to hit him) */
    rmove(1, mdir, T_FIGHTING);
    lyinginwait = 0;
    foughtmonster = DIDFIGHT;

    return 1;
}

/*
 * tomonster: If we can see a monster (and either it is awake or we
 * think we can beat it) then pick the worst one, call battlestations,
 * and then call gotowards to move toward the monster. If the monster
 * is an odd number of turns away, sit once to assure initiative before
 * charing after him. Special case for sitting on a door.
 */
int tomonster()
{
    int i;
    int dist;
    int rr;
    int cc;
    int mdir = NONE;
    int mbad = NONE;
    int closest;
    int which;
    int danger = 0;
    int adj = 0;
    char monc = ':';
    char monchar = ':';
    char *monster;

    /* If no monsters, fail */
    if(mlistlen == 0) {
        return 0;
    }

    /*
     * Loop through the monsters, 'which' and 'closest' record the index
     * and distance of the closest monster worth fighting.
     */
    which = NONE;
    closest = 999;

    for(i = 0; i < mlistlen; ++i) {
        dist = max(abs(mlist[i].mrow - atrow), abs(mlist[i].mcol - atcol));
        monchar = mlist[i].chr;

        /*
         * IF   we are not using a magic arrow OR
         *      we want to wake this monster up AND we can beat him OR
         *      he is standing near something we want and we will have to
         *        fight him anyway
         * THEN consider fighting the monster.
         *
         * Don't pick fights with sleepers if cosmic.  DR UTexas 25 Jan 84
         */
        if(usingarrow
           || (mlist[i].q == AWAKE) 
           || (!cosmic
               && wanttowake(monchar)
               && (((avghit(i) <= 50) || ((maxhit(i) + 50 - k_wake) < Hp))
                   || ((mlist[i].q == HELD) && (Hp >= Hpmax))))) {
            /* Track total danger */
            danger += maxhit(i);

            /* Count number of monsters */
            ++adj;

            /* If he is the closest monster, save his index and distance */
            if(dist < closest) {
                closest = dist;
                which = i;
                monc = mlist[i].chr;
                mbad = avghit(i);
            }
            else if((dist == closest) && (avghit(i) > avghit(which))) {
                /* 
                 * Or if is he meaner than another equally close monster,
                 * save him
                 */
                dwait(D_BATTLE,
                      "Chasing %c(%d) rather than %c(%d) at distance %d.",
                      mlist[i].chr,
                      avghit(i),
                      mlist[which].chr,
                      avghit(which),
                      dist);

                closest = dist;
                which = i;
                monc = mlist[i].chr;
                mbad = avghit(i);
            }
        }
    }

    /* No monsters worth bothering, return failure */
    if(which < 0) {
        return 0;
    }

    /* Save the monster's location in registers */
    rr = mlist[which].mrow - atrow;
    cc = mlist[which].mcol - atcol;

    /* If the monster is on an exact diagonal, record direction */
    if((rr = 0) || (cc = 0) || (abs(rr) == abs(cc))) {
        mdir = direc(rr, cc);
    }
    else {
        mdir = -1;
    }

    /* Get a string which names the monster */
    monster = monname(monc);

    /* If 'battlestations' has an action, use that action */
    if(battlestations(which, monster, mbad, danger, mdir, closest, 1, adj)) {
        return 1;
    }
    
    /* If he is an odd number of squares away, lie in wait for him */
    if(((closest & 1) == 0) && !lyinginwait) {
        command(T_FIGHTING, "s");
        dwait(D_BATTLE, "Waiting for monster an odd number of squares away...");
        lyinginwait = 1;
    
        return 1;
    }

    /* "We have him! Move toward him!" */
    if(gotowards(mlist[which].mrow, mlist[which].mcol, 0)) {
        goalr = mlist[which].mrow;
        goalc = mlist[which].mcol;
        lyinginwait = 0;
        
        return 1;
    }

    /* could not find a path to the monster, record failure */
    return 0;
}

/*
 * wanttowake is true for mosnters without special attacks, such that the
 * expected damage from hist is a reasonable estimate of their vorpalness.
 * Some monsters are included here because we want to shoot arrows at them.
 */
int wanttowake(char c)
{
    char *monster = monname(c);

    if(missedstairs) {
        return 1;
    }

    /*
     * If monster sleep but won't wake up when we move around him,
     * return wanttowake as false.  DR UTexas 09 Jan 84
     */
    if(streq(monster, "centaur")
       || streq(monster, "dragon")
       || streq(monster, "floating eye")
       || streq(monster, "ice monster")
       || streq(monster, "leprechaun")
       || streq(monster, "nymph")
       || streq(monster, "wraith")
       || streq(monster, "purple worm")) {
        return 0;
    }

    return 1;
}

/*
 * aftermelee: Called when we have just fought a monster, assures
 *             that it wasn't just a confused monster that blockes
 *             away and might get a hit on us if we move. Now only
 *             used when we lost a monster from view.
 * 
 *             Also rest if we are critically weak and have some food.
 */
int aftermelee()
{
    if(foughtmonster > 0) {
        lyinginwait = 1;
        command(T_RESTING, "s");
        dwait(D_BATTLE, "Aftermelee: waiting for %d rounds.", foughtmonster);
        
        return 1;
    }

    /* If critically weak, rest up so traps won't kill us.  DR UTexas */
    if((Hp < 6) && (larder > 0)) {
        command(T_RESTING, "s");
        display("Recovering from severe beating...");
        
        return 1;
    }

    foughtmonster = 0;

    return foughtmonster;
}

/*
 * battlestations: We are going into battle. Can we thing of anything better to
 *                 do than simply hacking at him with our weapon?
 */

#define die_in(n) ((Hp / n) < ((danger * 50) / (100 - k_run)))
#define live_for(n) !die_in(n)

/*
 * m: Monster index
 * monster: What is it?
 * mbad: How bad is it?
 * danger: How many points damage per round?
 * mdir: Which direction (clear line of sight)?
 * mdist: How many turns until battle?
 * alert: Is he known to be awake?
 * adj How many attackers are there?
 */
int battlestations(int m,
                   char *monster, 
                   int mbad,
                   int danger,
                   int mdir,
                   int mdist,
                   int alert,
                   int adj)
{
    int obj;
    int turns;
    static int stepback = 0;

    /* Ascertain whether we have a clear path to this monster */
    if((mdir != NONE) && !checkcango(mdir, mdist)) {
        mdir = NONE;
    }

    /* Number of turns is one less than distance (modified if we are hasted) */
    if(hasted) {
        turns = (mdist - 1) * 2;
    }
    else {
        turns = mdist - 1;
    }

    /* No point in wasting resources when we are invulnerable */
    if(on(SCAREM) && ((turns > 0) || confused) && !streq(monster, "dragon")) {
        command(T_RESTING, "s");
        display("Resting on scare monster");
        dwait(D_BATTLE, "Battlestations: resting, on scaremonster.");

        return 1;
    }

    /*
     * Take invisible stalkers into account,
     * fightmonster() and tomonster() can't see stalkers.
     */
    if(beingstalked > INVPRES) {
        turns = 0;
        danger += INVDAM;
    }

    /* Debugging breakpoint */
    dwait(D_BATTLE,
          "Battlestations: %s(%d), total danger %d, dir %d, %d turns, %d addj.",
          monster,
          mbad,
          danger,
          mdir,
          turns,
          adj);

    /* Switch back to our mace or sword? */
    if(live_for(1) && (turns < 2) && wielding(thrower_obj) && handleweapon()) {
        dwait(D_BATTLE, "Switching to sword [2]");
        
        return 1;
    }

    /* Don't waste magic when on a scare monster scroll */
    if(on(SCAREM) && !streq(monster, "dragon")) {
        dwait(D_BATTLE, "Battlestations: hitting from scaremonster.");
        
        return 0;
    }

    /*
     * If we were busy resting on the stairs and we see a monster, go down
     * Go on down if about to be attacked by monster with an effective
     * magic attack.  DR UTexas 25 Jan 84
     */
    if(on(STAIRS)
       && (((Level > 18) && (Level < 26)) || exploredlevel)
       && !floating
       && (die_in(5)
           || ((seeawakemonster("rattlesnake") || seeawakemonster("giant ant"))
               && (havenamed(ring_obj, "sustain strength") < 0))
           || ((seeawakemonster("aquator") || seeawakemonster("rust monster"))
               && (turns < 2)
               && willrust(currentarmor)
               && (wearing("maintain armor") == NONE))
           || seeawakemonster("medusa")
           || seeawakemonster("umber hulk"))) {
        if(goupstairs(RUNNING) || godownstairs(RUNNING)) {
            return 1;
        }
    }

    /* Are healing postions worthwhile? */
    if(die_in(1) && ((Hpmax - Hp) > 10) && (turns > 0)) {
        obj = havenamed(potion_obj, "extra healing");
        
        if(obj != NONE) {
            return quaff(obj);
        }
        else {
            obj = havenamed(potion_obj, "healing");

            if(obj != NONE) {
                return quaff(obj);
            }
        }
    }

    /*
     * Run away if we are sure of the direction and we are in trouble
     * Don't try to run if a fungi has ahold of us. If we are confused,
     * we will try other things, and we will decide to run later.
     * If we are on a door wait until the monster is on us (that way
     * we can shoot arrows at him if we want to).
     * Don't run away from Dragons!! They'll just flame you.
     */
    if(!confused
       && !beingheld
       && (!on(DOOR) || (turns < 1))
       && (!streq(monster, "dragon") || cosmic)
       && ((Hp + Explev) < Hpmax)
       && (die_in(1) 
           || (Hp <= (danger + between(Level - 10, 0, 10)))
           || chicken)
       && runaway()) {
        display("Run away! Run away!");
        darkdir = NONE;
        darkturns = 0;

        return 1;
    }

    /* Be clever when facing multiple monsters? */
    if((adj > 1)
       && !confused
       && !beingheld
       && !on(STAIRS | DOOR)
       && backtodoor(turns)) {
        return 1;
    }

    /* Stepback to see if he is awake. */
    if(!alert
       && !beingheld
       && !stepback
       && (mdir != NONE)
       && (turns == 0)
       && !on(DOOR | STAIRS)) {
        int rdir = (mdir + 4) % 8;

        if(onrc(CANGO | TRAP, atdrow(rdir), atdcol(rdir)) == CANGO) {
            move1(rdir);
            stepback = 7;
            
            return 1;
        }
    }
     
    /* Decrement turns until step back again */
    if(stepback) {
        --stepback;
    }

    /* Should we put on our ring of maintain armor?  DR UTexas 19 Jan 84 */
    if(live_for(1)
       && (currentarmor != NONE)
       && ((leftring == NONE) || (rightring == NONE))
       && (seemonster("aquator") || seemonster("rust monster"))
       && willrust(currentarmor)
       && (wearing("maintain armor") == NONE)) {
        obj = havenamed(ring_obj, "maintain armor");

        if((obj != NONE) && puton(obj)) {
            return 1;
        }
    }

    if((turns > 1)
       && live_for(2)
       && (leftring != NONE)
       && (rightring != NONE)
       && (seemonster("aquator") || seemonster("rust monster"))
       && (wearing("maintain armor") < 0)
       && findring("maintain armor")) {
        return 1;
    }

    /* Should we put on our ring of sustain strength?  DR UTexas 19 Jan 84 */
    if((live_for(1) || (turns > 0))
       && ((leftring == NONE) || (rightring == NONE))
       && (seemonster("giant ant") || seemonster("rattlesnake"))
       && (wearing("sustain strength") < 0)) {
        obj = havenamed(ring_obj, "sustain strength");

        if((obj != NONE) && puton(obj)) {
            return 1;
        }
    }

    if((live_for(2) || (turns > 1))
       && (leftring != NONE)
       && (rightring != NONE)
       && (seemonster("giant ant") || seemonster("rattlesnake"))
       && (wearing("sustain strength") < 0)
       && findring("sustain strength")) {
        return 1;
    }

    /*
     * Should we put on our ring of regeneration? Make sure we won't kill
     * ourselves trying to do it, by checking how many turns it will take to
     * get it on compared to the number of hits we can take.
     */

    /* Have a ring and a free hand, one turn */
    if(die_in(4)
       && (live_for(1) || (turns > 0))
       && ((leftring == NONE) || (rightring == NONE))
       && !((turns == 0)
            && (streq(monster, "rattlesnake") || streq(monster, "giant ant")))
       && (wearing("regeneration") < 0)) {
        obj = havenamed(ring_obj, "regeneration");

        if((obj != NONE) && puton(obj)) {
            return 1;
        }
    }

    /* Have a ring and both hands are full, takes two turns */
    if(die_in(4)
       && (live_for(2) || (turns > 1))
       && (leftring != NONE)
       && (rightring != NONE)
       && (wearing("regeneration") < 0)
       && findring("regeneration")) {
        return 1;
    }

    /* Haste ourselves? */
    if(!hasted
       && (version > RV36B)
       && ((turns > 0) || live_for(1))
       && die_in(2)) {
        obj = havenamed(potion_obj, "haste self");

        if((obj != NONE) && quaff(obj)) {
            return 1;
        }
    }

    /* Confuse the poor beast? */
    if(die_in(2) && (turns > 0) && !redhands) {
        obj = havenamed(scroll_obj, "monster confusion");

        if(obj != NONE) {
            return reads(obj);
        }
    }

    /*
     * Put them all to sleep? This does us little good, since we can't
     * currently infer that we have a scroll of Hold Monster. But we
     * will read scrolls of identify on the second one. Bug, this
     * does not put them to sleep, it just holds them in place.
     * We have a lot more programming to do here!!!!    Fuzzy
     */
    if(die_in(1)) {
        obj = havenamed(scroll_obj, "hold monster");

        if((obj != NONE) && reads(obj)) {
            holdmonsters();

            return 1;
        }
    }

    /* Drop a scare monster? */
    if(die_in(1) && !streq(monster, "dragon"))
        obj = havenamed(scroll_obj, "scare monster");

    if((obj != NONE) && drop(obj)) {
        ++droppedscare;

        return 1;
    }

    /* Buy buy birdy! */
    if(die_in(1) && (mdir != NONE) && (turns == 0)) {
        obj = havewand("teleport away");

        if((obj != NONE) && point(obj, mdir)) {
            if(streq(monster, "violet fungi")) {
                beingheld = 0;
            }

            if(streq(monster, "venus flytrap")) {
                beingheld = 0;
            }

            return 1;
        }
    }

    /* Eat dust, turkey! */
    if(die_in(1) && (turns == 0)) {
        obj = havenamed(scroll_obj, "teleportation");

        if(obj != NONE) {
            beingheld = 0;
            
            return reads(obj);
        }
    }

    /* The better part of valor... */
    if((die_in(1) && ((turns == 0) || fainting())) && quitforhonors()) {
        return 1;
    }

    /* If we trust our magic arrow, give it a whirl */
    if(!confused && cheat && usingarrow && (goodarrow > 10) && (turns == 0)) {
        return 0;
    }

    /* Try to protect our armor from Rusties. */
    if(!cursedarmor
       && (currentarmor != NONE)
       && (seeawakemonster("rust monster") || seeawakemonster("aquator"))
       && live_for(1)
       && !(cosmic && (Level < 8))    /* DR UTexas 25 Jan 84 */
       && willrust(currentarmor)
       && (wearing("maintain armor") == NONE)
       && takeoff()) {
        return 1;
    }

    /* Any life saving wands? */
    if(die_in(2)
       && (Hp > 40)
       && (turns < 3)
       && !(streq(monster, "purple worm") || streq(monster, "jabberwock"))) {
        obj = havewand("drain life");

        if(obj != NONE) {
            return point(obj, 0);
        }
    }

    if((mdir != NONE)
       && die_in(2)
       && (!cosmic || (Level > 18))    /* DR UTexas 31 Jan 84 */
       && (streq(monster, "dragon")
           || streq(monster, "purple worm")
           || streq(monster, "jabberwock")
           || streq(monster, "medusa")
           || streq(monster, "xorn")
           || streq(monster, "violet fungi")
           || streq(monster, "griffin")
           || streq(monster, "venus flytrap")
           || streq(monster, "umber hulk")
           || streq(monster, "black unicorn"))) {
        obj = havewand("polymorph");

        if(obj != NONE) {
            return point(obj, mdir);
        }
    }

    /* Any life prolonging wands? */
    if((die_in(1)
        || ((turns == 0) && streq(monster, "floating eye"))
        || ((turns == 0) && streq(monster, "ice monster")))
       && (mdir != NONE)
       && (mdist < 6)
       && !on(DOOR)) {
        obj = havewand("fire");

        if((obj != NONE) && !streq(monster, "dragon")) {
            return point(obj, mdir);
        }
        else {
            obj = havewand("cold");

            if(obj != NONE) {
                return point(obj, mdir);
            }
            else {
                obj = havewand("lightning");

                if(obj != NONE) {
                    return point(obj, mdir);
                }
            }
        }
    }

    if(die_in(2)
       && (mdir != NONE)
       && !slowed
       && ((turns > 0) || live_for(2))) {
        obj = havewand("slow monster");

        if(obj != NONE) {
            slowed = 5;

            return point(obj, mdir);
        }
    }

    if((mdir != NONE)
       && !cancelled
       && (turns == 0)
       && (streq(monster, "wraith")
           || streq(monster, "vampire")
           || streq(monster, "floating eye")
           || streq(monster, "ice monster")
           || streq(monster, "leprechaun")
           || streq(monster, "violet fungi")
           || streq(monster, "venus flytrap"))) {
        obj = havewand("cancellation");

        if(obj != NONE) {
            cancelled = 10;

            if(streq(monster, "violet fungi")
               || streq(monster, "venus flytrap")) {
                beingheld = 0;
            }

            return point(obj, mdir);
        }
    }

    if(((die_in(3) && live_for(1))
        || ((turns == 0) && streq(monster, "floating eye"))
        || ((turns == 0) && streq(monster, "ice monster")))
       && (mdir != NONE)) {
        obj = havewand("magic missile");

        if((obj != NONE) && (turns > 0)) {
            return point(obj, mdir);
        }
        else {
            obj = havewand("striking");

            if((obj != NONE) && (turns == 0)) {
                return point(obj, mdir);
            }
        }
    }

    /*
     * Since we have no directional things, we will try to run even though
     * we are confused. Again, wait at door until the monster is on us.
     * Don't run away from dragons, they'll just flame you!!
     */
    if(confused
       && !beingheld
       && (!on(DOOR) || (turns < 1))
       && !streq(monster, "dragon")
       && (die_in(1) && (((Hp + (Explev / 2) + 3) < Hpmax) || chicken))
       && runaway()) {
        display("Run away! Run away!");
        
        return 1;
    }

    /*
     * We can live for a while, try to get to a position where we can run
     * away if we really get into trouble. Don't run away from dragons,
     * they'll just flame you!!!
     */
    if(!confused
       && !beingheld
       && !streq(monster, "dragon")
       && ((mdir < 0) || (turns < 5))
       && (((((adj > 1) || live_for(1) || die_in(4)) && !canrun())) && unpin())) {
        display("Unpinning!!!");

        return 1;
    }

    /* Light up the room if we are in combat */
    if((turns > 0) && die_in(3) && lightroom()) {
        return 1;
    }

    /* We aren't yet in danger and can shoot at the old monster. */
    if((live_for(5) || (turns > 1)) && shootindark()) {
        return 1;
    }

    /*
     * Try out an unknown wand? Try shooting unknown wands at
     * rattlesnakes since they are such a pain.    DR UTexas 19 Jan 84
     */
    if(live_for(2)
       && ((Level > 8)
           || streq(monster, "rattlesnake")
           || streq(monster, "giant ant"))
       && (mdir != NONE)
       && on(ROOM)
       && (mdist < 6)) {
        obj = unknown(wand_obj);

        if((obj != NONE) && point(obj, mdir)) {
            usesynch = 0;

            /* DR UTexas 19 Jan 84 */
            /* zappedunknown = TRUE; */
            
            return 1;
        }
    }

    /* Wait to see if he is realy awake. */
    if(!alert && !lyinginwait && (turns > 0)) {
        command(T_FIGHTING, "s");
        dwait(D_BATTLE, "Waiting to see if he is awake...");
        lyinginwait = 1;

        return 1;
    }

    /*
     * Archery: try to move into a better position, and after that, try to
     * shoot an arrow at the beast. Conserve arrows below SAVEARROWS.
     */
    if(streq(monster, "leprechaun")
       || streq(monster, "nymph")
       || streq(monster, "floating eye")
       || streq(monster, "ice monster")
       || streq(monster, "giant ant")
       || streq(monster, "rattlesnake")
       || streq(monster, "wraith")
       || streq(monster, "vampire")
       || streq(monster, "centaur")  /* DR UTexas 21 Jan 84 */
       || die_in(1 + (k_arch / 20))
       || ((SAVEARROWS + 5 - (k_arch / 10)) < ammo)) {
        obj = havemissile();

        if(obj != NONE) {
            /* Move into position */
            if((!alert || (mdir < 0)) && (turns > 0) && archmonster(m, 1)) {
                return 1;
            }

            /* If in position */
            if(!on(HALL) && (mdir != NONE) && (turns > 0)) {
                int bow;

                /* Wield the bow if we have time */
                if(!cursedweapon && !wielding(thrower_obj) && (turns > 4)) {
                    bow = havebow(1, NOPRINT);

                    if((bow != NONE) && wield(bow)) {
                        return 1;
                    }
                }

                /* And shoot! */
                throw(obj, mdir);

                return 1;
            }
        }
    }

    /* Switch back to your mace or sword? */
    if(!cursedweapon && wielding(thrower_obj) && handleweapon()) {
        dwait(D_BATTLE, "Switching to sword [3]");
        
        return 1;
    }

    /* No bright ideas. Return an let the called figured out what to do. */
    return 0;
}

/*
 * tostuff: If we see something to pick up, go to it. If out pack is full,
 * try to drop our least useful item. If pack is still full, fail.
 */
int tostuff()
{
    int i;
    int closest;
    int dist;
    int w;
    int worst;
    int worstval;
    int which;
    int wrow;
    int wcol;
    stuff what;

    /* If we don't see anything (or don't care),  return failure */
    if((slistlen == 0) || ((Level == 1) && (have(amulet_obj) != NONE))) {
        return 0;
    }

    /*
     * Not find the closest thing to pick up. Don't consider things we have
     * already dropped (those squares have the USELESS bit set), unless we
     * have dropped a scroll of SCARE MONSTER, in which case we want our
     * pack to be full. Don't be fooled by stairs when hallucinating.
     *
     * NOTE: Don't pick up the scaremonster scroll!!!    MLM
     */
    which = NONE;
    closest = 999;

    for(i = 0; i < slistlen; ++i) {
        if(!onrc(USELESS, slist[i].srow, slist[i].scol)
           || (droppedscare 
               && (objcount < maxobj)
               && !onrc(SCAREM, slist[i].srow, slist[i].scol))) {
            dist = max(abs(slist[i].srow - atrow), abs(slist[i].scol - atcol));

            /* Make junk look farther away, but not farther than infinity */
            if(onrc(USELESS, slist[i].srow, slist[i].scol)) {
                dist += 500;
            }

            /* If this is the closest item, save its distance and index */
            if(dist < closest) {
                closest = dist;
                which = i;
            }
        }
    }

    /* Could not find anything worth picking up, return failure */
    if(which < 0) {
        return 0;
    }

    /* Found something, save its location and type in registers */
    what = slist[which].what;
    wrow = slist[which].srow;
    wcol = slist[which].scol;

    /* We can always pick up more gold */
    if(what == gold_obj) {
        return gotowards(wrow, wcol, 0);
    }

    /* Have space in our pack, go get it */
    if(objcount < maxobj) {
        return gotowards(wrow, wcol, 0);
    }

    /* No space in pack and we cannot drop something here, fail */
    if(on(STUFF | DOOR | TRAP | STAIRS)) {
        return 0;
    }

    /* Must drop something, pick least valuable item to drop */
    worst = NONE;
    worstval = 9999;
    
    for(i = 0; i < invcount; ++i) {
        if(inven[i].count && !itemis(i, INUSE)) {
            w = worth(i);

            if(w < worstval) {
                worst = i;
                worstval = w;
            }
        }
        
        /* Once we have found a totally useless item, stop looking */
        if(worstval == 0) {
            break;
        }
    }

    /* Found an item, drop it */
    if(worst != NONE) {
        return drop(worst);
    }

    /* Pack is full and we can't find something to drop, fail */
    return 0;
}

/*
 * fightinvisible: Being hounded by uness beasties, try something clever.
 */
int fightinvisible()
{
    char cmd[20];
    int dir;
    int liberties = 0;
    int lastdir;
    int obj;

    /* Count down the time since we were last hit by a stalker */
    if(--beingstalked < 0) {
        beingstalked = 0;

        return beingstalked;
    }

    /* If we are in real trouble we might want to quit */
    if((beingstalked > INVPRES) && (Hp < INVDAM) && quitforhonors()) {
        return 1;
    }

    /* Can we teleport out of here? */
    if((Hp < INVDAM) && (beingstalked > INVPRES)) {
        obj = havenamed(scroll_obj, "teleport");

        if((obj != NONE) && reads(obj)) {
            beingstalked = INVPRES - 1;

            return 1;
        }
    }

    /* Can we quaff a potion of see invisible? */
    obj = havenamed(potion_obj, "see invisible");

    if((obj != NONE) && quaff(obj)) {
        beingstalked = 0;

        return 1;
    }

    /* If we have some time, try putting on a ring of see invisible */
    if((Hp > ((INVDAM * 3) / 2))
       && (beingstalked > INVLURK)
       && findring("see invisible")) {
        return 1;
    }

    /* If we can bail out to next level, do so */
    if((((beingstalked < INVPRES) && (Hp < (INVDAM * 2)))
        || ((beingstalked >= INVPRES) && (Hp < (INVDAM * 3))))
       && godownstairs(RUNNING)) {
        display("Running like hell from an invisible stalker...");

        return 1;
    }

    /* Nothing worth doing, but he is around somewhere */
    if(beingstalked <= INVPRES) {
        return 0;
    }

    /* Must fight him 'mano a mano', tell the user (who can't see him either) */
    display("Fighting invisible stalker...");
    *cmd = '\0';

    /* Record the monster type (for didhit and didmiss, see mess.c) */
    if(version < RV53A) {
        lastmonster = 'I' - 'A' + 1;
    }
    else {
        lastmonster = 'P' - 'A' + 1;
    }

    /* Count how many orthognal moves we can make */
    for(dir = 0; dir < 8; ++dir) {
        if((atdrow(dir) > 0)
           && (atdrow(dir) < 23)
           && onrc(CANGO, atdrow(dir), atdcol(dir))
           && onrc(CANGO, atdrow(dir), atcol)
           && onrc(CANGO, atrow, atdcol(dir))) {
            ++liberties;
            lastdir = dir;
        }
    }

    /* If we can only go two ways, then go back and forth (will hit) */
    if((liberties == 1) || (liberties == 2)) {
        command(T_FIGHTING, "%c%c", keydir[lastdir], keydir[(lastdir + 4) & 7]);
        
        return 1;
    }
    else if(runaway()) {
        /* Try to get away, usually gets to a square with only 2 liberties */
        return 1;
    }

    /*
     * Else run two and then double back on him. If that will
     * not work, run in a circle (will hit one out of 4)
     */
    for(dir = 0; dir < 8; dir += 2) {
        if(onrc(CANGO, atdrow(dir), atdcol(dir))
           && onrc(CANGO, atrow + (2 * deltr[dir]), atcol + (2 * deltc[dir]))) {
            break;
        }
    }

    if(dir > 7) {
        command(T_FIGHTING, "hjkl");
    }
    else {
        command(T_FIGHTING,
                "%c%c%c", 
                keydir[dir],
                keydir[dir],
                keydir[(dir + 4) & 7]);
    }

    return 1;
}

/*
 * archery: Try to arch sleeping monsters. the 'mtokill' attr keeps track
 * of how may arrows we want to be able to pump into a monster before we
 * decide to wake him up. that means we must be that far away AND have
 * that many missiles in our pack. This number can be modified by our hit
 * and damage bonuses.
 *
 * Note: Some monsters are wimpy to archery, and some too mean.    MLM
 */
int archery()
{
    int m;
    int mtk;
    char *monster;

    for(m = 0; m < mlistlen; ++m) { /* Find a sleeping monster */
        monster = monname(mlist[m].chr);
    }

    /*
     * If he is not awake and either
     *   we are much stronger than he is or
     *   he is a known target for archery and
     * we have enough arrows to wipe this dude out and
     * we have food or he is a leprechaun and we aren't hungry yet
     *
     * Then try calling archmonster to move to the right place.
     */
    if((mlist[m].q != AWAKE)
       && (gplushit != NONE)
       && !((mlist[m].q == HELD) && (Hp < Hpmax)) /* DR UTexas 26 Jan 84 */
       && ((maxhit(m) > (Hp / 3))
           || streq(monster, "leprechaun")
           || streq(monster, "nymph")
           || streq(monster, "floating eye")
           || streq(monster, "giant ant")
           || streq(monster, "rattlesnake")
           || streq(monster, "centaur")
           || streq(monster, "ice monster"))) {
        mtk = monatt[mlist[m].chr - 'A'].mtokill - gplushit;

        if(((ammo >= mtk) && (larder > 0))
           || (((streq(monster, "leprechaun") && !hungry()))
               || streq(monster, "nymph"))) {
            dwait(D_BATTLE, 
                  "Arching at %c at (%d,%d)",
                  mlist[m].chr,
                  mlist[m].mrow,
                  mlist[m].mcol);

            if(archmonster(m, mtk)) {
                return 1;
            }

            dwait(D_BATTLE, "Archmonster failed in archery.");
        }
    }

    return 0;
}

/*
 * pickupafter: Go stand on square where the monster used to be.
 *              If (s)he left something behind (even just arrows
 *              that missed) we will find it and pick it up.
 *
 * Bug: Sometimes goes the long way around and doesn't see things.
 */
int pickupafter()
{
    /* If no goal */
    if((agoalr < 0) || (agoalc < 0)) {
        return 0;
    }

    /* If on goal */
    if((atrow == agoalr) && (atcol == agoalc)) {
        agoalc = NONE;
        agoalr = agoalc;
    
        return 0;
    }

    /* Else go for it */
    return gotowards(agoalr, agoalc, 0);
}

/*
 * dropjunk: This doesn't just drop something. It destroys it.
 *           When an object is thrown diagonally into a corner,
 *           rogue can't find a place to put it, and the object is
 *           removed from the game. Used to get rid of empty wands
 *           and staves. This way, we don't pick them up later,
 *           and mistake them for fresh wands.
 */
int dropjunk()
{
    int obj;

    obj = haveuseless();

    if((obj != NONE) && (gotocorner() || throw(obj, 7))) {
        return 1;
    }

    return 0;
}

/*
 * quitforhonors: We are in mortal danger. Do we want to quit?
 *
 * Strategy: 'quitat' is the score to beat (set in steup);
 *           If we will beat it anyway, don't quit. If we
 *           won't beat it anyway, don't quit. If we will just
 *           beat the score by quitting, then do so.
 *
 * Assumes a 10 percent death tax.
 */
int quitforhonors()
{
    if((Gold > quitat) && ((Gold - (Gold / 10)) <= quitat)) {
        quitrogue("quit (scoreboard)", Gold, 0);

        return 1;
    }

    return 0;
}
