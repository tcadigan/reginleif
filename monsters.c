// File with various monster functions in it
//
// @(#)monsters.c 3.18 (Berkeley) 6/15/81
#include "monsters.h"

#include "chase.h"
#include "daemon.h"
#include "daemons.h"
#include "fight.h"
#include "io.h"
#include "list.h"
#include "main.h"
#include "newlevel.h"
#include "rooms.h"

#include <ctype.h>

/* TC_DEBUG: Start */
#include "debug.h"
#include <stdio.h>
/* TC_DEBUG: Finish */

// List of monsters in rough order of vorpalness
static char *lvl_mons =  "KJBSHEAOZGLCRQNYTWFIXUMVDP";
static char *wand_mons = "KJBSH AOZG CRQ Y W IXU V  ";

// randmonster:
//     Pick a monster to show up. The lower the level,
//     the meaner the monster
char randmonster(bool wander)
{
    int d;
    char *mons;

    if(wander) {
        mons = wand_mons;
    }
    else {
        mons = lvl_mons;
    }

    d = level + (rnd(10) - 5);
    
    if(d < 1) {
        d = rnd(5) + 1;
    }

    if(d > 26) {
        d = rnd(5) + 22;
    }

    --d;
    
    while(mons[d] == ' ') {        
	d = level + (rnd(10) - 5);
	if(d < 1) {
	    d = rnd(5) + 1;
        }
        
	if(d > 26) {
	    d = rnd(5) + 22;
        }

        --d;
    }
    
    return mons[d];
}

// new_monster:
//     Pick a new monster and add it to the list
int new_monster(struct linked_list *item, char type, coord *cp)
{
    struct thing *tp;
    struct monster *mp;

    _attach(&mlist, item);
    tp = (struct thing *)item->l_data;
    tp->t_type = type;
    tp->t_pos = *cp;
    tp->t_oldch = mvwinch(cw, cp->y, cp->x);
    mvwaddch(mw, cp->y, cp->x, tp->t_type);
    mp = &monsters[tp->t_type-'A'];
    tp->t_stats.s_hpt = roll(mp->m_stats.s_lvl, 8);
    tp->t_stats.s_lvl = mp->m_stats.s_lvl;
    tp->t_stats.s_arm = mp->m_stats.s_arm;
    tp->t_stats.s_dmg = mp->m_stats.s_dmg;
    tp->t_stats.s_exp = mp->m_stats.s_exp;
    tp->t_stats.s_str.st_str = 10;
    tp->t_flags = mp->m_flags;
    tp->t_turn = TRUE;
    tp->t_pack = NULL;

    if(((cur_ring[LEFT] != NULL) && (cur_ring[LEFT]->o_which == R_AGGR))
       || ((cur_ring[RIGHT] != NULL) && (cur_ring[RIGHT]->o_which == R_AGGR))) {
	runto(cp, &player.t_pos);
    }
    
    if(type == 'M') {
	char mch;
        
	if(tp->t_pack != NULL) {
	    mch = ((struct object *)tp->t_pack->l_data)->o_type;
        }
	else {
            int value = 8;
            
            if(level > 25) {
                value = 9;
            }
            
	    switch(rnd(value)) {
            case 0:
                mch = GOLD;
                
                break;
            case 1:
                mch = POTION;
                
                break;
            case 2:
                mch = SCROLL;
                
                break;
            case 3:
                mch = STAIRS;
                
                break;
            case 4:
                mch = WEAPON;
                
                break;
            case 5:
                mch = ARMOR;
                
                break;
            case 6:
                mch = RING;
                
                break;
            case 7:
                mch = STICK;
                
                break;
            case 8:
                mch = AMULET;
                
                break;
	    }
        }
        
	tp->t_disguise = mch;
    }

    /* TC_DEBUG: Start */
    FILE *output;
    output = fopen("debug.txt", "a+");
    print_thing(tp, output);
    fclose(output);
    /* TC_DEBUG: Finish */
    
    return 0;
}

