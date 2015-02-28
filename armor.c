// This file contains misc. functions for dealing with armor
// @(#)armor.c  3.9 (Berkeley) 6/15/81

#include "armor.h"

#include "daemon.h"
#include "io.h"
#include "pack.h"
#include "things.h"

#include "rogue.h"

#include <ncurses.h>

// wear:
//     The player wants to wear something, so let him/her put it on.
int wear()
{
    struct linked_list *item;
    struct object *obj;

    if(cur_armor != NULL) {
	addmsg("You are already wearing some");
	if(!terse) {
	    addmsg(". You'll have to take it off first");
        }
	endmsg();
	after = FALSE;
	return 0;
    }
    item = get_item("wear", ARMOR);
    if(item == NULL) {
	return 0;
    }
    obj = (struct object *)item->l_data;
    if(obj->o_type != ARMOR) {
	msg("You can't wear that.");
	return 0;
    }
    waste_time();
    if(!terse) {
	addmsg("You are now w");
    }
    else {
	addmsg("W");
    }
    msg("earing %s.", armors[obj->o_which].a_name);
    cur_armor = obj;
    obj->o_flags |= ISKNOW;

    return 0;
}

// take_off:
//     Get the armor off of the player's back
int take_off()
{
    struct object *obj;

    obj = cur_armor;
    if(obj == NULL) {
	if(terse) {
            msg("Not wearing armor");
        }
	else {
            msg("You aren't wearing any armor");
        }
	return 0;
    }
    if(!dropcheck(cur_armor)) {
	return 0;
    }
    cur_armor = NULL;
    if(terse) {
	addmsg("Was");
    }
    else {
	addmsg("You used to be");
    }
    addmsg(" wearing %c) %s", pack_char(obj), inv_name(obj, TRUE));
    endmsg();
    
    return 0;
}

// waste_time:
//     Do nothing but let other things happen
int waste_time()
{
    do_daemons(BEFORE);
    do_fuses(BEFORE);
    do_daemons(AFTER);
    do_fuses(AFTER);

    return 0;
}
