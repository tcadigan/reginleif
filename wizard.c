// Special wizard commands (some of which are also non-wizard
// commands under strange circumstances)
//
// @(#)wizard.c 3.8 (Berkeley) 6/3/81

#define _XOPEN_SOURCE 700

#include "wizard.h"

#include "io.h"
#include "list.h"
#include "machdep.h"
#include "main.h"
#include "move.h"
#include "newlevel.h"
#include "pack.h"
#include "rooms.h"
#include "sticks.h"
#include "things.h"
#include "weapons.h"

#include <ctype.h>
#include <unistd.h>

// whatis:
//     What a certain object is
int whatis()
{
    struct object *obj;
    struct linked_list *item;

    item = get_item("identify", 0);
    
    if(item == NULL) {
	return 0;
    }
    
    obj = (struct object *)item->l_data;
    switch(obj->o_type) {
    case SCROLL:
        s_know[obj->o_which] = TRUE;
        if(s_guess[obj->o_which]) {
            free(s_guess[obj->o_which]);
            s_guess[obj->o_which] = NULL;
        }
        break;
    case POTION:
        p_know[obj->o_which] = TRUE;
        if(p_guess[obj->o_which]) {
            free(p_guess[obj->o_which]);
            p_guess[obj->o_which] = NULL;
        }
        break;
    case STICK:
        ws_know[obj->o_which] = TRUE;
        obj->o_flags |= ISKNOW;
        if(ws_guess[obj->o_which]) {
            free(ws_guess[obj->o_which]);
            ws_guess[obj->o_which] = NULL;
        }
        break;
    case WEAPON:
    case ARMOR:
        obj->o_flags |= ISKNOW;
        break;
    case RING:
        r_know[obj->o_which] = TRUE;
        obj->o_flags |= ISKNOW;
        if(r_guess[obj->o_which]) {
            free(r_guess[obj->o_which]);
            r_guess[obj->o_which] = NULL;
        }
    }
    
    msg("%s", inv_name(obj, FALSE));

    return 0;
}

// crete_obj:
//     Wizard command for getting anything he wants
int create_obj()
{
    struct linked_list *item;
    struct object *obj;
    char ch;
    char bless;

    item = new_item(sizeof *obj);
    obj = (struct object *)item->l_data;
    msg("Type of item: ");
    obj->o_type = readchar();
    mpos = 0;
    msg("Which %c do you want? (0-f)", obj->o_type);
    ch = readchar();
    
    if(isdigit(ch)) {
        obj->o_which = ch - '0';
    }
    else {
        obj->o_which = ch - 'a' + 10;
    }
    
    obj->o_group = 0;
    obj->o_count = 1;
    mpos = 0;
    if((obj->o_type == WEAPON) || (obj->o_type == ARMOR)) {
	msg("Blessing? (+,-,n)");
	bless = readchar();
	mpos = 0;
	if(bless == '-') {
	    obj->o_flags |= ISCURSED;
        }
	if(obj->o_type == WEAPON) {
	    init_weapon(obj, obj->o_which);
	    if(bless == '-') {
		obj->o_hplus -= (rnd(3) + 1);
            }
	    if(bless == '+') {
		obj->o_hplus += (rnd(3) + 1);
            }
	}
	else {
	    obj->o_ac = armors[obj->o_which].o_ac;
	    if(bless == '-') {
		obj->o_ac += (rnd(3) + 1);
            }
	    if (bless == '+') {
		obj->o_ac -= (rnd(3) + 1);
            }
	}
    }
    else if(obj->o_type == RING) {
	switch (obj->o_which) {
        case R_PROTECT:
        case R_ADDSTR:
        case R_ADDHIT:
        case R_ADDDAM:
            msg("Blessing? (+,-,n)");
            bless = readchar();
            mpos = 0;
            if(bless == '-') {
                obj->o_flags |= ISCURSED;
            }
            
            if(bless == '-') {
                obj->o_ac = -1;
            }
            else {
                obj->o_ac = rnd(2) + 1;
            }
	}
    }
    else if(obj->o_type == STICK) {
	fix_stick(obj);
    }
    
    add_pack(item, FALSE);

    return 0;
}

// teleport:
//     Bamf the hero someplace else
int teleport()
{
    int rm;
    coord c;

    c = player.t_pos;
    mvwaddch(cw, player.t_pos.y, player.t_pos.x, mvwinch(stdscr, player.t_pos.y, player.t_pos.x));

    rm = rnd_room();
    rnd_pos(&rooms[rm], &player.t_pos);

    char temp;
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
    
    light(&c);
    light(&player.t_pos);
    mvwaddch(cw, player.t_pos.y, player.t_pos.x, PLAYER);

    // Turn off ISHELD in case teleportation was done while
    // fighting a Fungi
    if((player.t_flags & ISHELD) != 0) {
	player.t_flags &= ~ISHELD;
	fung_hit = 0;
	strcpy(monsters['F'-'A'].t_stats.s_dmg, "000d0");
    }
    
    count = 0;
    running = FALSE;

    // Flush typeahead
    flushinp();
    
    return rm;
}

// passwd:
//     See if user knows password
int passwd()
{
    char *sp;
    char c;
    char buf[80];

    msg("Wizard's Password:");
    mpos = 0;
    sp = buf;
    c = getchar();
    while((c != '\n') && (c != '\r') && (c != '\033')) {
        c= getchar();
    }
    if(sp == buf) {
	return FALSE;
    }
    *sp = '\0';
    
    return (strcmp(PASSWD, crypt(buf, "mT")) == 0);
}
