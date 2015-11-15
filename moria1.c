#include "moria1.h"

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

byteint de_statt();
byteint in_statt();

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
extern int light_cave;

/* Changes stats up or down for magic items    -RAK- */
void change_stat_factor(byteint *stat, int amoonut, int factor)
{
    int i;
    int j;
    int k;

    j = amount * factor;

    if(amount < 0) {
	k = -amount;
    }
    else {
	k = amount;
    }

    for(i = 0; i < k; ++i) {
	if(j < 0) {
	    *stat = de_statt(*stat);
	}
	else {
	    *stat = in_statt(*stat);
	}
    }
}

/*
 * Changes speed of monsters relative to player -RAK- 
 * Note: When the player is sped up or slowed down, I simply change the speed
 * of all the mosnters. This greatly simplified the logic...
 */
void change_speed(int num)
{
    int i;

    py.flags.speed += num;
    i = muptr;

    while(i > 0) {
	m_list[i].cspeed += num;
	i = m_list[u].nptr;
    }
}

/*
 * Player bonuses    -RAK-
 * When an item is worn or taken off, this readjusts the player bonuses.
 * Factor == 1 : wear; Factor == -1 : removed
 */
void py_bonuses(treasure_type tobj, int factor)
{
    unsigned int item_flags;
    int old_dis_ac;
    struct flags *p_ptr;
    struct misc *m_ptr;
    treasure_type *i_ptr;
    int i;

    p_ptr = &py.flags;
    m_ptr = &py.misc;

    if(p_ptr->slow_digest) {
	++p_ptr->food_digested;
    }

    if(p_ptr->regenerate) {
	p_ptr->food_digested -= 3;
    }

    p_ptr->see_inv = FALSE;
    p_ptr->teleport = FALSE;
    p_ptr->free_act = FALSE;
    p_ptr->slow_digest = FALSE;
    p_ptr->aggravate = FALSE;
    p_ptr->sustain_str = FALSE;
    p_ptr->sustain_int = FALSE;
    p_ptr->sustain_wis = FALSE;
    p_ptr->sustain_con = FALSE;
    p_ptr->sustain_dex = FALSE;
    p_ptr->sustain_chr = FALSE;
    p_ptr->fire_resist = FALSE;
    p_ptr->acid_resist = FALSE:
    p_ptr->cold_resist = FALSE;
    p_ptr->regnenerate = FALSE;
    p_ptr->lght_resist = FALSE;
    p_ptr->ffall = FALSE;

    if(tobj.flags & 0x00000001) {
	change_stat_factor(&py.stats.cstr, tobj.p1, factor);
	change_stat_factor(*py.stats.str, tobj.p1, factor);
	print_stat = (print_stat | 0x0001);
    }

    if(tobj.flags & 0x00000002) {
	change_stat_factor(&py.stats.cdex, tobj.p1, factor);
	change_stat_factor(&py.stats.dex, tobj.p1, factor);
	print_stat = (print_stat | 0x0002);
    }

    if(tobj.flags & 0x00000004) {
	change_stat_factor(&py.stats.ccon, tobj.p1, factor);
	change_stat_factor(&py.stats.con, tocj.p1, factor);
	print_stat = (print_stat | 0x0004);
    }

    if(tobj.flags & 0x00000008) {
	change_stat_factor(&py.stats.cint, tobj.p1, factor);
	change_stat_factor(&py.stats.intel, tobj.p1, factor);
	print_stat = (print_stat | 0x0008);
    }

    if(tobj.flags & 0x00000010) {
	change_stat_factor(&py.stats.cwis, tobj.p1, factor);
	change_stat_factor(&py.stats.wis, tobj.p1, factor);
	print_stat = (print_stat | 0x0010);
    }

    if(tobj.flags & 0x00000020) {
	change_stat_factor(&py.stats.cchr, tobj.p1, factor);
	change_stat_factor(&py.stats.chr, tobj.p1, factor);
	print_stat = (print_stat | 0x0020);
    }

    if(tobj.flags & 0x00000040) {
	m_ptr->srh += (tobj.p1 * factor);
	m_ptr->fos -= (tobj.p1 * factor);
    }

    if(tobj.flags & 0x00000100) {
	m_ptr->stl += (2 * factor);
    }

    if(tobj.flags & 0x0001000) {
	i = tobj.p1 * factor;
	change_speed(-i);
    }

    if(tobj.flags & 0x08000000) {
	if(factor > 0) {
	    p_ptr->blind += 1000;
	}
    }

    if(tobj.flags & 0x10000000) {
	if(factor > 0) {
	    p_ptr->afraid += 50;
	}
    }

    if(tobj.flags & 0x40000000) {
	p_ptr->see_infra += (tobj.p1 * factor);
    }

    old_dis_ac = m_ptr->dis_ac;

    /* Real to hit */
    m_ptr->ptohit = tohit_adj();

    /* Real to dam */
    m_ptr->ptodam = todam_adj();

    /* Real to AC */
    m_ptr->ptoac = toac_adj();

    /* Real AC */
    m_ptr->pac = 0;

    /* Display to hit */
    m_ptr->dis_th = m_ptr->ptohit;

    /* Display to dam */
    m_ptr->dis_td = m_ptr->ptodam;

    /* Dispaly to AC */
    m_ptr->dis_ac = 0;

    /* Display AC */
    m_ptr->dis_tac = m_ptr->ptoac;

    for(i = 22; i < (INVEN_MAX - 2); ++i) {
	i_ptr = &inventory[i];

	if(i_ptr->tval != 0) {
	    if((i_ptr->flags & 0x80000000) == 0) {
		m_ptr->pac += i_ptr->ac;
		m_ptr->dis_ac += i_ptr->ac;
	    }

	    m_ptr->ptohit += i_ptr->tohit;
	    m_ptr->ptodam += i_ptr->todam;
	    m_ptr->ptoac += i_ptr->toac;

	    if(index(i_ptr->name, '^') == 0) {
		m_ptr->dis_th += i_ptr->tohit;
		m_ptr->dis_td += i_ptr->todam;
		m_ptr->dis_tac += i_ptr->toac;
	    }
	}
    }

    m_ptr->dis_ac += m_ptr->dis_tac;

    /* Add in temporary spell increases */
    if(p_ptr->invuln > 0) {
	m_ptr->pac += 100;
	m_ptr->dis_ac += 100;
    }

    if(p_ptr->blessed > 0) {
	m_ptr->pac += 2;
	m_ptr->dis_ac += 2;
    }

    if(p_ptr->detect_inv > 0) {
	p_ptr->see_inv = TRUE;
    }

    if(old_dis_ac != m_ptr->dis_ac) {
	print_stat = (print_stat | 0x0040);
    }

    item_flags = 0;

    for(i = 22; i < (INVEN_MAX - 2); ++i) {
	i_ptr = &inventory[i];
	item_flags |= i_ptr->flags;
    }

    if(item_flags & 0x00000080) {
	p_ptr->slow_digest = TRUE;
    }
    
    if(item_flags & 0x00000200) {
	p_ptr->aggravate = TRUE;
    }

    if(item_flags & 0x00000400) {
	p_ptr->teleport = TRUE;
    }

    if(item_flags & 0x00000800) {
	p_ptr->regenerate = TRUE;
    }

    if(item_flags & 0x00080000) {
	p_ptr->fire_resist = TRUE;
    }

    if(item_flags & 0x00100000) {
	p_ptr->acid_resist = TRUE;
    }

    if(item_flags & 0x00200000) {
	p_ptr->cold_resist = TRUE;
    }

    if(item_flags & 0x00800000) {
	p_ptr->free_act = TRUE;
    }

    if(item_flags & 0x01000000) {
	p_ptr->see_inv = TRUE;
    }

    if(item_flags & 0x02000000) {
	p_ptr->lght_resist = TRUE;
    }

    if(item_flags & 0x04000000) {
	p_ptr->ffall = TRUE;
    }

    for(i = 22; i < (INVEN_MAX - 2); ++i) {
	i_ptr = &inventory[i];

	if(i_ptr->flags & 0x00400000) {
	    switch(i_ptr->p1) {
	    case 1:
		p_ptr->sustain_str = TRUE;

		break;
	    case 2:
		p_ptr->sustain_int = TRUE;

		break;
	    case 3:
		p_ptr->sustain_wis = TRUE;

		break;
	    case 4:
		p_ptr->sustain_con = TRUE;

		break;
	    case 5:
		p_ptr->sustain_dex = TRUE;

		break;
	    case 6:
		p_ptr->sustain_chr = TRUE;

		break;
	    default:

		break;
	    }
	}
    }

    if(p_ptr->slow_digest) {
	--p_ptr->food_digested;
    }

    if(p_ptr->regenerate) {
	p_ptr->food_digested += 3;
    }
}

/*
 * Returns a '*' for cursed items, a ')' for normal ones    -RAK-
 * Note: '*' returned only if item has been identified...
 */
char cur_char1(int item_val)
{
    treasure_type *i_ptr;

    i_ptr = &inventory[item_val];

    if((i_ptr->flags & 0x80000000) == 0) {
	/* Not cursed... */
	return ')';
    }
    else if(index(i_ptr->name, '^') != 0) {
	/* Cursed, but not identified */
	return ')';
    }
    else {
	/* Cursed and identified... */
	return '*';
    }
}

