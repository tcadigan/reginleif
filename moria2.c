#include "moria2.h"

#include <stdio.h>

#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>

#else
#include <strings.h>

#endif

/* Correct SUN stupidity in the stdio.h file */
#ifdef sun
char *sprintf();
#endif

/* Global flags */
/* Next level when true */
extern int moria_flag;

/* Player is searching */
extern int search_flag;

/* Handle teleport traps */
extern int teleport_flag;

/* Player carrying light */
extern int player_light;

/* Used in get_panel */
extern int cave_flag;

/* Used in move_light */
extern int light_flag;

char cur_char1();

/* Teleport the player to a new location    -RAK- */
void teleport(int dis)
{
    int y;
    int x;
    cave_type *c_ptr;
    int i;
    int j;

    y = randint(cur_height) - 1;
    x = randint(cur_width) - 1;

    while(distance(y, x, char_row, char_col) > dis) {
	y += ((char_row - y) / 2);
	x += ((char_row - x) / 2);
    }

    while(!cave[y][x].fopen || (cave[y][x].cptr >= 2)) {
	y = randint(cur_height) - 1;
	x = randint(cur_width) - 1;

	while(distance(y, x, char_row, char_col) > dis) {
	    y += ((char_row - y) / 2);
	    x += ((char_col - x) / 2);
	}
    }

    move_rec(char_row, char_col, y, x);

    for(i = (char_row - 1); i <= (char_row + 1); ++i) {
	for(j = (char_col - 1); j <= (char_col + 1); ++j) {
	    c_ptr = &cave[i][j];
	    c_ptr->tl = FALSE;

	    if(!test_light(i, j)) {
		unlite_spot(i, j);
	    }
	}
    }

    if(test_light(char_row, char_col)) {
	lite_spot(char_row, char_col);
    }

    char_row = y;
    char_col = x;
    move_char(5);
    creatures(FALSE);
    teleport_flag = FALSE;
}

/* Player hit a trap...(Chuckle)    -RAK- */
void hit_trap(int *y, int *x)
{
    int i;
    int ty;
    int tx;
    int dam;
    cave_type *c_ptr;
    struct misc *p_ptr;

    change_trap(*y, *x);
    lite_spot(char_row, char_col);

    if(find_flag) {
	find_flag = FALSE;
	move_light(char_row, char_col, char_row, char_col);
    }

    c_ptr = &cave[*y][*x];
    p_ptr = &py.misc;
    dam = damroll(t_list[c_ptr->tptr].damage);

    switch(t_list[c_ptr->tptr].subval) {
    case 1: /* Open pit */
	msg_print("You fell into a pit!");

	if(py.flags.ffall) {
	    msg_print("You gently float down.");
	}
	else {
	    take_hit(dam, "an open pit.");
	}

	break;
    case 2: /* Arrow trap */
	if(test_hit(125, 0, 0, p_ptr->pac + p_ptr->ptoac)) {
	    take_hit(dam, "an arrow trap.");
	    msg_print("An arrow hits you.");
	}
	else {
	    msg_print("An arrow barely misses you.");
	}

	break;
    case 3: /* Covered pit */
	msg_print("You fell into a covered pit.");

	if(py.flags.ffall) {
	    msg_print("You gently float down.");
	}
	else {
	    take_hit(dam, "a covered pit.");
	}

	place_trap(*y, *x, 2, 1);

	break;
    case 4: /* Trap door */
	msg_print("You fell through a trap door!");

	/* Make sure player reads message before new level drawn! */
	msg_print(" ");
	moria_flag = TRUE;
	++dun_level;

	if(py.flags.ffall) {
	    msg_print("You gently float down.");
	}
	else {
	    take_hit(dam, "a trap door.");
	}

	break;
    case 5: /* Sleep gas */
	if(py.flags.paralysis == 0) {
	    msg_print("A strange white mist surrounds you!");

	    if(py.flags.free_act) {
		msg_print("You are unaffected.");
	    }
	    else {
		msg_print("You fall asleep.");
		py.flags.paralysis += (randint(10) + 4);
	    }
	}

	break;
    case 6: /* Hidden object */
	c_ptr->fm = FALSE:
	pusht((int)c_ptr->tptr);
	place_object(*y, *x);
	msg_print("Hmmm, there was something under this rock.");

	break;
    case 7: /* STR dart */
	if(test_hit(125, 0, 0, p_ptr->pac + p_ptr->ptoac)) {
	    if(!py.flags.sustain_str) {
		py.stats.cstr = de_statp(py.stats.cstr);
		take_hit(dam, "a dart trap.");
		print_stat |= 0x0001;
		msg_print("A small dart weakens you!");

		/* Adjust misc stats */
		py_bonuses(blank_treasure, 0);
	    }
	    else {
		msg_print("A small dart hits you.");
	    }
	}
	else {
	    msg_print("A small dart barely misses you.");
	}

	break;
    case 8: /* Teleport */
	teleport_flag = TRUE;
	msg_print("You hit a teleport trap!");

	break;
    case 9: /* Rock fall */
	take_hit(dam, "a falling rock.");
	pusht((int)c_ptr->tptr);
	place_rubble(*y, *x);
	msg_print("You are hit by a falling rock");

	break;
    case 10: /* Corrode gas */
	corrode_gas("corrosion gas.");
	msg_print("A strange red gas surrounds you.");

	break;
    case 11: /* Summon monster */
	/* Rune disappears... */
	c_ptr->fm = FALSE;
	pusht((int)c_ptr->tptr);
	c_ptr->tptr = 0;

	for(i = 0; i < (2 + randint(3)); ++i) {
	    ty = char_row;
	    tx = char_col;
	    summon_monster(&ty, &tx, FALSE);
	}

	break;
    case 12: /* Fire trap */
	fire_dam(dam, "a fire trap.");
	msg_print("You are enveloped in flames!");

	break;
    case 13: /* Acid trap */
	acid_dam(dam, "an acid trap.");
	msg_print("You are splashed with acid!");

	break;
    case 14: /* Poison gas */
	poison_gas(dam, "a poison gas trap.");
	msg_print("A pungent green gas surrounds you!");

	break;
    case 15: /* Blind gas */
	msg_print("A black gas surrounds you!");
	py.flags.blind += (randint(50) + 50);

	break;
    case 16: /* Confuse gas */
	msg_print("A gas of scintillating colors surrounds you!");
	py.flags.confused += (randint(15) + 15);

	break;
    case 17: /* Slow dart */
	if(test_hit(125, 0, 0, p_ptr->pac + p_ptr->ptoac)) {
	    take_hit(dam, "a dart trap.");
	    msg_print("A small dart hits you!");
	    py.flags.slow += (randint(20) + 10);
	}
	else {
	    msg_print("A small dart barely misses you.");
	}

	break;
    case 18: /* CON dart */
	if(test_hit(125, 0, 0, p_ptr->pac + p_ptr->ptoac)) {
	    if(!py.flags.sustain_con) {
		py.stats.ccon = de_statp(py.stats.ccon);
		take_hit(dam, "a dart trap.");
		print_stat |= 0x0004;
		msg_print("A small dart weakens you!");
	    }
	    else {
		msg_print("A small dart hits you.");
	    }
	}
	else {
	    msg_print("a small dart barely misses.");
	}

	break;
    case 19: /* Secret door */

	break;
    case 99: /* Scare monster */

	break;
    case 101: /* General */
	/* Town level traps are special, the stores... */

	enter_store(0);

	break;
    case 102: /* Armory */
	enter_store(1);

	break;
    case 103: /* Weaponsmith */
	enter_store(2);

	break;
    case 104: /* Temple */
	enter_store(3);

	break;
    case 105: /* Alchemist */
	enter_store(4);

	break;
    case 106: /* Magic shop */
	enter_store(5);

	break;
    default:
	msg_print("Unknown trap value");

	break;
    }
}

/*
 * Return spell number and failure chance    -RAK-
 * Returns -1 if not spells in book
 * Returns  1 if choose a spell in book to cast
 * Returns  0 if don't choose a spell, i.e. exit with an escape
 */
int cast_spell(char *prompt, int item_val, int *sn, int *sc, int *redraw)
{
    unsigned int j;
    int i;
    int k;
    spl_tpye spell;
    int cast;
    spell_type *s_ptr;

    cast = -1;
    i = 0;
    j = inventory[item_val].flags;

    k = bit_pos(&j);

    if(k >= 0) {
	s_ptr = &magic_spell[py.misc.pclass][k];

	if(s_ptr->slevel <= py.misc.lev) {
	    if(s_ptr->learned) {
		spell[i].splnum = k;
		++i;
	    }
	}
    }

    while(j != 0) {
	k = bit_pos(*j);

	if(k >= 0) {
	    s_ptr = &magic_spell[py.misc.pclass][k];

	    if(s_ptr->slevel <= py.misc.lev) {
		if(s_ptr->learned) {
		    spell[i].splnum = k;
		    ++i;
		}
	    }
	}
    }

    if(i > 0) {
	cast = get_spell(spell, i, sn, sc, prompt, redraw);

	if(cast && (magic_spell[py.misc.pclass][*sn].smana > py.misc.cmana)) {
	    cast = confirm();
	}
    }

    if(*redraw) {
	draw_cave();
    }

    return cast;
}

