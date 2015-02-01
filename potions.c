// Functions for dealing with potions
//
// @(#)potions.c 3.1 (Berkeley) 5/7/81
#include "potions.h"

#include "daemon.h"
#include "daemons.h"
#include "fight.h"
#include "io.h"
#include "list.h"
#include "main.h"
#include "misc.h"
#include "move.h"
#include "options.h"
#include "pack.h"

// quaff:
//     Something...
int quaff()
{
    struct object *obj;
    struct linked_list *item;
    struct linked_list *titem;
    struct thing *th;
    char buf[80];

    item = get_item("quaff", POTION);

    // Make certain that it is something that we want to drink
    if(item == NULL) {
	return 0;
    }
    obj = (struct object *)item->l_data;
    if(obj->o_type != POTION) {
	if (!terse) {
	    msg("Yuk! Why would you want to drink that?", 0);
        }
	else {
	    msg("That's undrinkable", 0);
        }
        
	return 0;
    }
    if(obj == cur_weapon) {
	cur_weapon = NULL;
    }

    // Calculate the effect if has on the poor guy
    switch(obj->o_which) {
    case P_CONFUSE:
        if((player.t_flags & ISHUH) == 0) {
            msg("Wait, what's going on here. Huh? What? Who?", 0);

            if((player.t_flags & ISHUH) != 0) {
                lengthen(unconfuse, rnd(8) + HUHDURATION);
            }
            else {
                fuse(unconfuse, 0, rnd(8) + HUHDURATION, AFTER);
            }
            
            player.t_flags |= ISHUH;
        }
        
        p_know[P_CONFUSE] = TRUE;
        break;
    case P_POISON:
        if(((cur_ring[LEFT] != NULL) && (cur_ring[LEFT]->o_which == R_SUSTSTR))
           || ((cur_ring[RIGHT] != NULL) && (cur_ring[RIGHT]->o_which == R_SUSTSTR))) {
            chg_str(-(rnd(3) + 1));
            msg("You feel very sick now.", 0);
        }
        else {
            msg("You feel momentarily sick", 0);
        }
        
        p_know[P_POISON] = TRUE;
        break;
    case P_HEALING:
        player.t_stats.s_hpt += roll(player.t_stats.s_lvl, 4);
        
        if(player.t_stats.s_hpt > max_hp) {
            ++max_hp;
            player.t_stats.s_hpt = max_hp;
        }
        
        msg("You begin to feel better.", 0);
        sight();
        p_know[P_HEALING] = TRUE;
        break;
    case P_STRENGTH:
        msg("You feel stronger, now.  What bulging muscles!", 0);
        chg_str(1);
        p_know[P_STRENGTH] = TRUE;
        break;
    case P_MFIND:
        // Potion of monster detections, if there are monsters, detect them
        if(mlist != NULL) {
            wclear(hw);
            overwrite(mw, hw);
            show_win(hw,
                     "You begin to sense the presence of monsters.--More--");
            p_know[P_MFIND] = TRUE;
        }
        else {
            msg("You have a strange feeling for a moment, then it passes.", 0);
        }

        break;
    case P_TFIND:
        // Potion of magic detection. Show the potions and scrolls
        if(lvl_obj != NULL) {
            struct linked_list *mobj;
            struct object *tp;
            bool show;
            
            show = FALSE;
            wclear(hw);
            for(mobj = lvl_obj; mobj != NULL; mobj = mobj->l_next) {
                tp = (struct object *)mobj->l_data;
                if(is_magic(tp)) {
                    show = TRUE;
                    mvwaddch(hw, tp->o_pos.y, tp->o_pos.x, MAGIC);
                }
                p_know[P_TFIND] = TRUE;
            }
            
            for(titem = mlist; titem != NULL; titem = titem->l_next) {
                struct linked_list *pitem;
                
                th = (struct thing *)titem->l_data;
                for(pitem = th->t_pack; pitem != NULL; pitem = pitem->l_next) {
                    if(is_magic((struct object *)pitem->l_data)) {
                        show = TRUE;
                        mvwaddch(hw, th->t_pos.y, th->t_pos.x, MAGIC);
                    }
                    p_know[P_TFIND] = TRUE;
                }
            }
            
            if(show) {
                show_win(hw, 
                         "You sense the presence of magic on this level.--More--");
                break;
            }
        }
        
        msg("You have a strange feeling for a moment, then it passes.", 0);
        break;
    case P_PARALYZE:
        msg("You can't move.", 0);
        no_command = HOLDTIME;
        p_know[P_PARALYZE] = TRUE;
        break;
    case P_SEEINVIS:
        msg("This potion tastes like %s juice.", fruit);

        if((player.t_flags & CANSEE) == 0) {
            player.t_flags |= CANSEE;
            fuse(unsee, 0, SEEDURATION, AFTER);
            light(&player.t_pos);
        }

        sight();

        break;
    case P_RAISE:
        msg("You suddenly feel much more skillful", 0);
        p_know[P_RAISE] = TRUE;
        raise_level();
        break;
    case P_XHEAL:
        player.t_stats.s_hpt += roll(player.t_stats.s_lvl, 8);
        
        if(player.t_stats.s_hpt > max_hp) {
            ++max_hp;
            player.t_stats.s_hpt = max_hp;
        }
        
        msg("You begin to feel much better.", 0);
        p_know[P_XHEAL] = TRUE;
        sight();
        break;
    case P_HASTE:
        add_haste(TRUE);
        msg("You feel yourself moving much faster.", 0);
        p_know[P_HASTE] = TRUE;
        break;
    case P_RESTORE:
        msg("Hey, this tastes great.  It make you feel warm all over.", 0);
        if((player.t_stats.s_str.st_str < max_stats.s_str.st_str)
           || ((player.t_stats.s_str.st_str == 18)
               && (player.t_stats.s_str.st_add < max_stats.s_str.st_add))) {
	    player.t_stats.s_str = max_stats.s_str;
        }
        break;
    case P_BLIND:
        msg("A cloak of darkness falls around you.", 0);

        if((player.t_flags & ISBLIND) == 0) {
            player.t_flags |= ISBLIND;
            fuse(sight, 0, SEEDURATION, AFTER);
            look(FALSE);
        }
        
        p_know[P_BLIND] = TRUE;
        
        break;
    case P_NOP:
        msg("This potion tastes extremely dull.", 0);
        break;
    default:
        msg("What an odd tasting potion!", 0);
        return 0;
    }
    
    status();
    
    if(p_know[obj->o_which] && p_guess[obj->o_which]) {
	free(p_guess[obj->o_which]);
	p_guess[obj->o_which] = NULL;
    }
    else if(!p_know[obj->o_which] && askme && (p_guess[obj->o_which] == NULL)) {
        if(terse) {
            msg("Call it: ", 0);
        }
        else {
            msg("What do you want to call it? ", 0);
        }
        
	if(get_str(buf, cw) == NORM) {
	    p_guess[obj->o_which] = malloc((unsigned int) strlen(buf) + 1);
	    strcpy(p_guess[obj->o_which], buf);
	}
    }
    /*
     * Throw the item away
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
