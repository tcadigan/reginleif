// All the fighting gets done here
//
// @(#)fight.c 3.28 (Berkeley) 6/15/81

#include "fight.h"

#include "chase.h"
#include "io.h"
#include "list.h"
#include "main.h"
#include "misc.h"
#include "move.h"
#include "rip.h"
#include "things.h"
#include "weapons.h"

#include <ctype.h>

long e_levels[] = {
    10L,20L,40L,80L,160L,320L,640L,1280L,2560L,5120L,10240L,20480L,
    40920L, 81920L, 163840L, 327680L, 655360L, 1310720L, 2621440L, 0L };

// fight:
//     The player attacks the monster.
int fight(coord *mp, char mn, struct object *weap, bool thrown)
{
    struct thing *tp;
    struct linked_list *item;
    bool did_hit = TRUE;

    // Find the monster we want to fight
    item = find_mons(mp->y, mp->x);
    if(item == NULL) {
        if(wizard) {
            int args[] = { mp->y, mp->x };
            msg("Fight what @ %d,%d", args);
        }
    }
    tp = (struct thing *)item->l_data;

    // Since we are fighting, things are not quiet so no healing taks place
    quiet = 0;
    runto(mp, &player.t_pos);

    // Let him know it was really a mimic (if it was one)
    if((tp->t_type == 'M')
       && (tp->t_disguise != 'M')
       && ((player.t_flags & ISBLIND) == 0)) {
	msg("Wait! That's a mimic!", 0);
	tp->t_disguise = 'M';
	did_hit = thrown;
    }
    if (did_hit) {
	char *mname;

	did_hit = FALSE;

        if((player.t_flags & ISBLIND) != 0) {
	    mname = "it";
        }
	else {
	    mname = monsters[mn-'A'].m_name;
        }
        
	if(roll_em(&player.t_stats, &tp->t_stats, weap, thrown)) {
	    did_hit = TRUE;

	    if(thrown) {
		thunk(weap, mname);
            }
	    else {
		hit(NULL, mname);
            }

            if((player.t_flags & CANHUH) != 0) {
		msg("Your hands stop glowing red", 0);
		msg("The %s appears confused.", mname);
		tp->t_flags |= ISHUH;
		player.t_flags &= ~CANHUH;
	    }

	    if(tp->t_stats.s_hpt <= 0) {
		killed(item, TRUE);
            }
	}
	else {
	    if(thrown) {
		bounce(weap, mname);
            }
	    else {
		miss(NULL, mname);
            }
        }
    }
    count = 0;
    
    return did_hit;
}