/* Finds range of item in inventory list    -RAK- */
int find_range(int item1, int item2, int *j, int *k)
{
    int i;
    int flag;

    i = 0;
    *j = -1;
    *k = -1;
    flag = FALSE;

    while(i < inven_ctr) {
	if(((inventory[i].tval == item1) || (inventory[i].tval == item2)) && !flag) {
	    flag = TRUE;
	    *j = i;
	}

	if(((inventory[i].tval != item1) && (inventory[i].tval != item2))
	   && flag
	   && (*k == -1)) {
	    *k = i - i;
	}

	++i;
    }

    if(flag && (*k == -1)) {
	*k = inven_ctr - 1;
    }

    return flag;
}

/* Examine a book    -RAK- */
void examine_book()
{
    unsigned int j;
    int i;
    int k;
    int item_val;
    int redraw;
    int flag;
    char dummy;
    vtype out_val;
    treasure_type *i_ptr;
    spell_type *s_ptr;

    redraw = FALSE;

    if(!find_range(90, 91, &i, &k)) {
	msg_print("You are not carrying any books.");
    }
    else if(get_item(&item_val, "Which book?", &redraw, i, k)) {
	flag = TRUE;
	i_ptr = &inventory[item_val];

	if(class[py.misc.pclass].mspell) {
	    if(i_ptr->tval != 90) {
		msg_print("You do not understand the language.");
		flag = FALSE;
	    }
	}
	else if(class[py.misc.pclass].pspell) {
	    if(i_ptr->tval != 91) {
		msg_print("You do not understand the language.");
		flag = FALSE;
	    }
	}
	else {
	    msg_print("You do not understand the language.");
	    flag = FALSE;
	}

	if(flag) {
	    redraw = TRUE;
	    i = 0;
	    j = inventory[item_val].flags;
	    clear_screen(0, 0);
	    strcpy(out_val, "   Name                         Level  Mana   Known");
	    prt(out_val, 0, 0);

	    k = bit_pos(&j);
	    
	    if(k >= 0) {
		s_ptr = &magic_spell[py.misc.pclass][k];
	    }

	    if(s_ptr->slevel < 99) {
		sprintf(out_val,
			"%c) %s%d     %d   %d",
			97 + i,
			pad(s_ptr->sname, " ", 30),
			s_ptr->slevel,
			s_ptr->smana,
			s_ptr->learned);

		prt(out_val, i + 1, 0);
	    }
	    else {
		/* Clear the line */
		prt("", i + 1, 0);
	    }

	    ++i;

	    while(j != 0) {
		k = bit_pos(&j);

		if(k >= 0) {
		    s_ptr = &magic_spell[py.misc.pclass][k];
		}

		if(s_ptr->slevel < 99) {
		    sprintf(out_val,
			    "%c) %s%d     %d   %d",
			    97 + i,
			    pad(s_ptr->sname, " ", 30),
			    s_ptr->slevel,
			    s_ptr->smana,
			    s_ptr->learned);

		    prt(out_val, i + 1, 0);
		}
		else {
		    /* Clear the line */
		    prt("", i + 1, 0);
		}

		++i;
	    }

	    prt("[Press any key to continue]", 23, 19);
	    inkey(&dummy);
	}
    }

    if(redraw) {
	draw_cave();
    }
}

/*
 * Player is on an object. Many things can happen based on the TVAL of
 * the object. Traps are set off, money and most objects are picked
 * up. Some objects, such as doors just sit there...    -RAK-
 */
void carry(int y, int x)
{
    int item_val;
    vtype out_val;
    vtype tmp_str;
    cave_type *c_ptr;
    treasure_type *i_ptr;

    if(find_flag) {
	find_flag = FALSE;
	move_light(char_row, char_col, char_row, char_col);
    }

    c_ptr = &cave[y][x];
    inventory[INVEN_MAX] = t_list[c_ptr->tptr];

    /* There's GOLD in them thar hills! */
    if(t_list[c_ptr->tptr].tval == 100) {
	pusht((int)c_ptr->tptr);
	c_ptr->tptr = 0;
	i_ptr = &inventory[INVEN_MAX];
	py.misc.au += i_ptr->cost;
	
	sprintf(out_val,
		"You have found %d gold pieces worth %s.",
		i_ptr->cost,
		i_ptr->name);

	prt_gold();
	msg_print(out_val);
    }
    else if((t_list[c_ptr->tptr].tval == 101)
	    || (t_list[c_ptr->tptr].tval == 102)
	    || (t_list[c_ptr->tptr].tval == 109)
	    || (t_list[c_ptr->tptr].tval == 110)) {
	/* Oops */
	hit_trap(&y, &x);
    }
    else if(t_list[c_ptr->tptr].tval < 100) {
	/* Attempt to pick up an object */
	/* Weight limit check */
	if(inven_check_weight()) {
	    /* Too many objects? */
	    if(inven_check_num()) {
		/* Okay, pick it up */
		pusht((int)c_ptr->tptr);
		c_ptr->tptr = 0;
		inven_carry(&item_val);
		objdes(tmp_str, item_val, TRUE);

		sprintf(out_val,
			"You have %s (%c%c",
			tmp_str,
			item_val + 97,
			cur_char1(item_val));

		msg_print(out_val);
	    }
	    else {
		msg_print("You can't carry that many items.");
	    }
	}
	else {
	    msg_print("You can't carry that much weight.");
	}
    }
}

/*
 * Drop an object being carried    -RAK-
 * Note: Only one object per floor spot...
 */
void drop()
{
    int com_val;
    int redraw;
    vtype out_val;
    vtype tmp_str;
    cave_type *c_ptr;

    redraw = FALSE;
    reset_flag = TRUE;

    if(inven_ctr > 0) {
	c_ptr = &cave[char_row][char_col];

	if(c_ptr->tptr != 0) {
	    msg_print("There is something there already.");
	}
	else if(get_item(&com_val, "Which one?", &redraw, 0, inven_ctr - 1)) {
	    reset_flag = FALSE;

	    if(redraw) {
		draw_cave();
	    }

	    inven_drop(com_val, char_row, char_col);
	    objdes(tmp_str, INVEN_MAX, TRUE);
	    sprintf(out_val, "Dropped %s", tmp_stR);
	    msg_print(out_val);
	}
	else if(redraw) {
	    draw_cave();
	}
    }
    else {
	msg_print("You are not carrying anything.");
    }
}

/* Deletes a monster entry from the level    -RAK- */
void delete_monster(int j)
{
    int i;
    int k;
    cave_type *c_ptr;
    monster_type *m_ptr;

    i = muptr;
    k = m_list[j].nptr;

    if(i == j) {
	muptr = k;
    }
    else {
	while(m_list[j].nptr != j) {
	    i = m_list[i].nptr;
	}

	m_list[i].nptr = k;
    }

    m_ptr = &m_list[j];
    cave[m_ptr->fy][m_ptr->fx].cptr = 0;

    if(m_ptr->ml) {
	c_ptr = &cave[m_ptr->fy][m_ptr->fx];

	if(c_ptr->pl || c_ptr->tl) {
	    lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
	}
	else {
	    unlite_spot((int)m_ptr->fy, (int)m_ptr->fx);
	}
    }

    pushm(j);

    if(mon_tot_mult > 0) {
	--mon_tot_mult;
    }
}

/* Makes sure new creature gets lit up    -RAK- */
void check_mon_lite(int y, int x)
{
    cave_type *c_ptr;

    c_ptr = &cave[y][x];

    if(c_ptr->cptr > 1) {
	if(!m_list[c_ptr->cptr].ml) {
	    if(c_ptr->tl || c_ptr->pl) {
		if(los(char_row, char_col, y, x)) {
		    m_list[c_ptr->cptr].ml = TRUE;
		    lite_spot(y, x);
		}
	    }
	}
    }
}

/*
 * Places creature adjacent to given location    -RAK-
 * Rats and flys are fun!
 */