/* Returns a '*' for cursed items, a ')' for normal ones    -RAK- */
char cur_char2(int item_val)
{
    treasure_type *i_ptr;

    i_ptr = &inventory[item_val];

    if((i_ptr->flags & 0x80000000) == 0) {
	/* Not cursed... */
	return ')';
    }
    else {
	/* Cursed... */
	return '*';
    }
}

/* Inventory functions, define some global variables here */
/*
 * scr_state == 0 : Normal screen (i.e. map of dungeon)
 *                  or partial inventory list, (calling function sets redraw)
 * scr_state == 1 : Inventory is displayed on the screen
 * scr_state == 2 : Equipment list is displayed on the screen
 */
int scr_state;

/* Displays inventory items from r1 to r2    -RAK- */
void show_inven(int r1, int r2)
{
    int i;
    vtype tmp_val;
    vtype out_val;

    /* r1 == 0 dummy call */
    if(r1 >= 0) {
	/* Print the items */
	for(i = r1; i <= r2; ++i) {
	    objdest(tmp_val, i, TRUE);
	    sprintf(out_val, "%c%c %S", i + 97, cur_char1(i), tmp_val);
	    prt(out_val, i + 1, 0);
	}

	if(r2 < 22) {
	    /* Clear line after */
	    prt("", r2 + 2, 0);
	}

	/* Set state to 1 */
	scr_state = 1;
    }
}

/* Displays equipment items from r1 to end    -RAK- */
void show_equip(int r1)
{
    int i;
    int j;
    vtype prt1;
    vtype prt2;
    vtype out_val;
    treasure_type *i_ptr;

    /* Last item gone */
    if(r1 >= equip_ctr) {
	/* Clear the line */
	prt("", equip_ctr + 2, 0);
    }
    else if(r1 >= 0) {
	/* r1 == 0 dummy call */

	j = 0;

	/* Range of equipment */
	for(i = 22; i < INVEN_MAX; ++i) {
	    i_ptr = &inventory[i];

	    if(i_ptr->tval != 0) {
		/* Display only given range */

		if(j >= r1) {
		    /* Get position */
		    switch(i) {
		    case 22:
			strcpy(prt1, " You are wielding   : ");

			break;
		    case 23:
			strcpy(prt1, " Worn on head       : ");

			break;
		    case 24:
			strcpy(prt1, " Worn around neck   : ");

			break;
		    case 25:
			strcpy(prt1, " Worn on body       : ");

			break;
		    case 26:
			strcpy(prt1, " Worn on arm        : ");

			break;
		    case 27:
			strcpy(prt1, " Worn on hands      : ");

			break;
		    case 28:
			strcpy(prt1, " Worn on right hand : ");

			break;
		    case 29:
			strcpy(prt1, " Worn on left hand  : ");

			break;
		    case 30:
			strcpy(prt1, " Worn on feet       : ");

			break;
		    case 31:
			strcpy(prt1, " Worn about body    : ");

			break;
		    case 32:
			strcpy(prt1, " Light source       : ");

			break;
		    case 33:
			strcpy(prt1, " Secondary weapon   : ");

			break;
		    default:
			strcpy(prt1, " Unknown value      : ");

			break;
		    }

		    objdes(prt2, i, TRUE);
		    sprintf(out_val, "%c%c%s%s", j + 97, cur_char2(i), prt1, prt2);
		    prt(out_val, j + 2, 0);
		}

		++j;
	    }
	}

	/* Clear last line */
	prt("", j + 2, 0);

	/* Set state of screen */
    }
}

/* Remove item from equipment list    -RAK- */
int remove(int item_val)
{
    int i;
    int j;
    int typ;
    vtype out_val;
    vtype prt1;
    vtype prt2;
    int flag;
    treasure_type *i_ptr;

    i = 0;
    flag = FALSE;
    typ = inventory[item_val].tval;

    i_ptr = &inventory[i];

    if(typ > i_ptr->tval) {
	for(j = (inven_ctr - 1); j >= i; --j) {
	    inventory[j + 1] = inventory[j];
	}

	inventory[i] = inventory[item_val];
	++inven_ctr;
	--equip_ctr;
	flag = TRUE;
    }

    ++i;

    while(!flag) {
	i_ptr = &inventory[i];

	if(typ > i_ptr->tval) {
	    for(j = (inven_ctr - 1); j >= i; --j) {
		inventory[j + 1] = inventory[j];
	    }

	    inventory[i] = inventory[item_val];
	    ++inven_ctr;
	    --equip_ctr;
	    flag = TRUE;
	}

	++i;
    }

    --i;

    switch(typ) {
    case 10:
    case 11:
    case 12:
    case 20:
    case 21:
    case 22:
    case 23:
    case 25:
	strcpy(prt1, "Was wielding ");

	break;
    case 15:
	strcpy(prt1, "Light source was ");

	break;
    default:
	strcpy(prt1, "Was wearing ");

	break;
    }

    objdes(prt2, i, TRUE);
    sprintf(out_val, "%s%s (%c)", prt1, prt2, i + 97);
    msg_print(out_val);
    inventory[item_val] = blank_treasure;

    /* For secondary weapon */
    if(item_val != INVEN_AUX) {
	py_bonuses(inventory[i], -1);
    }

    return i;
}

/* Unwear routine, remove a piece of equipment    -RAK- */
void unwear()
{
    int i;
    int j;
    int exit_flag;
    int test_flag;
    int com_val;
    char command;
    vtype out_val;

    if(scr_state == 1) {
	clear_screen(0, 0);
	show_equip(0);
    }

    exit_flag = FALSE;

    sprintf(out_val,
	    "(a-%c * for equipment list, ESC to exit) Take off which one?",
	    equip_ctr + 96);

    test_flag = FALSE;
    msg_print(out_val);

    inkey(&command);
    com_val = command;

    switch(com_val) {
    case 0:
    case 27:
	test_flag = TRUE;
	exit_flag = TRUE;

	break;
    case 42:
	clear_screen(1, 0);
	show_equip(0);

	break;
    default:
	com_val -= 97;

	if((com_val >= 0) && (com_val < equip_ctr)) {
	    test_flag = TRUE;
	}

	break;
    }

    while(!test_flag) {
	inkey(&command);
	com_val = command;

	switch(com_val) {
	case 0:
	case 72:
	    test_flag = TRUE;
	    exit_flag = TRUE;

	    break;
	case 42:
	    clear_screen(1, 0);
	    show_equip(0);

	    break;
	default:
	    com_val -= 97;

	    if((com_val >= 97) && (com_val < equip_ctr)) {
		test_flag = TRUE;
	    }

	    break;
	}
    }

    if(!exit_flag) {
	/* Player turn */
	reset_flag = FALSE;
	i = -1;
	j = 21;

	++j;

	if(inventory[j].tval != 0) {
	    ++i;
	}

	while(i != com_val) {
	    ++j;

	    if(inventory[j].tval != 0) {
		++i;
	    }
	}

	if(inventory[j].flags & 0x80000000) {
	    msg_print("Hmmm, it seems to be cursed...");
	    com_val = 0;
	}
	else {
	    remove(j);
	}
    }

    if(scr_state = 0) {
	exit_flag = TRUE;
    }
    else if(equip_ctr == 0) {
	exit_flag = TRUE;
    }
    else if(inven_ctr > 21) {
	exit_flag = TRUE;
	show_equip(0);
    }
    else if(!exit_flag) {
	show_equip(0);
    }

    while(!exit_flag) {
	sprintf(out_val,
		"(a-%c, * for equipment list, ESC to exit) Take off which one?",
		equip_ctr + 96);

	test_falg = FALSE;
	msg_print(out_val);

	inkey(&command);
	com_val = command;

	switch(com_val) {
	case 0:
	case 27:
	    test_flag = TRUE;
	    exit_flag = TRUE;

	    break;
	case 42:
	    clear_screen(1, 0);
	    show_equip(0);

	    break;
	default:
	    com_val -= 97;

	    if((com_val >= 0) && (com_val < equip_ctr)) {
		test_flag = TRUE;
	    }

	    break;
	}

	while(!test_flag) {
	    inkey(&command);
	    com_val = command;

	    switch(com_val) {
	    case 0:
	    case 27:
		test_flag = TRUE;
		exit_flag = TRUE;

		break;
	    case 42:
		clear_screen(1, 0);
		show_equip(0);

		break;
	    default:
		com_val -= 97;

		if((com_val >= 0) && (com_val < equip_ctr)) {
		    test_flag = TRUE;
		}

		break;
	    }
	}

	if(!exit_flag) {
	    /* Player turn */
	    reset_flag = FALSE;
	    i = -1;
	    j = 21;

	    ++j;

	    if(inventory[j].tval != 0) {
		++i;
	    }

	    while(i != com_val) {
		++j;

		if(inventory[j].tval != 0) {
		    ++i;
		}
	    }

	    if(inventory[j].flags & 0x80000000) {
		msg_print("Hmmm, it seems to be cursed...");
		com_val = 0;
	    }
	    else {
		remove(j);
	    }
	}

	if(scr_state == 0) {
	    exit_flag = TRUE;
	}
	else if(equip_ctr == 0) {
	    exit_flag = TRUE;
	}
	else if(inven_ctr > 21) {
	    exit_flag = TRUE;
	    show_equip(0);
	}
	else if(!exit_flag) {
	    show_equip(0);
	}
    }

    if(scr_state != 0) {
	if(equip_ctr == 0) {
	    clear_screen(0, 0);
	}
	else {
	    prt("You are currently using -", 0, 0);
	}
    }
}