// attack:
//     The monster attacks the player
int attack(struct thing *mp)
{
    char *mname;

    // Since this is an attack, stop running and any healing that was
    // going on at the time
    running = FALSE;
    quiet = 0;

    if((mp->t_type == 'M') && ((player.t_flags & ISBLIND) == 0)) {
	mp->t_disguise = 'M';
    }

    if((player.t_flags & ISBLIND) != 0) {
	mname = "it";
    }
    else {
	mname = monsters[mp->t_type - 'A'].m_name;
    }
    
    if(roll_em(&mp->t_stats, &player.t_stats, NULL, FALSE)) {
	if(mp->t_type != 'E') {
	    hit(mname, NULL);
        }
        
	if(player.t_stats.s_hpt <= 0) {
            // Bye bye life...
	    death(mp->t_type);
        }

        if((mp->t_flags & ISCANC) == 0) {
	    switch(mp->t_type) {
            case 'R':
                // If a rust monster hits, you lose armor
                if((cur_armor != NULL) && (cur_armor->o_ac < 9)) {
                    if(!terse) {
                        msg("Your armor appears to be weaker now. Oh my!", 0);
                    }
                    else {
                        msg("Your armor weakens", 0);
                    }
                    ++cur_armor->o_ac;
                }
                break;
            case'E':
                // The gaze of the floating eye hypnotizes you
                if((player.t_flags & ISBLIND) != 0) {
                    break;
                }
                
                if(!no_command) {
                    addmsg("You are transfixed", 0);
                    if(!terse) {
                        addmsg(" by the gaze of the floating eye.", 0);
                    }
                    endmsg();
                }
                no_command += (rnd(2) + 2);
                break;
            case'A':
                // Ants have poisonous bites
                if(!save(VS_POISON)) {
                    if(!(((cur_ring[LEFT] != NULL) && (cur_ring[LEFT]->o_which == R_SUSTSTR))
                         || ((cur_ring[RIGHT] != NULL) && (cur_ring[RIGHT]->o_which == R_SUSTSTR)))) {
                        chg_str(-1);
                        if(!terse) {
                            msg("You feel a sting in your arm and now feel weaker", 0);
                        }
                        else {
                            msg("A sting has weakened you", 0);
                        }
                    }
                    else {
                        if(!terse) {
                            msg("A sting momentarily weakens you", 0);
                        }
                        else {
                            msg("Sting has no effect", 0);
                        }
                    }
                }
                break;
            case 'W':
                // Wraiths might drain energy levels
                if(rnd(100) < 15) {
                    int fewer;
                    
                    if(player.t_stats.s_exp == 0) {
                        // All levels gone
                        death('W');
                    }
                    
                    msg("You suddenly feel weaker.", 0);
                    --player.t_stats.s_lvl;
                    if(player.t_stats.s_lvl == 0) {
                        player.t_stats.s_exp = 0;
                        player.t_stats.s_lvl = 1;
                    }
                    else {
                        player.t_stats.s_exp = e_levels[player.t_stats.s_lvl - 1] + 1;
                    }
                    
                    fewer = roll(1, 10);
                    player.t_stats.s_hpt -= fewer;
                    max_hp -= fewer;
                    if(player.t_stats.s_hpt < 1) {
                        player.t_stats.s_hpt = 1;
                    }
                    if(max_hp < 1) {
                        death('W');
                    }
                }
                break;
            case'F':
                // BRL fix (I think)
		{
                    // Violet fungi stops the poor guy from moving
                    player.t_flags |= ISHELD;
		    sprintf(monsters['F'-'A'].m_stats.s_dmg, "%dd1", ++fung_hit);
                }
                break;
            case 'L':
                {
                    // Leprechaun steals some gold
		    long lastpurse;
                    
		    lastpurse = purse;
                    purse -= (rnd(50 + (10 * level)) + 2);
		    if(!save(VS_MAGIC)) {
                        purse -= (rnd(50 + (10 * level)) + 2);
                        purse -= (rnd(50 + (10 * level)) + 2);
                        purse -= (rnd(50 + (10 * level)) + 2);
                        purse -= (rnd(50 + (10 * level)) + 2);
                    }
		    if(purse < 0) {
			purse = 0;
                    }
		    if(purse != lastpurse) {
			msg("Your purse feels lighter", 0);
                    }
		    remove_monster(&mp->t_pos, find_mons(mp->t_pos.y, mp->t_pos.x));
		}
                break;
            case 'N':
		{
                    // Nymphs steal a magic item, look through the pack and
                    // pick out one we like
                    
		    struct linked_list *list;
                    struct linked_list *steal;
		    struct object *obj;
		    int nobj;

		    steal = NULL;
		    for(nobj = 0, list = player.t_pack; list != NULL; list = list->l_next) {
			obj = (struct object *)list->l_data;
			if((obj != cur_armor)
                           && (obj != cur_weapon)
                           && is_magic(obj)
                           && (rnd(++nobj) == 0)) {
                            steal = list;
                        }
                    }
                    
		    if(steal != NULL) {
			struct object *obj;
                        
			obj = (struct object *)steal->l_data;
			remove_monster(&mp->t_pos, find_mons(mp->t_pos.y, mp->t_pos.x));
                        
			if((obj->o_count > 1) && (obj->o_group == 0)) {
                            int oc;
                            
			    oc = obj->o_count--;
			    obj->o_count = 1;
			    msg("She stole %s!", inv_name(obj, TRUE));
			    obj->o_count = oc;
			}
			else {
			    msg("She stole %s!", inv_name(obj, TRUE));
			    _detach(&player.t_pack, steal);
			    discard(steal);
			}
                        
			--inpack;
		    }
		}
                break;
            default:
                break;
	    }
        }
    }
    else if(mp->t_type != 'E') {
	if(mp->t_type == 'F') {
	    player.t_stats.s_hpt -= fung_hit;
	    if(player.t_stats.s_hpt <= 0) {
                // Bye bye life...
		death(mp->t_type);
            }
	}
	miss(mname, NULL);
    }

    // Check to see if this is a regenerating monster and let it headl
    // if it is
    if(((mp->t_flags & ISREGEN) != 0) && (rnd(100) < 33)) {
	++mp->t_stats.s_hpt;
    }
    
    if(fight_flush) {
        // flush typeahead
	raw();
	noraw();
    }
    
    count = 0;
    status();

    return 0;
}