void multiply_monster(int y, int x, int z, int slp)
{
    int i;
    int j;
    int k;
    cave_type *c_ptr;

    i = 0;

    j = y - 2 + randint(3);
    k = x - 2 + randint(3);

    if(in_bounds(j, k)) {
	c_ptr = &cave[j][k];

	if((c_ptr->fval >= 1) && (c_ptr->fval <= 7) && (c_ptr->fval != 3)) {
	    if((c_ptr->tptr == 0) && (c_ptr->cptr != 1)) {
		/* Creature there already? */
		if(c_ptr->cptr > 1) {
		    /* Some critters are cannibalistic! */
		    if(c_list[z].cmove & 0x00080000) {
			delete_monster((int)c_ptr->cptr);
			place_monster(j, k, z, slp);
			check_mon_lite(j, k);
			++mon_tot_mult;
		    }
		}
		else {
		    /* All clear, place a monster */
		    place_monster(j, k, z, slp);
		    check_mon_lite(j, k);
		    ++mon_tot_mult;
		}

		i = 18;
	    }
	}
    }

    ++i;

    while(i <= 18) {
	j = y - 2 + randint(3);
	k = x - 2 + randint(3);

	if(in_bounds(j, k)) {
	    c_ptr = &cave[j][k];

	    if((c_ptr->fval >= 1) && (c_ptr->fval <= 7) && (c_ptr->fval != 3)) {
		if((c_ptr->tptr == 0) && (c_ptr->cptr != 1)) {
		    /* Creature there already? */
		    if(c_ptr->cptr > 1) {
			/* Some critters are cannabalistic! */
			if(c_list[z].cmove & 0x00080000) {
			    delete_monster((int)c_ptr->cptr);
			    place_monster(j, k, z, slp);
			    check_mon_lite(j, k);
			    ++mon_tot_mult;
			}
		    }
		    else {
			/* All clear, place a monster */
			place_monster(j, k, z, slp);
			check_mon_lite(j, k);
			++mon_tot_mult;
		    }

		    i = 18;
		}
	    }
	}

	++i;
    }
}

/*
 * Creates objects nearby the coordinates given    -RAK-
 * BUG: Because of the range, objects can actually be placed into areas 
 *      closed off to the player, this is rarely noticeable, and never a 
 *      problem to the game.
 */
void summon_object(int y, int x, int num, int typ)
{
    int i;
    int j;
    int k;
    cave_type *c_ptr;

    i = 0;

    j = y - 3 + randint(5);
    k = x - 3 + randint(5);

    if(in_bounds(j, k)) {
	c_ptr = &cave[j][k];

	if((c_ptr->fval >= 1) && (c_ptr->fval <= 7) && (c_ptr->fval != 3)) {
	    if(c_ptr->tptr == 0) {
		/* Select type of object */
		switch(typ) {
		case 1:
		    place_object(j, k);

		    break;
		case 2:
		    place_gold(j, k);

		    break;
		case 3:
		    if(randint(100) < 50) {
			place_object(j, k);
		    }
		    else {
			place_gold(j, k);
		    }

		    break;
		default:

		    break;
		}

		if(test_light(j, k)) {
		    lite_spot(j, k);
		}

		i = 10;
	    }
	}
    }

    ++i;

    while(i <= 10) {
	j = y - 3 + randint(5);
	k = x - 3 + randint(5);

	if(in_bounds(j, k)) {
	    c_ptr = &cave[j][k];

	    if((c_ptr->fval >= 1) && (c_ptr->fval <= 7) && (c_ptr->fval != 3)) {
		if(c_ptr->tptr == 0) {
		    /* Select type of object */
		    switch(typ) {
		    case 1:
			place_object(j, k);

			break;
		    case 2:
			place_gold(j, k);

			break;
		    case 3:
			if(randint(100) < 50) {
			    place_object(j, k);
			}
			else {
			    place_gold(j, k);
			}

			break;
		    default:

			break;
		    }

		    if(test_light(j, k)) {
			lite_spot(j, k);
		    }

		    i = 10;
		}
	    }
	}

	++i;
    }

    --num;

    while(num != 0) {
	i = 0;

	j = y - 3 + randint(5);
	k = x - 3 + randint(5);

	if(in_bounds(j, k)) {
	    c_ptr = &cave[j][k];

	    if((c_ptr->fval >= 1) && (c_ptr->fval <= 7) && (c_ptr->fval != 3)) {
		if(c_ptr->tptr == 0) {
		    /* Select type of object */
		    switch(typ) {
		    case 1:
			place_object(j, k);

			break;
		    case 2:
			place_gold(j, k);

			break;
		    case 3:
			if(randint(100) < 50) {
			    place_object(j, k);
			}
			else {
			    place_gold(j, k);
			}

			break;
		    default:

			break;
		    }

		    if(test_light(j, k)) {
			lite_spot(j, k);
		    }

		    i = 10;
		}
	    }
	}

	++i;

	while(i <= 10) {
	    j = y - 3 + randint(5);
	    k = x - 3 + randint(5);

	    if(in_bounds(j, k)) {
		c_ptr = &cave[j][k];

		if((c_ptr->fval >= 1) && (c_ptr->fval <= 7) && (c_ptr->fval != 3)) {
		    if(c_ptr->tptr == 0) {
			/* Select type of object */
			switch(typ) {
			case 1:
			    place_object(j, k);

			    break;
			case 2:
			    place_gold(j, k);

			    break;
			case 3:
			    if(randint(100) < 50) {
				place_object(j, k);
			    }
			    else {
				place_gold(j, k);
			    }

			    break;
			default:

			    break;
			}

			if(test_light(j, k)) {
			    lite_spot(j, k);
			}

			i = 10;
		    }
		}
	    }

	    ++i;
	}

	--num;
    }
}

/* Deletes object from given location    -RAK- */
int delete_object(int y, int x)
{
    int delete;
    cave_type *c_ptr;

    delete = FALSE;
    c_ptr = &cave[y][x];

    if((t_list[c_ptr->tptr].tval == 109)
       || (t_list[c_ptr->tptr].tval == 105)
       || (t_list[c_ptr->tptr].tval == 104)) {
	c_ptr->fval = corr_floor2.ftval;
    }

    c_ptr->fopen = TRUE;
    pusht((int)c_ptr->tptr);
    c_ptr->tptr = 0;
    c_ptr->fm = FALSE;

    if(test_light(y, x)) {
	lite_spot(y, x);
	delete = TRUE;
    }
    else {
	unlite_spot(y, x);
    }

    return delete;
}

/*
 * Allocates objects upon a creature's death    -RAK-
 * Oh well, another creature bites the dust... Reward the victor based on 
 * flags set in the main creature record
 */
void monster_death(int y, int x, unsigned int flags)
{
    int i;

    if(flags & 0x01000000) {
	i = 1;
    }
    else {
	i = 0;
    }

    if(flags & 0x02000000) {
	i += 2;
    }

    if(flags & 0x04000000) {
	if(randint(100) < 60) {
	    summon_object(y, x, 1, i);
	}
    }

    if(flags & 0x08000000) {
	if(randint(100) < 90) {
	    summon_object(y, x, 1, i);
	}
    }

    if(flags & 0x10000000) {
	summon_object(y, x, randint(2), i);
    }

    if(flags & 0x20000000) {
	summon_object(y, x, damroll("2d2"), i);
    }

    if(flags & 0x40000000) {
	summon_object(y, x, damroll("4d3"), i);
    }

    if(flags & 0x80000000) {
	total_winner = TRUE;
	prt_winner();
	msg_print("*** CONGRATULATIONS *** You have won the game...");
	msg_print("Use <CONTROL>-K when you are ready to quit.");
    }
}

/*
 * Decreases monster's hit points and deletes monster if needed.    -RAK-
 * (Picking on my babies...)
 */
int mon_take_hit(int monptr, int dam)
{
    int i;
    double acc_tmp;
    monster_type *m_ptr;
    struct misc *p_ptr;
    creature_type *c_ptr;
    int m_take_hit;

    m_ptr = &m_list[monptr];
    m_ptr->hp -= dam;
    m_ptr->csleep = 0;

    if(m_ptr->hp < 0) {
	monster_death((int)m_ptr->fy, (int)m_ptr->fx, c_list[m_ptr->mptr].cmove);
	c_ptr - &c_list[m_ptr->mptr];
	p_ptr = &py.misc;
	acc_tmp = c_ptr->mexp * ((c_ptr->level + 0.1) / p_ptr->lev);
	i = (int)acc_tmp;
	acc_exp += (acc_tmp - i);

	if(acc_exp > 1) {
	    ++i;
	    acc_exp -= 1.0;
	}

	p_ptr->exp += i;

	if(i > 0) {
	    prt_experience();
	}

	m_take_hit = m_ptr->mptr;
	delete_monster(monptr);
    }
    else {
	m_take_hit = -1;
    }

    return m_take_hit;
}

/* Special damage due to magical abilities of object    -RAK- */
int tot_dam(treasure_type item, int tdam, creature_type monster)
{
    treasure_type *i_ptr;
    creature_type *m_ptr;

    i_ptr = &item;

    if(((i_ptr->tval >= 10) && (i_ptr->tval <= 12))
       || ((i_ptr->tval >= 20) && (i_ptr->tval <= 23))
       || (i_ptr->tval == 77)) {
	m_ptr = &monster;

	/* Slay dragon */
	if((m_ptr->cdefense & 0x0001) && (i_ptr->flags & 0x00002000)) {
	    tdam = tdam * 4;
	}
	else if((m_ptr->cdefense & 0x0008) && (i_ptr->flags & 0x00010000)) {
	    /* Slay undead */
	    tdam = tdam * 3;
	}
	else if((m_ptr->cdefense & 0x0002) && (i_ptr->flags & 0x00004000)) {
	    /* Slay monster */
	    tdam = tdam * 2;
	}
	else if((m_ptr->cdefense & 0x0004) && (i_ptr->flags & 0x00008000)) {
	    /* Slay evil */
	    tdam = tdam * 2;
	}
	else if((m_ptr->cdefense & 0x0010) && (i_ptr->flags & 0x00020000)) {
	    /* Frost */
	    tdam = tdam * 1.5;
	}
	else if((m_ptr->cdefense & 0x0020) && (i_ptr->flags & 0x00040000)) {
	    /* Fire */
	    tdam = tdam * 1.5;
	}
    }

    return tdam;
}