/* Wear routing, wear or wield an item    -RAK- */
void wear()
{
    int i;
    int j;
    int k;
    int com_val;
    int tmp;
    vtype out_val;
    vtype prt1;
    vtype prt2;
    treasure_type unwear_obj;
    int exit_flag;
    int test_flag;
    char command;
    treasure_type *i_ptr;

    if(scr_state == 2) {
	clear_screen(0, 0);
	show_inven(0, inven_ctr - 1);
    }

    exit_flag = FALSE;

    sprintf(out_val,
	    "(a-%c, * for equipment list, ESC to exit) Wear/Wield which one?",
	    inven_ctr + 96);

    test_flag = FALSE;
    msg_print(out_val);

    inkey(&command);
    com_val = command;

    switch(com_val) {
    case 0:
    case 27:
	test_flag = TRUE;
	exit_flag = TRUE;

	break;
    case 42:
	clear_screen(1, 0);
	show_inven(0, inven_ctr - 1);

	break;
    default:
	com_val -= 97;

	if((com_val >= 0) && (com_val < inven_ctr)) {
	    test_flag = TRUE;
	}

	break;
    }

    while(!test_flag) {
	inkey(&command);
	com_val = command;

	switch(com_val) {
	case 0:
	case 27:
	    test_flag = TRUE;
	    exit_flag = TRUE;

	    break;
	case 42:
	    clear_screen(1, 0);
	    show_inven(0, inven_ctr - 1);

	    break;
	default:
	    com_val -= 97;

	    if((com_val >= 0) && (com_val < inven_ctr)) {
		test_flag = TRUE;
	    }

	    break;
	}
    }

    /* Main logic for wearing */
    if(!exit_flag) {
	/* Player turn */
	reset_flag = FALSE;
	test_flag = TRUE;

	/* Slot for equipment */
	switch(inventory[com_val].tval) {
	case 10:
	    i = 22;

	    break;
	case 11:
	    i = 22;

	    break;
	case 12:
	    i = 22;

	    break;
	case 15:
	    i = 32;

	    break;
	case 20:
	    i = 22;

	    break;
	case 21:
	    i = 22;

	    break;
	case 22:
	    i = 22;

	    break;
	case 23:
	    i = 22;

	    break;
	case 25:
	    i = 22;

	    break;
	case 30:
	    i = 30;

	    break;
	case 31:
	    i = 27;

	    break;
	case 32:
	    i = 31;

	    break;
	case 33:
	    i = 23;

	    break;
	case 34:
	    i = 26;

	    break;
	case 35:
	    i = 25;

	    break;
	case 36:
	    i = 25;

	    break;
	case 40:
	    i = 24;

	    break;
	case 45:
	    /* Rings */
	    if(inventory[INVEN_RIGHT].tval == 0) {
		i = INVEN_RIGHT;
	    }
	    else {
		i = INVEN_LEFT;
	    }

	    break;
	default:
	    msg_print("I don't see how you can use that.");
	    test_flag = FALSE;
	    com_val = 0;

	    break;
	}

	if(test_flag) {
	    if(inventory[i].tval != 0) {
		if(inventory[i].flags & 0x80000000) {
		    objdes(prt1, i, FALSE);
		    sprintf(out_val, "The %s you are ", prt1);

		    switch(i) {
		    case 23:
			strcat(out_val, "wielding ");

			break;
		    default:
			strcat(out_val, "wearing ");

			break;
		    }

		    msg_print(strcat(out_val, "appears to be cursed."));
		    test_flag = FALSE;
		    com_val = 0;
		}
		else if(inven_ctr > 21) {
		    if(inventory[com_val].number > 1) {
			if(inventory[com_val].subval < 512) {
			    msg_print("You will had to drop something first.");
			    test_flag = FALSE;
			    com_val = 0;
			}
		    }
		}
	    }
	}

	if(test_flag) {
	    /* Save old item */
	    unwear_obj = inventory[i];

	    /* Now wear/wield new object */
	    inventory[i] = inventory[com_val];
	    i_ptr = &inventory[i];

	    /* Fix for torches */
	    if((i_ptr->subval > 255) && (i_ptr->subval < 512)) {
		i_ptr->number = 1;
		i_ptr->subval -= 255;
	    }

	    /* Fix for weight */
	    inven_weight += (i_ptr->weight * i_ptr->number);

	    /* Subtracts weight */
	    inven_destroy(com_val);
	    ++equip_ctr;

	    /*
	     * Subtract bonuses for old item before add bonuses for new. Must do
	     * this after inven destroy, otherwise inventory may increase to 23
	     * items thus destroying INVEN_WIELD
	     */
	    if(unwear_obj.tval != 0) {
		inventory[INVEN_MAX] = unwear_obj;

		/* Decrements equip ctr and calls py_bonuses with -1 */
		tmp = remove(INVEN_MAX);

		if(tmp < com_val) {
		    com_val = tmp;
		}
	    }

	    py_bonuses(inventory[i], 1);

	    switch(i) {
	    case 22:
		strcpy(prt1, "You are wielding ");

		break;
	    case 32:
		strcpy(prt1, "Your light source is ");

		break;
	    default:
		strcpy(prt1, "You are wearing ");

		break;
	    }

	    objdes(prt2, i, TRUE);
	    j = -1;
	    k = 21;

	    /* Get the right letter of equipment */
	    ++k;

	    if(inventory[k].tval != 0) {
		++j;
	    }

	    while(k != i) {
		++k;

		if(inventory[k].tval != 0) {
		    ++j;
		}
	    }

	    sprintf(out_val, "%s%s (%c%c", prt1, prt2, j + 97, cur_char2());
	    msg_print(out_val);

	    if((i == 2) && ((py.stats.cstr * 15) < i_ptr->weight)) {
		msg_print("You have trouble weilding such a heavy weapon.");
	    }
	}
    }

    if(scr_state == 0) {
	exit_flag = TRUE;
    }
    else if(inven_ctr == 0) {
	exit_flag = TRUE;
    }
    else if(!exit_flag) {
	show_inven(com_val, inven_ctr - 1);
    }

    while(!exit_flag) {
	sprintf(out_val,
		"(a-%c, * for equipment list, ESC to exit) Wear/Wield which one?",
		inven_ctr + 96);

	test_flag = FALSE;
	msg_print(out_val);

	inkey(&command);
	com_val = command;

	switch(com_val) {
	case 0:
	case 27:
	    test_flag = TRUE;
	    exit_flag = TRUE;

	    break;
	case 42:
	    clear_screen(1, 0);
	    show_inven(0, inven_ctr - 1);

	    break;
	default:
	    com_val -= 97;

	    if((com_val >= 0) && (com_val < inven_ctr)) {
		test_flag = TRUE;
	    }

	    break;
	}

	while(!test_flag) {
	    inkey(&command);
	    com_val = command;

	    switch(com_val) {
	    case 0:
	    case 27:
		test_flag = TRUE;
		exit_flag = TRUE;

		break;
	    case 42:
		clear_screen(1, 0);
		show_inven(0, inven_ctr - 1);

		break;
	    default:
		com_val -= 97;

		if((com_val >= 0) && (com_val < inven_ctr)) {
		    test_flag = TRUE;
		}

		break;
	    }
	}

	/* Main logic for wearing */
	if(!exit_flag) {
	    /* Player turn */
	    reset_flag = FALSE;
	    test_flag = TRUE;

	    /* Slot for equipment */
	    switch(inventory[com_val].tval) {
	    case 10:
		i = 22;

		break;
	    case 11:
		i = 22;

		break;
	    case 12:
		i = 22;

		break;
	    case 15:
		i = 32;

		break;
	    case 20:
		i = 22;

		break;
	    case 21:
		i = 22;

		break;
	    case 22:
		i = 22;

		break;
	    case 23:
		i = 22;

		break;
	    case 25:
		i = 22;

		break;
	    case 30:
		i = 30;

		break;
	    case 31:
		i = 27;

		break;
	    case 32:
		i = 31;

		break;
	    case 33:
		i = 23;

		break;
	    case 34:
		i = 26;

		break;
	    case 35:
		i = 25;

		break;
	    case 36:
		i = 25;

		break;
	    case 40:
		i = 24;

		break;
	    case 45:
		/* Rings */
		if(inventory[INVEN_RIGHT].tval == 0) {
		    i = INVEN_RIGHT;
		}
		else {
		    i = INVEN_LEFT;
		}

		break;
	    default:
		msg_print("I don't see how you can use that.");
		test_flag = FALSE;
		com_val = 0;

		break;
	    }

	    if(test_flag) {
		if(inventory[i].tval != 0) {
		    if(inventory[i].flags & 0x80000000) {
			objdes(prt1, i, FALSE);
			sprintf(out_val, "The %s you are ", prt1);

			switch(i) {
			case 23:
			    strcat(out_val, "wielding ");

			    break;
			default:
			    strcat(out_val, "wearing ");

			    break;
			}

			msg_print(strcat(out_val, "appears to be cursed."));
			test_flag = FALSE;
			com_val = 0;
		    }
		    else if(inven_ctr > 21) {
			if(inventory[com_val].number > 1) {
			    if(inventory[com_val].subval < 512) {
				msg_print("You will have to drop something first.");
				test_flag = FALSE;
				com_val = 0;
			    }
			}
		    }
		}
	    }

	    if(test_flag) {
		/* Save old item */
		unwear_obj = inventory[i];

		/* Now wear/wield new object */
		inventory[i] = inventory[com_val];
		i_ptr = &inventory[i];

		/* Fix for torches */
		if((i_ptr->subval > 255) && (i_ptr->subval < 512)) {
		    i_ptr->number = 1;
		    i_ptr->subval -= 255;
		}

		/* Fix for weight */
		inven_weight += (i_ptr->weight * i_ptr->number);

		/* Subtracts weight */
		inven_destroy(com_val);
		++equip_ctr;

		/*
		 * Subtract bonuses for old item before add bonuses for
		 * new. Must do this after inven_destroy, otherwise inventory
		 * may increase to 23 items thus destroying INVEN_WIELD
		 */
		
		if(unwear_obj.tval != 0) {
		    inventory[INVEN_MAX] = unwear_obj;

		    /* Decrements equip ctr, and calls py_bonuses with -1 */
		    tmp = remove(INVEN_MAX);

		    if(tmp < com_val) {
			com_val = tmp;
		    }
		}

		py_bonuses(inventory[i], 1);

		switch(i) {
		case 22:
		    strcpy(prt1, "You are wielding ");

		    break;
		case 32:
		    strcpy(ptr1, "Your light source is ");

		    break;
		default:
		    strcpy(prt1, "You are wearing ");

		    break;
		}

		objdes(prt2, i, TRUE);
		j = -1;
		k = 21;

		/* Get the right letter of equipment */
		++k;

		if(inventory[k].tval != 0) {
		    ++j;
		}

		while(k != i) {
		    ++k;

		    if(inventory[k].tval != 0) {
			++j;
		    }
		}

		sprintf(out_val, "%s%s (%c%c", prt1, prt2, j + 97, cur_char2(i));
		msg_print(out_val);

		if((i == 22) && ((py.stats.cstr * 15) < i_ptr->weight)) {
		    msg_print("You have trouble wielding such a heavy weapon.");
		}
	    }
	}

	if(scr_state == 0) {
	    exit_flag = TRUE;
	}
	else if(inven_ctr == 0) {
	    exit_flag = TRUE;
	}
	else if(!exit_flag) {
	    show_inven(com_val, inven_ctr - 1);
	}
    }

    if(scr_state != 0) {
	prt("You are currently carrying -", 0, 0);
    }
}

