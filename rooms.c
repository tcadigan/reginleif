// Draw the nine rooms on the screen
//
// @(#)rooms.c 3.8 (Berkeley) 6/15/81

#include "rooms.h"

#include "chase.h"
#include "list.h"
#include "main.h"
#include "monsters.h"
#include "newlevel.h"
#include "things.h"

// do_rooms:
//     Something...
int do_rooms()
{
    int i;
    struct room *rp;
    struct linked_list *item;
    struct thing *tp;
    int left_out;
    coord top;
    coord bsze;
    coord mp;

    // bsze is the maximum room size
    bsze.x = COLS / 3;
    bsze.y = LINES / 3;

    // Clear things for a new level
    for(rp = rooms; rp < &rooms[MAXROOMS]; ++rp) {
        rp->r_flags = 0;
        rp->r_nexits = 0;
        rp->r_goldval = 0;
    }

    // Put the gone rooms, if any, on the level
    left_out = rnd(4);
    for(i = 0; i < left_out; ++i) {
	rooms[rnd_room()].r_flags |= ISGONE;
    }

    // Dig and populate all the rooms on the level
    for(i = 0, rp = rooms; i < MAXROOMS; rp++, ++i) {
        // Find the upper left corner of box that this room goes in
	top.x = ((i % 3) * bsze.x) + 1;
	top.y = (i / 3 ) * bsze.y;
	if(rp->r_flags & ISGONE) {
            // Place a gone room. Make certain that there is a blank line
            // for passage drawing.

            rp->r_pos.x = top.x + rnd(bsze.x - 2) + 1;
            rp->r_pos.y = top.y + rnd(bsze.y - 2) + 1;
            rp->r_max.x = -COLS;
            rp->r_max.x = -LINES;

            while(!((rp->r_pos.y > 0) && (rp->r_pos.y < (LINES - 1)))) {
                rp->r_pos.x = top.x + rnd(bsze.x - 2) + 1;
                rp->r_pos.y = top.y + rnd(bsze.y - 2) + 1;
                rp->r_max.x = -COLS;
                rp->r_max.x = -LINES;
            }

	    continue;
	}
	if(rnd(10) < (level - 1)) {
	    rp->r_flags |= ISDARK;
        }

        // Find a place and size for a random room
        rp->r_max.x = rnd(bsze.x - 4) + 4;
        rp->r_max.y = rnd(bsze.y - 4) + 4;
        rp->r_pos.x = top.x + rnd(bsze.x - rp->r_max.x);
        rp->r_pos.y = top.y + rnd(bsze.y - rp->r_max.y);

        while(rp->r_pos.y == 0) {
            rp->r_max.x = rnd(bsze.x - 4) + 4;
            rp->r_max.y = rnd(bsze.y - 4) + 4;
            rp->r_pos.x = top.x + rnd(bsze.x - rp->r_max.x);
            rp->r_pos.y = top.y + rnd(bsze.y - rp->r_max.y);
        }

        // Put the gold in
	if((rnd(100) < 50) && (!amulet || (level >= max_level))) {
            rp->r_goldval = (rnd(50 + (10 * level)) + 2);

	    rnd_pos(rp, &rp->r_gold);
	    if(roomin(&rp->r_gold) != rp) {
		endwin();
                abort();
            }
	}
        
	draw_room(rp);

        // Put the monsters in
        if(rp->r_goldval > 0) {
            if(rnd(100) < 80) {
                item = new_item(sizeof *tp);
                tp = (struct thing *)item->l_data;
                
                rnd_pos(rp, &mp);
                
                while(mvwinch(stdscr, mp.y, mp.x) != FLOOR) {
                    rnd_pos(rp, &mp);
                }
                
                new_monster(item, randmonster(FALSE), &mp);
                // See if we want to give it treasure to carry around.
                if (rnd(100) < monsters[tp->t_type - 'A'].m_carry) {
                    _attach(&tp->t_pack, new_thing());
                }
            }
        }
        else {
            if(rnd(100) < 25) {
                item = new_item(sizeof *tp);
                tp = (struct thing *)item->l_data;
                
                rnd_pos(rp, &mp);
                
                while(mvwinch(stdscr, mp.y, mp.x) != FLOOR) {
                    rnd_pos(rp, &mp);
                }
                
                new_monster(item, randmonster(FALSE), &mp);
                // See if we want to give it a treasure to carry around.
                if (rnd(100) < monsters[tp->t_type - 'A'].m_carry) {
                    _attach(&tp->t_pack, new_thing());
                }
            }
        }
    }

    return 0;
}

// draw_room:
//     Draw a box around a room
int draw_room(struct room *rp)
{
    int j;
    int k;

    move(rp->r_pos.y, rp->r_pos.x + 1);
    // Draw the left side
    vert(rp->r_max.y - 2);
    move(rp->r_pos.y + rp->r_max.y - 1, rp->r_pos.x);
    // Draw bottom
    horiz(rp->r_max.x);
    move(rp->r_pos.y, rp->r_pos.x);
    // Draw top
    horiz(rp->r_max.x);
    // Draw right side
    vert(rp->r_max.y - 2);

    // Put the floor down
    for(j = 1; j < rp->r_max.y-1; ++j) {
	move(rp->r_pos.y + j, rp->r_pos.x + 1);
	for(k = 1; k < rp->r_max.x-1; ++k) {
	    addch(FLOOR);
        }
    }

    // Put the gold there
    if(rp->r_goldval) {
	mvaddch(rp->r_gold.y, rp->r_gold.x, GOLD);
    }

    return 0;
}

// horiz:
//     Draw a horizontal line
int horiz(int cnt)
{
    while(cnt--) {
	addch('-');
    }

    return 0;
}

// vert:
//     Draw a vertical line
int vert(int cnt)
{
    int x;
    int y;

    getyx(stdscr, y, x);
    x--;
    
    while(cnt--) {
	move(++y, x);
	addch('|');
    }

    return 0;
}

// rnd_pos:
//     Pick a random spot in a room
int rnd_pos(struct room *rp, coord *cp)
{
    cp->x = rp->r_pos.x + rnd(rp->r_max.x - 2) + 1;
    cp->y = rp->r_pos.y + rnd(rp->r_max.y - 2) + 1;

    return 0;
}