/* Player attacks a (poor, defenseless) creature    -RAK- */
int py_attack(int y, int x)
{
    int k;
    int blows;
    int i;
    int j;
    int tot_tohit;
    int attack;
    vtype m_name;
    vtype out_val;
    treasure_type *i_ptr;
    struct misc *p_ptr;

    attack = FALSE;
    i = cave[y][x].cptr;
    j = m_list[i].mptr;
    m_list[i].csleep = 0;

    /* Does the player know what he's fighting? */
    if((c_list[j].cmove & 0x10000) && !py.flags.see_inv) {
	strcpy(m_name, "it");
    }
    else if(py.flags.blind > 0) {
	strcpy(m_name, "it");
    }
    else if(!m_list[i].ml) {
	strcpy(m_name, "it");
    }
    else {
	sprintf(m_name, "the %s", c_list[j].name);
    }

    /* Proper weapon */
    if(inventory[INVEN_WIELD].tval != 0) {
	blows = attack_blows((int)inventory[INVEN_WIELD].weight, &tot_tohit);
    }
    else {
	/* Bare hands? */
	blows = 2;
	tot_tohit = -3;
    }

    if((inventory[INVEN_WIELD].tval >= 10) && (inventory[INVEN_WIELD].tval <= 12)) {
	/* Fix for arrows */
	blows = 1;
    }

    tot_tohit += py.misc.ptohit;

    /* Loop for number of blows, trying to hit the critter... */
    p_ptr = &py.misc;

    if(test_hit(p_ptr->bth, (int)p_ptr->lev, tot_tohit, (int)c_list[j].ac)) {
	sprintf(out_val, "You hit %s.", m_name);
	msg_print(out_val);
	i_ptr = &inventory[INVEN_WIELD];

	/* Weapon? */
	if(i_ptr->tval != 0) {
	    k = damroll(i_ptr->damage);
	    k = tot_dam(inventory[INVEN_WIELD], k, c_list[j]);
	    k = critical_blow((int)i_ptr->weight, tot_tohit, k);
	}
	else {
	    /* Bare hands!? */
	    k = damroll(bare_hands);
	    k = critical_blow(1, 0, k);
	}

	k += p_ptr->ptodam;

	if(k < 0) {
	    k = 0;
	}

	/* See if we done it in... */
	if(mon_take_hit(i, k) >= 0) {
	    sprintf(out_val, "You have slain %s.", m_name);
	    msg_print(out_val);
	    blows = 0;
	    attack = FALSE;
	}
	else {
	    /* If creature hit, but alive */
	    attack = TRUE;
	}

	i_ptr = &inventory[INVEN_WIELD];

	/* Use missiles up */
	if((i_ptr->tval >= 10) && (i_ptr->tval <= 12)) {
	    --i_ptr->number;

	    if(i_ptr->number == 0) {
		inven_weight = inven_weight - i_ptr->weight;
		--equip_ctr;
		inventory[INVEN_MAX] = inventory[INVEN_WIELD];
		inventory[INVEN_WIELD] = blank_treasure;
		py_bonuses(inventory[INVEN_MAX], -1);
	    }
	}
    }
    else {
	sprintf(out_val, "You miss %s.", m_name);
	msg_print(out_val);
    }

    --blows;

    while(blows >= 1) {
	if(test_hit(p_ptr->bth, (int)p_ptr->lev, tot_tohit, (int)c_list[j].ac)) {
	    sprintf(out_val, "You hit %s.", m_name);
	    msg_print(out_val);
	    i_ptr = &inventory[INVEN_WIELD];

	    /* Weapon? */
	    if(i_ptr->tval != 0) {
		k = damroll(i_ptr->damage);
		k = tot_dam(inventory[INVEN_WIELD], k, c_list[j]);
		k = critical_blow((int)i_ptr->weight, tot_tohit, k);
	    }
	    else {
		/* Bare hands!? */
		k = damroll(bare_hands);
		k = critical_blow(1, 0, k);
	    }

	    k += p_ptr->ptodam;

	    if(k < 0) {
		k = 0;
	    }

	    /* See if we done it in... */
	    if(mon_take_hit(i, k) >= 0) {
		sprintf(out_val, "You have slain %s.", m_name);
		msg_print(out_val);
		blows = 0;
		attack = FALSE;
	    }
	    else {
		/* If creature hit, but alive... */
		attack = TRUE;
	    }

	    i_ptr = &inventory[INVEN_WIELD];

	    /* Use missiles up */
	    if((i_ptr->tval >= 10) && (i_ptr->tval <= 12)) {
		--i_ptr->number;

		if(i_ptr->number == 0) {
		    inven_weight = inven_weight - i_ptr->weight;
		    --equip_ctr;
		    inventory[INVEN_MAX] = inventory[INVEN_WIELD];
		    inventory[INVEN_WIELD] = blank_treasure;
		    py_bonuses(inventory[INVEN_MAX], -1);
		}
	    }
	}
	else {
	    sprintf(out_val, "You miss %s.", m_name);
	    msg_print(out_val);
	}

	--blows;
    }

    return attack;
}
	    
/*
 * Moves player from one space to another...    -RAK-
 * Note: This routing has been pre-declred; see that for argument
 */
void move_char(int dir)
{
    int test_row;
    int test_col;
    int i;
    int j;
    cave_type *c_ptr;
    cave_type *d_ptr;

    test_row = char_row;
    test_col = char_col;

    /* Confused */
    if(py.flags.confused > 0) {
	/* 75% random movement */
	if(randint(4) > 1) {
	    /* Never random if sitting */
	    if(dir != 5) {
		dir = randint(9);

		/* No need for move_light() */
		find_flag = FALSE;
	    }
	}
    }

    /* Legal move? */
    if(move(dir, &test_row, &test_col)) {
	c_ptr = &cave[test_row][test_col];

	/* No creature? */
	if(c_ptr->cptr < 2) {
	    /* Open floor spot */
	    if(c_ptr->fopen) {
		/* Move character record (-1) */
		move_rec(char_row, char_col, test_row, test_col);

		/* Check for new panel */
		if(get_panel(test_row, test_col)) {
		    prt_map();
		}

		/* Check to see if he should stop */
		if(find_flag) {
		    area_affect(dir, test_row, test_col);
		}

		/* Check to see if he notices something */
		if(py.flags.blind < 1) {
		    /* fos may be negative if have good rings of searching */
		    if((py.misc.fos <= 1) || (randint(py.misc.fos) == 1) || search_flag) {
			search(test_row, test_col, py.misc.srh);
		    }
		}

		/* An object is beneath him... */
		if(c_ptr->tptr != 0) {
		    carry(test_row, test_col);
		}

		/* Move the light source */
		move_light(char_row, char_col, test_row, test_col);

		/* A room of light should be lit... */
		if(c_ptr->fval == lopen_floor.ftval) {
		    if(py.flags.blind < 1) {
			if(!c_ptr->pl) {
			    light_room(test_row, test_col);
			}
		    }
		}
		else if((c_ptr->fval == 5) || (c_ptr->fval == 6)) {
		    /* In doorway of lit room? */
		    if(py.flags.blind < 1) {
			for(i = (test_row - 1); i <= (test_row + 1); ++i) {
			    for(j = (test_col - 1); j <= (test_col + 1); ++j) {
				if(in_bounds(i, j)) {
				    d_ptr = &cave[i][j];

				    if(d_ptr->fval == lopen_floor.ftval) {
					if(!d_ptr->pl) {
					    light_room(i, j);
					}
				    }
				}
			    }
			}
		    }
		}

		/* Make final assignments of char coordinates */
		char_row = test_row;
		char_col = test_col;
	    }
	    else {
		/* Can't move onto floor space */
		/* Try a new direction if in find mode */
		if(!pick_dir(dir)) {
		    if(find_flag) {
			find_flag = FALSE;
			move_light(char_row, char_col, char_row, char_col);
		    }
		    else if(c_ptr-tptr != 0) {
			reset_flag = TRUE;

			if(t_list[c_ptr->tptr].tval == 103) {
			    msg_print("There is rubble blocking your way.");
			}
			else if(t_list[c_ptr->tptr].tval == 105) {
			    msg_print("There is a closed door blocking your way.");
			}
		    }
		    else {
			reset_flag = TRUE;
		    }
		}
	    }
	}
	else {
	    /* Attacking a creature! */
	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }

	    /* Coward? */
	    if(py.flags.afraid < 1) {
		py_attack(test_row, test_col);
	    }
	    else {
		/* Coward! */
		msg_print("You are too afraid!");
	    }
	}
    }
}

/*
 * Chest have traps too...    -RAK- 
 * Note: Chest traps are based on the FLAGS value
 */
