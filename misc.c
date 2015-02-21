// All sorts of miscellaneous routines
//
// @(#)misc.c 3.13 (Berkeley) 6/15/81
#include "misc.h"

#include "chase.h"
#include "daemon.h"
#include "daemons.h"
#include "fight.h"
#include "io.h"
#include "list.h"
#include "main.h"
#include "move.h"
#include "monsters.h"
#include "pack.h"

#include <ctype.h>

// tr_name:
//     Print the name of the trap
char *tr_name(char ch)
{
    char *s;

    switch(ch) {
    case TRAPDOOR:
        if(terse) {
            s = "A trapdoor.";
        }
        else {
            s = "You found a trapdoor.";
        }

        break;
    case BEARTRAP:
        if(terse) {
            s = "A beartrap.";
        }
        else {
            s = "You found a breartrap.";
        }

        break;
    case SLEEPTRAP:
        if(terse) {
            s = "A sleeping gas trap.";
        }
        else {
            s = "You found a sleeping gas trap.";
        }
        
        break;
    case ARROWTRAP:
        if(terse) {
            s = "An arrow trap";
        }
        else {
            s = "You found an arrow trap.";
        }

        break;
    case TELTRAP:
        if(terse) {
            s = "A teleport trap.";
        }
        else {
            s = "You found a teleport trap.";
        }
        
        break;
    default:
        if(terse) {
            s = "A dart trap.";
        }
        else {
            s = "You found a poison dart trap.";
        }

        break;
    }
    
    return s;
}

// look:
//     A quick glance all around the player
int look(bool wakeup)
{
    int x;
    int y;
    char ch;
    int oldx;
    int oldy;
    bool inpass;
    int passcount = 0;
    struct room *rp;
    int ey;
    int ex;

    getyx(cw, oldy, oldx);

    if((oldrp != NULL)
       && (oldrp->r_flags & ISDARK)
       && ((player.t_flags & ISBLIND) == 0)) {
	for(x = oldpos.x - 1; x <= oldpos.x + 1; ++x) {
	    for (y = oldpos.y - 1; y <= oldpos.y + 1; ++y) {
		if(((y != player.t_pos.y) || (x != player.t_pos.x))
                   && (show(y, x) == FLOOR)) {
		    mvwaddch(cw, y, x, ' ');
                }
            }
        }
    }

    rp = roomin(&player.t_pos);
    inpass = (rp == NULL);
    ey = player.t_pos.y + 1;
    ex = player.t_pos.x + 1;
    for(x = player.t_pos.x - 1; x <= ex; ++x) {
	if((x >= 0) && (x < COLS)) {
            for(y = player.t_pos.y - 1; y <= ey; ++y) {
                if((y <= 0) || (y >= (LINES - 1))) {
                    continue;
                }
                
		/* TC_DEBUG: Start */
		FILE *output;
		output = fopen("debug.txt", "a+");
		fprintf(output, "TC_DEBUG: here\n");
		fprintf(output, "TC_DEBUG: %d %d\n", y, x);
		fclose(output);
		/* TC_DEBUG: Finish */

                if(isupper(mvwinch(mw, y, x))) {
                    struct linked_list *it;
                    struct thing *tp;

                    if(wakeup){
                        it = wake_monster(y, x);
                    }
                    else {
                        it = find_mons(y, x);
                    }
                    
                    tp = (struct thing *)it->l_data;
                    tp->t_oldch = mvinch(y, x);
                    
                    if(tp->t_oldch == TRAP) {
                        if(trap_at(y,x)->tr_flags & ISFOUND) {
                            tp->t_oldch = TRAP;
                        }
                        else {
                            tp->t_oldch = FLOOR;
                        }
                    }

                    if((tp->t_oldch == FLOOR)
                       && (rp->r_flags & ISDARK)
                       && ((player.t_flags & ISBLIND) == 0)) {
                        tp->t_oldch = ' ';
                    }
                }

                // Secret doors show as walls
                ch = show(y, x);
                if(ch == SECRETDOOR) {
                    ch = secretdoor(y, x);
                }

                // Don't show room walls if he is in a passage
                if((player.t_flags & ISBLIND) == 0) {
                    if(((y == player.t_pos.y) && (x == player.t_pos.x))
                       || (inpass && ((ch == '-') || (ch == '|')))) {
                        continue;
                    }
                }
                else if((y != player.t_pos.y) || (x != player.t_pos.x)) {
                    continue;
                }
            
                wmove(cw, y, x);
                waddch(cw, ch);
                if(door_stop && !firstmove && running) {
                    switch(runch) {
                    case 'h':
                        if(x == ex) {
                            continue;
                        }
                        break;
                    case 'j':
                        if(y == (player.t_pos.y - 1)) {
                            continue;
                        }
                        break;
                    case 'k':
                        if(y == ey) {
                            continue;
                        }
                        break;
                    case 'l':
                        if(x == (player.t_pos.x - 1)) {
                            continue;
                        }
                        break;
                    case 'y':
                        if(((x + y) - (player.t_pos.x + player.t_pos.y)) >= 1) {
                            continue;
                        }
                        break;
                    case 'u':
                        if(((y - x) - (player.t_pos.y - player.t_pos.x)) >= 1) {
                        continue;
                        }
                    break;
                    case 'n':
                        if(((x + y) - (player.t_pos.x + player.t_pos.y)) <= -1) {
                            continue;
                        }
                        break;
                    case 'b':
                        if(((y - x) - (player.t_pos.y - player.t_pos.x)) <= -1) {
                            continue;
                        }
                    }
                
                    switch(ch) {
                    case DOOR:
                        if((x == player.t_pos.x) || (y == player.t_pos.y)) {
                            running = FALSE;
                        }
                        break;
                    case PASSAGE:
                        if((x == player.t_pos.x) || (y == player.t_pos.y)) {
                            passcount++;
                        }
                        break;
                    case FLOOR:
                    case '|':
                    case '-':
                    case ' ':
                        break;
                    default:
                        running = FALSE;
                        break;
                    }
                }
            }
        }
    }
    
    if(door_stop && !firstmove && (passcount > 1)) {
	running = FALSE;
    }
    
    mvwaddch(cw, player.t_pos.y, player.t_pos.x, PLAYER);
    wmove(cw, oldy, oldx);
    oldpos = player.t_pos;
    oldrp = rp;

    return 0;
}