// swing:
//     Returns true if the swing hits
int swing(int at_lvl, int op_arm, int wplus)
{
    int res = rnd(20) + 1;
    int need = (21 - at_lvl) - op_arm;

    return ((res + wplus) >= need);
}

// check_level:
//     Check to see if the guy has gone up a level
int check_level()
{
    int i;
    int add;

    for(i = 0; e_levels[i] != 0; ++i) {
	if(e_levels[i] > player.t_stats.s_exp) {
	    break;
        }
    }
    
    i++;
    
    if(i > player.t_stats.s_lvl) {
	add = roll(i - player.t_stats.s_lvl, 10);
	max_hp += add;
        player.t_stats.s_hpt += add;

	if(player.t_stats.s_hpt > max_hp) {
	    player.t_stats.s_hpt = max_hp;
        }
        
        int args[] = { i };
	msg("Welcome to level %d", args);
    }
    
    player.t_stats.s_lvl = i;

    return 0;
}

// roll_em:
//     Roll several attacks
int roll_em(struct stats *att, struct stats *def, struct object *weap, bool hurl)
{
    char *cp;
    int ndice;
    int nsides;
    int def_arm;
    bool did_hit = FALSE;
    int prop_hplus;
    int prop_dplus;

    prop_dplus = 0;
    prop_hplus = 0;

    if(weap == NULL) {
	cp = att->s_dmg;
    }
    else if(hurl) {
	if((weap->o_flags & ISMISL)
           && (cur_weapon != NULL)
           && (cur_weapon->o_which == weap->o_launch)) {
	    cp = weap->o_hurldmg;
	    prop_hplus = cur_weapon->o_hplus;
	    prop_dplus = cur_weapon->o_dplus;
	}
	else {
            if(weap->o_flags & ISMISL) {
                cp = weap->o_damage;
            }
            else {
                cp = weap->o_hurldmg;
            }
        }
    }
    else {
	cp = weap->o_damage;

        // Drain a staff of striking
	if((weap->o_type == STICK)
           && (weap->o_which == WS_HIT)
           && (weap->o_ac == 0)) {
            weap->o_damage = "0d0";
            weap->o_hplus = weap->o_dplus = 0;
        }
    }

    while(1) {
	int damage;
        int hplus = prop_hplus;
        int dplus = prop_dplus;

        if(weap != NULL) {
            hplus += weap->o_hplus;
        }

        if(weap != NULL) {
            dplus += weap->o_dplus;
        }

	if(weap == cur_weapon) {
            if((cur_ring[LEFT] != NULL) && (cur_ring[LEFT]->o_which == R_ADDDAM)) {
		dplus += cur_ring[LEFT]->o_ac;
            }
            else if((cur_ring[LEFT] != NULL) && (cur_ring[LEFT]->o_which == R_ADDHIT)) {
		hplus += cur_ring[LEFT]->o_ac;
            }

            if((cur_ring[RIGHT] != NULL) && (cur_ring[RIGHT]->o_which == R_ADDDAM)) {
		dplus += cur_ring[RIGHT]->o_ac;
            }
            else if((cur_ring[RIGHT] != NULL) && (cur_ring[RIGHT]->o_which == R_ADDHIT)) {
		hplus += cur_ring[RIGHT]->o_ac;
            }
	}
        
	ndice = atoi(cp);
        cp = strchr(cp, 'd');
        
	if(cp == NULL) {
	    break;
        }

        ++cp;
	nsides = atoi(cp);
        
	if(def == &player.t_stats) {
	    if(cur_armor != NULL) {
		def_arm = cur_armor->o_ac;
            }
	    else {
		def_arm = def->s_arm;
            }

            if((cur_ring[LEFT] != NULL) && (cur_ring[LEFT]->o_which == R_PROTECT)) {
		def_arm -= cur_ring[LEFT]->o_ac;
            }
            else if((cur_ring[RIGHT] != NULL) && (cur_ring[RIGHT]->o_which == R_PROTECT)) {
		def_arm -= cur_ring[RIGHT]->o_ac;
            }
	}
	else {
	    def_arm = def->s_arm;
        }
        
	if(swing(att->s_lvl, def_arm, hplus + str_plus(&att->s_str))) {
	    int proll;

	    proll = roll(ndice, nsides);
	    if(((ndice + nsides) > 0) && (proll < 1)) {
                if(wizard) {
                    int args[] = { ndice, nsides, proll };
                    msg("Damage for %dd%d came out %d.", args);
                }
            }
	    damage = dplus + proll + add_dam(&att->s_str);

            if(0 > damage) {
                def->s_hpt -= 0;
            }
            else {
                def->s_hpt -= damage;
            }
            
	    did_hit = TRUE;
	}

        cp = strchr(cp, '/');
	if(cp == NULL) {
	    break;
        }
        
	++cp;
    }
    
    return did_hit;
}