void chest_trap(int y, int x)
{
    int i;
    int j;
    int k;
    treasure_type *t_ptr;

    t_ptr = &t_list[cave[y][x].tptr];

    if(t_ptr->flags & 0x00000010) {
	msg_print("A small needle has pricked you!");

	if(!py.flags.sustain_str) {
	    py.stats.cstr = de_statp(py.stats.cstr);
	    take_hit(damroll("1d4"), "a poison needle.");
	    print_stat |= 0x0001;
	    msg_print("You feel weakened!");

	    /* Adjust misc stats */
	    py_bonuses(blank_treasure, 0);
	}
	else {
	    msg_print("You are unaffected.");
	}
    }

    if(t_ptr->flags & 0x00000020) {
	msg_print("A small needle has pricked you!");
	take_hit(damroll("1d6"), "a poison needle.");
	py.flags.poisoned += (10 + randint(20));
    }

    if(t_ptr->flags & 0x00000040) {
	msg_print("A puff of yellow gas surrounds you!");

	if(py.flags.free_act) {
	    msg_print("You are unaffected.");
	}
	else {
	    msg_print("You choke and pass out.");
	    py.flags.paralysis = 10 + randint(20);
	}
    }

    if(t_ptr->flags & 0x00000080) {
	msg_print("There is a sudden explosion!");
	delete_object(y, x);
	take_hit(damroll("5d8"), "an exploding chest.");
    }

    if(t_ptr->flags & 0x00000100) {
	for(i = 0; i < 3; ++i) {
	    j = y;
	    k = x;
	    summon_monster(&j, &k, FALSE);
	}
    }
}

/* Opens a closed door or closed chest...    -RAK- */
void openobject()
{
    int y;
    int x;
    int tmp;
    int flag;
    char *tmp_str;
    cave_type *c_ptr;
    treasure_type *t_ptr;
    struct misc *p_ptr;

    y = char_row;
    x = char_col;

    if(get_dir("Which direction?", &tmp, &tmp, &y, &x)) {
	c_ptr = &cave[y][x];

	if(c_ptr->tptr != 0) {
	    /* Closed door */
	    if(t_list[c_ptr->tptr].tval == 105) {
		t_ptr = &t_list[c_ptr->tptr];

		/* It's locked... */
		if(t_ptr->pl > 0) {
		    p_ptr = &py.misc;
		    tmp = p_ptr->disarm + p_ptr->lev + (2 * todis_adj()) + int_adj();

		    if(py.flags.confused > 0) {
			msg_print("You are too confused to pick the lock.");
		    }
		    else if((tmp - t_ptr->p1) > randint(100)) {
			msg_print("You have picked the lock.");
			++py.misc.exp;
			prt_experience();
			t_ptr->p1 = 0;
		    }
		    else {
			msg_print("You failure to pick the lock.");
		    }
		}
		else if(t_ptr->p1 < 0) {
		    /* It's stuck */
		    msg_print("It appears to be stuck.");
		}

		if(t_ptr->p1 == 0) {
		    t_list[c_ptr->tptr] = door_list[0];
		    c_ptr->fopen = TRUE;
		    lite_spot(y, x);
		}
	    }
	    else if(t_list[c_ptr->tptr].tval == 2) {
		/* Open a closed chest... */
		p_ptr = &py.misc;
		tmp = p_ptr->disarm + p_ptr->lev + (2 * todis_adj()) + int_adj();
		t_ptr = &t_list[c_ptr->tptr];
		flag = FALSE;

		if(t_ptr->flags & 0x00000001) {
		    if(py.flags.confused > 0) {
			msg_print("You are too confused to pick the lock.");
		    }
		    else if((tmp - (2 * t_ptr->level)) > randint(100)) {
			msg_print("You have picked the lock.");
			flag = TRUE;
			py.misc.exp += t_ptr->level;
			prt_experience();
		    }
		    else {
			msg_print("You failed to pick the lock.");
		    }
		}
		else {
		    flag = TRUE;
		}

		if(flag) {
		    t_ptr->flags &= 0xFFFFFFFE;
		    tmp_str = index(t_ptr->name, '(');

		    if(tmp_str != 0) {
			tmp_str[0] = '\0';
		    }

		    strcat(t_ptr->name, " (Empty)");
		    known2(t_ptr->name);
		    t_ptr->cost = 0;
		}

		flag = FALSE;

		/* Was check still trapped? (Snicker) */
		if((t_ptr->flags & 0x00000001) == 0) {
		    chest_trap(y, x);

		    if(c_ptr->tptr != 0) {
			flag = TRUE;
		    }
		}

		/* Chest treasure is allocated as if a creature had been killed */
		if(flag) {
		    monster_death(y, x, t_list[c_ptr->tptr].flags);
		    t_list[c_ptr->tptr].flags = 0;
		}
	    }
	    else {
		msg_print("I do not see anything you can open there.");
	    }
	}
	else {
	    msg_print("I do not see anything you can open there.");
	}
    }
}

/* Closes an open door...    -RAK- */
void closeobject()
{
    int y;
    int x;
    int tmp;
    vtype out_val;
    cave_type *c_ptr;

    y = char_row;
    x = char_col;

    if(get_dir("Which direction?", &tmp, &tmp, &y, &x)) {
	c_ptr = &cave[y][x];

	if(c_ptr->tptr != 0) {
	    if(list[c_ptr->tptr].tval == 104) {
		if(c_ptr->cptr == 0) {
		    if(t_list[c_ptr->tptr].p1 == 0) {
			t_list[cptr->tptr] = door_list[1];
			c_ptr->fopen = FALSE;
			lite_spot(y, x);
		    }
		    else {
			msg_print("The door appears to be broken.");
		    }
		}
		else {
		    sprintf(out_val,
			    "The %s is in your way!",
			    c_list[m_list[c_ptr->cptr].mptr].name);

		    msg_print(out_val);
		}
	    }
	    else {
		msg_print("I do not see anything you can close there.");
	    }
	}
	else {
	    msg_print("I do not see anything you can close there.");
	}
    }
}

/* Go up one level    -RAK- */
void go_up()
{
    cave_type *c_ptr;

    c_ptr = &cave[char_row][char_col];

    if(c_ptr->tptr != 0) {
	if(t_list[c_ptr->tptr].tval == 107) {
	    --dun_level;
	    moria_flag = TRUE;
	    msg_print("You enter a maze of up staircases.");
	    msg_print("You pass through a one-way door.");
	}
	else {
	    msg_print("I see no up staircases here.");
	}
    }
    else {
	msg_print("I see no up staircases here.");
    }
}

/* Go down one level    -RAK- */
void go_down()
{
    cave_type *c_ptr;

    c_ptr = &cave[char_row][char_col];

    if(c_ptr->tptr != 0) {
	if(t_list[c_ptr->tptr].tval == 108) {
	    ++dun_level;
	    moria_flag = TRUE;
	    msg_print("You enter a maze of down staircases.");
	    msg_print("You pass through a one-way door.");
	}
	else {
	    msg_print("I see no down staircase here.");
	}
    }
    else {
	msg_print("I see no down staircase here.");
    }
}

/*
 * Tunnelling through real wall: 10, 11, 12    -RAK-
 * Used by tunnel() and wall_to_mud()
 */
int twall(int y, int x, int t1, int t2)
{
    int res;
    cave_type *c_ptr;

    res = FALSE;
    c_ptr = &cave[y][x];

    if(t1 > t2) {
	if(next_to4(y, x, 1, 2, -1) > 0) {
	    c_ptr->fval = corr_floor2.ftval;
	    c_ptr->fopen = corr_dloor2.ftopen;
	}
	else {
	    c_ptr->fval = corr_floor1.ftval;
	    c_ptr->fopen = corr_floor1.ftopen;
	}

	if(test_light(y, x)) {
	    if(panel_contains(y, x)) {
		msg_print("You have found something!");
	    }

	    lite_spot(y, x);
	}

	c_ptr->fm = FALSE;
	c_ptr->pl = FALSE;
	res = TRUE;
    }

    return res;
}

/* 
 * Tunnels through rubble and walls    -RAK-
 * Must take into account: secret doors, special tools
 */