// secretdoor:
//     Figure out what a secret door looks like
char secretdoor(int y, int x)
{
    int i;
    struct room *rp;
    coord *cpp;
    static coord cp;

    cp.y = y;
    cp.x = x;
    cpp = &cp;
    for(rp = rooms, i = 0; i < MAXROOMS; ++rp, ++i) {
        if((cpp->x <= (rp->r_pos.x +(rp->r_max.x - 1)))
           && (rp->r_pos.x <= cpp->x)
           && (cpp->y <= rp->r_pos.y + (rp->r_max.y - 1))
           && (rp->r_pos.y <= cpp->y)) {        
	    if((y == rp->r_pos.y)
               || (y == (rp->r_pos.y + rp->r_max.y - 1))) {
		return('-');
            }
	    else {
		return('|');
            }
        }
    }
    
    return('p');
}

// find_obj:
//     Find the unclaimed object at y, x
struct linked_list *find_obj(int y, int x)
{
    struct linked_list *obj;
    struct object *op;

    for(obj = lvl_obj; obj != NULL; obj = obj->l_next) {
	op = (struct object *)obj->l_data;
	if((op->o_pos.y == y) && (op->o_pos.x == x)) {
		return obj;
        }
    }

    if(wizard) {
        msg("Non-object %d,%d", y, x);
    }
    
    return NULL;
}

// eat:
//     He wants to eat something, so let him try
int eat()
{
    struct linked_list *item;
    struct object *obj;

    item = get_item("eat", FOOD);
    if(item == NULL) {
	return 0;
    }
    obj = (struct object *)item->l_data;
    if(obj->o_type != FOOD) {
	if (!terse) {
	    msg("Ugh, you would get ill if you ate that.");
        }
	else {
	    msg("That's Inedible!");
        }
        
	return 0;
    }
    inpack--;
    if(--obj->o_count < 1) {
	_detach(&player.t_pack, item);
	discard(item);
    }
    if(obj->o_which == 1) {
	msg("My, that was a yummy %s", fruit);
    }
    else {
	if(rnd(100) > 70) {
	    msg("Yuk, this food tastes awful");
	    ++player.t_stats.s_exp;
	    check_level();
	}
	else {
	    msg("Yum, that tasted good");
        }
    }

    food_left += (HUNGERTIME + rnd(400) - 200);
    if(food_left > STOMACHSIZE) {
	food_left = STOMACHSIZE;
    }
    hungry_state = 0;
    if(obj == cur_weapon) {
	cur_weapon = NULL;
    }

    return 0;
}

// chg_str:
//     Used to modify the player's strength,
//     it keeps track of the highest it has been, just in case
int chg_str(int amt)
{
    if(amt == 0) {
	return 0;
    }

    if(amt > 0) {
	while(amt) {
            amt--;
            
	    if(player.t_stats.s_str.st_str < 18) {
		++player.t_stats.s_str.st_str;
            }
	    else if(player.t_stats.s_str.st_add == 0) {
		player.t_stats.s_str.st_add = rnd(50) + 1;
            }
	    else if(player.t_stats.s_str.st_add <= 50) {
		player.t_stats.s_str.st_add = 51 + rnd(24);
            }
	    else if(player.t_stats.s_str.st_add <= 75) {
		player.t_stats.s_str.st_add = 76 + rnd(14);
            }
	    else if(player.t_stats.s_str.st_add <= 90) {
		player.t_stats.s_str.st_add = 91;
            }
	    else if(player.t_stats.s_str.st_add < 100) {
		++player.t_stats.s_str.st_add;
            }
	}
        
	if((player.t_stats.s_str.st_str > max_stats.s_str.st_str)
           || ((player.t_stats.s_str.st_str == 18)
               && (player.t_stats.s_str.st_add > max_stats.s_str.st_add))) {
            max_stats.s_str = player.t_stats.s_str;
        }
    }
    else {
	while(amt) {
            amt++;

	    if((player.t_stats.s_str.st_str < 18)
               || (player.t_stats.s_str.st_add == 0)) {
		--player.t_stats.s_str.st_str;
            }
	    else if(player.t_stats.s_str.st_add < 51) {
		player.t_stats.s_str.st_add = 0;
            }
	    else if(player.t_stats.s_str.st_add < 76) {
		player.t_stats.s_str.st_add = 1 + rnd(50);
            }
	    else if(player.t_stats.s_str.st_add < 91) {
		player.t_stats.s_str.st_add = 51 + rnd(25);
            }
	    else if(player.t_stats.s_str.st_add < 100) {
		player.t_stats.s_str.st_add = 76 + rnd(14);
            }
	    else {
		player.t_stats.s_str.st_add = 91 + rnd(8);
            }
	}
        
	if(player.t_stats.s_str.st_str < 3) {
	    player.t_stats.s_str.st_str = 3;
        }
    }

    return 0;
}