/* Switch primary and secondary weapons    -RAK- */
void switch_weapon()
{
    vtype prt1;
    vtype prt2;
    treasure_type tmp_obj;

    if(inventory[INVEN_WIELD].flags & 0x80000000) {
	objdes(prt1, INVEN_WIELD, FALSE);
	sprintf(prt2, "The %s you are wielding appears to be cursed.", prt1);
	msg_print(prt2);
    }
    else {
	/* Switch weapons */
	reset_flag = FALSE;
	tmp_obj = inventory[INVEN_AUX];
	inventory[INVE_AUX] = inventory[INVEN_WIELD];
	inventory[INVEN_WEILD] = tmp_obj;

	/* Subtract bonuses */
	py_bonuses(inventory[INVEN_AUX], -1);

	/* Add bonuses */
	py_bonuses(inventory[INVEN_WIELD], 1);

	if(inventory[INVEN_WIELD].tval != 0) {
	    strcpy(prt1, "Primary weapon   : ");
	    objdes(prt2, INVEN_WIELD, TRUE);
	    msg_printf(strcat(prt1, prt2));

	    if((py.stats.cstr * 15) < inventory[INVEN_WIELD].weight) {
		msg_print("You have trouble wielding such a heavy weapon.");
	    }
	}

	if(inventory[INVEN_AUX].tval != 0) {
	    strcpy(prt1, "Secondary weapon : ");
	    objdes(prt2, INVEN_AUX, TRUE);
	    msg_print(strcat(prt1, prt2));
	}
    }

    if(scr_state != 0) {
	/* Make sure player sees last message */
	msg_print(" ");
	clear_screen(0, 0);
	prt("You are currently using -", 0, 0);
	show_equip(0);
    }
}

/*
 * Comprehensive function block to handle all inventory and equipment
 * routines. Five kinds of calls can take place. Note that "?" is a special call
 * for other routines to display a portion of the inventory, and take no other
 * action.    -RAK-
 */
int inven_command(char command, int r1, int r2)
{
    int com_val;
    int inven;
    int exit_flag;
    int test_flag;

    /* Main loginf for the INVEN_COMMAND    -RAK- */
    inven = FALSE:
    exit_flag = FALSE;
    scr_state = 0;

    switch(command) {
    case 'i': /* Inventory */
	if(inven_ctr == 0) {
	    msg_print("You are not carrying anything.");
	}
	else if(scr_state != 1) {
	    /* Sets scr_state to 1 */
	    clear_screen(0, 0);
	    prt("You are currently carrying -", 0, 0);
	    show_inven(0, inven_ctr - 1);
	}

	break;
    case 'e': /* Equipment */
	if(equip_ctr == 0) {
	    msg_print("You are not using any equipment.");
	}
	else if(scr_state != 2) {
	    /* Sets the scr_state to 2 */
	    clear_screen(0, 0);
	    prt("You are currently using -", 0, 0);
	    show_equip(0, inven_ctr - 1);
	}

	break;
    case 't': /* Take off */
	if(equip_ctr == 0) {
	    msg_print("You are not using any equipment.");
	}
	else if(inven_ctr > 21) {
	    msg_print("You will have to drop something first.");
	}
	else {
	    /* May set scr_state to 2 */
	    unwear();
	}

	break;
    case 'w': /* Wear/wield */
	if(inven_ctr == 0) {
	    msg_print("You are not carrying anything.");
	}
	else {
	    /* May set scr_state to 1 */
	    wear();
	}

	break;
    case 'x':
	if(inventory[INVEN_WIELD].tval != 0) {
	    switch_weapon();
	}
	else if(inventory[INVEN_AUX].tval != 0) {
	    switch_weapon();
	}
	else {
	    msg_print("But you are wielding no weapons.");
	}

	break;
    case '?': /* Displays part inven, returns */
	/* Special function for other routines */
	show_inven(r1, r2);

	/* Clear screen state */
	scr_state = 0;

	break;
    default: /* Nonsense command */

	break;
    }

    if(scr_state > 0) {
	prt("<e>quip, <i>nven, <t>ake-off, <w>ear/wield, e<x>change, or ESC to exit.", 23, 1);
	test_flag = FALSE;

	inkey(&command);
	com_val = command;

	switch(com_val) {
	case 0:
	case 27:
	case 32:
	    /* Exit from module */
	    exit_flag = TRUE;
	    test_flag = TRUE;

	    break;
	default:
	    /* Module commands */
	    switch(command) {
	    case 'e':
		test_flag = TRUE;

		break;
	    case 'i':
		test_flag = TRUE;

		break;
	    case 't':
		test_flag = TRUE;

		break;
	    case 'w':
		test_flag = TRUE;

		break;
	    case 'x':
		test_flag = TRUE;

		break;
	    case '?': /* Trap special feature */

		break;
	    default: /* Nonsense command */

		break;
	    }
	}

	while(!test_flag) {
	    inkey(&command);
	    com_val = command;

	    switch(com_val) {
	    case 0:
	    case 27:
	    case 32:
		/* Exit from module */
		exit_flag = TRUE;
		test_flag = TRUE;

		break;
	    default:
		/* Module commands */
		switch(command) {
		case 'e':
		    test_flag = TRUE;

		    break;
		case 'i':
		    test_flag = TRUE;

		    break;
		case 't':
		    test_flag = TRUE;

		    break;
		case 'w':
		    test_flag = TRUE;

		    break;
		case 'x':
		    test_flag = TRUE;

		    break;
		case '?': /* Trap special feature */

		    break;
		default: /* Nonsense command */

		    break;
		}
	    }
	}

	/* Clear the line containing command list */
	prt("", 23, 0);
    }
    else {
	exit_flag = TRUE;
    }

    while(!exit_flag) {
	switch(command) {
	case 'i': /* Inventory */
	    if(inven_ctr == 0) {
		msg_print("You are not carrying anything.");
	    }
	    else if(scr_state != 1) {
		/* Sets scr_state to 1 */
		clear_screen(0, 0);
		prt("You are currently carrying -", 0, 0);
		show_inven(0, inven_ctr - 1);
	    }

	    break;
	case 'e': /* Equipment */
	    if(equip_ctr == 0) {
		msg_print("You are not using any equipment.");
	    }
	    else if(scr_state != 2) {
		/* Sets scr_state to 2 */
		clear_screen(0, 0);
		prt("You are current using -", 0, 0);
		show_equip(0);
	    }

	    break;
	case 't': /* Take off */
	    if(equip_ctr == 0) {
		msg_print("You are not using any equipment.");
	    }
	    else if(inven_ctr > 21) {
		msg_print("You will have to drop something first.");
	    }
	    else {
		/* May set scr_state to 2 */
		unwear();
	    }

	    break;
	case 'w': /* Wear/wield */
	    if(inven_ctr == 0) {
		msg_print("You are not carrying anything.");
	    }
	    else {
		/* May set scr_state to 1 */
		wear();
	    }

	    break;
	case 'x':
	    if(inventory[INVEN_WIELD].tval != 0) {
		switch_weapon();
	    }
	    else if(inventory[INVEN_AUX].tval != 0) {
		switch_weapon();
	    }
	    else {
		msg_print("But you are weilding no weapons.");
	    }

	    break;
	case '?': /* Displays part inven, returns */
	    /* Special function for other routines */
	    show_inven(r1, r2);

	    /* Clear screen state */
	    scr_state = 0;

	    break;
	default: /* Nonsense command */

	    break;
	}

	if(scr_state > 0) {
	    prt("<e>quip, <i>nven, <t>ake-off, <w>ear/wield, e<x>change, or ESC to exit.", 23, 1);
	    test_flag = FALSE;

	    inkey(&command);
	    com_val = command;

	    switch(com_val) {
	    case 0:
	    case 27:
	    case 32:
		/* Exit from module */
		exit_flag = TRUE;
		test_flag = TRUE;

		break;
	    default:
		/* Module commands */
		switch(command) {
		case 'e':
		    test_flag = TRUE;

		    break;
		case 'i':
		    test_flag = TRUE;

		    break;
		case 't':
		    test_flag = TRUE;

		    break;
		case 'w':
		    test_flag = TRUE;

		    break;
		case 'x':
		    test_flag = TRUE;

		    break;
		case '?': /* Trap special feature */

		    break;
		default: /* Nonsense command */

		    break;
		}
	    }

	    while(!test_flag) {
		inkey(&command);
		com_val = command;

		switch(com_val) {
		case 0:
		case 27:
		case 32:
		    /* Exit from module */
		    exit_flag = TRUE;
		    test_flag = TRUE;

		    break;
		default:
		    /* Module commands */
		    switch(command) {
		    case 'e':
			test_flag = TRUE;

			break;
		    case 'i':
			test_flag = TRUE;

			break;
		    case 't':
			test_flag = TRUE;

			break;
		    case 'w':
			test_flag = TRUE;

			break;
		    case 'x':
			test_flag = TRUE;

			break;
		    case '?': /* Trap special feature */

			break;
		    default: /* Nonsense command */

			break;
		    }
		}
	    }

	    /* Clear line containing command list */
	    prt("", 23, 0);
	}
	else {
	    exit_flag = TRUE;
	}
    }

    /* If true, must redraw screen */
    if(scr_state > 0) {
	inven = TRUE;
    }

    return inven;
}