void tunnel(int y, int x)
{
    int i;
    int tabil;
    cave_type *c_ptr;
    treasure_type *i_ptr;

    c_ptr = &cave[y][x];

    /* 
     * Compute the digging ability of player; based on strength 
     * and type of tool used
     */
    tabil = py.stats.cstr;

    if(inventory[INVEN_WIELD].tval != 0) {
	i_ptr = &inventory[INVEN_WIELD];

	if(i_ptr->flags & 0x20000000) {
	    tabil += (25 + (i_ptr->p1 * 50));
	}

	/* 
	 * Regular walls; granite, magma intrusion, quartz vein
	 * don't forget the boundary walls, made of titanium (255)
	 */
	switch(c_ptr->fval) {
	case 10:
	    i = randint(1200) + 80;

	    if(twall(y, x, tabil, i)) {
		msg_print("You have finished the tunnel.");
	    }
	    else {
		msg_print("You tunnel into the granite wall.");
	    }

	    break;
	case 11:
	    i = randint(600) + 10;

	    if(twall(y, x, tabil, i)) {
		msg_print("You have finished the tunnel.");
	    }
	    else {
		msg_print("You tunnel into the magma intrusion.");
	    }

	    break;
	case 12:
	    i = randint(400) + 10;

	    if(twall(y, x, tabil, i)) {
		msg_print("You have finished the tunnel.");
	    }
	    else {
		msg_print("You tunnel into the quartz vein.");
	    }

	    break;
	default:
	    /* Is there an object in the way? (Rubble and secret doors) */
	    if(c_ptr->tptr != 0) {
		/* Rubble... */
		if(t_list[c_ptr->tptr].tval == 103) {
		    if(tabil > randint(180)) {
			pusht((int)c_ptr->tptr);
			c_ptr->tptr = 0;
			c_ptr->fm = FALSE;
			c_ptr->fopen = TRUE;
			msg_print("You have removed the rubble.");

			if(randint(10) == 1) {
			    place_object(y, x);

			    if(test_light(y, x)) {
				msg_print("You have found something!");
			    }
			}

			lite_spot(y, x);
		    }
		    else {
			msg_print("You dig in the rubble...");
		    }
		}
		else if(t_list[c_ptr->tptr].tval == 109) {
		    /* Secret doors... */
		    msg_print("You tunnel into a granite wall.");
		    search(char_row, char_col, py.misc.srh);
		}
		else {
		    msg_print("You can't tunnel through that.");
		}
	    }
	    else {
		msg_print("Tunnel through what? Empty air???");
	    }

	    break;
	}
    }
}

/* Disarms a trap    -RAK- */
void disarm_trap()
{
    int y;
    int x;
    int i;
    int tdir;
    int tot;
    int t1;
    int t2;
    int t3;
    int t4;
    int t5;
    cave_type *c_ptr;
    treasure_type *i_ptr;
    char *tmp_str;

    y = char_row;
    x = char_col;

    if(get_dir("Which direction?", &tdir, &i, &y, &x)) {
	c_ptr = &cave[y][x];

	if(c_ptr->tptr != 0) {
	    /* Ability to disarm */
	    t1 = py.misc.disarm;

	    /* Level adjustment */
	    t2 = py.misc.lev;

	    /* Dexterity adjustment */
	    t3 = 2 * todis_adj();

	    /* Intelligence adjustment */
	    t4 = int_adj();

	    tot = t1 + t2 + t3 + t4;

	    if(py.flags.blind > 0) {
		tot = tot / 5.0;
	    }
	    else if(no_light()) {
		tot = tot / 2.0;
	    }

	    if(py.flags.confused > 0) {
		tot = tot / 3.0;
	    }

	    i = t_list[c_ptr->tptr].tval;
	    t5 = t_list[c_ptr->tptr].level;

	    /* Floor trap */
	    if(i == 102) {
		i_ptr = &t_list[c_ptr->tptr];

		if((tot - t5) > randint(100)) {
		    msg_print("You have disarmed the trap.");
		    py.misc.exp += i_ptr->p1;
		    c_ptr->fm = FALSE;
		    pusht((int)c_ptr->tptr);
		    c_ptr->tptr = 0;
		    move_char(tdir);
		    lite_spot(y, x);
		    prt_experience();
		}
		else if((tot > 5) && (randint(tot) > 5)) {
		    /* Avoid randint(0) call */
		    msg_print("You failed to disarm the trap.");
		}
		else {
		    msg_print("You set the trap off!");
		    move_char(tdir);
		}
	    }
	    else if(i == 2) {
		/* Chest trap */
		i_ptr = &t_list[c_ptr->tptr];

		if(index(i_ptr->name, "^") != 0) {
		    msg_print("I don't see a trap...");
		}
		else if(i_ptr->flags & 0x000001F0) {
		    if((tot - t5) > randint(100)) {
			i_ptr->flags &= 0xFFFFFE0F;
			tmp_str = index(i_ptr->name, "(");

			if(tmp_str != 0) {
			    tmp_str[0] = '\0';
			}

			if(i_ptr->flags & 0x00000001) {
			    strcat(i_ptr->name, " (Locked)");
			}
			else {
			    strcat(i_ptr->name, " (Disarmed)");
			}

			msg_print("Your have disarmed the chest.");
			known2(i_ptr->name);
			py.misc.exp += t5;
			prt_experience();
		    }
		    else if((tot > 5) && (randint(tot) > 5)) {
			msg_print("You have failed to disarm the chest.");
		    }
		    else {
			msg_print("You set a trap off!");
			known2(i_ptr->name);
			chest_trap(y, x);
		    }
		}
		else {
		    msg_print("The chest was not trapped.");
		}
	    }
	    else {
		msg_print("I do not see anything to disarm there.");
	    }
	}
	else {
	    msg_print("I do not see anything to disarm there.");
	}
    }
}

/*
 * Look at an object, trap, or monster    -RAK-
 * Note: Looking is a free move, see where invoked...
 */
void look()
{
    int i;
    int j;
    int y;
    int x;
    int dir;
    int dummy;
    int flag;
    char fchar;
    cave_type *c_ptr;
    vtype out_val;
    vtype tmp_str;

    flag = FALSE;
    y = char_row;
    x = char_col;

    if(get_dir("Look which direction?", &dir, &dummy, &y, &x)) {
	if(py.flags.blind < 1) {
	    y = char_row;
	    x = char_col;
	    i = 0;

	    move(dir, &y, &x);
	    c_ptr = &cave[y][x];

	    if(c_ptr->cptr > 1) {
		if(m_list[c_ptr->cptr].ml) {
		    j = m_list[c_ptr->cptr].mptr;
		    fchar = c_list[j].name[0];

		    if(is_a_vowel(fcha)) {
			sprintf(out_val, "You see an %s.", c_list[j].name);
		    }
		    else {
			sprintf(out_val, "You see a %s.", c_list[j].name);
		    }

		    msg_print(out_val);

		    flag = TRUE;
		}
	    }

	    if(c_ptr->tl || c_ptr->pl || c_ptr->fm) {
		if(c_ptr->tptr != 0) {
		    if(t_list[c_ptr->tptr].tval == 109) {
			msg_print("You see a granite wall.");
		    }
		    else if(t_list[c_ptr->tptr].tval != 101) {
			inventory[INVEN_MAX] = t_list[c_ptr->tptr];
			objdes(tmp_str, INVEN_MAX, TRUE);
			sprintf(out_val, "You see %s", tmp_str);
			msg_print(out_val);
			flag = TRUE;
		    }
		}

		if(!c_ptr->fopen) {
		    flag = TRUE;

		    switch(c_ptr->fval) {
		    case 10:
			msg_print("You see a granite wall.");

			break;
		    case 11:
			msg_print("You see some dark rock.");

			break;
		    case 12:
			msg_print("You see a quartz vein.");

			break;
		    case 15:
			msg_print("You see a granite wall.");

			break;
		    default:

			break;
		    }
		}
	    }

	    ++i;

	    while(cave[y][x].fopen && (i <= MAX_SIGHT)) {
		move(dir, &y, &x);
		c_ptr = &cave[y][x];

		if(c_ptr->cptr > 1) {
		    if(m_list[c_ptr->cptr].ml) {
			j = m_list[c_ptr->cptr].mptr;
			fchar = c_list[j].name[0];

			if(is_a_vowel(fchar)) {
			    sprintf(out_val, "You see an %s.", c_list[j].name);
			}
			else {
			    sprintf(out_val, "You see a %s.", c_list[j].name);
			}

			msg_print(out_val);
			flag = TRUE;
		    }
		}

		if(c_ptr->tl || c_ptr->pl || c_ptr->fm) {
		    if(c_ptr->tptr != 0) {
			if(t_list[c_ptr->tptr].tval == 109) {
			    msg_print("You see a granite wall.");
			}
			else if(t_list[c_ptr->tptr].tval != 101) {
			    inventory[INVEN_MAX] = t_list[c_ptr->tptr];
			    objdes(tmp_str, INVEN_MAX, TRUE);
			    sprintf(out_val, "You see %s", tmp_str);
			    msg_print(out_val);
			    flag = TRUE;
			}
		    }

		    if(!c_ptr->fopen) {
			flag = TRUE;

			switch(c_ptr->fval) {
			case 10:
			    msg_print("You see a granite wall.");

			    break;
			case 11:
			    msg_print("You see some dark rock.");

			    break;
			case 12:
			    msg_print("You see a quartz vein.");

			    break;
			case 15:
			    msg_print("You see a granite wall.");

			    break;
			default:

			    break;
			}
		    }
		}

		++i;
	    }

	    if(!flag) {
		msg_print("You see nothing of interest in that direction.");
	    }
	}
    }
    else {
	msg_print("You can't see a damn thing!");
    }
}

/* Add to the player's food time    -RAK- */
void add_food(int num)
{
    struct flags *p_ptr;

    p_ptr = &py.flags;

    if(p_ptr->food < 0) {
	p_ptr->food = 0;
    }

    p_ptr->food += num;

    if(p_ptr->food > PLAYER_FOOD_MAX) {
	msg_print("You are bloated from overeating.");
	p_ptr->slow = (p_ptr->food - PLAYER_FOOD_MAX) / 50;
	p_ptr->food = PLAYER_FOOD_MAX;
    }
    else if(p_ptr->food > PLAYER_FOOD_FULL) {
	msg_print("You are full.");
    }
}

