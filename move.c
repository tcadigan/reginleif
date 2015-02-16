// Hero movement commands
//
// @(#)move.c 3.26 (Berkeley) 6/15/81

#include "move.h"

#include "chase.h"
#include "fight.h"
#include "io.h"
#include "list.h"
#include "main.h"
#include "misc.h"
#include "monsters.h"
#include "newlevel.h"
#include "rip.h"
#include "weapons.h"
#include "wizard.h"

#include <ctype.h>

// Used to hold the new hero position
coord nh;

// do_run:
//     Start the hero running
int do_run(char ch)
{
    running = TRUE;
    after = FALSE;
    runch = ch;

    return 0;
}

// do_move:
//     Check to see that a move is legal. If it is handle
//     the consequences (fighting, picking up, etc.)
int do_move(int dy, int dx)
{
    char ch;

    firstmove = FALSE;
    if(no_move) {
	--no_move;
	msg("You are still stuck in the bear trap");
        
	return 0;
    }

    // Do a confused move (maybe)
    if((rnd(100) < 80) && ((player.t_flags & ISHUH) != 0)) {
	nh = *rndmove(&player);
    }
    else {
	nh.y = player.t_pos.y + dy;
	nh.x = player.t_pos.x + dx;
    }

    // Check if he tried to move off the screen or make an illegal
    // diagonal move, and stop him if he did
    if((nh.x < 0)
       || (nh.x > (COLS - 1))
       || (nh.y < 0)
       || (nh.y > (LINES - 1))
       || !diag_ok(&player.t_pos, &nh)) {
	after = FALSE;
	running = FALSE;
        
	return 0;
    }
    
    if(running && (player.t_pos.x == nh.x) && (player.t_pos.y == nh.y)) {
        running = FALSE;
	after = FALSE;
    }
    
    if(mvwinch(mw, nh.y, nh.x) == ' ') {
        ch = mvwinch(stdscr, nh.y, nh.x);
    }
    else {
        ch = winch(mw);
    }

    if(((player.t_flags & ISHELD) != 0) && (ch != 'F')) {
	msg("You are being held");
	return 0;
    }
    
    switch(ch) {
    case ' ':
    case '|':
    case '-':
    case SECRETDOOR:
        running = FALSE;
        after = FALSE;

        return 0;
    case TRAP:
        {
            char temp;
            
	    ch = be_trapped(&nh);
	    if((ch == TRAPDOOR) || (ch == TELTRAP)) {
		return 0;
            }

            if(mvwinch(mw, player.t_pos.y, player.t_pos.x) == ' ') {
                temp = mvwinch(stdscr, player.t_pos.y, player.t_pos.x);
            }
            else {
                temp = winch(mw);
            }

            if((ch == PASSAGE) && (temp == DOOR)) {
                light(&player.t_pos);
            }
            else if(ch == DOOR) {
                running = FALSE;

                if(temp == PASSAGE) {
                    light(&nh);
                }
            }
            else if(ch == STAIRS) {
                running = FALSE;
            }
            else if(isupper(ch)) {
                running = FALSE;
                fight(&nh, ch, cur_weapon, FALSE);
                return 0;
            }

            ch = temp;
            wmove(cw, player.t_pos.y, player.t_pos.x);
            waddch(cw, ch);
            player.t_pos = nh;
            wmove(cw, player.t_pos.y, player.t_pos.x);
            waddch(cw, PLAYER);
        }

        break;
    case GOLD:
    case POTION:
    case SCROLL:
    case FOOD:
    case WEAPON:
    case ARMOR:
    case RING:
    case AMULET:
    case STICK:
        running = FALSE;
        take = ch;
    default:
        {
            char temp;
            
            if(mvwinch(mw, player.t_pos.y, player.t_pos.x) == ' ') {
                temp = mvwinch(stdscr, player.t_pos.y, player.t_pos.x);
            }
            else {
                temp = winch(mw);
            }
            
            if((ch == PASSAGE) && (temp == DOOR)) {
                light(&player.t_pos);
            }
            else if(ch == DOOR) {
                running = FALSE;
                
                if(temp == PASSAGE) {
                    light(&nh);
                }
            }
            else if(ch == STAIRS) {
                running = FALSE;
            }
            else if(isupper(ch)) {
                running = FALSE;
                fight(&nh, ch, cur_weapon, FALSE);
                return 0;
            }
            
            ch = temp;
            wmove(cw, player.t_pos.y, player.t_pos.x);
            waddch(cw, ch);
            player.t_pos = nh;
            wmove(cw, player.t_pos.y, player.t_pos.x);
            waddch(cw, PLAYER);
        }
    }
    
    return 0;
}