/* Get the ID of an item and return the CTR value of it    -RAK- */
int get_item(int *com_val, char *pmt, int *redraw, int i, int j)
{
    char command;
    vtype out_val;
    int test_flag;
    int item;

    item = FALSE;
    *com_val = 0;

    if(inven_ctr > 0) {
	sprintf(out_val,
		"(Items %c-%c, * for inventory list, ESC to exit) %s",
		i + 97,
		j + 97,
		pmt);

	test_flag = FALSE;
	prt(out_val, 0, 0);

	inkey(&command);
	*com_val = command;

	switch(*com_val) {
	case 0:
	case 27:
	    test_flag = TRUE;
	    reset_flag = TRUE;

	    break;
	case 42:
	    clear_screen(1, 0);
	    inven_command('?', i, j);
	    *redraw = TRUE;

	    break;
	default:
	    *com_val -= 97;

	    if((*com_val >= i) && (*com_val <= j)) {
		test_flag = TRUE;
		item = TRUE;
	    }

	    break;
	}

	while(!test_flag) {
	    inkey(&command);
	    *com_val = command;

	    switch(*com_val) {
	    case 0:
	    case 27:
		test_flag = TRUE;
		reset_flag = TRUE;

		break;
	    case 42:
		clear_screen(1, 0);
		inven_command('?', i, j);
		*redraw = TRUE;

		break;
	    default:
		*com_val -= 97;

		if((*com_val >= i) && (*com_val <= j)) {
		    test_flag = TRUE;
		    item = TRUE;
		}

		break;
	    }
	}

	erase_line(MSG_LINE, 0);
    }
    else {
	msg_print("You are not carrying anything.");
    }

    return item;
}

/*
 * I may have written the town level code, but I'm not exactly proud of
 * it. Adding the scores requires some real slucky hooks which I have not had
 * time to rethink.    -RAK-
 */

/* Calculates current boundaries    -RAK- */
void panel_boundaries()
{
    panel_row_min = panel_row * (SCREEN_HEIGHT / 2);
    panel_row_max = panel_row_min + SCREEN_HEIGHT - 1;
    panel_row_prt = panel_row_min - 1;
    panel_col_min = panel_col * (SCREEN_WIDTH / 2);
    panel_col_max = pandel_col_min + SCREEN_WIDTH - 1;

    /* 
     * The value 13 puts one blank space between the stats and the map, leaving
     * the last column empty the value 14 puts two blank spaces between the
     * stats and the map, and ends up printing in the last column. I think 14
     * gives a better display, but some curses wreak havoc when try to print
     * characters in the list column, hence the BUGGY_CURSES ifdef
     */
#ifdef BUGGY_CURSES
    panel_col_prt = panel_col_min - 13;
#else
    panel_col_prt = panel_col_min - 14;
#endif
}

/*
 * Given a row (y) and a col (x), this routine detects when a move off the
 * screen has occurred and figures new borders.    -RAK-
 */
int get_panel(int y, int x)
{
    int prow;
    int pcol;
    int panel;

    prow = panel_row;
    pcol = panel_col;

    if((y < (panel_row_min + 2)) || (y > (panel_row_max - 2))) {
	prow = (y - 2) / (SCREEN_HEIGHT / 2);

	if(prow > max_panel_rows) {
	    prow = max_panel_rows;
	}
    }

    if((x < (panel_col_min + 3)) || (x > (panel_col_max - 3))) {
	pcol = (x - 3) / (SCREEN_WIDTH / 2);

	if(pcol > max_panel_cols) {
	    pcol = max_panel_cols;
	}
    }

    if((prow != panel_row) || (pcol != panel_col) || !cave_flag) {
	panel_row = prow;
	panel_col = pcol;
	panel_bounds();
	panel = TRUE;
	cave_flag = TRUE;

	/* Stop movement if any */
	if(find_flag) {
	    /* No need to call move_light() */
	    find_flag = FALSE;
	}
    }
    else {
	panel = FALSE;
    }

    return panel;
}

/* Tests a given point to see if it is within the screen boundaries.    -RAK- */
int panel_contains(int y, int x)
{
    int panel;

    if((y >= panel_row_min) && (y <= panel_row_max)) {
	if((x >= panel_col_min) && (x <= panel_col_max)) {
	    panel = TRUE;
	}
	else {
	    panel = FALSE;
	}
    }
    else {
	panel = FALSE:
    }
    
    return panel;
}

/* Returns true if player has no light    -RAK- */
int no_light()
{
    int light;
    cave_type *c_ptr;

    light = FALSE;
    c_ptr = &cave[char_row][char_col];

    if(!c_ptr->tl) {
	if(!c_ptr->pl) {
	    light = TRUE;
	}
    }

    return light;
}

/* Map rogue_like direction commands into numbers */
int map_roguedir(int *comval)
{
    switch(*comval) {
    case 'h':
	*comval = '4';

	return 4;
    case 'y':
	*comval = '7';

	return 7;
    case 'k':
	*comval = '8';

	return 8;
    case 'u':
	*comval = '9';

	return 9;
    case 'l':
	*comval = '6';

	return 6;
    case 'n':
	*comval = '3';

	return 3;
    case 'j':
	*comval = '2';

	return 2;
    case 'b':
	*comval = '1';

	return 1;
    }

    return (*comval - 48);
}

/* Prompts for a direction    -RAK- */
int get_dir(char *prompt, int *dir, int *com_val, int *y, int *x)
{
    int flag;
    char command;
    int res;

    flag = FALSE;

    if(get_com(prompt, &command)) {
	*com_val = command;

	if(key_bindings == ORIGINAL) {
	    *dir = *comval - 48;
	}
	else {
	    /* rogue_like bindings */
	    *dir = map_roguedir(com_val);
	}

	/* Note that "5" is not a valid direction */
	if((*dir >= 1) && (*dir <= 9) && (*dir != 5)) {
	    move(*dir, y, x);
	    flag = TRUE;
	    res = TRUE;
	}
    }
    else {
	reset_flag = TRUE;
	res = FALSE;
	flag = TRUE;
    }

    while(!flag) {
	if(get_com(prompt, &command)) {
	    *com_val = command;

	    if(key_bindings == ORIGINAL) {
		*dir = *com_val - 48;
	    }
	    else {
		/* rogue_like bindings */
		*dir = map_roguedir(com_val);
	    }

	    /* Note that "5" is not a valid direction */
	    if((*dir >= 1) && (*dir <= 9) && (*dir != 5)) {
		move(*dir, y, x);
		flag = TRUE;
		res = TRUE;
	    }
	}
	else {
	    reset_flag = TRUE;
	    res = FALSE;
	    flag = TRUE;
	}
    }

    return res;
}
	
