// Routines to deal with the pack
//
// @(#)pack.c 3.6 (Berkeley) 6/15/81
#include "pack.h"

#include "chase.h"
#include "io.h"
#include "list.h"
#include "misc.h"
#include "things.h"

#include <sgtty.h>

// add_pack:
//     Pick up an object and add it to the pack. If the argument is non-null
//     use it as the linked_list pointer instead of getting it off the ground.
int add_pack(struct linked_list *item, bool silent)
{
    struct linked_list *ip;
    struct linked_list *lp;
    struct object *obj;
    struct object *op;
    bool exact;
    bool from_floor;

    if(item == NULL) {
	from_floor = TRUE;
        item = find_obj(player.t_pos.y, player.t_pos.x);

        if(item == NULL) {
	    return 0;
        }
    }
    else {
	from_floor = FALSE;
    }
    
    obj = (struct object *)item->l_data;

    // Link it into the pack. Seach the pack for an object of similar type
    // if there isn't one, stuff it at the beginning, if there is, look for one
    // that is exactly the same and just increment the count if there is.
    // Food is always put at the beginning for ease of access, but is not
    // ordered so that you can't tell good from bad. First check to see if there
    // is something in the same group and if there is then increment the count.
    if(obj->o_group) {
	for(ip = player.t_pack; ip != NULL; ip = ip->l_next) {
	    op = (struct object *)ip->l_data;
	    if (op->o_group == obj->o_group) {
                // Put it in the pack and notify the user
		++op->o_count;
		if(from_floor){
		    _detach(&lvl_obj, item);
                    
                    if(roomin(&player.t_pos)) {
                        mvaddch(player.t_pos.y, player.t_pos.x, PASSAGE);
                    }
                    else {
                        mvaddch(player.t_pos.y, player.t_pos.x, FLOOR);
                    }
		}
		discard(item);
		item = ip;

                // Notify the user
                obj = (struct object *)item->l_data;
                if(notify && !silent) {
                    if(!terse) {
                        addmsg("You now have ", 0);
                    }

                    msg("%s ", inv_name(obj, !terse));
                    int args[] = { pack_char(obj) };
                    msg("(%c)", args);
                }

                if(obj->o_type == AMULET) {
                    amulet = TRUE;
                }

                return 0;
	    }
	}
    }

    // Check if there is room
    if(inpack == MAXPACK - 1) {
	msg("You can't carry anything else.", 0);
	return 0;
    }

    // Check for and deal with scare monster scrolls
    if(obj->o_type == SCROLL && obj->o_which == S_SCARE) {
	if(obj->o_flags & ISFOUND) {
	    msg("The scroll turns to dust as you pick it up.", 0);
	    _detach(&lvl_obj, item);
	    mvaddch(player.t_pos.y, player.t_pos.x, FLOOR);

	    return 0;
	}
	else {
	    obj->o_flags |= ISFOUND;
        }
    }

    ++inpack;
    if(from_floor) {
	_detach(&lvl_obj, item);

        if(roomin(&player.t_pos)) {
            mvaddch(player.t_pos.y, player.t_pos.x, PASSAGE);
        }
        else {
            mvaddch(player.t_pos.y, player.t_pos.x, FLOOR);
        }
    }

    // Search for an object of the same type
    exact = FALSE;
    for(ip = player.t_pack; ip != NULL; ip = ip->l_next) {
	op = (struct object *)ip->l_data;
	if(obj->o_type == op->o_type) {
	    break;
        }
    }
    if(ip == NULL) {
        // Put it at the end of the pack since it is a new type
	for(ip = player.t_pack; ip != NULL; ip = ip->l_next) {
	    op = (struct object *)ip->l_data;
	    if (op->o_type != FOOD)
		break;
	    lp = ip;
	}
    }
    else {
        // Search for an object which is exactly the same
	while((ip != NULL) && (op->o_type == obj->o_type)) {
	    if(op->o_which == obj->o_which) {
		exact = TRUE;
		break;
	    }
	    lp = ip;
            ip = ip->l_next;
	    if(ip == NULL) {
		break;
            }
	    op = (struct object *)ip->l_data;
	}
    }
    if(ip == NULL) {
        // Didn't find an exact match, just stick it here
	if(player.t_pack == NULL) {
	    player.t_pack = item;
        }
	else {
	    lp->l_next = item;
	    item->l_prev = lp;
	    item->l_next = NULL;
	}
    }
    else {
        // If we found an exact mathc. If it is a potion, food, or a
        // scroll, increase the count, otherwise put it with its clones
        if(exact && ((obj->o_type == POTION) || (obj->o_type == SCROLL) || (obj->o_type == FOOD))) {
	    ++op->o_count;
	    discard(item);
	    item = ip;
	}
        else {
            item->l_prev = ip->l_prev;
            
            if(item->l_prev != NULL) {
                item->l_prev->l_next = item;
            }
            else {
                player.t_pack = item;
            }
            
            item->l_next = ip;
            ip->l_prev = item;
        }
    }
    
    // Notify the user
    obj = (struct object *)item->l_data;
    if(notify && !silent) {
	if(!terse) {
	    addmsg("You now have ", 0);
        }

        msg("%s ", inv_name(obj, !terse));
        int args[] = { pack_char(obj) };
	msg("(%c)", args);
    }
    
    if(obj->o_type == AMULET) {
	amulet = TRUE;
    }
 
    return 0;
}