/*
 * Called to illuminate a room.
 * If it is dark, remove anything that might move.
 */
// light:
//     Called to illuminat a room. If it is dark,
//     remove anything that might move.
int light(coord *cp)
{
    struct room *rp;
    int j;
    int k;
    char ch;
    char rch;
    struct linked_list *item;

    rp = roomin(cp);

    if((rp != NULL) && ((player.t_flags & ISBLIND) == 0)) {
	for(j = 0; j < rp->r_max.y; ++j) {
	    for(k = 0; k < rp->r_max.x; ++k) {
		ch = show(rp->r_pos.y + j, rp->r_pos.x + k);
		wmove(cw, rp->r_pos.y + j, rp->r_pos.x + k);

                // Figure out how to display a secret door
		if(ch == SECRETDOOR) {
		    if((j == 0) || (j == (rp->r_max.y - 1))) {
			ch = '-';
                    }
		    else {
			ch = '|';
                    }
		}

                // If the room is a dark room, we might want to remove
                // monster and the like from it (since they might move)
		if(isupper(ch)) {
		    item = wake_monster(rp->r_pos.y+j, rp->r_pos.x+k);
                    
		    if(((struct thing *)item->l_data)->t_oldch == ' ') {
			if(!(rp->r_flags & ISDARK)) {
			    ((struct thing *)item->l_data)->t_oldch =
				mvwinch(stdscr, rp->r_pos.y + j, rp->r_pos.x + k);
                        }
                    }
		}
		if(rp->r_flags & ISDARK) {
		    rch = mvwinch(cw, rp->r_pos.y + j, rp->r_pos.x + k);
		    switch(rch) {
                    case DOOR:
                    case STAIRS:
                    case TRAP:
                    case '|':
                    case '-':
                    case ' ':
                        ch = rch;
                        break;
                    case FLOOR:
                        if((player.t_flags & ISBLIND) != 0) {
                            ch = FLOOR;
                        }
                        else {
                            ch = ' ';
                        }

                        break;
                    default:
                        ch = ' ';
		    }
		}
		mvwaddch(cw, rp->r_pos.y+j, rp->r_pos.x+k, ch);
	    }
	}
    }

    return 0;
}

// show:
//     Returns what a certain thing will display as to the un-initiated
char show(int y, int x)
{
    char ch;
    struct linked_list *it;
    struct thing *tp;

    if(mvwinch(mw, y, x) == ' ') {
        ch = mvwinch(stdscr, y, x);
    }
    else {
        ch = winch(mw);
    }
    
    if(ch == TRAP) {
        if(trap_at(y,x)->tr_flags & ISFOUND) {
            return TRAP;
        }
        else {
            return FLOOR;
        }
    }
    else if((ch == 'M') || (ch == 'I')) {
        it = find_mons(y, x);
        
	if(it == NULL) {
	    msg("Can't find monster in show");
        }
        
	tp = (struct thing *)it->l_data;
        
	if(ch == 'M') {
	    ch = tp->t_disguise;
        }
        else if((player.t_flags & CANSEE) == 0) {
            // Hide the invisible monsters
	    ch = mvwinch(stdscr, y, x);
        }
    }
    return ch;
}

