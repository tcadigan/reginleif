// Routines dealing specifically with rings
//
// @(#)rings.c 3.17 (Berkeley) 6/15/81
#include "rings.h"

#include "io.h"
#include "options.h"
#include "main.h"
#include "misc.h"
#include "move.h"
#include "pack.h"
#include "things.h"
#include "weapons.h"

// ring_on:
//     Something...
int ring_on()
{
    struct object *obj;
    struct linked_list *item;
    register int ring;
    short save_max_st_str;
    short save_max_st_add;
    char buf[80];

    item = get_item("put on", RING);
    // Make certain that it is something that we want to wear
    if(item == NULL) {
	return 0;
    }
    obj = (struct object *)item->l_data;
    if(obj->o_type != RING) {
	if(!terse) {
	    msg("It would be difficult to wrap that around a finger");
        }
	else {
	    msg("Not a ring");
        }
        
	return 0;
    }

    // Find out which hand to put it on
    if(is_current(obj)) {
	return 0;
    }

    if((cur_ring[LEFT] == NULL) && (cur_ring[RIGHT] == NULL)) {
        ring = gethand();
	if(ring < 0) {
	    return 0;
        }
    }
    else if(cur_ring[LEFT] == NULL) {
	ring = LEFT;
    }
    else if(cur_ring[RIGHT] == NULL) {
	ring = RIGHT;
    }
    else {
	if(!terse) {
	    msg("You already have a ring on each hand");
        }
	else {
	    msg("Wearing two");
        }
	return 0;
    }
    cur_ring[ring] = obj;

    // Calculate the effect it has on the poor guy
    switch(obj->o_which) {
    case R_ADDSTR:
	save_max_st_str = max_stats.st_str;
	save_max_st_add = max_stats.st_add;
        chg_str(obj->o_ac);
	max_stats.st_str = save_max_st_str;
	max_stats.st_add = save_max_st_add;
        break;
    case R_SEEINVIS:
        player.t_flags |= CANSEE;
        light(&player.t_pos);
        mvwaddch(cw, player.t_pos.y, player.t_pos.x, PLAYER);
        break;
    case R_AGGR:
        aggravate();
        break;
    }
    
    status();
    
    if(r_know[obj->o_which] && r_guess[obj->o_which]) {
	free(r_guess[obj->o_which]);
	r_guess[obj->o_which] = NULL;
    }
    else if(!r_know[obj->o_which] && askme && (r_guess[obj->o_which] == NULL)) {
	mpos = 0;
        
        if(terse) {
            msg("Call it: ");
        }
        else {
            msg("What do you want to call it? ");
        }

	if(get_str(buf, cw) == NORM) {
	    r_guess[obj->o_which] = malloc((unsigned int) strlen(buf) + 1);
	    strcpy(r_guess[obj->o_which], buf);
	}
        
	msg("");
    }

    return 0;
}

// ring_off:
//     Something...
int ring_off()
{
    int ring;
    struct object *obj;

    if((cur_ring[LEFT] == NULL) && (cur_ring[RIGHT] == NULL)) {
	if(terse) {
	    msg("No rings");
        }
	else {
	    msg("You aren't wearing any rings");
        }
        
	return 0;
    }
    else if(cur_ring[LEFT] == NULL) {
	ring = RIGHT;
    }
    else if(cur_ring[RIGHT] == NULL) {
	ring = LEFT;
    }
    else {
        ring = gethand();
	if(ring < 0) {
	    return 0;
        }
    }
    mpos = 0;
    obj = cur_ring[ring];
    if(obj == NULL) {
	msg("Not wearing such a ring");

	return 0;
    }
    if(dropcheck(obj)) {
	msg("Was wearing %s", inv_name(obj, TRUE));
    }

    return 0;
}

// gethand:
//     Something...
int gethand()
{
    int c;

    while(1) {
	if(terse) {
	    msg("Left or Right ring? ");
        }
	else {
	    msg("Left hand or right hand? ");
        }
        c = readchar();
	if((c == 'l') || (c == 'L')) {
	    return LEFT;
        }
	else if((c == 'r') || (c == 'R')) {
	    return RIGHT;
        }
	else if(c == ESCAPE_KEY) {
	    return -1;
        }
	mpos = 0;
	if(terse) {
	    msg("L or R");
        }
	else {
	    msg("Please type L or R");
        }
    }
}

// ring_eat:
//     How much food does this ring use up?
int ring_eat(int hand)
{
    if(cur_ring[hand] == NULL) {
	return 0;
    }
    
    switch(cur_ring[hand]->o_which) {
    case R_REGEN:
        return 2;
    case R_SUSTSTR:
        return 1;
    case R_SEARCH:
        return (rnd(100) < 33);
    case R_DIGEST:
        return -(rnd(100) < 50);
    default:
        return 0;
    }
}

// ring_num:
//     Print ring bonuses
char *ring_num(struct object *obj)
{
    static char buf[5];

    if(!(obj->o_flags & ISKNOW)) {
	return "";
    }
    
    switch(obj->o_which) {
    case R_PROTECT:
    case R_ADDSTR:
    case R_ADDDAM:
    case R_ADDHIT:
        buf[0] = ' ';
        strcpy(&buf[1], num(obj->o_ac, 0));
        break;
    default:
        return "";
    }
    
    return buf;
}