// wanderer:
//     A wandering monster has awakened and is headed for the player
int wanderer()
{
    int i;
    int ch;
    struct room *rp;
    struct room *hr = roomin(&player.t_pos);
    struct linked_list *item;
    struct thing *tp;
    coord cp;

    item = new_item(sizeof *tp);

    i = rnd_room();
    rp = &rooms[i];
    
    if(rp != hr) {
        rnd_pos(rp, &cp);
        ch = mvwinch(stdscr, cp.y, cp.x);
        
        if(ch == ERR) {
            if(wizard) {
                int args[] = { cp.y, cp.x };
                msg("Routine wanderer: mvwinch failed to %d,%d", args);
            }
            
            wait_for('\n');
            
            return 0;
        }
    }

    while(!((hr != rp) && step_ok(ch))) {
        i = rnd_room();
        rp = &rooms[i];

        if(rp != hr) {
            rnd_pos(rp, &cp);
            ch = mvwinch(stdscr, cp.y, cp.x);
            
            if(ch == ERR) {
                if(wizard) {
                    int args[] = { cp.y, cp.x };
                    msg("Routine wanderer: mvwinch failed to %d,%d", args);
                }

                wait_for('\n');

                return 0;
            }
        }
    }

    new_monster(item, randmonster(TRUE), &cp);
    tp = (struct thing *)item->l_data;
    tp->t_flags |= ISRUN;
    tp->t_pos = cp;
    tp->t_dest = &player.t_pos;
    if(wizard) {
	msg("Started a wandering %s", monsters[tp->t_type - 'A'].m_name);
    }

    return 0;
}

// wake_monster:
//     What to do when the hero steps next to a monster
struct linked_list *wake_monster(int y, int x)
{
    struct thing *tp;
    struct linked_list *it;
    struct room *rp;
    char ch;

    it = find_mons(y, x);
    if(it == NULL) {
	msg("Can't find monster in show", 0);
    }
    tp = (struct thing *)it->l_data;
    ch = tp->t_type;

    // Every time he sees a mean monster, it might start chasing him
    if((rnd(100) > 33)
       && ((tp->t_flags & ISMEAN) != 0)
       && ((tp->t_flags & ISHELD) == 0)
       && !(((cur_ring[LEFT] != NULL) && (cur_ring[LEFT]->o_which == R_STEALTH))
            || ((cur_ring[RIGHT] != NULL) && (cur_ring[RIGHT]->o_which == R_STEALTH)))) {
	tp->t_dest = &player.t_pos;
	tp->t_flags |= ISRUN;
    }

    if((ch == 'U') && ((player.t_flags & ISBLIND) == 0)) {
        rp = roomin(&player.t_pos);
        int distance = ((player.t_pos.x - x) * (player.t_pos.x - x)) + ((player.t_pos.y - y) * (player.t_pos.y - y));
        
	if(((rp != NULL) && !(rp->r_flags & ISDARK))
           || (distance < 3)) {
            if(((tp->t_flags & ISFOUND) == 0) && !save(VS_MAGIC)) {
		msg("The umber hulk's gaze has confused you.", 0);

                if((player.t_flags & ISHUH) != 0) {
		    lengthen(unconfuse, rnd(20) + HUHDURATION);
                }
		else {
		    fuse(unconfuse, 0, rnd(20) + HUHDURATION, AFTER);
                }
                
		player.t_flags |= ISHUH;
	    }
	    tp->t_flags |= ISFOUND;
	}
    }

    // Hide the invisible monsters
    if(((tp->t_flags & ISINVIS) != 0) && ((player.t_flags & CANSEE) == 0)) {
	ch = mvwinch(stdscr, y, x);
    }
    
    // Let the greedy ones guard gold
    if(((tp->t_flags & ISGREED) != 0) && ((tp->t_flags & ISRUN) == 0)) {
	if((rp != NULL) && (rp->r_goldval)) {
	    tp->t_dest = &rp->r_gold;
	    tp->t_flags |= ISRUN;
	}
    }
    
    return it;
}

// genocide:
//     Something...
int genocide()
{
    struct linked_list *ip;
    struct thing *mp;
    char c;
    int i;
    struct linked_list *nip;

    addmsg("Which monster", 0);
    if(!terse) {
	addmsg(" do you wish to wipe out", 0);
    }
    msg("? ", 0);
    c = readchar();
    while(!isalpha(c)) {
	if(c == ESCAPE) {
	    return 0;
        }
	else {
	    mpos = 0;
	    msg("Please specifiy a letter between 'A' and 'Z'", 0);
	}

        c = readchar();
    }
    if(islower(c)) {
	c = toupper(c);
    }
    
    for(ip = mlist; ip; ip = nip) {
	mp = (struct thing *)ip->l_data;
	nip = ip->l_next;
	if(mp->t_type == c) {
	    remove_monster(&mp->t_pos, ip);
        }
    }
    for(i = 0; i < 26; ++i) {
	if(lvl_mons[i] == c) {
	    lvl_mons[i] = ' ';
	    wand_mons[i] = ' ';
	    break;
	}
    }
    
    return 0;
}
