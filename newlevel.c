// Dig and draw a new level
//
// @(#)new_level.c 3.7 (Berkeley) 6/2/81

#include "newlevel.h"

#include "io.h"
#include "list.h"
#include "main.h"
#include "move.h"
#include "passages.h"
#include "rooms.h"
#include "things.h"

/* TC_DEBUG: Start */
#include "debug.h"
#include <stdio.h>
/* TC_DEBUG: Finish */

// new_level:
//     Dig and draw a new level
int new_level()
{
    int rm;
    int i;
    char ch;
    coord stairs;

    if(level > max_level) {
	max_level = level;
    }
    wclear(cw);
    wclear(mw);
    clear();
    status();

    // Free up the monsters on the last level
    _free_list(&mlist);
    // Draw rooms
    do_rooms();
    // Draw passages
    do_passages();
    ++no_food;

    // Place objects (if any)
    put_things();

    // Place the staircase down
    rm = rnd_room();
    rnd_pos(&rooms[rm], &stairs);
    
    char temp;
    if(mvwinch(mw, stairs.y, stairs.x) == ' ') {
        temp = mvwinch(stdscr, stairs.y, stairs.x);
    }
    else {
        temp = winch(mw);
    }
    
    while(temp != FLOOR) {
        rm = rnd_room();
        rnd_pos(&rooms[rm], &stairs);

        if(mvwinch(mw, stairs.y, stairs.x) == ' ') {
            temp = mvwinch(stdscr, stairs.y, stairs.x);
        }
        else {
            temp = winch(mw);
        }
    }

    addch(STAIRS);

    // Place the traps
    if(rnd(10) < level) {
	ntraps = rnd(level / 4) + 1;
	if(ntraps > MAXTRAPS) {
	    ntraps = MAXTRAPS;
        }
	i = ntraps;
	while(i) {
            --i;
            rm = rnd_room();
            rnd_pos(&rooms[rm], &stairs);

            if(mvwinch(mw, stairs.y, stairs.x) == ' ') {
                temp = mvwinch(stdscr, stairs.y, stairs.x);
            }
            else {
                temp = winch(mw);
            }
            
            while(temp != FLOOR) {
                rm = rnd_room();
                rnd_pos(&rooms[rm], &stairs);

                if(mvwinch(mw, stairs.y, stairs.x) == ' ') {
                    temp = mvwinch(stdscr, stairs.y, stairs.y);
                }
                else {
                    temp = winch(mw);
                }
            }
            
	    switch(rnd(6)) {
		case 0:
                    ch = TRAPDOOR;
                    break;
		case 1:
                    ch = BEARTRAP;
                    break;
		case 2:
                    ch = SLEEPTRAP;
                    break;
		case 3:
                    ch = ARROWTRAP;
                    break;
		case 4:
                    ch = TELTRAP;
                    break;
		case 5:
                    ch = DARTTRAP;
                    break;
	    }

	    addch(TRAP);
	    traps[i].tr_type = ch;
	    traps[i].tr_flags = 0;
	    traps[i].tr_pos = stairs;
	}
    }

    rm = rnd_room();
    rnd_pos(&rooms[rm], &player.t_pos);

    if(mvwinch(mw, player.t_pos.y, player.t_pos.x) == ' ') {
        temp = mvwinch(stdscr, player.t_pos.y, player.t_pos.x);
    }
    else {
        temp = winch(mw);
    }
    
    while(temp != FLOOR) {
        rm = rnd_room();
        rnd_pos(&rooms[rm], &player.t_pos);

        if(mvwinch(mw, player.t_pos.y, player.t_pos.x) == ' ') {
            temp = mvwinch(stdscr, player.t_pos.y, player.t_pos.x);
        }
        else {
            temp = winch(mw);
        }
    }
    
    light(&player.t_pos);
    wmove(cw, player.t_pos.y, player.t_pos.x);
    waddch(cw, PLAYER);

    return 0;
}

// rnd_room:
//     Pick a room that is really there
int rnd_room()
{
    int rm;

    rm = rnd(MAXROOMS);

    while(rooms[rm].r_flags & ISGONE) {
        rm = rnd(MAXROOMS);
    }

    return rm;
}

// put_things:
//     Put potions and scrolls on this level
int put_things() {
    int i;
    struct linked_list *item;
    struct object *cur;
    int rm;
    coord tp;

    // Throw away stuff left on the previous level (if anything)
    _free_list(&lvl_obj);

    // Once you have found the amulet, the only way to get new stuff
    // is to go down into the dungeon.
    if(amulet && (level < max_level)) {
	return 0;
    }

    // Do MAXOBJ attempts to put things on a level
    for(i = 0; i < MAXOBJ; ++i) {
	if(rnd(100) < 35) {
            // Pick a new object and link it in the list
	    item = new_thing();
	    _attach(&lvl_obj, item);
	    cur = (struct object *)item->l_data;

            // Put it somewhere
	    rm = rnd_room();

            rnd_pos(&rooms[rm], &tp);

            char temp;
            if(mvwinch(mw, tp.y, tp.x) == ' ') {
                temp = mvwinch(stdscr, tp.y, tp.x);
            }
            else {
                temp = winch(mw);
            }

            while(temp != FLOOR) {
                rnd_pos(&rooms[rm], &tp);

                if(mvwinch(mw, tp.y, tp.x) == ' ') {
                    temp = mvwinch(stdscr, tp.y, tp.x);
                }
                else {
                    temp = winch(mw);
                }
            }

	    mvaddch(tp.y, tp.x, cur->o_type);
	    cur->o_pos = tp;

            /* TC_DEBUG: Start */
            FILE *output;
            output = fopen("debug.txt", "a+");
            print_object(cur, output);
            fclose(output);
            /* TC_DEBUG: Finish */
	}
    }

    // If he is really deep in the dungeon and he hasn't found the
    // amulet yet, put it somewhere on the ground
    if((level > 25) && !amulet) {
	item = new_item(sizeof *cur);
	_attach(&lvl_obj, item);
	cur = (struct object *)item->l_data;
	cur->o_hplus = cur->o_dplus = 0;
	cur->o_damage = cur->o_hurldmg = "0d0";
	cur->o_ac = 11;
	cur->o_type = AMULET;

        // Put it somewhere
	rm = rnd_room();

        rnd_pos(&rooms[rm], &tp);

        char temp;
        if(mvwinch(mw, tp.y, tp.x) == ' ') {
            temp = mvwinch(stdscr, tp.y, tp.x);
        }
        else {
            temp = winch(mw);
        }

        while(temp != FLOOR) {
            rnd_pos(&rooms[rm], &tp);

            if(mvwinch(mw, tp.y, tp.x) == ' ') {
                temp = mvwinch(stdscr, tp.y, tp.x);
            }
            else {
                temp = winch(mw);
            }
        }

	mvaddch(tp.y, tp.x, cur->o_type);
	cur->o_pos = tp;
    }

    return 0;
}