// prname:
//     The print name of a combatant
char *prname(char *who, bool upper)
{
    static char tbuf[80];

    *tbuf = '\0';
    
    if(who == 0) {
	strcpy(tbuf, "you");
    }
    else if((player.t_flags & ISBLIND) != 0) {
	strcpy(tbuf, "it");
    }
    else {
	strcpy(tbuf, "the ");
	strcat(tbuf, who);
    }

    if(upper) {
	*tbuf = toupper(*tbuf);
    }
    
    return tbuf;
}

// hit:
//     Print a message to indicate a successful hit
int hit(char *er, char *ee)
{
    char *s;

    addmsg("%s", prname(er, TRUE));
    if(terse) {
	s = " hit.";
    }
    else {
	switch(rnd(4)) {
        case 0:
            s = " scored an excellent hit on ";
            break;
        case 1:
            s = " hit ";
            break;
        case 2:
            if(er == 0) {
                s = " have injured ";
            }
            else {
                s = " has injured ";
            }

            break;
        case 3:
            if(er == 0) {
                s = " swing and hit ";
            }
            else {
                s = " swings and hits ";
            }

            break;
	}
    }
    
    addmsg("%s", s);
    if(!terse) {
	addmsg("%s", prname(ee, FALSE));
    }
    
    endmsg();

    return 0;
}

// miss:
//     Print a message to indicate a poor swing
int miss(char *er, char *ee)
{
    char *s;
    
    addmsg("%s", prname(er, TRUE));

    int value = 0;

    if(!terse) {
        rnd(4);
    }
    
    switch(value) {
    case 0:
        if(er == 0) {
            s = " miss";
        }
        else {
            s = " misses";
        }

        break;
    case 1:
        if(er == 0) {
            s = " swing and miss";
        }
        else {
            s = " swings and misses";
        }

        break;
    case 2:
        if(er == 0) {
            s = " barely miss";
        }
        else {
            s = " barely misses";
        }

        break;
    case 3:
        if(er == 0) {
            s = " don't hit";
        }
        else {
            s = " doesn't hit";
        }

        break;
    }
    addmsg("%s", s);

    if(!terse) {
	addmsg(" %s", prname(ee, FALSE));
    }
    
    endmsg();
    
    return 0;
}

// save_throw:
//     See if a creature saves against something
int save_throw(int which, struct thing *tp)
{
    int need;

    need = 14 + which - (tp->t_stats.s_lvl / 2);
    return (roll(1, 20) >= need);
}

// save:
//     See if he saves against various nasty things
int save(int which)
{
    return save_throw(which, &player);
}

// str_plus:
//     Compute bonus/penalties for strength on the "to hit" roll
int str_plus(str_t *str)
{
    if(str->st_str == 18) {
	if(str->st_add == 100) {
	    return 3;
        }
	if(str->st_add > 50) {
	    return 2;
        }
    }
    if(str->st_str >= 17) {
	return 1;
    }
    if(str->st_str > 6) {
	return 0;
    }
    
    return (str->st_str - 7);
}

// add_dam:
//     Compute additional damage done for exceptionally high or low strength
int add_dam(str_t *str)
{
    if(str->st_str == 18) {
	if(str->st_add == 100) {
	    return 6;
        }
	if(str->st_add > 90) {
	    return 5;
        }
	if(str->st_add > 75) {
	    return 4;
        }
	if(str->st_add != 0) {
	    return 3;
        }
        
	return 2;
    }
    if(str->st_str > 15) {
	return 1;
    }
    if(str->st_str > 6) {
	return 0;
    }
    return (str->st_str - 7);
}