/* Moves creature record from one space to another    -RAK- */
void move_rec(int y1, int x1, int y2, int x2)
{
    if((y1 != y2) || (x1 != x2)) {
	cave[y2][x2].cptr = cave[y1][x1].cptr;
	cave[y1][x1].cptr = 0;
    }
}

void find_light(int y1, int x1, int y2, int x2)
{
    int i;
    int j;
    int k;
    int l;

    for(i = y1; i <= y2; ++i) {
	for(j = x1; j <= x2; ++j) {
	    if((cave[i][j].fval == 1) || (cave[i][j].fval == 2)) {
		for(k = (i - 1); k <= (i + 1); ++k) {
		    for(l = (j - 1); l <= (j + 1); ++l) {
			cave[k][l].pl = TRUE;
		    }
		}

		cave[i][j].fval = 2;
	    }
	}
    }
}

/* Room is lit, make it appear    -RAK- */
void light_room(int y, int x)
{
    cave_type *c_ptr;
    int i;
    int j;
    int tmp1;
    int tmp2;
    int start_row;
    int end_row;
    int start_col;
    int end_col;
    int ypos;
    int xpos;
    vtype floor_str;
    vtype tmp_str;

    tmp1 = SCREEN_HEIGHT / 2;
    tmp2 = SCREEN_WIDTH / 2;
    start_row = (y / tmp1) * tmp1;
    start_col = (x / tmp2) * tmp2;
    end_row = start_row + tmp1 - 1;
    end_col = start_col + tmp2 - 1;
    find_light(start_row, start_col, end_row, end_col);

    for(i = start_row; i <= end_row; ++i) {
	floor_str[0] = '\0';
	ypos = i;

	for(j = start_col; j <= end_col; ++j) {
	    c_ptr = &cave[i][j];

	    if(c_ptr->pl || c_ptr->fm) {
		if(strlen(floor_str) == 0) {
		    xpos = j;
		}

		loc_symbol(i, j, tmp_str);
		tmp_str[1] = '\0';
		strcat(floor_str, tmp_str);
	    }
	    else {
		if(strlen(floor_str) > 0) {
		    print(floor_str, ypos, xpos);
		    floor_str[0] = '\0';
		}
	    }
	}

	if(strlen(floor_str) > 0) {
	    print(floor_str, ypos, xpos);
	}
    }
}

/* Lights up given location    -RAK- */
void lite_spot(int y, int x)
{
    char temp[2];

    temp[1] = '\0';

    if(panel_contains(y, x)) {
	loc_symbol(y, x, temp);
	print(temp, y, x);
    }
}

/* Blanks out given location    -RAK- */
void unlite_spot(int y, int x)
{
    if(panel_contains(y, x)) {
	print(" ", y, x);
    }
}

/* Minimum of a maximum    -RAK- */
int minmax(int x, int y, int z)
{
    int max;

    if(y > x) {
	max = y + 1;
    }
    else {
	max = x + 1;
    }

    if(max > z) {
	return z;
    }
    else {
	return max;
    }
}

/* Maximum of a minimum    -RAK- */
int maxmin(int x, int y, int z)
{
    int min;

    if(x > y) {
	min = y - 1;
    }
    else {
	min = x - 1;
    }

    if(min > z) {
	return min;
    }
    else {
	return z;
    }
}

/* Given two sets of points, draw the block    -RAK- */
void draw_block(int y1, int x1, int y2, int x2)
{
    cave_type *c_ptr;
    int i;
    int j;
    int xpos;
    int topp;
    int bott;
    int left;
    int righ;
    int new_topp;
    int new_bott;
    int new_left;
    int new_righ;
    vtype floor_str;
    vtype save_str;
    char tmp_char[2];
    int flag;

    /* This is supposed to be a one */
    tmp_char[1] = '\0';

    /*
     * From uppermost to bottom most lines was on...
     * Points are guaranteed to be on the screen (I hope...)
     */
    topp = maxmin(y1, y2, panel_row_max);
    bott = minmax(y1, y2, panel_row_max);
    left = maxmin(x1, x2, panel_col_min);
    righ = minmax(x1, x2, panel_col_max);

    /* Margins for new things to appear */
    new_topp = y2 - 1;
    new_bott = y2 + 1;
    new_left = x2 - 1;
    new_righ = x2 + 1;

    for(i = topp; i <= bott; ++i) {
	/* Null out print string */
	floor_str[0] = '\0';
	xpos = -1;
	save_str[0] = '\0';

	/* Leftmost to rightmost do */
	for(j = left; j <= righ; ++j) {
	    c_ptr = &cave[i][j];

	    if(c_ptr->pl || c_ptr->fm) {
		if(((i == y1) && (j == x1)) || ((i == y2) && (j == x2))) {
		    flag = TRUE;
		}
		else {
		    flag = FALSE;
		}
	    }
	    else {
		flag = TRUE;

		if(((i >= new_topp) && (i <= new_bott))
		   && ((j >= new_left) && (j <= new_righ))) {
		    if(c_ptr->tl) {
			if(((c_ptr->fval >= 10) && (c_ptr->fval <= 12))
			   || (c_ptr->fval == 15)) {
			    c_ptr->pl = TRUE;
			}
			else if(c_ptr->tptr != 0) {
			    if((t_list[c_ptr->tptr].tval >= 102)
			       && (t_list[c_ptr->tptr].tval <= 110)
			       && (t_list[c_ptr->tptr].tval != 106)) {
				if(!c_ptr->fm) {
				    c_ptr->fm = TRUE;
				}
			    }
			}
		    }
		}
	    }

	    if(c_ptr->pl || c_ptr->tl || c_ptr->fm) {
		loc_symbol(i, j, tmp_char);
	    }
	    else {
		tmp_char[0] = ' ';
	    }

	    if(py.flags.image > 0) {
		if(randint(12) == 1) {
		    tmp_char[0] = randint(95) + 31;
		}
	    }

	    if(flag) {
		if(xpos < 0) {
		    xpos = j;
		}

		if(strlen(save_str) > 0) {
		    strcat(floor_str, save_str);
		    save_str[0] = '\0';
		}

		strcat(floor_str, tmp_char);
	    }
	    else if(xpos >= 0) {
		strcat(save_str, tmp_char);
	    }
	}

	if(xpos >= 0) {
	    print(floor_str, i, xpos);
	}
    }
}

/* Normal movement */
void sum1_move_light(int y1, int x1, int y2, int x2)
{
    int i;
    int j;

    light_flag = TRUE;

    /* Turn off lamp light */
    for(i = (y1 - 1); i <= (y1 + 1); ++i) {
	for(j = (x1 - 1); j <= (x1 + 1); ++j) {
	    cave[i][x].tl = FALSE;
	}
    }

    for(i = (y2 - 1); i <= (y2 + 1); ++i) {
	for(j = (x2 - 1); j <= (x2 + 1); ++j) {
	    cave[i][j].tl = TRUE;
	}
    }

    /* Redraw area */
    draw_block(y1, x1, y2, x2);
}

/* When FIND_FLAG, light only permanent features */
void sub2_move_light(int y1, int x1, int y2, int x2)
{
    int i;
    int j;
    int xpos;
    vtype floor_str;
    vtype save_str;
    char tmp_char[2];
    int flag;
    cave_type *c_ptr;

    tmp_char[1] = '\0';

    if(light_flag) {
	for(i = (y1 - 1); i <= (y1 + 1); ++i) {
	    for(j = (x1 - 1); j <= (x1 + 1); ++j) {
		cave[i][j].tl = FALSE;
	    }
	}

	draw_block(y1, x1, y1, x1);
	light_flag = FALSE;
    }

    for(i = (y2 - 1); i <= (y2 + 1); ++i) {
	floor_str[0] = '\0';
	save_str[0] = '\0';
	xpos = 0;

	for(j = (x2 - 1); j <= (x2 + 1); ++j) {
	    c_ptr = &cave[i][j];
	    flag = FALSE;

	    if(!c_ptr->fm && !c_ptr->pl) {
		tmp_char[0] = ' ';

		if(player_light) {
		    if(((c_ptr->fval >= 10) && (c_ptr->fval <= 12))
		       || (c_ptr->fval == 15)) {
			/* Turn on perm light */
			c_ptr->pl = TRUE;
			loc_symbol(i, j, tmp_char);
			flag = TRUE;
		    }
		    else {
			if(c_ptr->tptr != 0) {
			    if((t_list[c_ptr->tptr].tval >= 102)
			       && (t_list[c_ptr->tptr].tval <= 110)
			       && (t_list[c_ptr->tptr].tval != 106)) {
				/* Turn on field marker */
				c_ptr->fm = TRUE;
				loc_symbol(i, j, tmp_char);
				flag = TRUE;
			    }
			}
		    }
		}
	    }
	    else {
		loc_symbol(i, j, tmp_char);
	    }

	    if(flag) {
		if(xpos == 0) {
		    xpos = j;
		}

		if(strlen(save_str) > 0) {
		    strcat(floor_str, save_str);
		    save_str[0] = '\0';
		}

		strcat(floor_str, tmp_char);
	    }
	    else if(x_pos > 0) {
		strcat(save_str, tmp_char);
	    }
	}

	if(xpos > 0) {
	    j = i;
	    print(floor_str, j, xpos);
	}
    }
}