// add_haste:
//     Add haste to the player
int add_haste(bool potion)
{
    if((player.t_flags & ISHASTE) != 0) {
	msg("You faint from exhaustion.");
	no_command += rnd(8);
	extinguish(nohaste);
    }
    else {
	player.t_flags |= ISHASTE;
	if(potion) {
	    fuse(nohaste, 0, rnd(4) + 4, AFTER);
        }
    }

    return 0;
}

// aggravate:
//     Aggravate all the monsters on this level
int aggravate()
{
    struct linked_list *mi;

    for(mi = mlist; mi != NULL; mi = mi->l_next) {
	runto(&((struct thing *)mi->l_data)->t_pos, &player.t_pos);
    }

    return 0;
}

// vowelstr:
//     for printf()'s, if string starts with a vowel,
//     return 'n' for an "an"
char *vowelstr(char *str)
{
    switch (*str) {
	case 'a':
	case 'e':
	case 'i':
	case 'o':
	case 'u':
	    return "n";
	default:
	    return "";
    }
}

// is_current:
//     See if the object is one of the currently used items
int is_current(struct object *obj)
{
    if(obj == NULL) {
	return FALSE;
    }
    
    if((obj == cur_armor)
       || (obj == cur_weapon)
       || (obj == cur_ring[LEFT])
       || (obj == cur_ring[RIGHT])) {
        if(terse) {
            msg("In use.");
        }
        else {
            msg("That's already in use.");
        }
        
	return TRUE;
    }
    return FALSE;
}

// get_dir:
//     Set up the direction coordinate for use
//     in various "prefix" commands
int get_dir()
{
    char *prompt;
    bool gotit;

    if(!terse) {
        prompt = "Which direction? ";
	msg("%s", prompt);
    }
    else {
	prompt = "Direction: ";
    }

    gotit = TRUE;

    switch(readchar()) {
    case 'h':
    case 'H':
        delta.y = 0;
        delta.x = -1;
        break;
    case 'j':
    case 'J':
        delta.y = 1;
        delta.x = 0;
        break;
    case 'k':
    case 'K':
        delta.y = -1;
        delta.x = 0;
        break;
    case 'l':
    case 'L':
        delta.y = 0;
        delta.x = 1;
        break;
    case 'y':
    case 'Y':
        delta.y = -1;
        delta.x = -1;
        break;
    case 'u':
    case 'U':
        delta.y = -1;
        delta.x = 1;
        break;
    case 'b':
    case 'B':
        delta.y = 1;
        delta.x = -1;
        break;
    case 'n':
    case 'N':
        delta.y = 1;
        delta.x = 1;
        break;
    case ESCAPE:
        return FALSE;
    default:
        mpos = 0;
        msg(prompt);
        gotit = FALSE;
    }

    while(!gotit) {
        gotit = TRUE;
        
	switch(readchar()) {
        case 'h':
        case 'H':
            delta.y = 0;
            delta.x = -1;
            break;
        case 'j':
        case 'J':
            delta.y = 1;
            delta.x = 0;
            break;
        case 'k':
        case 'K':
            delta.y = -1;
            delta.x = 0;
            break;
        case 'l':
        case 'L':
            delta.y = 0;
            delta.x = 1;
            break;
        case 'y':
        case 'Y':
            delta.y = -1;
            delta.x = -1;
            break;
        case 'u':
        case 'U':
            delta.y = -1;
            delta.x = 1;
            break;
        case 'b':
        case 'B':
            delta.y = 1;
            delta.x = -1;
            break;
        case 'n':
        case 'N':
            delta.y = 1;
            delta.x = 1;
            break;
        case ESCAPE:
            return FALSE;
        default:
            mpos = 0;
            msg(prompt);
            gotit = FALSE;
	}
    }

    if(((player.t_flags & ISHUH) != 0) && (rnd(100) > 80)) {
        delta.y = rnd(3) - 1;
        delta.x = rnd(3) - 1;

        while((delta.y == 0) && (delta.x == 0)) {
            delta.y = rnd(3) - 1;
            delta.x = rnd(3) - 1;
        }
    }
    
    mpos = 0;
    return TRUE;
}