/* Describe number of remaining charges...    -RAK- */
void desc_charges(int item_val)
{
    int rem_num;
    vtype out_val;

    if(index(inventory[item_val].name, '^') == 0) {
	rem_num = inventory[item_val].p1;
	sprintf(out_val, "You have %d charges remaining.", rem_num);
	msg_print(out_val);
    }
}

/* Describe amount of item remaining...    -RAK- */
void desc_remain(int item_val)
{
    vtype out_val;
    vtype tmp_str;
    treasure_type *i_ptr;

    inventory[INVEN_MAX] = inventory[item_val];
    i_ptr = &inventory[INVEN_MAX];
    --i_ptr->number;
    objdes(tmp_str, INVEN_MAX, TRUE);
    tmp_str[strlen(tmp_str) - 1] = '\0';
    sprintf(out_val, "You have %s.", tmp_str);
    msg_print(out_val);
}

void inven_throw(int item_val)
{
    treasure_type *i_ptr;

    inventory[INVEN_MAX] = inventory[item_val];
    inventory[INVEN_MAX].number = 1;
    i_ptr = &inventory[item_val];

    if((i_ptr->number > 1) && (i_ptr->subval > 511)) {
	--i_ptr->number;
	inven_weight -= i_ptr->weight;
    }
    else {
	inven_destroy(item_val);
    }
}

void facts(int *tbth, int *tpth, int *tdam, int *tdis)
{
    int tmp_weight;
    treasure_type *i_ptr;

    i_ptr = &inventory[INVEN_MAX];

    if(i_ptr->weight < 1) {
	tmp_weight = 1;
    }
    else {
	tmp_weight = i_ptr->weight;
    }

    /* Throwing objects */
    *tdam = damroll(i_ptr->damage) + i_ptr->todam;
    *tbth = py.misc.bthb * 0.75;
    *tpth = py.misc.ptohit + i_ptr->tohit;
    *tdis = ((py.stats.cstr  + 20) * 10) / tmp_weight;

    if(*tdis > 10) {
	*tdis = 10;
    }

    /* Using bows, slings, or crossbows */
    if(inventory[INVEN_WIELD].tval == 20) {
	switch(inventory[INVEN_WIELD].p1) {
	case 1:
	    /* Sling and bullet */
	    if(i_ptr->tval == 10) {
		*tbth = py.misc.bthb;
		*tpth += inventory[INVEN_WIELD].tohit;
		*tdam += 2;
		*tdis = 20;
	    }

	    break;
	case 2:
	    /* Short bow and arrow */
	    if(i_ptr->tval == 12) {
		*tbth = py.misc.bthb;
		*tpth += inventory[INVEN_WIELD].tohit;
		*tdam += 2;
		*tdis = 25;
	    }

	    break;
	case 3:
	    /* Long bow and arrow */
	    if(i_ptr->tval == 12) {
		*tbth = py.misc.bthb;
		*tpath += inventory[INVEN_WIELD].tohit;
		*tdam += 3;
		*tdis = 30;
	    }

	    break;
	case 4:
	    /* Composite bow and arrow */
	    if(i_ptr->tval == 12) {
		*tbth = py.misc.bthb;
		*tpth += inventory[INVEN_WIELD].tohit;
		*tdam += 4;
		*tdis = 35;
	    }

	    break;
	case 5:
	    /* Light crossbow and bolt */
	    if(i_ptr->tval == 11) {
		*tbth = py.misc.bthb;
		*tpth += inventory[INVEN_WIELD].tohit;
		*tdam += 2;
		*tdis = 25;
	    }

	    break;
	case 6:
	    /* Heavy crossbow and bolt */
	    if(i_ptr->tval == 11) {
		*tbth = py.misc.bthb;
		*tpth += inventory[INVEN_WIELD].tohit;
		*tdam += 4;
		*tdis = 35;
	    }

	    break;
	}
    }
}

void drop_throw(int y, int x)
{
    int i;
    int j;
    int k;
    int flag;
    int cur_pos;
    vtype out_val;
    vtype tmp_str;
    cave_type *c_ptr;

    flag = FALSe;
    i = y;
    j = x;
    k = 0;

    if(randint(10) > 1) {
	if(in_bounds(i, j)) {
	    c_ptr = &cave[i][j];

	    if(c_ptr->fopen) {
		if(c_ptr->tptr == 0) {
		    flag = TRUE;
		}
	    }
	}

	if(!flag) {
	    i = y + randint(3) - 2;
	    j = x + randint(3) - 2;
	    ++k;
	}


	while(!flag && (k <= 9)) {
	    if(in_bounds(i, j)) {
		c_ptr = &cave[i][j];

		if(c_ptr->fopen) {
		    if(c_ptr->tptr == 0) {
			flag = TRUE;
		    }
		}
	    }

	    if(!flag) {
		i = y + randint(3) - 2;
		j = x + randint(3) - 2;
		++k;
	    }
	}
    }

    if(flag) {
	popt(&cur_pos);
	cave[i][j].tptr = cur_pos;
	t_list[cur_pos] = inventory[INVEN_MAX];

	if(test_light(i, j)) {
	    lite_spot(i, j);
	}
    }
    else {
	objdes(tmp_str, INVEN_MAX, FALSE);
	sprintf(out_val, "The %s disappears", tmp_str);
	msg_print(out_val);
    }
}

/*
 * Throw an object across the dungeon...    -RAK-
 * Note: Flasks of oil do fire damage
 *       Extra damage and chance of hitting when missiles are used with 
 *       correct weapon. i.e. Wield bow and throw arrow.
 */
void throw_object()
{
    int item_val;
    int tbth;
    int tpth;
    int tdam;
    int tdis;
    int y_dumy;
    int x_dumy;
    int dumy;
    int y;
    int x;
    int oldy;
    int oldx;
    int dir;
    int cur_dis;
    int redraw;
    int flag;
    char tchar[2];
    vtype out_val;
    int tmp_str;
    int m_name;
    treasure_type *i_ptr;
    cave_type *c_ptr;
    monster_type *m_ptr;
    int i;

    redraw = FALSE;

    if(inven_ctr == 0) {
	msg_print("But you are not carrying anything.");
    }
    else if(get_item(&item_val, "Fire/Throw which one?", &redraw, 0, inven_ctr - 1)) {
	if(redraw) {
	    draw_cave();
	}

	y_dumy = char_row;
	x_dumy = char_col;

	if(get_dir("Which direction?", &dir, &dumy, &y_dumy, &x_dumy)) {
	    desc_remain(item_val);

	    if(py.flags.confused > 0) {
		msg_print("You are confused...");

		dir = randint(9);

		while(dir == 5) {
		    dir = randint(9);
		}
	    }

	    inven_throw(item_val);
	    facts(&tbth, &tpth, &tdam, &tdis);
	    i_ptr = &inventory[INVEN_MAX];
	    tchar[0] = i_ptr->tchar;
	    tchar[1] = '\0';
	    flag = FALSE;
	    y = char_row;
	    x = char_col;
	    oldy = char_row;
	    oldx = char_col;
	    cur_dis = 0;

	    move(dir, &y, &x);
	    ++cur_dis;

	    if(test_light(oldy, oldx)) {
		lite_spot(oldx, oldy);
	    }

	    if(cur_dis > tdis) {
		flag = TRUE;
	    }

	    c_ptr = &cave[y][x];

	    if(c_ptr->fopen && !flag) {
		if(c_ptr->cptr > 1) {
		    flag = TRUE;
		    m_ptr = &m_list[c_ptr->cptr];
		    tbth = tbth - cur_dis;

		    if(test_hit(tbth, (int)py.misc.lev, tpth, (int)c_list[m_ptr->mptr].ac)) {
			i = m_ptr->mptr;
			objdes(tmp_str, INVEN_MAX, FALSE);

			/* Does the player know what he's fighting? */
			if((c_list[i].cmove & 0x10000) && !py.flags.see_inv) {
			    strcpy(m_name, "it");
			}
			else if(py.flags.blind > 0) {
			    strcpy(m_name, "it");
			}
			else if(!m_ptr->ml) {
			    strcpy(m_name, "it");
			}
			else {
			    sprintf(m_name, "the %s", c_list[i].name);
			}

			sprintf(out_val, "The %s hits the %s.", tmp_str, m_name);
			msg_print(out_val);
			tdam = tot_dam(inventory[INVEN_MAX], tdam, c_list[i]);
			i_ptr = &inventory[INVEN_MAX];
			tdam = critical_blow((int)i_ptr->weight, tpth, tdam);
			i = mon_take_hit((int)c_ptr->cptr, tdam);

			if(i >= 0) {
			    sprintf(out_val, "You have killed %s.", m_name);
			    msg_print(out_val);
			}
		    }
		    else {
			drop_throw(oldy, oldx);
		    }
		}
		else {
		    if(panel_contains(y, x)) {
			if(test_light(y, x)) {
			    print(tchar, y, x);
			    put_qio();
			}
		    }
		}
	    }
	    else {
		flag = TRUE;
		drop_throw(oldy, oldx);
	    }

	    oldy = y;
	    oldx = x;

	    while(!flag) {
		move(dir, &y, &x);
		++cur_dis;

		if(test_light(oldy, oldx)) {
		    lite_spot(oldy, oldx);
		}

		if(cur_dis > tdis) {
		    flag = TRUE;
		}

		c_ptr = &cave[y][x];

		if(c_ptr->fopen && !flag) {
		    if(c_ptr->cptr > 1) {
			flag = TRUE;
			m_ptr = &m_list[c_ptr->cptr];
			tbth = tbth - cur_dis;

			if(test_hit(tbth, (int)py.misc.lev, tpth, (int)c_list[m_ptr->mptr].ac)) {
			    i = m_ptr->mptr;
			    objdes(tmp_str, INVEN_MAX, FALSE);

			    /* Does the player know what he's fightint? */
			    if((c_list[c].cmove & 0x10000) && !py.flags.see_inv) {
				strcpy(m_name, "it");
			    }
			    else if(py.flags.blind > 0) {
				strcpy(m_name, "it");
			    }
			    else if(!m_ptr->ml) {
				strcpy(m_name, "it");
			    }
			    else {
				sprintf(m_name, "the %s", c_list[i].name);
			    }

			    sprintf(out_val, "The %s hits %s.", tmp_str, m_name);
			    msg_print(out_val);
			    tdam = tot_dam(inventory[INVEN_MAX], tdam, c_list[i]);
			    i = mon_take_hit((int)c_ptr->cptr, tdam);

			    if(i >= 0) {
				sprintf(out_val, "You have killed %s.", m_name);
				msg_print(out_val);
			    }
			}
			else {
			    drop_throw(oldy, oldx);
			}
		    }
		    else {
			if(panel_contains(y, x)) {
			    if(test_light(y, x)) {
				print(tchar, y, x);
				put_qio();
			    }
			}
		    }
		}
		else {
		    flag = TRUE;
		    drop_throw(oldy, oldx);
		}

		oldy = y;
		oldx = x;
	    }
	}
    }
    else {
	if(redraw) {
	    draw_cave();
	}
    }
}