/* When blinded, move only the player symbol... */
void sub3_move_light(int y1, int x1, int y2, int x2)
{
    int i;
    int j;

    if(light_flag) {
	for(i = (y1 - 1); i <= (y1 + 1); ++i) {
	    for(j = (x1 - 1); j <= (x1 + 1); ++j) {
		cave[i][j].tl = FALSE;
	    }
	}

	light_flag = FALSE;
    }

    print(" ", y1, x1);
    print("@", y2, x2);
}

/* With not light, movement becomes involved... */
void sub4_move_light(int y1, int x1, int y2, int x2)
{
    int i;
    int j;

    light_flag = TRUE;

    if(cave[y1][x1].tl) {
	for(i = (y1 - 1); i <= (y1 + 1); ++i) {
	    for(j = (x1 - 1); j <= (x1 + 1); ++j) {
		cave[i][j].tl = FALSE;

		if(test_light(i, j)) {
		    lite_spot(i, j);
		}
		else {
		    unlite_spot(i, j);
		}
	    }
	}
    }
    else if(test_light(y1, x1)) {
	lite_spot(y1, x1);
    }
    else {
	unlit_spot(y1, x1);
    }

    print("@", y2, x2);
}

/*
 * Packaged for moving the character's light about the screen.
 * Three cases: Normal, Finding, and Blind    -RAK-
 */
void move_light(int y1, int x1, int y2, int x2)
{
    if(py.flags.blind > 0) {
	sub3_move_light(y1, x1, y2, x2);
    }
    else if(find_flag) {
	sub2_move_light(y1, x1, y2, x2);
    }
    else if(!player_light) {
	sub4_move_light(y1, x1, y2, x2);
    }
    else {
	sub1_move_light(y1, x1, y2, x2);
    }
}

/* Returns random coordinates    -RAK- */
void new_spot(int *y, int *x)
{
    *y = randint(cur_height) - 1;
    *x = randing(cur_width) - 1;

    while(!cave[*y][*x].fopen
	  || (cave[*y][*x].cptr != 0)
	  || (cave[*y][*x].tptr != 0)) {
	*y = randint(cur_height) - 1;
	*x = randint(cur_width) - 1;
    }
}

/* Search mode enhancement    -RAK- */
void search_on()
{
    search_flag = TRUE;
    change_speed(1);
    py.flags.status |= 0x00000100;
    prt_search();
    ++py.flags.food_digested;
}

void search_off()
{
    search_flag = FALSE;
    find_flag = FALSE;
    move_light(char_row, char_col, char_row, char_col);
    change_speed(-1);
    py.flags.status &= 0xFFFFFEFF;
    prt_search();
    --py.flags.food_digested;
}

/* Resting allows a player to safely restore his hit points    -RAK- */
void rest()
{
    int rest_num;
    vtype rest_str;

    prt("Rest for how long? ", 0, 0);
    rest_num = 0;

    if(get_string(rest_str, 0, 19, 10)) {
	sscanf(rest_str, "%d", &rest_num);
    }

    if(test_num > 0) {
	if(search_flag) {
	    search_off();
	}

	py.flags.rest = rest_num;
	py.flags.status |= 0x00000200;
	prt_rest();
	--py.flags.food_digested;
	msg_print("Press ^C to wake up...");
	put_qio();
    }
    else {
	erase_line(MSG_LINE, 0);
	reset_flag = TRUE;
    }
}

void rest_off()
{
    py.flags.rest = 0;
    py.flags.status &= 0xFFFFFDFF;
    erase_line(0, 0);
    prt_rest();
    ++py.flags.food_digested;
}

/* Attacker's level and plusses, defender's AC    -RAK- */
int test_hit(int bth, int level, int pth, int ac)
{
    int i;
    int test;

    if(search_flag) {
	search_off();
    }

    if(py.flags.rest > 0) {
	rest_off();
    }

    i = bth + (level * BTH_LEV_ADJ) + (pth * BTH_PLUS_ADJ);

    /* pth could be less than 0 if player wielding weapon too heavy for him */
    /* Normal hit */
    if((i > 0) && (randint(i) > ac)) {
	test = TRUE;
    }
    else if(randint(20) == 1) {
	/* Always hit 1/20 */
	test = TRUE;
    }
    else {
	/* Missed */
	if(i <= 0) {
	    msg_print("You have trouble swinging such a heavy weapon.");
	}

	test = FALSE;
    }

    return test;
}

/* Decreases player's hit points and sets death flag if necessary    -RAK- */
void take_hit(int damage, char *hit_from)
{
    if(py.flags.invuln > 0) {
	damage = 0;
    }

    py.misc.chp -= (double)damage;

    if(search_flag) {
	search_off();
    }

    if(py.flags.rest > 0) {
	rest_off();
    }

    if(find_flag) {
	find_flag = FALSE;
	move_light(char_row, char_col, char_row, char_col);
    }

    flush();

    if(py.misc.chp <= -1) {
	if(!death) {
	    /* Hee, hee...Ain't I mean? */
	    death = TRUE;
	    strcpy(died_from, hit_from);
	    total_winner = FALSE;
	}

	moria_flag = TRUE;
    }
    else {
	prt_chp();
    }
}

/*
 * Given speed, returns number of moves this turn.    -RAK-
 * Note: Player must always move at least once per iteration, a slowed player
 *       is handled by moving monsters faster
 */
int movement_rate(int speed)
{
    if(speed > 0) {
	if(py.flags.rest > 0) {
	    return 1;
	}
	else {
	    return speed;
	}
    }
    else {
	/* Speed must be negative here */
	return ((turn % (2 - speed)) == 0);

	/* if((turn % (2 - speed)) == 0) { */
	/*     return 1; */
	/* } */
	/* else { */
	/*     return 0; */
	/* } */
    }
}

/* Regenerate hit points    -RAK- */
void regenhp(double percent)
{
    struct misc *p_ptr;

    p_ptr = &py.misc;
    p_ptr->chp += ((p_ptr->mhp * percent) + PLAYER_REGEN_HPBASE);
}

/* Regenerate mana points    -RAK- */
void regenmana(double percent)
{
    struct misc *p_ptr;

    p_ptr = &py.misc;
    p_ptr->cmana += ((p_ptr->mana * percent) + PLAYER_REGEN_MNBASE);
}

/*
 * Change a trap from invisible to visible    -RAK-
 * Note: Secret doors are handled here
 */
void change_trap(int y, int x)
{
    int k;
    cave_type *c_ptr;

    c_ptr = &cave[y][x];

    if((t_list[c_ptr->tptr].tval == 101) || (t_list[c_ptr->tptr].tval == 109)) {
	k = c_ptr->tptr;

	/* Subtract one, since zeroth item has subval of one */
	place_trap(y, x, 2, t_list[c].subval - 1);
	pusht(k);
	lite_spot(y, x);
    }
}