// be_trapped:
//     The guy stepped on a trap...Make him pay.
int be_trapped(coord *tc)
{
    struct trap *tp;
    char ch;

    tp = trap_at(tc->y, tc->x);
    count = running = FALSE;
    mvwaddch(cw, tp->tr_pos.y, tp->tr_pos.x, TRAP);
    tp->tr_flags |= ISFOUND;

    ch = tp->tr_type;
    
    switch(ch) {
    case TRAPDOOR:
        ++level;
        new_level();
        msg("You fell into a trap!");
        break;
    case BEARTRAP:
        no_move += BEARTIME;
        msg("You are caught in a bear trap");
        break;
    case SLEEPTRAP:
        no_command += SLEEPTIME;
        msg("A strange white mist envelops you and you fall asleep");
        break;
    case ARROWTRAP:
        if(swing(player.t_stats.s_lvl - 1, player.t_stats.s_arm, 1)) {
            msg("Oh no! An arrow shot you");

            player.t_stats.s_hpt -= roll(1, 6);
            
            if(player.t_stats.s_hpt <= 0) {
                msg("The arrow killed you.");
                death('a');
            }
        }
        else {
            struct linked_list *item;
            struct object *arrow;
            
            msg("An arrow shoots past you.");
            item = new_item(sizeof *arrow);
            arrow = (struct object *)item->l_data;
            arrow->o_type = WEAPON;
            arrow->o_which = ARROW;
            init_weapon(arrow, ARROW);
            arrow->o_count = 1;
            arrow->o_pos = player.t_pos;
            fall(item, FALSE);
        }
        break;
    case TELTRAP:
        teleport();
        break;
    case DARTTRAP:
        if(swing(player.t_stats.s_lvl + 1, player.t_stats.s_arm, 1)) {
            msg("A small dart just hit you in the shoulder");

            player.t_stats.s_hpt -= roll(1, 4);
            
            if(player.t_stats.s_hpt <= 0) {
                msg("The dart killed you.");
                death('d');
            }

            if(!(((cur_ring[LEFT] != NULL) && (cur_ring[LEFT]->o_which == R_SUSTSTR))
                 || ((cur_ring[RIGHT] != NULL) && (cur_ring[RIGHT]->o_which == R_SUSTSTR)))) {
                chg_str(-1);
            }
        }
        else {
            msg("A small dart whizzes by your ear and vanishes.");
        }
    }

    // Flush typeahead
    /* raw(); */
    /* noraw(); */

    return(ch);
}

// trap_at:
//     Find the trap at (y, x) on screen
struct trap *trap_at(int y, int x)
{
    struct trap *tp;
    struct trap *ep;

    ep = &traps[ntraps];
    for(tp = traps; tp < ep; ++tp) {
	if((tp->tr_pos.y == y) && (tp->tr_pos.x == x)) {
	    break;
        }
    }
    if(tp == ep) {
        if(wizard) {
            msg("Trap at %d,%d not in array", y, x);
        }
    }
    
    return tp;
}

// rndmove:
//     Move in a random direction if the monster/person is confused
coord *rndmove(struct thing *who)
{
    int x;
    int y;
    char ch;
    int ex;
    int ey;
    int nopen = 0;
    struct linked_list *item;
    struct object *obj;
    // What we will be returning
    static coord ret;
    static coord dest;

    ret = who->t_pos;

    // Now go through the spaces surrounding the player and
    // set that place in the array to true if the space can
    // be moved into
    ey = ret.y + 1;
    ex = ret.x + 1;
    for(y = who->t_pos.y - 1; y <= ey; ++y) {
	if((y >= 0) && (y < LINES)) {
	    for(x = who->t_pos.x - 1; x <= ex; ++x) {
		if((x < 0) || (x >= COLS)) {
		    continue;
                }
                if(mvwinch(mw, y, x) == ' ') {
                    ch = mvwinch(stdscr, y, x);
                }
                else {
                    ch = winch(mw);
                }
		if(step_ok(ch)) {
		    dest.y = y;
		    dest.x = x;
		    if(!diag_ok(&who->t_pos, &dest)) {
			continue;
                    }
		    if(ch == SCROLL) {
			item = NULL;
			for(item = lvl_obj; item != NULL; item = item->l_next) {
			    obj = (struct object *)item->l_data;
			    if((y == obj->o_pos.y) && (x == obj->o_pos.x)) {
				break;
                            }
			}
			if((item != NULL) && (obj->o_which == S_SCARE)) {
			    continue;
                        }
		    }
                    ++nopen;
                    
		    if(rnd(nopen) == 0) {
			ret = dest;
                    }
		}
            }
        }
    }
    
    return &ret;
}