// inventory:
//     List what is in the pack
int inventory(struct linked_list *list, int type)
{
    struct object *obj;
    char ch;
    int n_objs;
    char inv_temp[80];

    n_objs = 0;
    for(ch = 'a'; list != NULL; ch++, list = list->l_next) {
	obj = (struct object *)list->l_data;
	if(type
           && (type != obj->o_type)
           && !((type == CALLABLE)
                && ((obj->o_type == SCROLL)
                    || (obj->o_type == POTION)
                    || (obj->o_type == RING)
                    || (obj->o_type == STICK)))) {
		continue;
        }
        
	switch(n_objs++) {
        case 0:
            // For the first thing in the inventory, just save the
            // string in case there is only one.
            sprintf(inv_temp, "%c) %s", ch, inv_name(obj, FALSE));
            break;
        case 1:
            // If there is more than one, clear the screen,
            // print the saved message and fall through to...
            if(slow_invent) {
                msg("%s", inv_temp);
            }
            else {
                wclear(hw);
                waddstr(hw, inv_temp);
                waddch(hw, '\n');
            }
        default:
            // Print the line for this object
            if(slow_invent) {
                int args[] = { ch };
                msg("%c) ", args);
                msg("%s", inv_name(obj, FALSE));
            }
            else {
                wprintw(hw, "%c) %s\n", ch, inv_name(obj, FALSE));
            }
	}
    }
    
    if(n_objs == 0) {
	if(terse) {
            if(type == 0) {
                msg("Empty handed.", 0);
            }
            else {
                msg("Nothing appropriate", 0);
            }
        }
	else {
            if(type == 0) {
                msg("You are empty handed.", 0);
            }
            else {
                msg("You don't have anything appropriate", 0);
            }
        }
        
	return FALSE;
    }
    
    if(n_objs == 1) {
	msg("%s", inv_temp);
        
	return TRUE;
    }
    
    if(!slow_invent) {
	mvwaddstr(hw, LINES-1, 0, "--Press space to continue--");
        wrefresh(hw);
	wait_for(' ');
	clearok(cw, TRUE);
	touchwin(cw);
    }

    return TRUE;
}

// pick_up:
//     Add something to the character's pack
int pick_up(char ch)
{
    switch(ch) {
    case GOLD:
        money();
        break;
    default:
        if(wizard) {
            msg("Where did you pick that up???", 0);
        }
    case ARMOR:
    case POTION:
    case FOOD:
    case WEAPON:
    case SCROLL:	
    case AMULET:
    case RING:
    case STICK:
        add_pack(NULL, FALSE);
        break;
    }
    
    return 0;
}

// pick_inven:
//     Allow player to inventory a single item
int picky_inven()
{
    struct linked_list *item;
    char ch;
    char mch;

    if(player.t_pack == NULL) {
	msg("You aren't carrying anything", 0);
    }
    else if(player.t_pack->l_next == NULL) {
	msg("a) %s", inv_name((struct object *)player.t_pack->l_data, FALSE));
    }
    else {
        if(terse) {
            msg("Item: ", 0);
        }
        else {
            msg("Which item do you wish to inventory", 0);
        }

	mpos = 0;
        mch = readchar();
	if(mch == ESCAPE) {
	    msg("", 0);
	    return 0;
	}
	for(ch = 'a', item = player.t_pack; item != NULL; item = item->l_next, ++ch) {
	    if(ch == mch) {
                int args[] = { ch };
                msg("%c) ", args);
		msg("%s", inv_name((struct object *)item->l_data, FALSE));
                
		return 0;
	    }
        }
	if(!terse) {
	    msg("'%s' not in pack", unctrl(mch));
        }
        
        int args[] = { --ch };
	msg("Range is 'a' to '%c'", args);
    }

    return 0;
}

// get_item:
//     Pick something out of a pack for a purpose
struct linked_list *get_item(char *purpose, int type)
{
    struct linked_list *obj;
    char ch;
    char och;

    if(player.t_pack == NULL) {
	msg("You aren't carrying anything.", 0);
    }
    else {
        while(1) {
	    if(!terse) {
		addmsg("Which object do you want to ", 0);
            }
	    addmsg("%s", purpose);
	    if(terse) {
		addmsg(" what", 0);
            }
	    msg("? (* for list): ", 0);
	    ch = readchar();
	    mpos = 0;

            // Give the poor player a chance to abore the command
	    if((ch == ESCAPE) || (ch == CTRL('G'))) {
		after = FALSE;
		msg("", 0);
                
		return NULL;
	    }
            
	    if(ch == '*') {
		mpos = 0;
		if(inventory(player.t_pack, type) == 0) {
		    after = FALSE;
		    return NULL;
		}
		continue;
	    }

	    for(obj = player.t_pack, och = 'a'; obj != NULL; obj = obj->l_next, ++och) {
		if(ch == och) {
		    break;
                }
            }

	    if(obj == NULL) {
                int args[] = { och - 1 };
		msg("Please specify a letter between 'a' and '%c'", args);
		continue;
	    }
	    else { 
		return obj;
            }
	}
    }
    
    return NULL;
}

// pack_char:
//     Something...
char pack_char(struct object *obj)
{
    struct linked_list *item;
    char c;

    c = 'a';
    for(item = player.t_pack; item != NULL; item = item->l_next) {
	if((struct object *)item->l_data == obj) {
	    return c;
        }
	else {
	    c++;
        }
    }
    
    return 'z';
}