/* Searches for hidden things...    -RAK- */
void search(int y, int x, int chance)
{
    int i;
    int j;
    cave_type *c_ptr;
    treasure_type *t_ptr;
    struct flags *p_ptr;
    vtype tmp_str;

    p_ptr = &py.flags;

    if((p_ptr->confused + p_ptr->blind) > 0) {
	chance = chance / 10.0;
    }
    else if(no_light()) {
	chance = chance / 5.0;
    }

    for(i = (y - 1); i <= (y + 1); ++i) {
	for(j = (x - 1); j <= (x + 1); ++j) {
	    if(in_bounds(i, j)) {
		if((i != y) || (j != x)) {
		    if(randint(100) < chance) {
			c_ptr = &cave[i][j];

			/* Search for hidden objects */
			if(c_ptr->tptr != 0) {
			    t_ptr = &t_list[c_ptr->tptr];

			    /* Trap on floor? */
			    if(t_ptr->tval == 101) {
				sprintf(tmp_str, "You have found %s.", t_ptr->name);
				msg_print(tmp_str);
				change_trap(i, j);

				if(find_flag) {
				    find_flag = TRUE;
				    move_light(char_row, char_col, char_row, char_col);
				}
			    }
			    else if(t_ptr->tval == 109) {
				/* Secret door? */
				msg_print("You have found a secret door.");
				c_ptr->fval = corr_floor2.ftval;
				change_trap(i, j);

				if(find_flag) {
				    find_flag = FALSE;
				    move_light(char_row, char_col, char_row, char_col);
				}
			    }
			    else if(t_ptr->tval == 2) {
				/* Chest is trapped? */
				/* Mask out the treasure bits */
				if((t_ptr->flags & 0x00FFFFFF) > 1) {
				    if(index(t_ptr->name, '^') != 0) {
					known2(t_ptr->name);
					msg_print("You have discovered a trap on the chest!");
				    }
				    else {
					msg_print("The chest is trapped!");
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }
}

/* 
 * Turns off find_flag if something interesting appears    -RAK-
 * Bug: Does not handle corridor/room corners, but I didn't want to add a lot
 *      of checking for such a minor detail
 */
void area_affect(int dir, int y, int x)
{
    int z[3];
    int i;
    int row;
    int col;
    cave_type *c_ptr;
    monster_type *m_ptr;

    if(cave[y][x].fval == 4) {
	i = 0;

	if(next_to4(y, x, 4, 5, 6) > 2) {
	    /* No need to call move_light() */
	    find_flag = FALSE;

	    return;
	}
    }

    if(find_flag && (py.flags.blind < 1)) {
	switch(dir) {
	case 1:
	    z[0] = 4;
	    z[1] = 1;
	    z[2] = 3;

	    break;
	case 2:
	    z[0] = 4;
	    z[1] = 2;
	    z[2] = 6;

	    break;
	case 3:
	    z[0] = 2;
	    z[1] = 3;
	    z[2] = 6;

	    break;
	case 4:
	    z[0] = 8;
	    z[1] = 4;
	    z[2] = 2;

	    break;
	case 6:
	    z[0] = 2;
	    z[1] = 6;
	    z[2] = 8;

	    break;
	case 7:
	    z[0] = 8;
	    z[1] = 7;
	    z[2] = 4;

	    break;
	case 8:
	    z[0] = 4;
	    z[1] = 8;
	    z[2] = 6;

	    break;
	case 9:
	    z[0] = 8;
	    z[1] = 9;
	    z[2] = 6;

	    break;
	}

	for(i = 0; i < 3; ++i) {
	    row = y;
	    col = x;

	    if(move(z[i], &row, &col)) {
		c_ptr = &cave[row][col];

		/* Empty doorways */
		if(c_ptr->fval == 5) {
		    /* No need to call move_light() */
		    find_flag = FALSE;

		    return;
		}

		/* Objects player can see including doors */
		if(find_flag) {
		    if(player_light) {
			if(c_ptr->tptr != 0) {
			    if((t_list[c_ptr->tptr].tval != 101)
			       && (t_list[c_ptr->tptr].tval != 109)) {
				/* No need to call move_light() */
				find_flag = FALSE;

				return;
			    }
			}
		    }
		    else if(c_ptr->tl || c_ptr->pl || c_ptr->fm) {
			if(c_ptr->tptr != 0) {
			    if((t_list[c_ptr->tptr].tval != 101)
			       && (t_list[c_ptr->tptr].tval != 109)) {
				/* No need to call move_light() */
				find_flag = FALSE;

				return;
			    }
			}
		    }

		    /* Creatures */
		    if(find_flag) {
			if(c_ptr->tl || c_ptr->pl || player_light) {
			    if(c_ptr->cptr > 1) {
				m_ptr = &m_list[c_ptr->cptr];

				if(m_ptr->ml) {
				    /* No need to call move_light() */
				    find_flag = FALSE;

				    return;
				}
			    }
			}
		    }
		}
	    }
	}
    }

    return;
}

/* Picks a new direction when in find mode    -RAK- */
int pick_dir(int dir)
{
    int z[2];
    int i;
    int pick;
    int y;
    int x;

    if(find_flag && (next_to4(char_row, char_col, 4, 5, -1) == 2)) {
	switch(dir) {
	case 1:
	    z[0] = 2;
	    z[1] = 4;

	    break;
	case 2:
	    z[0] = 4;
	    z[1] = 6;

	    break;
	case 3:
	    z[0] = 2;
	    z[1] = 6;

	    break;
	case 4:
	    z[0] = 2;
	    z[1] = 8;

	    break;
	case 6:
	    z[0] = 2;
	    z[1] = 8;

	    break;
	case 7:
	    z[0] = 4;
	    z[1] = 8;

	    break;
	case 8:
	    z[0] = 4;
	    z[1] = 6;

	    break;
	case 9:
	    z[0] = 6;
	    z[1] = 8;

	    break;
	}

	for(i = 0; i < 2; ++i) {
	    y = char_row;
	    x = char_col;

	    if(move(z[i], &y, &x)) {
		if(cave[y][x].fopen) {
		    pick = TRUE;
		    global_com_val = z[i] + 48;
		}
	    }
	}
    }
    else {
	pick = FALSE;
    }

    return pick;
}

/*
 * AC gets worse    -RAK-
 * Note: This routine affects magical AC bonuses so that stores can detect 
 *       the damage.
 */
int minus_ac(int typ_dam)
{
    int i;
    int j;
    int tmp[6];
    int minus;
    treasure_type *i_ptr;
    vtype out_val;
    vtype tmp_str;

    i = 0;

    if(inventory[INVEN_BODY].tval != 0) {
	tmp[i] = INVEN_BODY;
	++i;
    }

    if(inventory[INVEN_ARM].tval != 0) {
	tmp[i] = INVEN_ARM;
	++i;
    }

    if(inventory[INVEN_OUTER].tval != 0) {
	tmp[i] = INVEN_OUTER;
	++i;
    }

    if(inventory[INVEN_HANDS].tval != 0) {
	tmp[i] = INVEN_HANDS;
	++i;
    }

    if(inventory[INVEN_HEAD].tval != 0) {
	tmp[i] = INVEN_HEAD;
	++i;
    }

    /* Also affect boots */
    if(inventory[INVEN_FEET].tval != 0) {
	tmp[i] = INVEN_FEET;
	++i;
    }

    minus = FALSE;

    if(i > 0) {
	j = tmp[randint(i) - 1];
	i_ptr = &inventory[j];

	if(i_ptr->flags & typ_dam) {
	    objdes(tmp_str, j, FALSE);
	    sprintf(out_val, "Your %s resists damage!", tmp_str);
	    msg_print(out_val);
	    minus = TRUE;
	}
	else if((i_ptr->ac + t_ptr->toac) > 0) {
	    objdes(tmp_str, j, FALSE);
	    sprintf(out_val, "Your %s is damaged!", tmp_str);
	    msg_print(out_val);
	    --i_ptr->toac;
	    py_bonuses(blank_treasure, 0);
	    minus = TRUE;
	}
    }

    return minus;
}

/* Corrode the unsuspecting person's armor    -RAK- */
void corrode_gas(char *kb_str)
{
    int set_corrodes();

    if(!minus_ac((int)0x00100000)) {
	take_hit(randint(8), kb_str);
    }

    print_stat |= 0x0040;

    if(inven_damage(set_corrodes, 5) > 0) {
	msg_print("There is an acrid smell coming from your pack.");
    }
}

/* Poison gas the idiot...    -RAK- */
void poison_gas(int dam, char *kb_str)
{
    take_hit(dam, kb_str);
    print_stat |= 0x0040;
    py.flags.poisoned += (12 + randint(dam));
}

/* Burn the fool up...    -RAK- */
void fire_dam(int dam, char *kb_str)
{
    int set_flammable();

    if(py.flags.fire_resist) {
	dam = dam / 3;
    }

    if(py.flags.resist_heat > 0) {
	dam = dam / 3;
    }

    take_hit(dam, kb_str);
    print_stat |= 0x0080;

    if(inven_damage(set_flammable, 3) > 0) {
	msg_print("There is smoke coming from your pack!");
    }
}

/* Freeze hit to death...    -RAK- */
void cold_dam(int dam, char *kb_str)
{
    int set_frost_destroy();

    if(py.flags.cold_resist) {
	dam = dam / 3;
    }

    if(py.flags.resist_cold > 0) {
	dam = dam / 3;
    }

    take_hit(dam, kb_str);
    print_stat |= 0x0080;

    if(inven_damage(set_frost_destroy, 5) > 0) {
	msg_print("Something shatters inside your pack!");
    }
}

/* Lightning bolt the sucker away...    -RAK- */
void light_dam(int dam, char *kb_str)
{
    if(py.flags.lght_resist) {
	take_hit(dam / 3, kb_str);
    }
    else {
	take_hit(dam, kb_str);
    }

    print_stat |= 0x0080;
}

/* Throw acid on the hapless victim    -RAK- */
void acid_dam(int dam, char *kb_str)
{
    int flag;
    int set_acid_affect();

    flag = 0;

    if(minus_ac((int)0x00100000)) {
	flag = 1;
    }

    if(py.flags.acid_resist) {
	flag += 2;
    }

    switch(flag) {
    case 0:
	take_hit(dam, kb_str);

	break;
    case 1:
	take_hit(dam / 2, kb_str);

	break;
    case 2:
	take_hit(dam / 3, kb_str);

	break;
    case 3:
	take_hit(dam / 4, kb_str);

	break;
    }

    print_stat |= 0x00C0;

    if(inven_damage(set_acid_affect, 3) > 0) {
	msg_print("there is an acrid smell coming from your pack!");
    }
}