/*
 * Bash open a door or chest    -RAK-
 * Note: Affected by strenght and weight of character
 */
void bash()
{
    int y;
    int x;
    int tmp;
    int old_ptodam;
    int old_ptohit;
    int old_bth;
    vtype tmp_str;
    vtype m_name;
    cave_type *c_ptr;
    treasure_type *i_ptr;
    treasure_type *t_ptr;
    player_type *p_ptr;
    monster_type *m_ptr;

    y = char_row;
    x = char_col;

    if(get_dir("Which direction?", &tmp, &tmp, &y, &x)) {
	c_ptr = &cave[y][x];

	if(c_ptr->cptr > 1) {
	    if(py.flags.afraid > 0) {
		msg_print("You are afraid!");
	    }
	    else {
		/* Save old values of attacking */
		inventory[INVEN_MAX] = inventory[INVEN_WIELD];
		old_ptohit = py.misc.ptohit;
		old_ptodam = py.misc.ptodam;
		old_bth = py.misc.bth;

		/* Use these values */
		inventory[INVEN_WIELD] = blank_treasure;
		i_ptr = &inventory[INVEN_WIELD];
		strcpy(i_ptr->damage, inventory[26].damage);
		i_ptr->weight = py.stats.cstr;
		i_ptr->tval = 1;
		p_ptr = &py;
		p_ptr->misc.bth = (p_ptr->stats.cstr + p_ptr->misc.wt) / 6.0;
		p_ptr->misc.ptohit = 0;
		p_ptr->misc.ptodam = (p_ptr->misc.wt / 75.0) + 1;

		if(py.attack(y, x)) {
		    m_ptr = &m_list[c_ptr->cptr];
		    m_ptr->stunned += (randint(2) + 1);

		    if(m_ptr->stunned > 24) {
			m_ptr->stunned = 24;
		    }

		    /* Does the player know what he's fighting? */
		    if((c_list[m_ptr->mptr].cmove & 0x10000) && !py.flags.see_inv) {
			strcpy(m_name, "It");
		    }
		    else if(py.flags.blind > 0) {
			strcpy(m_name, "It");
		    }
		    else if(!m_list[c_ptr->cptr].ml) {
			strcpy(m_name, "It");
		    }
		    else {
			sprintf(m_name, "The %s", c_list[m_ptr->mptr].name);
		    }

		    sprintf(tmp_str, "%s appears stunned!", m_name);
		    msg_print(tmp_str);
		}

		/* Restore old values */
		inventory[INVEN_WIELD] = inventory[INVEN_MAX];
		py.misc.ptohit = old_ptohit;
		py.misc.ptodam = old_ptodam;
		py.misc.bth = old_bth;

		if(randint(140) > py.stats.cdex) {
		    msg_print("You are off-balance.");
		    py.flags.paralysis = randint(3);
		}
	    }
	}
	else if(c_ptr->tptr != 0) {
	    t_ptr = &t_list[c_ptr->tptr];

	    if(t_ptr->tval == 105) {
		msg_print("You smash into the door!");
		p_ptr = &py;

		if(test_hit((int)(p_ptr->misc.wt + p_ptr->stats.cstr), 0, 0, abs(t_ptr->p1) + 150)) {
		    msg_print("The door crashes open!");
		    t_list[c_ptr->tptr] = door_list[0];
		    t_ptr->p1 = 1;
		    c_ptr->fopen = TRUE;
		    lite_spot(y, x);
		}
		else {
		    msg_print("The door holds firm.");
		    py.flags.paralysis = 2;
		}
	    }
	    else if(t_ptr->tval == 2) {
		if(randint(10) == 1) {
		    msg_print("You have destroyed the chest...");
		    msg_print("and it's contents!");
		    strcpy(t_ptr->name, "& ruiuned chest");
		    t_ptr->flags = 0;
		}
		else if(t_ptr->flags & 0x00000001) {
		    if(randint(10) == 1) {
			msg_print("The lock breaks open!");
			t_ptr->flag &= 0xFFFFFFFE;
		    }
		}
	    }
	    else {
		msg_print("I do not see anything you can bash there."};
	    }
	}
	else {
	    msg_print("I do not see anything you can bash there.");
	}
    }
}

/* Jam a closed door    -RAK- */
void jamdoor()
{
    int y;
    int x;
    int tmp;
    int i;
    int j;
    cave_type *c_ptr;
    treasure_type *t_ptr;
    treasure_type *i_ptr;
    char tmp_str[80];

    y = char_row;
    x = char_col;

    if(get_dir("Which direction?", &tmp, &tmp, &y, &x)) {
	c_ptr = &cave[y][x];

	if(c_ptr->tptr != 0) {
	    t_ptr = &t_list[c_ptr->tptr];

	    if(c_ptr->tval == 105) {
		if(c_ptr->cptr == 0) {
		    if(find_range(13, -1, &i, &j)) {
			msg_print("You jam the door with a spike.");
			i_ptr = &inventory[i];

			if(i_ptr->number > 1) {
			    --i_ptr->number;
			}
			else {
			    inven_destroy(i);
			}

			t_ptr->p1 = -abs(i_ptr->p1) - 20;
		    }
		    else {
			msg_print("But you have no spikes...");
		    }
		}
		else {
		    sprintf(tmp_str,
			    "The %s is in your way!",
			    c_list[m_list[c_ptr->cptr].mptr].name);

		    msg_print(tmp_str);
		}
		else if(t_ptr->tval == 104) {
		    msg_print("The door must be closed first.");
		}
		else {
		    msg_print("That isn't a door!");
		}
	    }
	}
	else {
	    msg_print("That isn't a door!");
	}
    }
}

/* Refill the player's lamp    -RAK- */
void refill_lamp()
{
    int i;
    int j;
    int k;
    treasure_type *i_ptr;

    k = inventory[32].subval;

    if((k > 0) && (k < 10)) {
	if(find_range(77, -1, &i, &j)) {
	    i_ptr = &inventory[32];
	    i_ptr->p1 += inventory[i].p1;

	    if(i_ptr->p1 > OBJ_LAMP_MAX) {
		i_ptr->p1 = OBJ_LAMP_MAX;
		msg_print("Your lamp overflows, spilling oil on the ground.");
		msg_print("Your lam is full.");
	    }
	    else if(i_ptr->p1 > (OBJ_LAMP_MAX / 2)) {
		msg_print("Your lamp is more than half full.");
	    }
	    else if(i_ptr->p1 == (OBJ_LAMP_MAX / 2)) {
		msg_print("Your lamp is half full.");
	    }
	    else {
		msg_print("You lamp is less than half full.");
	    }

	    desc_remain(i);
	    inven_destroy(i);
	}
	else {
	    msg_print("You have no oil.");
	}
    }
    else {
	msg_print("But you are not using a lamp.");
    }
}
		
