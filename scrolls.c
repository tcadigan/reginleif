// Read a scroll and let it happen
//
// @(#)scrolls.c 3.5 (Berkeley) 6/15/81
#include "scrolls.h"

#include "chase.h"
#include "io.h"
#include "list.h"
#include "main.h"
#include "misc.h"
#include "monsters.h"
#include "move.h"
#include "options.h"
#include "pack.h"
#include "wizard.h"

#include <ctype.h>

// read_scroll:
//     Something...
int read_scroll()
{
    struct object *obj;
    struct linked_list *item;
    struct room *rp;
    int i;
    int j;
    char ch;
    char nch;
    struct linked_list *titem;
    char buf[80];

    item = get_item("read", SCROLL);
    if(item == NULL) {
	return 0;
    }
    obj = (struct object *)item->l_data;
    if(obj->o_type != SCROLL) {
	if(!terse) {
	    msg("There is nothing on it to read");
        }
	else {
	    msg("Nothing to read");
        }
        
	return 0;
    }
    msg("As you read the scroll, it vanishes.");

    // Calculate the effect is has on the poor guy
    if(obj == cur_weapon) {
	cur_weapon = NULL;
    }
    
    switch(obj->o_which) {
    case S_CONFUSE:
        // Scroll of monster confusion. Give him that power.
        msg("Your hands begin to glow red");
        player.t_flags |= CANHUH;
        break;
    case S_LIGHT:
        s_know[S_LIGHT] = TRUE;
        rp = roomin(&player.t_pos);
        
        if(rp == NULL) {
            msg("The corridor glows and then fades");
        }
        else {
            addmsg("The room is lit");
            if(!terse) {
                addmsg(" by a shimmering blue light.");
            }

            endmsg();
            rp->r_flags &= ~ISDARK;

            // Light the room and put the player back up
            light(&player.t_pos);
            mvwaddch(cw, player.t_pos.y, player.t_pos.x, PLAYER);
        }
        break;
    case S_ARMOR:
        if(cur_armor != NULL) {
	    msg("Your armor glows faintly for a moment");
	    --cur_armor->o_ac;
	    cur_armor->o_flags &= ~ISCURSED;
        }
        break;
    case S_HOLD:
        // Hold monster scroll. Stop all monster within two spaces from
        // cahsing after the hero.
        {
            int x;
            int y;
            struct linked_list *mon;
            
            for(x = player.t_pos.x - 2; x <= player.t_pos.x + 2; ++x) {
                for(y = player.t_pos.y - 2; y <= player.t_pos.y + 2; ++y) {
                    if((y > 0) && (x > 0) && isupper(mvwinch(mw, y, x))) {
                        mon = find_mons(y, x);
                        if(mon != NULL) {
                            struct thing *th;
                            
                            th = (struct thing *)mon->l_data;
                            th->t_flags &= ~ISRUN;
                            th->t_flags |= ISHELD;
                        }
                    }
                }
	    }
        }
        break;
    case S_SLEEP:
        // Scroll which makes you fall asleep
        s_know[S_SLEEP] = TRUE;
        msg("You fall asleep.");
        no_command += (4 + rnd(SLEEPTIME));
        break;
    case S_CREATE:
        // Create a monster. First look in a circle around him, next try his
        // room otherwise give up
        {
            int x;
            int y;
            bool appear = 0;
            struct coord mp;

            // Search for an open place
            for(y = player.t_pos.y; y <= player.t_pos.y + 1; ++y) {
                for(x = player.t_pos.x; x <= player.t_pos.x + 1; ++x) {
                    // Don't put a monster on top of the player.
                    if((y == player.t_pos.y) && (x == player.t_pos.x)) {
                        continue;
                    }

                    // Or anything else nasty
                    char temp;
                    if(mvwinch(mw, y, x) == ' ') {
                        temp = mvwinch(stdscr, y, x) & A_CHARTEXT;
                    }
                    else {
                        temp = winch(mw);
                    }
                        
                    if(step_ok(temp)) {
                        ++appear;
                        if(rnd(appear) == 0) {
                            mp.y = y;
                            mp.x = x;
                        }
                    }
                }
            }
                
            if(appear) {
                titem = new_item(sizeof (struct thing));
                new_monster(titem, randmonster(FALSE), &mp);
            }
            else {
                msg("You hear a faint cry of anguish in the distance.");
            }
        }
        break;
    case S_IDENT:
        // Identify, let the rogue figure something out
        msg("This scroll is an identify scroll");
        s_know[S_IDENT] = TRUE;
        whatis();
        break;
    case S_MAP:
        // Scroll of magic mapping
        s_know[S_MAP] = TRUE;
        msg("Oh, now this scroll has a map on it.");
        overwrite(stdscr, hw);

        // Take all the things we want to keep hidden out of the window
        for(i = 0; i < LINES; ++i) {
            for(j = 0; j < COLS; ++j) {
                ch = mvwinch(hw, i, j) & A_CHARTEXT;
                nch = ch;
                switch(nch) {
                case SECRETDOOR:
                    mvaddch(i, j, nch = DOOR);
                case '-':
                case '|':
                case DOOR:
                case PASSAGE:
                case ' ':
                case STAIRS:
                    if(mvwinch(mw, i, j) != ' ') {
                        struct thing *it;
                        
                        it = (struct thing *)find_mons(i, j)->l_data;
                        if(it->t_oldch == ' ') {
                            it->t_oldch = nch;
                        }
                    }
                    break;
                default:
                    nch = ' ';
                }
                
                if(nch != ch) {
                    waddch(hw, nch);
		}
            }
        }

        // Copy in what he has discovered
        overlay(cw, hw);

        // And set up for display
        overwrite(hw, cw);
        break;
    case S_GFIND:
        // Potion of gold detection
        {
            int gtotal = 0;
            
            wclear(hw);
            for(i = 0; i < MAXROOMS; ++i) {
                gtotal += rooms[i].r_goldval;
                if((rooms[i].r_goldval != 0)
                   && (mvwinch(stdscr, rooms[i].r_gold.y, rooms[i].r_gold.x) == GOLD)) {
                    mvwaddch(hw,rooms[i].r_gold.y,rooms[i].r_gold.x,GOLD);
                }
            }
            
            if(gtotal) {
                s_know[S_GFIND] = TRUE;
                show_win(hw,
                         "You begin to feel greedy and you sense gold.--More--");
            }
            else {
                msg("You begin to feel a pull downward");
            }
        }
        break;
    case S_TELEP:
        // Scroll of teleportation, make him disappear and reappear
        {
            int rm;
            struct room *cur_room;
            
            cur_room = roomin(&player.t_pos);
            rm = teleport();
            if(cur_room != &rooms[rm]) {
                s_know[S_TELEP] = TRUE;
            }
        }
        break;
    case S_ENCH:
        if(cur_weapon == NULL) {
            msg("You feel a strange sense of loss.");
        }
        else {
            cur_weapon->o_flags &= ~ISCURSED;
            if(rnd(100) > 50) {
                ++cur_weapon->o_hplus;
            }
            else {
                ++cur_weapon->o_dplus;
            }
            msg("Your %s glows blue for a moment.", w_names[cur_weapon->o_which]);
        }
        break;
    case S_SCARE:
        // A monster will refuse to step on a scare monster scroll
        // if it is dropped. Thus reading it is a mistake and produces
        // laughter at the poor rogue's boo boo.
        msg("You hear maniacal laughter in the distance.");
        break;
    case S_REMOVE:
        if(cur_armor != NULL) {
            cur_armor->o_flags &= ~ISCURSED;
        }
        
        if(cur_weapon != NULL) {
            cur_weapon->o_flags &= ~ISCURSED;
        }
        
        if(cur_ring[LEFT] != NULL) {
            cur_ring[LEFT]->o_flags &= ~ISCURSED;
        }
        
        if(cur_ring[RIGHT] != NULL) {
            cur_ring[RIGHT]->o_flags &= ~ISCURSED;
        }
        
        msg("You feel as if somebody is watching over you.");
        break;
    case S_AGGR:
        // This scroll aggravates all the monsters on the current
        // level and set them running towards the hero
        aggravate();
        msg("You hear a high pitched humming noise.");
        break;
    case S_NOP:
        msg("This scroll seems to be blank.");
        break;
    case S_GENOCIDE:
        msg("You have been granted the boon of genocide");
        genocide();
        s_know[S_GENOCIDE] = TRUE;
        break;
    default:
        msg("What a puzzling scroll!");
        return 0;
    }

    look(TRUE);	/* put the result of the scroll on the screen */
    status();
    
    if(s_know[obj->o_which] && s_guess[obj->o_which]) {
	free(s_guess[obj->o_which]);
	s_guess[obj->o_which] = NULL;
    }
    else if(!s_know[obj->o_which] && askme && (s_guess[obj->o_which] == NULL)) {
        if(terse) {
            msg("Call it: ");
        }
        else {
            msg("What do you want to call it? ");
        }

	if(get_str(buf, cw) == NORM) {
	    s_guess[obj->o_which] = malloc((unsigned int) strlen(buf) + 1);
	    strcpy(s_guess[obj->o_which], buf);
	}
    }
    /*
     * Get rid of the thing
     */
    --inpack;

    if(obj->o_count > 1) {
	--obj->o_count;
    }
    else {
	_detach(&player.t_pack, item);
        discard(item);
    }

    return 0;
}
