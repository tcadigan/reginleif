// Draw the connecting passages
//
// @(#)passages.c 3.4 (Berkeley) 6/15/81

#include "passages.h"

#include "io.h"
#include "main.h"

struct roomdes {
    // Possible to connect to room i?
    bool conn[MAXROOMS];
    // Connection been made to room i?
    bool isconn[MAXROOMS];
    // This room in graph already?
    bool ingraph;
};

// do_passages:
//     Draw all the passages on a level
int do_passages()
{
    struct roomdes *r1;
    struct roomdes *r2 = NULL;
    int i;
    int j;
    int roomcount;

    static struct roomdes rdes[MAXROOMS] = {
	{ { 0, 1, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 1, 0, 1, 0, 1, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 0, 1, 0, 0, 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 1, 0, 0, 0, 1, 0, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 0, 1, 0, 1, 0, 1, 0, 1, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 0, 0, 1, 0, 1, 0, 0, 0, 1 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 0, 0, 0, 1, 0, 0, 0, 1, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 0, 0, 0, 0, 1, 0, 1, 0, 1 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 0, 0, 0, 0, 0, 1, 0, 1, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
    };

    // Reinitialize room graph description
    for(r1 = rdes; r1 < &rdes[MAXROOMS]; ++r1) {
	for(j = 0; j < MAXROOMS; ++j) {
	    r1->isconn[j] = FALSE;
        }
        
	r1->ingraph = FALSE;
    }

    // Starting with one room, connect it to a random adjacent room
    // and then pick a new room to start with
    roomcount = 1;
    r1 = &rdes[rnd(MAXROOMS)];
    r1->ingraph = TRUE;

    // Find a room to connect with
    j = 0;
    for(i = 0; i < MAXROOMS; ++i) {
        if(r1->conn[i] && !rdes[i].ingraph) {
            ++j;
            
            if(rnd(j) == 0) {
                r2 = &rdes[i];
            }
        }
    }

    // If no adjacent rooms are outside the graph,
    // pick a new room to look from
    if(j == 0) {
        r1 = &rdes[rnd(MAXROOMS)];

        while(!r1->ingraph) {
            r1 = &rdes[rnd(MAXROOMS)];
        }
    }
    else {
        // Otherwise, connect new room to the graph,
        // and draw a tunnel to it
        r2->ingraph = TRUE;
        i = r1 - rdes;
        j = r2 - rdes;
        conn(i, j);
        r1->isconn[j] = TRUE;
        r2->isconn[i] = TRUE;
        ++roomcount;
    }
    
    while(roomcount < MAXROOMS) {
        // Find a room to connect with
	j = 0;
	for(i = 0; i < MAXROOMS; ++i) {
	    if (r1->conn[i] && !rdes[i].ingraph && rnd(++j) == 0) {
		r2 = &rdes[i];
            }
        }

        // If no adjacent rooms are outside the graph,
        // pick a new room to look from
	if(j == 0) {
            r1 = &rdes[rnd(MAXROOMS)];

            while(!r1->ingraph) {
                r1 = &rdes[rnd(MAXROOMS)];
            }
	}
	else {
            // Otherwise, connect new room to the graph,
            // and draw a tunnel to it

	    r2->ingraph = TRUE;
	    i = r1 - rdes;
	    j = r2 - rdes;
	    conn(i, j);
	    r1->isconn[j] = TRUE;
	    r2->isconn[i] = TRUE;
	    roomcount++;
	}
    }
    
    // Attempt to add passages to the graph a random number of times so
    // that there isn't just one unique passage through it.
    for(roomcount = rnd(5); roomcount > 0; --roomcount) {
        // A random room to look from
	r1 = &rdes[rnd(MAXROOMS)];

        // Find an adjacent room not already connected
	j = 0;
	for(i = 0; i < MAXROOMS; ++i) {
	    if(r1->conn[i] && !r1->isconn[i] && rnd(++j) == 0) {
		r2 = &rdes[i];
            }
        }

        // If there is on, connect it and look for the next added passage
	if(j != 0) {
	    i = r1 - rdes;
	    j = r2 - rdes;
	    conn(i, j);
	    r1->isconn[j] = TRUE;
	    r2->isconn[i] = TRUE;
	}
    }

    return 0;
}

// conn:
//     Draw a corridor from a room in a certain direction
int conn(int r1, int r2)
{
    struct room *rpf;
    struct room *rpt;
    char rmt;
    int distance;
    int turn_spot;
    int turn_distance;
    int rm;
    char direc;
    coord delta;
    coord curr;
    coord turn_delta;
    coord spos;
    coord epos;

    if(r1 < r2) {
	rm = r1;
	if((r1 + 1) == r2) {
	    direc = 'r';
        }
	else {
	    direc = 'd';
        }
    }
    else {
	rm = r2;
	if((r2 + 1) == r1) {
	    direc = 'r';
        }
	else {
	    direc = 'd';
        }
    }
    rpf = &rooms[rm];

    // Set up the movement variables, in two cases:
    // first drawing one down
    if(direc == 'd') {
        // Room # of dest
	rmt = rm + 3;
        // Room pointer of dest
	rpt = &rooms[(int)rmt];
        // Direction of move
	delta.x = 0;
	delta.y = 1;
        // Start of move
	spos.x = rpf->r_pos.x;
	spos.y = rpf->r_pos.y;
        // End of move
	epos.x = rpt->r_pos.x;
	epos.y = rpt->r_pos.y;
        // If not code pick door pos
	if(!(rpf->r_flags & ISGONE)) {
	    spos.x += (rnd(rpf->r_max.x - 2) + 1);
	    spos.y += (rpf->r_max.y - 1);
	}
	if(!(rpt->r_flags & ISGONE)) {
	    epos.x += (rnd(rpt->r_max.x - 2) + 1);
        }
        // Distance to move
	distance = abs(spos.y - epos.y) - 1;
        // Direction to turn
	turn_delta.y = 0;

        if(spos.x < epos.x) {
            turn_delta.x = 1;
        }
        else {
            turn_delta.x = -1;
        }

        // How far to turn
	turn_distance = abs(spos.x - epos.x);
        // Where turn starts
	turn_spot = rnd(distance - 1) + 1;
    }
    else if (direc == 'r') {
        // Setup for moving right

	rmt = rm + 1;
	rpt = &rooms[(int)rmt];
	delta.x = 1;
	delta.y = 0;
	spos.x = rpf->r_pos.x;
	spos.y = rpf->r_pos.y;
	epos.x = rpt->r_pos.x;
	epos.y = rpt->r_pos.y;
	if(!(rpf->r_flags & ISGONE)) {
	    spos.x += (rpf->r_max.x - 1);
            spos.y += (rnd(rpf->r_max.y - 2) + 1);
        }
	if(!(rpt->r_flags & ISGONE)) {
	    epos.y += (rnd(rpt->r_max.y - 2) + 1);
        }
	distance = abs(spos.x - epos.x) - 1;

        if(spos.y < epos.y) {
            turn_delta.y = 1;
        }
        else {
            turn_delta.y = -1;
        }

	turn_delta.x = 0;
	turn_distance = abs(spos.y - epos.y);
	turn_spot = rnd(distance - 1) + 1;
    }
    else {
        if(wizard) {
            msg("error in connection tables");
        }
    }

    // Draw in the doors on either side of the passage or
    // just put #'s if the rooms are gone
    if(!(rpf->r_flags & ISGONE)) {
        door(rpf, &spos);
    }
    else {
        move(spos.y, spos.x);
	addch('#');
    }
    
    if(!(rpt->r_flags & ISGONE)) {
        door(rpt, &epos);
    }
    else {
        move(epos.y, epos.x);
	addch('#');
    }

    // Get ready to move...
    curr.x = spos.x;
    curr.y = spos.y;
    while(distance) {
        // Move to new position
	curr.x += delta.x;
	curr.y += delta.y;
        // Check if we are at the turn place, if so do the turn
	if((distance == turn_spot) && (turn_distance > 0)) {
	    while(turn_distance) {
                move(curr.y, curr.x);
		addch(PASSAGE);
		curr.x += turn_delta.x;
		curr.y += turn_delta.y;
                --turn_distance;
	    }
        }

        // Continue digging along
        move(curr.y, curr.x);
	addch(PASSAGE);
	--distance;
    }
    curr.x += delta.x;
    curr.y += delta.y;
    
    if(!((curr.x == epos.x) && (curr.y == epos.y))) {
	msg("Warning, connectivity problem on this level.");
    }

    return 0;
}

// door:
//     Add a door or possibly a secret door.
//     Also enters the door in the exits array of the room
int door(struct room *rm, coord *cp)
{
    move(cp->y, cp->x);

    if((rnd(10) < (level - 1)) && (rnd(100) < 20)) {
        addch(SECRETDOOR);
    }
    else {
        addch(DOOR);
    }
    
    rm->r_exit[rm->r_nexits++] = *cp;

    return 0;
}

// add_pass:
//     Add the passages to the current window (wizard command)
int add_pass()
{
    int y;
    int x;
    int ch;

    for(y = 1; y < LINES - 2; ++y) {
	for (x = 0; x < COLS; ++x) {
            ch = mvinch(y, x);
	    if((ch == PASSAGE) || (ch == DOOR) || (ch == SECRETDOOR)) {
		mvwaddch(cw, y, x, ch);
            }
        }
    }

    return 0;
}