// raise_level:
//     The guy just magically went up a level
int raise_level()
{
    player.t_stats.s_exp = e_levels[player.t_stats.s_lvl - 1] + 1L;
    check_level();

    return 0;
}

// thunk:
//     A missile hits a monster
int thunk(struct object *weap, char *mname)
{
    if(weap->o_type == WEAPON) {
        msg("The %s hits the ", w_names[weap->o_which]);
	msg("%s", mname);
    }
    else {
	msg("You hit the %s.", mname);
    }

    return 0;
}

// bounce:
//     A missile missed a monster
int bounce(struct object *weap, char *mname)
{
    if(weap->o_type == WEAPON) {
        msg("The %s misses the ", w_names[weap->o_which]);
        msg("%s", mname);
    }
    else {
	msg("You missed the %s.", mname);
    }

    return 0;
}

// remove_monster:
//     Remove a monster from the screen
int remove_monster(coord *mp, struct linked_list *item)
{
    mvwaddch(mw, mp->y, mp->x, ' ');
    mvwaddch(cw, mp->y, mp->x, ((struct thing *)item->l_data)->t_oldch);
    _detach(&mlist, item);
    discard(item);

    return 0;
}

// is_magic:
//     Returns true if an object radiates magic
int is_magic(struct object *obj)
{
    switch(obj->o_type) {
    case ARMOR:
        return (obj->o_ac != a_class[obj->o_which]);
    case WEAPON:
        return ((obj->o_hplus != 0) || (obj->o_dplus != 0));
    case POTION:
    case SCROLL:
    case STICK:
    case RING:
    case AMULET:
        return TRUE;
    }
    
    return FALSE;
}

// killed:
//     Called to put a monster to death
int killed(struct linked_list *item, bool pr)
{
    struct thing *tp;
    struct linked_list *pitem;
    struct linked_list *nexti;

    tp = (struct thing *)item->l_data;
    if(pr) {
        if(terse) {
            addmsg("Defeated ", 0);
        }
        else {
            addmsg("You have defeated ", 0);
        }

        if((player.t_flags & ISBLIND) != 0) {
	    msg("it.", 0);
        }
	else {
	    if(!terse) {
		addmsg("the ", 0);
            }
            
	    msg("%s.", monsters[tp->t_type - 'A'].m_name);
	}
    }
    player.t_stats.s_exp += tp->t_stats.s_exp;

    // Do adjustments if he went up a level
    check_level();

    // If the monster was a violet fungi, un-hold him
    switch(tp->t_type) {
    case 'F':
        player.t_flags &= ~ISHELD;
        fung_hit = 0;
        strcpy(monsters['F'-'A'].m_stats.s_dmg, "000d0");
        break;
    case'L':
	{
	    struct room *rp;

            rp = roomin(&tp->t_pos);
	    if(rp == NULL) {
		break;
            }
            
	    if((rp->r_goldval != 0) || fallpos(&tp->t_pos,&rp->r_gold,FALSE)) {
                rp->r_goldval += (rnd(50 + (10 * level)) + 2);
                
		if(save(VS_MAGIC)) {
                    rp->r_goldval += (rnd(50 + (10 * level)) + 2);
                    rp->r_goldval += (rnd(50 + (10 * level)) + 2);
                    rp->r_goldval += (rnd(50 + (10 * level)) + 2);
                    rp->r_goldval += (rnd(50 + (10 * level)) + 2);
                }
                
                mvwaddch(stdscr, rp->r_gold.y, rp->r_gold.x, GOLD);
                
		if(!(rp->r_flags & ISDARK)) {
		    light(&player.t_pos);
		    mvwaddch(cw, player.t_pos.y, player.t_pos.x, PLAYER);
		}
	    }
	}
    }

    // Empty the monster's pack
    pitem = tp->t_pack;

    // Get rid of the monster
    remove_monster(&tp->t_pos, item);
    while(pitem != NULL) {
	struct object *obj;

	nexti = tp->t_pack->l_next;
	obj = (struct object *)pitem->l_data;
	obj->o_pos = tp->t_pos;
	_detach(&tp->t_pack, pitem);
	fall(pitem, FALSE);
	pitem = nexti;
    }

    return 0;
}
