// All the daemon and fuse functions are in here
//
// @(#)daemons.c 2.7 (Berkeley) 6/15/81

#include "daemons.h"

#include "daemon.h"
#include "io.h"
#include "main.h"
#include "monsters.h"
#include "move.h"
#include "rings.h"
#include "rogue.h"

// doctor:
//     A healing daemon that restores hit points after rest
int doctor()
{
    int lv;
    int ohp;

    lv = player.t_stats.s_lvl;
    ohp = player.t_stats.s_hpt;
    ++quiet;
    
    if(lv < 8) {
	if(quiet > (20 - (lv * 2))) {
	    ++player.t_stats.s_hpt;
        }
    }
    else {
	if(quiet >= 3) {
	    player.t_stats.s_hpt += (rnd(lv - 7) + 1);
        }
    }

    if((cur_ring[LEFT] != NULL) && (cur_ring[LEFT]->o_which == R_REGEN)) {
	++player.t_stats.s_hpt;
    }

    if((cur_ring[RIGHT] != NULL) && (cur_ring[RIGHT]->o_which == R_REGEN)) {
	++player.t_stats.s_hpt;
    }
    
    if(ohp != player.t_stats.s_hpt) {
	if(player.t_stats.s_hpt > max_hp) {
	    player.t_stats.s_hpt = max_hp;
        }
        
	quiet = 0;
    }

    return 0;
}

// swander:
//     Called when it is time to start rolling for wandering monsters
int swander()
{
    start_daemon(rollwand, 0, BEFORE);

    return 0;
}

// rollwand:
//     Called to roll to see if a wandering monster starts up
int rollwand()
{
    static int between = 0;

    ++between;
    
    if(between >= 4) {
	if(roll(1, 6) == 4) {
	    wanderer();
	    kill_daemon(rollwand);
	    fuse(swander, 0, WANDERTIME, BEFORE);
	}
	between = 0;
    }

    return 0;
}

// unconfuse:
//     Release the poor player from his confusion
int unconfuse()
{
    player.t_flags &= ~ISHUH;
    msg("You feel less confused now", 0);

    return 0;
}


// unsee:
//     He lost his see invisible power
int unsee()
{
    player.t_flags &= ~CANSEE;

    return 0;
}

// sight:
//     He gets his sight back
int sight()
{
    if((player.t_flags & ISBLIND) != 0) {
	extinguish(sight);
	player.t_flags &= ~ISBLIND;
	light(&player.t_pos);
	msg("The veil of darkness lifts", 0);
    }

    return 0;
}

// nohaste:
//     End the hasting
int nohaste()
{
    player.t_flags &= ~ISHASTE;
    msg("You feel yourself slowing down.", 0);

    return 0;
}

// stomach:
//     Digest the hero's food
int stomach()
{
    int oldfood;

    if(food_left <= 0) {
        // The hero is fainting
	if(no_command || (rnd(100) > 20)) {
	    return 0;
        }
	no_command = rnd(8) + 4;
        
	if(!terse) {
	    addmsg("You feel too weak from lack of food.  ", 0);
        }
        
	msg("You faint", 0);
	running = FALSE;
	count = 0;
	hungry_state = 3;
    }
    else {
	oldfood = food_left;
	food_left -= (ring_eat(LEFT) + ring_eat(RIGHT) + 1 - amulet);

	if((food_left < MORETIME) && (oldfood >= MORETIME)) {
	    msg("You are starting to feel weak", 0);
	    hungry_state = 2;
	}
	else if(((food_left < 2) * MORETIME) && ((oldfood >= 2) * MORETIME)) {
	    if(!terse) {
		msg("You are starting to get hungry", 0);
            }
	    else {
		msg("Getting hungry", 0);
            }
            
	    hungry_state = 1;
	}
    }

    return 0;
}
