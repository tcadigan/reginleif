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
	addmsg("You are already wearing some", 0);
	if(!terse) {
	    addmsg(".  You'll have to take it off first", 0);
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
	msg("You can't wear that.", 0);
	return 0;
    }
    waste_time();
    if(!terse) {
	addmsg("You are now w", 0);
    }
    else {
	addmsg("W", 0);
    }
    msg("earing %s.", a_names[obj->o_which]);
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
            msg("Not wearing armor", 0);
        }
	else {
            msg("You aren't wearing any armor", 0);
        }
	return 0;
    }
    if(!dropcheck(cur_armor)) {
	return 0;
    }
    cur_armor = NULL;
    if(terse) {
	addmsg("Was", 0);
    }
    else {
	addmsg("You used to be ", 0);
    }
    int args[] = { pack_char(obj) };
    addmsg(" wearing %c) ", args);
    addmsg("%s", inv_name(obj, TRUE));

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
