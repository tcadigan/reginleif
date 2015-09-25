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

#ifdef USG
unsigned sleep();
#endif

#ifdef ultrix
void sleep();
#endif

/* Global flags */
int moria_flag;    /* Next level when true */
int search_flag;   /* Player is searching */
int teleport_flag; /* Handle teleport traps */
int player_light;  /* Player carrying light */
int cave_flag;     /* Used in get_panel */
int light_flag;    /* Used in move_light */

/* Value of msg_flag at start of turn */
int save_msg_flag;

/*
 * Moria game module
 * The code in this section has gone through many revisions, and some of it 
 * could stand more hard work...    -RAK-
 */
void dungeon()
{
    /* Detect change */
    int old_chp;
    int old_cmana;

    /* Regenerate hp and mana */
    double regen_amoonut;

    /* Last command */
    char command;

    struct misc *p_ptr;
    treasure_type *i_ptr;
    struct flags *f_ptr;
    int set_floor();

    /*
     * Main proceedure for dungeon...    -RAK-
     * Note: There is a lot of preliminary magic going on here at first
     */

    /* Check light status for setup */
    i_ptr = &inventory[INVEN_LIGHT];

    if(i_ptr->p1 > 0) {
	player_light = TRUE;
    }
    else {
	palyer_light = FALSE;
    }

    /* Check for maximum level */
    p_ptr = &py.misc;

    if(dun_level > p_ptr->max_lev) {
	p_ptr->max_lev = dun_level;
    }

    /* Set up the character co-ords */
    if((char_row == -1) || (char_col == -1)) {
	new_spot(&char_row, &char_col);
    }

    /* Reset flags and initialize variables */
    moria_flag = FALSE;
    cave_flag = FALSE;
    find_flag = FALSE;
    search_flag = FALSE;
    teleport_flag = FALSE;
    mon_tot_mult = 0;
    cave[char_row][char_col].cptr = 1;
    old_chp = (int)py.misc.chp;
    old_cmana = (int)py.misc.cmana;

    /* Light up the area around character */
    move_char(5);

    /* Light, but do not move critters */
    creatures(FALSE);

    /* Print the depth */
    prt_depth();

    /* Increment turn counter */
    ++turn;

    /* Check for game hours */
    if(!wizard1) {
	if((turn % 250) == 1) {
	    if(!check_time()) {
		if(close_flag > 4) {
		    if(search_flag) {
			search_off();
		    }

		    if(py.flags.rest > 0) {
			rest_off();
		    }

		    find_flag = FALSE;
		    msg_print("The gates to Moria are now closed.");

		    /* Make sure player sees the message */
		    msg_print(" ");
		    save_char(TRUE, FALSE);

		    while(TRUE) {
			save_char(TRUE, FALSE);
		    }
		}
		else {
		    if(search_flag) {
			search_off();
		    }

		    if(py.flags.rest > 0) {
			rest_off();
		    }

		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		    ++closing_flag;
		    msg_print("The gates to Moria are closing...");
		    msg_print("Please finish up or save your game.");

		    /* Make sure the player sees the message */
		    msg_print(" ");
		}
	    }
	}
    }

    /* Turn over the store contents every, say, 1000 turns */
    if((dun_level != 0) && ((turn % 1000) == 0)) {
	store_maint();
    }

    /* Check for creature generation */
    if(randint(MAX_MALLOC_CHANCE) == 1) {
	alloc_monster(set_floor, 1, MAX_SIGHT, FALSE);
    }

    /* Screen may need updating, used mostly for stats */
    if(print_stat != 0) {
	if(print_stat & 0x0001) {
	    prt_strength();
	}

	if(print_stat & 0x0002) {
	    prt_dexterity();
	}

	if(print_stat & 0x0004) {
	    prt_constitution();
	}

	if(print_stat & 0x0008) {
	    prt_intelligence();
	}

	if(print_stat & 0x0010) {
	    prt_wisdom();
	}

	if(print_stat & 0x0020) {
	    prt_charisma();
	}

	if(print_stat & 0x0040) {
	    prt_pac();
	}

	if(print_stat & 0x0100) {
	    prt_mhp();
	}

	if(print_stat & 0x0200) {
	    prt_title();
	}

	if(print_stat & 0x0400) {
	    prt_level();
	}
    }

    /* Check light status */
    i_ptr = &inventory[INVEN_LIGHT];

    if(player_light) {
	if(i_ptr->p1 > 0) {
	    --i_ptr->p1;

	    if(i_ptr->p1 == 0) {
		player_light = FALSE;
		find_flag = FALSE;
		msg_print("Your light has gone out!");
		move_light(char_row, char_col, char_row, char_col);
	    }
	    else if(i_ptr->p1 < 400) {
		if(randint(5) == 1) {
		    if(find_flag) {
			find_flag = FALSE;
			move_light(char_row, char_col, char_row, char_col);
		    }

		    msg_print("Your light is growing faint.");
		}
	    }
	}
	else {
	    player_light = FALSE;
	    find_flag = FALSE;
	    move_light(char_row, char_col, char_row, char_col);
	}
    }
    else if(i_ptr->p1 > 0) {
	--i_ptr->p1;
	player_light = TRUE;
	move_light(char_row, char_col, char_row, char_col);
    }

    /* Update counters and messages */
    f_ptr = &py.flags;

    /* Check food status */
    regen_amount = PLAYER_REGEN_NORMAL;

    if(f_ptr->food < PLAYER_FOOD_ALERT) {
	if(f_ptr->food < PLAYER_FOOD_WEAK) {
	    if(f_ptr->food < 0) {
		regen_amount = 0;
	    }
	    else if(f_ptr->food < PLAYER_FOOD_FAINT) {
		regen_amount = PLAYER_REGEN_FAINT;
	    }
	    else if(f_ptr->food < PLAYER_FOOD_WEAK) {
		regen_amount = PLAYER_REGEN_WEAK;
	    }
	    
	    if((f_ptr->status & 0x00000002) == 0) {
		f_ptr->status |= 0x00000003;
		msg_print("You are getting weak from hunger.");
		
		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_rol, char_row, char_col);
		}
		
		prt_hunger();
	    }
	    
	    if(f_ptr->food < PLAYER_FOOD_FAINT) {
		if(randint(8) == 1) {
		    f_ptr->paralysis += randint(5);
		    msg_print("You faint from the lack of food.");
		    
		    if(find_flag) {
			find_flag = FALSE;
			move_light(char_row, char_col, char_row, char_col);
		    }
		}
	    }
	}
	else {
	    if((f_ptr->status & 0x00000001) == 0) {
		f_ptr->status |= 0x00000001;
		msg_print("You are getting hungry.");

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}

		prt_hunger();
	    }
	}
    }

    /* 
     * Food consumption 
     * Note: Speeded up characters really burn up the food!
     */
    if(f_ptr->speed < 0) {
	f_ptr->food += (-(f_ptr->speed * f_ptr->speed) - f_ptr->food_digested);
    }
    else {
	f_ptr->food -= f_ptr->food_digested;
    }

    /* Regenerate */
    p_ptr = &py.misc;

    if(f_ptr->regenerate) {
	regen_amount = regen_amount * 1.5;
    }

    if(f_ptr->rest > 0) {
	regen_amount = regen_amount * 2;
    }

    if(py.flags.poinsoned < 1) {
	if(p_ptr->chp < p_ptr->mhp) {
	    regenhp(regen_amount);
	}
    }

    if(p_ptr->cmana < p_ptr->mana) {
	regenmana(regen_amount);
    }

    /* Blindness */
    if(f_ptr->blind > 0) {
	if((f_ptr->status & 0x00000004) == 0) {
	    f_ptr->status |= 0x00000004;
	    prt_map();
	    prt_blind();

	    if(search_flag) {
		search_off();
	    }
	}

	--f_ptr->blind;

	if(f_ptr->blind == 0) {
	    f_ptr->status &= 0xFFFFFFFB;
	    prt_blind;
	    prt_map();
	    msg_print("The veil of darkness lifts.");

	    if(find_flag) {
		find_flag = FALSE;
	    }

	    /* Turn light back on */
	    move_char(5);

	    /* Light creatures */
	    creatures(FALSE);
	}
    }

    /* Confusion */
    if(f_ptr->confused > 0) {
	if((f_ptr->status & 0x00000008) == 0) {
	    f_ptr->status |= 0x00000008;
	    prt_confused();
	}

	--f_ptr->confused;

	if(f_ptr->confused == 0) {
	    f_ptr->status &= 0xFFFFFFF7;
	    prt_confused();
	    msg_print("You feel less confused now.");

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }
	}
    }

    /* Afraid */
    if(f_ptr->afraid > 0) {
	if((f_ptr->status & 0x00000010) == 0) {
	    if((f_ptr->shero + f_ptr->hero) > 0) {
		f_ptr->afraid = 0;
	    }
	    else {
		f_ptr->status |= 0x00000010;
		prt_afraid();
	    }
	}
	else if((f_ptr->shero + f_ptr->hero) > 0) {
	    f_ptr->afraid = 1;
	}

	--f_ptr->afraid;

	if(f_ptr->afraid == 0) {
	    f_ptr->status &= 0xFFFFFFEF;
	    prt_afraid();
	    msg_print("You feel bolder now.");

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }
	}
    }

    /* Poisoned */
    if(f_ptr->poisoned > 0) {
	if((f_ptr->status & 0x00000020) == 0) {
	    f_ptr->status |= 0x00000020;
	    prt_poisoned();
	}

	--f_ptr->poisoned;

	if(f_ptr->poisoned == 0) {
	    f_ptr->status &= 0xFFFFFFDF;
	    prt_poisoned();
	    msg_print("You feel better.");

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }
	}
	else {
	    switch(con_adj()) {
	    case -4:
		take_hit(4, "poison.");
		
		break;
	    case -3:
	    case -2:
		take_hit(3, "poison.");
		
		break;
	    case -1:
		take_hit(2, "poison.");
		
		break;
	    case 0:
		take_hit(1, "poison.");
		
		break;
	    case 1:
	    case 2:
	    case 3:
		if((turn % 2) == 0) {
		    take_hit(1, "poison.");
		}
		
		break;
	    case 4:
	    case 5:
		if((turn % 3) == 0) {
		    take_hit(1, "poison.");
		}
		
		break;
	    case 6:
		if((turn % 4) == 0) {
		    take_hit(1, "poison.");
		}
		
		break;
	    }
	}
    }

    /* Fast */
    if(f_ptr->fast > 0) {
	if((f_ptr->status & 0x00000040) == 0) {
	    f_ptr->status |= 0x00000040;
	    change_speed(-1);
	    msg_print("You feel yourself moving faster.");

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }
	}

	--f_ptr->fast;

	if(f_ptr->fast == 0) {
	    f_ptr->status &= 0xFFFFFFBF;
	    change_speed(1);
	    msg_print("You feel yourself slow down.");

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }
	}
    }

    /* Slow */
    if(f_ptr->slow > 0) {
	if((f_ptr->status & 0x00000080) == 0) {
	    f_ptr->status |= 0x00000080;
	    change_speed(1);
	    msg_print("You feel yourself moving slower.");

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }
	}

	--f_ptr->slow;

	if(f_ptr->slow == 0) {
	    f_ptr->status &= 0xFFFFFF7F;
	    change_speed(-1);
	    msg_print("You feel yourself speed up.");

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }
	}
    }

    /* Resting is over? */
    if(f_ptr->rest > 0) {
#ifdef SLOW
	/* 
	 * Hibernate every 20 iterations so that process does not eat up
	 *  system...
	 */
	if((f_ptr->rest % 20) == 1) {
	    sleep(1);
	}
#endif

	--f_ptr->rest;

	/*
	 * Do not need to refresh screen here, if any movement/hit occurs
	 * update_mon/take_hit will turn off resting and screen refreshes
	 */
	/* put_qio() */

	/* Resting over */
	if(f_ptr->rest == 0) {
	    rest_off();
	}
    }

    /* Hallucinating? (Random character appear!) */
    if(f_ptr->image > 0) {
	--f_ptr->image;

	if(f_ptr->image == 0) {
	    draw_cave();
	}
    }

    /* Paralysis */
    if(f_ptr->paralysis > 0) {
	/* When paralysis true, you can not see any movement that occurs */
	--f_ptr->paralysis;

	if(f_ptr->rest > 0) {
	    rest_off();
	}

	if(search_flag) {
	    search_off();
	}
    }

    /* Protection from evil counter */
    if(f_ptr->protevil > 0) {
	--f_ptr->protevil;
    }

    /* Invulnerability */
    if(f_ptr->invuln > 0) {
	if((f_ptr->status & 0x00001000) == 0) {
	    f_ptr->status |= 0x00001000;

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }

	    py.misc.pac += 100;
	    py.misc.dis_ac += 100;
	    prt_pac();
	    msg_print("Your skin turns into steel!");
	}

	--f_ptr->invuln;

	if(f_ptr->invuln == 0) {
	    f_ptr->status &= 0xFFFFEFFF;

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }

	    py.misc.pac -= 100;
	    py.misc.dis_ac -= 100;
	    prt_pac();
	    msg_print("Your skin returns to normal...");
	}
    }

    /* Heroism */
    if(f_ptr->hero > 0) {
	if((f_ptr->status & 0x00002000) == 0) {
	    f_ptr->status |= 0x00002000;

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }

	    p_ptr = &py.misc;
	    p_ptr->mhp += 10;
	    p_ptr->chp += 10.0;
	    p_ptr->bth += 12;
	    p_ptr->bthb += 12;
	    msg_print("You feel like a HERO!");
	    prt_mhp();
	}

	--f_ptr->hero;

	if(f_ptr->hero == 0) {
	    f_ptr->status &= 0xFFFFDFFF;

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }

	    p_ptr = &py.misc;
	    p_ptr->mhp -= 10;

	    if(p_ptr->chp > p_ptr->mhp) {
		p_ptr->chp = p_ptr->mhp;
	    }

	    p_ptr->bth -= 12;
	    p_ptr->bthb -= 12;
	    msg_print("The heroism wears off.");
	    prt_mhp();
	}
    }

    /* Super heroism */
    if(f_ptr->shero > 0) {
	if((f_ptr->status & 0x00004000) == 0) {
	    f_ptr->status |= 0x00004000;

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }

	    p_ptr = &py.misc;
	    p_ptr->mhp += 20;
	    p_ptr->chp += 20.0;
	    p_ptr->bth += 24;
	    p_ptr->bthb += 24;
	    msg_print("You feel like a SUPER HERO!");
	    prt_mhp();
	}

	--f_ptr->shero;

	if(f_ptr->shero == 0) {
	    f_ptr->status &= 0xFFFFBFFF;

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }

	    p_ptr = &py.misc;
	    p_ptr->mhp -= 20;

	    if(p_ptr->chp > p_ptr->mhp) {
		p_ptr->chp = p_ptr->mhp;
	    }

	    p_ptr->bth -= 24;
	    p_ptr->bthb -= 24;
	    msg_print("The super heroism wears off.");
	    prt_mhp();
	}
    }

    /* Blessed */
    if(f_ptr->blessed > 0) {
	if((f_ptr->status & 0x00008000) == 0) {
	    f_ptr->status |= 0x00008000;

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }

	    p_ptr = &py.misc;
	    p_ptr->bth += 5;
	    p_ptr->bthb += 5;
	    p_ptr->pac += 2;
	    p_ptr->dis_ac += 2;
	    msg_print("You feel righteous!");
	    prt_pac();
	}

	--f_ptr->blessed;

	if(f_ptr->blessed == 0) {
	    f_ptr->status &= 0xFFFF7FFF;

	    if(find_flag) {
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }

	    p_ptr = &py.misc;
	    p_ptr->bth -= 5;
	    p_ptr->bthb -= 5;
	    p_ptr->pac -= 2;
	    p_ptr->dis_ac -= 2;
	    msg_print("The prayer has expired.");
	    prt_pac();
	}
    }

    /* Resist heat */
    if(f_ptr->resist_heat > 0) {
	--f_ptr->resist_heat;
    }

    /* Resist cold */
    if(f_ptr->resist_cold > 0) {
	--f_ptr->resist_cold;
    }

    /* Detect invisible */
    if(f_ptr->detect_inv > 0) {
	if((f_ptr->status & 0x00010000) == 0) {
	    f_ptr->status |= 0x00010000;
	    f_ptr->see_inv = TRUE;
	}

	--f_ptr->detect_inv;

	if(f_ptr->detect_inv == 0) {
	    f_ptr->status &= 0xFFFEFFFF;
	    f_ptr->see_inv = FALSE;
	    py_bonuses(blank_treasure, 0);
	}
    }

    /* Timed infra-vision */
    if(f_ptr->tim_infra > 0) {
	if((f_ptr->status & 0x00020000) == 0) {
	    f_ptr->status |= 0x00020000;
	    ++f_ptr->see_infra;
	}

	--f_ptr->tim_infra;

	if(f_ptr->tim_infra == 0) {
	    f_ptr->status &= 0xFFFDFFFF;
	    --f_ptr->see_infra;
	}
    }

    /* 
     * Word-of-Recall
     * Note: Word-of-Recall is a delayed action
     */
    if(f_ptr->word_recall > 0) {
	if(f_ptr->word_recall == 1) {
	    moria_flag = TRUE;
	    ++f_ptr->paralysis;
	    f_ptr->word_recall = 0;

	    if(dun_level > 0) {
		dun_level = 0;
		msg_print("You feel yourself yanked upwards!");
	    }
	    else if(py.misc.max_lev != 0) {
		dun_level = py.misc.max_lev;
		msg_print("You feel yourself yanked downwards!");
	    }
	}
	else {
	    --f_ptr->word_recall;
	}
    }

    /* Check hit points for adjusting... */
    p_ptr = &py.misc;

    if(!find_flag) {
	if(py.flags.rest < 1) {
	    if(old_chp != (int)(p_ptr->chp)) {
		if(p_ptr->chp > p_ptr->mhp) {
		    p_ptr->chp = (double)p_ptr->mhp;
		}

		prt_chp();
		old_chp = (int)p_ptr->chp;
	    }

	    if(old_cmana != (int)(p_ptr->cmana)) {
		if(p_ptr->cmana > p_ptr->mana) {
		    p_ptr->cmana = (double)p_ptr->mana;
		}

		prt_cmana();
		old_cmana = (int)p_ptr->cmana;
	    }
	}
    }

    /* Accept a command? */
    if((py.flags.paralysis < 1)
       && (py.flags.rest < 1)
       && !death) {
	/* Attempt a command and execute it */

	print_stat = 0;
	reset_flag = FALSE;

	/* Random teleportation */
	if(py.flags.teleport) {
	    if(randint(100) == 1) {
		/* No need for move_char(5) */
		find_flag = FALSE;
		teleport(40);
	    }
	}

	if(!find_flag) {
	    /* Move the cursor to the player's character */
	    print("", char_row, char_col);
	    save_msg_flag = msg_flag;

	    inkey(&command);

	    while(command == ' ') {
		inkey(&command);
	    }

	    if(save_msg_flag) {
		erase_line(MSG_LINE, 0);
	    }

	    global_com_val = command;
	}

	/* Commands are executed in following subroutines */
	if(key_bindings == ORIGINAL) {
	    original_commands(&global_com_val);
	}
	else {
	    rogue_like_commands(&global_com_val);
	}

	/* End of commands */

	while(reset_flag && !moria_flag) {
	    print_stat = 0;
	    reset_flag = FALSE;

	    /* Random teleportation */
	    if(py.flags.teleport) {
		if(randint(100) == 1) {
		    /* No need for move_char(5) */
		    find_flag = FALSE;
		    teleport(40);
		}
	    }

	    if(!find_flag) {
		/* Move the cursor to the player's character */
		print("", char_row, char_col);
		save_msg_flag = msg_flag;

		inkey(&command);

		while(command == ' ') {
		    inkey(&command);
		}

		if(save_msg_flag) {
		    erase_line(MSG_LINE, 0);
		}

		global_com_val = command;
	    }

	    /* Commands are executed in following subroutines */
	    if(key_bindings == ORIGINAL) {
		original_commands(&global_com_val);
	    }
	    else {
		rogue_like_commands(&global_com_val);
	    }

	    /* End of commands */
	}
    }

    /* Teleport? */
    if(teleport_flag) {
	teleport(100);
    }

    /* Move the creatures */
    if(!moria_flag) {
	creatures(TRUE);
    }

    /* Exit when moria_flag is set */

    while(!moria_flag) {
	/* Increment turn counter */
	++turn;

	/* Check for game hours */
	if(!wizard1) {
	    if((turn % 250) == 1) {
		if(!checktime()) {
		    if(close_flag > 4) {
			if(search_flag) {
			    search_off();
			}

			if(py.flags.rest > 0) {
			    rest_off();
			}

			find_flag = FALSE;
			msg_print("The gates to Moria are now closed.");

			/* Make sure player sees the message */
			msg_print(" ");
			save_char(TRUE, FALSE);

			while(TRUE) {
			    save_char(TRUE, FALSE);
			}
		    }
		    else {
			if(search_flag) {
			    search_off();
			}

			if(py.flags.rest > 0) {
			    rest_off();
			}

			find_flag = FALSE;
			move_light(char_row, char_col, char_row, char_col);
			++closing_flag;
			msg_print("The gates to Moria are closing...");
			msg_print("Please finish up or save your game.");

			/* Make sure the player sees the message */
			msg_print(" ");
		    }
		}
	    }
	}

	/* Turn over the store contents every, say, 1000 turns */
	if((dun_level != 0) && ((turn % 1000) == 0)) {
	    store_maint();
	}

	/* Check for creature generation */
	if(randint(MAX_MALLOC_CHANCE) == 1) {
	    alloc_monster(set_floor, 1, MAX_SIGHT, FALSE);
	}

	/* Screen may need updating, used mostly for stats */
	if(print_stat != 0) {
	    if(print_stat & 0x0001) {
		prt_strength();
	    }

	    if(print_stat & 0x0002) {
		prt_dexterity();
	    }

	    if(print_stat & 0x0004) {
		prt_constitution();
	    }

	    if(print_stat & 0x0008) {
		prt_intelligence();
	    }

	    if(print_stat & 0x0010) {
		prt_wisdom();
	    }

	    if(print_stat & 0x0020) {
		prt_charisma();
	    }

	    if(print_stat & 0x0040) {
		prt_pac();
	    }

	    if(print_stat & 0x0100) {
		prt_mhp();
	    }

	    if(print_stat & 0x0200) {
		prt_title();
	    }

	    if(prt_stat & 0x0400) {
		prt_level();
	    }
	}

	/* Check light status */
	i_ptr = &inventory[INVEN_LIGHT];

	if(player_light) {
	    if(i_ptr->p1 > 0) {
		--i_ptr->p1;

		if(i_ptr->p1 == 0) {
		    player_light = FALSE;
		    find_flag = FALSE;
		    msg_print("Your light has gone out!");
		    move_light(char_row, char_col, char_row, char_col);
		}
		else if(i_ptr->p1 < 400) {
		    if(randint(5) == 1) {
			if(find_flag) {
			    find_flag = FALSE;
			    move_light(char_row, char_col, char_row, char_col);
			}

			msg_print("Your light is growing faint.");
		    }
		}
	    }
	    else {
		player_light = FALSE;
		find_flag = FALSE;
		move_light(char_row, char_col, char_row, char_col);
	    }
	}
	else if(i_ptr->p1 > 0) {
	    --i_ptr->p1;
	    player_light = TRUE;
	    move_light(char_row, char_col, char_row, char_col);
	}

	/* Update counters and messages */
	f_ptr = &py.flags;

	/* Check food status */
	regen_amount = PLAYER_REGEN_NORMAL;

	if(f_ptr->food < PLAYER_FOOD_ALERT) {
	    if(f_ptr->food < PLAYER_FOOD_WEAK) {
		if(f_ptr->food < 0) {
		    regen_amount = 0;
		}
		else if(f_ptr->food < PLAYER_FOOD_FAINT) {
		    regen_amount = PLAYER_REGEN_FAINT;
		}
		else if(f_ptr->food < PLAYER_FOOD_WEAK) {
		    regen_amount = PLAYER_REGEN_WEAK;
		}

		if((f_ptr->status & 0x00000002) == 0) {
		    f_ptr->status |= 0x00000003;
		    msg_print("You are getting weak from hunger.");

		    if(find_flag) {
			find_flag = FALSE;
			move_light(char_row, char_rol, char_row, char_col);
		    }

		    prt_hunger();
		}

		if(f_ptr->food < PLAYER_FOOD_FAINT) {
		    if(randint(8) == 1) {
			f_ptr->paralysis += randint(5);
			msg_print("You faint from the lack of food.");

			if(find_flag) {
			    find_flag = FALSE;
			    move_light(char_row, char_col, char_row, char_col);
			}
		    }
		}
	    }
	    else {
		if((f_ptr->status & 0x00000001) == 0) {
		    f_ptr->status |= 0x00000001;
		    msg_print("You are getting hungry.");

		    if(find_flag) {
			find_flag = FALSE;
			move_light(char_row, char_col , char_row, char_col);
		    }

		    prt_hunger();
		}
	    }
	}

	/*
	 * Food consumption
	 * Note: Speeded up characters really burn up the food!
	 */
	if(f_ptr->speed < 0) {
	    f_ptr->food += (-(f_ptr->speed * f_ptr->speed) - f_ptr->food_digested);
	}
	else {
	    f_ptr->food -= f_ptr->food_digested;
	}

	/* Regenerate */
	p_ptr = &py.misc;

	if(f_ptr->regenerate) {
	    regen_amount = regen_amount * 1.5;
	}

	if(f_ptr->rest > 0) {
	    regen_amount = regen_amount * 2;
	}

	if(py.flags.poinsoned < 1) {
	    if(p_ptr->chp < p_ptr->mhp) {
		regenhp(regen_amount);
	    }
	}

	if(p_ptr->cmana < p_ptr->mana) {
	    regenmana(regen_amount);
	}

	/* Blindness */
	if(f_ptr->blind > 0) {
	    if((f_ptr->status & 0x00000004) == 0) {
		f_ptr->status |= 0x00000004;
		prt_map();
		prt_blind();

		if(search_flag) {
		    search_off();
		}
	    }

	    --f_ptr->blind;

	    if(f_ptr->blind == 0) {
		f_ptr->status &= 0xFFFFFFFB;
		prt_blind();
		prt_map();
		msg_print("The veil of darkness lifts.");

		if(find_flag) {
		    find_flag = FALSE;
		}

		/* Turn light back on */
		move_char(5);

		/* Light creatures */
		creatures(FALSE);
	    }
	}

	/* Confusion */
	if(f_ptr->confused > 0) {
	    if((f_ptr->status & 0x00000008) == 0) {
		f_ptr->status |= 0x00000008;
		prt_confused();
	    }

	    --f_ptr->confused;

	    if(f_ptr->confused == 0) {
		f_ptr->status &= 0xFFFFFFF7;
		prt_confused();
		msg_print("You feel less confused now.");

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}
	    }
	}

	/* Afraid */
	if(f_ptr->afraid > 0) {
	    if((f_ptr->status & 0x00000010) == 0) {
		if((f_ptr->shero + f_ptr->hero) > 0) {
		    f_ptr->afraid = 0;
		}
		else {
		    f_ptr->status |= 0x00000010;
		    prt_afraid();
		}
	    }
	    else if((f_ptr->shero + f_ptr->hero) > 0) {
		f_ptr->afraid = 1;
	    }

	    --f_ptr->afraid;

	    if(f_ptr->afraid == 0) {
		f_ptr->status &= 0xFFFFFFEF;
		prt_afraid();
		msg_print("You feel bolder now.");

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}
	    }
	}

	/* Poisoned */
	if(f_ptr->poinsoned > 0) {
	    if((f_ptr->status & 0x00000020) == 0) {
		f_ptr->status |= 0x00000020;
		prt_poisoned();
	    }

	    --f_ptr->poisoned;

	    if(f_ptr->poinsoned == 0) {
		f_ptr->status &= 0xFFFFFFDF;
		prt_poisoned();
		msg_print("You feel better.");

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}
	    }
	    else {
		switch(con_adj()) {
		case -4:
		    take_hit(4, "poison.");

		    break;
		case -3:
		case -2:
		    take_hit(3, "poison.");

		    break;
		case -1:
		    take_hit(2, "poison.");

		    break;
		case 0:
		    take_hit(1, "poison.");

		    break;
		case 1:
		case 2:
		case 3:
		    if((turn % 2) == 0) {
			take_hit(1, "poison.");
		    }

		    break;
		case 4:
		case 5:
		    if((turn % 3) == 0) {
			take_hit(1, "poison.");
		    }

		    break;
		case 6:
		    if((turn % 4) == 0) {
			take_hit(1, "poison.");
		    }

		    break;
		}
	    }
	}

	/* Fast */
	if(f_ptr->fast > 0) {
	    if((f_ptr->status & 0x00000040) == 0) {
		f_ptr->status |= 0x00000040;
		change_speed(-1);
		msg_print("You feel yourself moving faster.");

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}
	    }

	    --f_ptr->fast;

	    if(f_ptr->fast == 0) {
		f_ptr->status &= 0xFFFFFFBF;
		change_speed(1);
		msg_print("You feel yourself slow down.");

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}
	    }
	}

	/* Slow */
	if(f_ptr->slow > 0) {
	    if((f_ptr->status & 0x00000080) == 0) {
		f_ptr->status |= 0x00000080;
		change_speed(1);
		msg_print("You feel yourself moving slower.");

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}
	    }

	    --f_ptr->slow;

	    if(f_ptr->slow == 0) {
		f_ptr->status &= 0xFFFFFF7F;
		change_speed(-1);
		msg_print("You feel yourself speed up.");

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}
	    }
	}

	/* Resting is over? */
	if(f_ptr->rest > 0) {
#ifdef SLOW
	    /*
	     * Hibernate every 20 iterations so that process does not eat up
	     * system...
	     */
	    if((f_ptr->rest % 20) == 1) {
		sleep(1);
	    }
#endif

	    --f_ptr->rest;

	    /*
	     * Do not need to refresh screen here, if any movement/hit occurs
	     * update_mon/take_hit will turn off resting and screen refreshes
	     */
	    /* put_qio() */

	    /* Resting over */
	    if(f_ptr->rest == 0) {
		rest_off();
	    }
	}

	/* Hallucinating? (Random character appear!) */
	if(f_ptr->image > 0) {
	    --f_ptr->image;

	    if(f_ptr->image == 0) {
		draw_cave();
	    }
	}

	/* Paralysis */
	if(f_ptr->paralysis > 0) {
	    /* When paralysis true, you can not see any movement that occurs */
	    --f_ptr->paralysis;

	    if(f_ptr->rest > 0) {
		rest_off();
	    }

	    if(search_flag) {
		search_off();
	    }
	}

	/* Protection from evil counter */
	if(f_ptr->protevil > 0) {
	    --f_ptr->protevil;
	}

	/* Invulnerability */
	if(f_ptr->invuln > 0) {
	    if((f_ptr->status & 0x00001000) == 0) {
		f_ptr->status |= 0x00001000;

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}

		py.misc.pac += 100;
		py.misc.dis_ac += 100;
		prt_pac();
		msg_print("Your skin turns into steel!");
	    }

	    --f_ptr->invuln;

	    if(f_ptr->invuln == 0) {
		f_ptr->status &= 0xFFFFEFFF;

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}

		py.misc.pac -= 100;
		py.misc.dis_ac -= 100;
		prt_pac();
		msg_print("Your skin returns to normal...");
	    }
	}

	/* Heroism */
	if(f_ptr->hero > 0) {
	    if((f_ptr->status & 0x00002000) == 0) {
		f_ptr->status |= 0x00002000;

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}

		p_ptr = &py.misc;
		p_ptr->mhp += 10;
		m_ptr->chp += 20.0;
		p_ptr->bth += 12;
		p_ptr->bthb += 12;
		msg_print("You feel like a HERO!");
		prt_mhp();
	    }

	    --f_ptr->hero;

	    if(f_ptr->hero == 0) {
		f_ptr->status &= 0xFFFFDFFF;

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}

		p_ptr = &py.misc;
		p_ptr->mhp -= 10;

		if(p_ptr->chp > p_ptr->mhp) {
		    p_ptr->chp = p_ptr->mhp;
		}

		p_ptr->bth -= 12;
		p_ptr->bthb -= 12;
		msg_print("The heroism wears off.");
		prt_mhp();
	    }
	}

	/* Super heroism */
	if(f_ptr->shero > 0) {
	    if((f_ptr->status 0x00004000) == 0) {
		f_ptr->status |= 0x00004000;

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}

		p_ptr = &py.misc;
		p_ptr->mhp += 20;
		p_ptr->chp += 20;
		p_ptr->bth += 24;
		p_ptr->bthb += 24;
		msg_print("You feel like a SUPER HERO!");
		prt_mhp();
	    }

	    --f_ptr->shero;

	    if(f_ptr->shero == 0) {
		f_ptr->status &= 0xFFFFBFFF;

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}

		p_ptr = &py.misc;
		p_ptr->mhp -= 20;

		if(p_ptr->chp > p_ptr->mhp) {
		    p_ptr->chp = p_ptr->mhp;
		}

		p_ptr->bth -= 24;
		p_ptr->bthb -= 24;
		msg_print("The super heroism wears off.");
		prt_mhp();
	    }
	}

	/* Blessed */
	if(f_ptr->blessed > 0) {
	    if((f_ptr->status & 0x00008000) == 0) {
		f_ptr->status |= 0x00008000;

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}

		p_ptr = &py.misc;
		p_ptr->bth += 5;
		p_ptr->bthb += 5;
		p_ptr->pac += 2;
		p_ptr->dis_ac += 2;
		msg_print("You feel righteous!");
		prt_pac();
	    }

	    --f_ptr->blessed;

	    if(f_ptr->blessed == 0) {
		f_ptr->status 7= 0xFFFF7FFF;

		if(find_flag) {
		    find_flag = FALSE;
		    move_light(char_row, char_col, char_row, char_col);
		}

		p_ptr = &py.misc;
		p_ptr->bth -= 5;
		p_ptr->bthb -= 5;
		p_ptr->pac -= 2;
		p_ptr->dis_ac -= 2;
		msg_print("The prayer has expired.");
		prt_pac();
	    }
	}

	/* Resist heat */
	if(f_ptr->resist_heat > 0) {
	    --f_ptr->resist_heat;
	}

	/* Resist cold */
	if(f_ptr->resist_cold > 0) {
	    --f_ptr->resist_cold;
	}

	/* Detect invisible */
	if(f_ptr->detect_inv > 0) {
	    if((f_ptr->status & 0x00010000) == 0) {
		f_ptr->status |= 0x00010000;
		f_ptr->see_inv = TRUE;
	    }

	    --f_ptr->detect_inv;

	    if(f_ptr->detect_inv == 0) {
		f_ptr->status &= 0xFFFEFFFF;
		f_ptr->see_inv = FALSE;
		py_bonuses(blank_treasure, 0);
	    }
	}

	/* Timed infra-vision */
	if(f_ptr->tim_infra > 0) {
	    if((f_ptr->status & 0x00020000) == 0) {
		f_ptr->status |= 0x00020000;
		++f_ptr->see_infra;
	    }

	    --f_ptr->tim_infra;

	    if(f_ptr->tim_infra == 0) {
		f_ptr->status &= 0xFFFDFFFF;
		--f_ptr->see_infra;
	    }
	}

	/* 
	 * Word-of-Recall
	 * Note: Word-of-Recall is a delayed action
	 */
	if(f_ptr->word_recall > 0) {
	    if(f_ptr->word_recall == 1) {
		moria_flag = TRUE;
		++f_ptr->paralysis;
		f_ptr->word_recall = 0;

		if(dun_level > 0) {
		    dun_level = 0;
		    msg_print("You feel yourself yanked upwards!");
		}
		else if(py.misc.max_lev != 0) {
		    dun_level = py.misc.max_lev;
		    msg_print("You feel yourse yanked downwards!");
		}
	    }
	    else {
		--f_ptr->word_recall;
	    }
	}

	/* Check hit points for adjusting... */
	p_ptr = &py.misc;

	if(!find_flag) {
	    if(py.flags.rest < 1) {
		if(old_chp != (int)(p_ptr->chp)) {
		    if(p_ptr->chp > p_ptr->mhp) {
			p_ptr->chp = (double)p_ptr->mhp;
		    }

		    prt_chp();
		    old_chp = (int)p_ptr->chp;
		}

		if(old_cmana != (int)(p_ptr->cmana)) {
		    if(p_ptr->cmana > p_ptr->mana) {
			p_ptr->camana = (double)p_ptr->mana;
		    }

		    prt_cmana();
		    old_cmana = (int)p_ptr->cmana;
		}
	    }
	}

	/* Accept a command? */
	if((py.flags.paralysis < 1)
	   && (py.flags.rest < 1)
	   && !death) {
	    /* Attempt a command and execute it */

	    print_stat = 0;
	    rest_falg = FALSE;

	    /* Random teleportation */
	    if(py.flags.teleport) {
		if(randint(100) == 1) {
		    /* No need for move_char(5) */
		    find_flag = FALSE;
		    teleport(40);
		}
	    }

	    if(!find_flag) {
		/* Move the cursor to the player's character */
		print("", char_row, char_col);
		save_msg_flag = msg_flag;

		inkey(&command);

		while(command == ' ') {
		    inkey(&command);
		}

		if(save_msg_flag) {
		    erase_line(MSG_LINE, 0);
		}

		global_com_val = command;
	    }

	    /* Commands are executed in following subroutines */
	    if(key_bindings == ORIGINAL) {
		original_commands(&global_com_val);
	    }
	    else {
		rogue_like_commands(&global_com_val);
	    }

	    /* End of commands */

	    while(rest_flag && !moria_flag) {
		print_stat = 0;
		reset_flag = FALSE;

		/* Random teleportation */
		if(py.flags.teleport) {
		    if(randint(100) == 1) {
			/* No need for move_char(5) */
			find_flag = FALSE;
			teleport(40);
		    }
		}

		if(!find_flag) {
		    /* Move the cursor to the player's character */
		    print("", char_row, char_col);
		    save_msg_flag = msg_flag;

		    inkey(&command);

		    while(command == ' ') {
			inkey(&command);
		    }

		    if(save_msg_flag) {
			erase_line(MSG_LINE, 0);
		    }

		    global_com_val = command;
		}

		/* Commands are executed in following subroutines */
		if(key_bindings == ORIGINAL) {
		    original_commands(&global_com_val);
		}
		else {
		    rogue_like_commands(&global_com_val);
		}

		/* End of commands */
	    }
	}

	/* Teleport? */
	if(teleport_flag) {
	    teleport(100);
	}

	/* Move the creatures */
	if(!moria_flag) {
	    creatures(TRUE);
	}

	/* Exit when moria_flag is set */
    }

    /* Fixed "SLOW" bug    06-11-86 RAK */
    if(search_flag) {
	search_off();
    }
}

void original_commands(int *com_val)
{
    int y;
    int x;
    int dir_val;
    char command;
    int i;
    vtype out_val;
    vtype tmp_str;
    struct stats *s_ptr;
    struct flags *f_ptr;
    int set_floor();

    switch(*com_val) {
    case 0:
    case 11:
	/* ^K == exit */
	flush();

	if(get_com("Enter 'Q' to quit", &command)) {
	    switch(command) {
	    case 'q':
	    case 'Q':
		moria_flag = TRUE;
		death = TRUE;

		/* Need dot on the end to be consistent with creatures.c */
		strcpy(died_from, "Quitting.");

		break;
	    default:

		break;
	    }
	}

	reset_flag = TRUE;

	break;
    case 16:
	/* ^P == repeat */
	repeat_msg();
	reset_flag = TRUE;

	break;
    case 23:
	/* ^W == password */
	if(wizard1) {
	    wizard1 = FALSE;
	    wizard2 = FALSE;
	    msg_print("Wizard mode off.");
	}
	else {
	    if(check_pswd()) {
		msg_print("Wizard mode on.");
	    }
	}

	reset_flag = TRUE;

	break;
    case 18:
	/* ^R == redraw */
	really_clear_screen();
	draw_cave();
	reset_flag = TRUE;

	break;
    case 24:
	/* ^X == save */
	if(total_winner) {
	    msg_print("You are a Total Winner, your character must be retired...");
	    msg_print("Use <Control>-K to when you are ready to quit.");
	}
	else {
	    if(search_flag) {
		search_off();
	    }

	    save_char(TRUE, FALSE);
	}

	reset_flag = TRUE;

	break;
    case 32:
	/* SPACE do nothing */
	reset_flag = TRUE;

	break;
    case '!':
	/* ! == shell */
	shell_out();
	reset_flag = TRUE;

	break;
    case 46:
	y = char_row;
	x = char_col;

	if(get_dir("Which direction?", &dir_val, com_val, &y, &x)) {
	    find_flag = TRUE;
	    move_char(dir_val);
	}

	break;
    case 47:
	/* / == identify */
	ident_char();
	reset_flag = TRUE;

	break;
    case 49:
	/* Move dir 1 */
	move_char(*com_val - 48);

	break;
    case 50:
	/* Move dir 2 */
	move_char(*com_val - 48);

	break;
    case 51:
	/* Move dir 3 */
	move_char(*com_val - 48);

	break;
    case 52:
	/* Move dir 4 */
	move_char(*com_val - 48);

	break;
    case 53:
	/* Rest one turn */
	move_char(*com_val - 48);

#ifdef SLOW
	/* Sleep 1/10th of a second */
	sleep(0);
#endif

	flush();

	break;
    case 54:
	/* Move dir 6 */
	move_char(*com_val - 48);

	break;
    case 55:
	/* Move dir 7 */
	move_char(*com_val - 48);

	break;
    case 56:
	/* Move dir 8 */
	move_char(*com_val - 48);

	break;
    case 57:
	/* Move dir 9 */
	move_char(*com_val - 48);

	break;
    case 60:
	/* < == go up */
	go_up();

	break;
    case 62:
	/* > == go down */
	go_down();

	break;
    case 63:
	/* ? == help */
	original_help();

	/* Free move */
	reset_flag = TRUE;

	break;
    case 66:
	/* B == bash */
	bash();

	break;
    case 67:
	/* C == character */
	if(get_com("Print to file? (Y/N)", &ocmmand)) {
	    switch(command) {
	    case 'y':
	    case 'Y':
		file_character();

		break;
	    case 'n':
	    case 'N':
		change_name();
		draw_cave();
		draw_cave();

		break;
	    default:
	    }
	}

	/* Free move */
	reset_flag = TRUE;

	break;
    case 68:
	/* D == disarm */
	disarm_trap();

	break;
    case 69:
	/* E == eat */
	eat();

	break;
    case 70:
	/* F == refill */
	refill_lamp();

	break;
    case 76:
	/* L == Location */
	if((py.flags.blind > 0) || no_light()) {
	    msg_print("You can't see your map.");
	}
	else {
	    sprintf(out_val,
		    "Section [%d,%d]; Location == [%d,%d]",
		    (((char_row - 1) / OUTPAGE_HEIGHT) + 1),
		    (((char_col - 1) / OUTPAGE_WIDTH) + 1),
		    char_row,
		    char_col);

	    msg_print(out_val);
	}

	/* Free move */
	reset_flag = TRUE;

	break;
    case 80:
	/* P == print map */
	if((py.flags.blind > 0) || no_light()) {
	    msg_print("You can't see to draw a map.");
	}
	else {
	    print_map();
	}

	/* Free move */
	rest_flag = TRUE;

	break;
    case 82:
	/* R == rest */
	rest();

	break;
    case 83:
	/* S == search mode */
	if(search_flag) {
	    search_off();

	    /* Free move */
	    reset_flag = TRUE;
	}
	else if(py.flags.blind > 0) {
	    msg_print("You are incapable of searching while blind.");
	}
	else {
	    search_on();

	    /* Free move */
	    reset_flag = TRUE;
	}

	break;
    case 84:
	/* T == tunnel */
	y = char_row;
	x = char_col;

	if(get_dir("Which direction?", &dir_val, com_val, &y, &x)) {
	    tunnel(y, x);
	}

	break;
    case 97:
	/* a == aim */
	aim();

	break;
    case 98:
	/* b == browse */
	examine_book();
	reset_flag = TRUE;

	break;
    case 99:
	/* c == close */
	closeobject();

	break;
    case 100:
	/* d == drop */
	drop();

	break;
    case 101:
	/* e == equipment */

	/* Free move */
	reset_flag = FALSE;

	if(inven_command('e', 0, 0)) {
	    draw_cave();
	}

	break;
    case 102:
	/* f == throw */
	throw_object();

	break;
#if 0
    case 104:
	/* h == moria help */
	moria_help("");
	draw_cave();

	/* Free move */
	reset_flag = TRUE;

	break;
#endif
    case 105:
	/* i == inventory */

	/* Free move */
	reset_flag = TRUE;

	if(inven_command('i', 0, 0)) {
	    draw_cave();
	}

	break;
    case 106:
	/* j == jam */
	jamdoor();

	break;
    case 108:
	/* l == look */
	look();

	/* Free move */
	reset_flag = TRUE;

	break;
    case 109:
	/* m == magick */
	cast();

	break;
    case 111:
	/* o == open */
	openobject();

	break;
    case 112:
	/* p == pray */
	pray();

	break;
    case 113:
	/* q == quaff */
	quaff();

	break;
    case 114:
	/* r == read */
	read_scroll();

	break;
    case 115:
	/* s == search */
	if(py.flags.blind > 0) {
	    msg_print("You are incapable of searching while blind.");
	}
	else {
	    search(char_row, char_col, py.misc.srh);
	}

	break;
    case 116:
	/* t = unwear  */
	reset_flag = TRUE;

	if(inven_command('t', 0, 0)) {
	    draw_cave();
	}

	break;
    case 117:
	/* u == use staff */
	use();

	break;
    case 118:
	/* v == version */
	game_version();
	reset_flag = TRUE;

	break;
    case 119:
	/* w == wear */
	reset_flag = TRUE;

	if(inven_command('w', 0, 0)) {
	    draw_cave();
	}

	break;
    case 120:
	/* x == exchange */
	reset_flag = TRUE;

	if(inven_command('x', 0, 0)) {
	    draw_cave();
	}

	break;
    default:
	if(wizard1) {
	    /* Wizard commands are free moves */
	    reset_flag = TRUE;

	    switch(*com_val) {
	    case 1:
		/* ^A == cure all */
		remove_curse();
		cure_blindness();
		cure_confusion();
		cure_poison();
		remove_fear();

		s_ptr = &py.stats;
		s_ptr->cstr = s_ptr->str;
		s_ptr->cint = s_ptr->intel;
		s_ptr->cwis = s_ptr->wis;
		s_ptr->cdex = s_ptr->dex;
		s_ptr->ccon = s_ptr->con;
		s_ptr->cchr = s_ptr->chr;
		f_ptr = &py.flags;

		if(f_ptr->slow > 1) {
		    f_ptr->slow = 1;
		}

		if(f_ptr->image > 1) {
		    f_ptr->image = 1;
		}

		/* put_qio(); */

		/* Adjust misc stats */
		py_bonuses(blank_treasure, 0);

		break;
	    case 2:
		/* ^B == objects */
		print_objects();

		break;
	    case 4:
		/* ^D == up/down */
		prt("Go to which level (0-1200)? ", 0, 0);
		i = -1;

		if(get_string(tmp_str, 0, 30, 10)) {
		    sscanf(tmp_str, "%d", &i);
		}

		if(i > -1) {
		    dun_level = i;

		    if(dun_level > 1200) {
			dun_level = 1200;
		    }

		    moria_flag = TRUE;
		}
		else {
		    erase_line(MSG_LINE, 0);
		}
	    case 8:
		/* ^H == wizard help */
		original_wizard_help();

		break;
	    case 9:
		/* ^I == identify */
		ident_spell();

		break;
	    case 12:
		/* ^L == wizard light */
		wizard_light();

		break;
	    case 14:
		/* ^N == monster map */
		print_monsters();

		break;
	    case 20:
		/* ^T == teleport */
		teleport(100);

		break;
	    case 22:
		/* ^V == restore */
		restore_char();
		prt_stat_block();
		moria_flag = TRUE;

		break;
	    default:
		if(wizard2) {
		    switch(*com_val) {
		    case 5:
			/* ^E == wizard character */
			char_character();

			break;
		    case 6:
			/* ^F == genocide */
			mass_genocide();
			/* put_qio(); */

			break;
		    case 7:
			/* ^G == treasure */
			alloc_object(set_floor, 5, 10);
			/* put_qio(); */

			break;
		    case 10:
			/* ^J == gain experience */
			if(py.misc.exp == 0) {
			    py.misc.exp = 1;
			}
			else {
			    py.misc.exp = py.misc.exp * 2;
			}

			prt_experience();

			break;
		    case 21:
			/* ^U == summon */
			y = char_row;
			x = char_col;
			summon_monster(&y, &x, TRUE);
			creatures(FALSE);

			break;
		    case '@':
			/* ^Q == create */
			wizard_create();

			break;
		    default:
			prt("Typer '?' or '^H' for help...", 0, 0);

			break;
		    }
		}
		else {
		    prt("Type '?' or '^H' for help...", 0, 0);
		}
	    }
	}
	else {
	    prt("Type '?' for help...", 0, 0);
	    reset_flag = TRUE;
	}
    }
}

void rogue_like_commands(int *com_val)
{
    int y;
    int x;
    char command;
    int i;
    vtype out_val;
    vtype tmp_str;
    struct stats *s_ptr;
    struct flags *f_ptr;
    int set_floor();

    switch(*com_val) {
    case 0:
    case 'Q':
	/* Q == exit */
	flush();

	if(get_com("Do you really want to quit?", &command)) {
	    switch(command) {
	    case 'y':
	    case 'Y':
		moria_flag = TRUE;
		death = TRUE;

		/* Need dot on the end to be consistent with creatures.c */
		strcpy(died_from, "Quitting.");

		break;
	    default:

		break;
	    }
	}

	reset_flag = TRUE;

	break;
    case 16:
	/* ^P == repeat */
	repeat_msg();
	reset_flag = TRUE;

	break;
    case 23:
	/* ^W == password */
	if(wizard1) {
	    wizard1 = FALSE;
	    wizard2 = FALSE;
	    msg_print("Wizard mode off.");
	}
	else {
	    if(check_pswd()) {
		msg_print("Wizard mode on.");
	    }
	}

	reset_flag = TRUE;

	break;
    case 18:
	/* ^R == redraw */
	really_clear_screen();
	draw_cave();
	reset_flag = TRUE;

	break;
    case 24:
	/* ^X == save */
	if(total_winner) {
	    msg_print("You are a Total Winner, your character must be retired...");
	    msg_print("Use 'Q' to when you are ready to quit.");
	}
	else {
	    if(search_flag) {
		search_off();
	    }

	    save_char(TRUE, FALSE);
	}

	reset_flag = TRUE;

	break;
    case ' ':
	/* SPACE do nothing */
	reset_flag = TRUE;

	break;
    case '!':
	/* ! = shell */
	shell_out();
	reset_flag = TRUE;

	break;
    case 'b':
	move_char(1);

	break;
    case 'j':
	move_char(2);

	break;
    case 'n':
	move_char(3);

	break;
    case 'h':
	move_char(4);

	break;
    case 'l':
	move_char(6);

	break;
    case 'y':
	move_char(7);

	break;
    case 'k':
	move_char(8);

	break;
    case 'u':
	move_char(9);

	break;
    case 'B':
	/* . == find */
	find_flag = TRUE;
	move_char(1);

	break;
    case 'J':
	/* . == find */
	find_flag = TRUE;
	move_char(2);

	break;
    case 'N':
	/* . == find */
	find_flag = TRUE;
	move_char(3);

	break;
    case 'H':
	/* . == find */
	find_flag = TRUE;
	move_char(4);

	break;
    case 'L':
	/* . == find */
	find_flag = TRUE;
	move_char(6);

	break;
    case 'Y':
	/* . == find */
	find_flag = TRUE;
	move_char(7);

	break;
    case 'K':
	/* . == find */
	find_flag = TRUE;
	move_char(8);

	break;
    case 'U':
	/* . == find */
	find_flag = TRUE;
	move_char(9);

	break;
    case '/':
	/* / == identify */
	ident_char();
	reset_flag = TRUE;

	break;
    case '1':
	/* Move dir 1 */
	move_char(1);

	break;
    case '2':
	/* Move dir 2 */
	move_char(2);

	break;
    case '3':
	/* Move dir 3 */
	move_char(3);

	break;
    case '4':
	/* Move dir 4 */
	move_char(4);

	break;
    case '5':
    case '.':
	/* Rest one turn */
	move_char(5);

#ifdef SLOW
	/* Sleep 1/10th of a second */
	sleep(0);
#endif

	flush();

	break;
    case '6':
	/* Move dir 6 */
	move_char(6);

	break;
    case '7':
	/* Move dir 7 */
	move_char(7);

	break;
    case '8':
	/* Move dir 8 */
	move_char(8);

	break;
    case '9':
	/* Move dir 9 */
	move_char(9);

	break;
    case '<':
	/* < == go up */
	go_up();

	break;
    case '>':
	/* > == go down */
	go_down();

	break;
    case '?':
	rogue_like_help();

	/* Free move */
	reset_flag = TRUE;

	break;
    case 'f':
	/* f == bash */
	bash();

	break;
    case 'C':
	/* C == character */
	if(get_com("Print to file? (Y/N)", &command)) {
	    switch(command) {
	    case 'y':
	    case 'Y':
		file_character();
		
		break;
	    case 'n':
	    case 'N':
		change_name();
		draw_cave();

		break;
	    default:

		break;
	    }
	}

	/* Free move */
	reset_flag = TRUE;

	break;
    case 'D':
	/* D = disarm */
	disarm_trap();

	break;
    case 'E':
	/* E == eat */
	eat();

	break;
    case 'F':
	/* F == refill */
	refill_lamp();

	break;
    case 'W':
	/* W == location */
	if((py.flags.blind > 0) || no_light()) {
	    msg_print("You can't see your map.");
	}
	else {
	    sprintf(out_val,
		    "Section [%d,%d]; Location = [%d,%d]",
		    ((char_row - 1) / OUTPAGE_HEIGHT) + 1,
		    ((char_col - 1) / OUTPAGE_WIDTH) + 1,
		    char_row,
		    char_col);

	    msg_print(out_val);
	}

	/* Free move */
	reset_flag = TRUE;

	break;
    case 'M':
	/* M == print map */
	if((py.flags.blind > 0) || no_light()) {
	    msg_print("You can't see to draw a map.");
	}
	else {
	    print_map();
	}

	/* Free move */
	reset_flag = TRUE;

	break;
    case 'R':
	/* R == rest */
	rest();

	break;
    case '#':
	/* ^S == search mode */
	if(search_flag) {
	    search_off();

	    /* Free move */
	    reset_flag = TRUE;
	}
	else if(py.flags.blind > 0) {
	    msg_print("You are incapable of searching while blind.");
	}
	else {
	    search_on();

	    /* Free move */
	    reset_flag = TRUE;
	}

	break;
    case 25:
	/* ^Y */
	x = char_row;
	y = char_col;
	move(7, &x, &y);
	tunnel(x, y);

	break;
    case 11:
	/* ^K */
	x = char_row;
	y = char_col;
	move(8, &x, &y);
	tunnel(x, y);

	break;
    case 21:
	/* ^U */
	x = char_row;
	y = char_col;
	move(9, &x, &y);
	tunnel(x, y);

	break;
    case 12:
	/* ^L */
	x = char_row;
	y = char_col;
	move(6, &x, &y);
	tunnel(x, y);

	break;
    case 14:
	/* ^N */
	x = char_row;
	y = char_col;
	move(3, &x, &y);
	tunnel(x, y);

	break;
    case 10:
	/* ^J */
	x = char_row;
	y = char_col;
	move(2, &x, &y);
	tunnel(x, y);

	break;
    case 2:
	/* ^B */
	x = char_row;
	y = char_col;
	move(1, &x, &y);
	tunnel(x, y);

	break;
    case 8:
	/* ^H */
	x = char_row;
	y = char_col;
	move(4, &x, &y);
	tunnel(x, y);

	break;
    case 'z':
	/* z == aim */
	aim();

	break;
    case 'P':
	/* P == browse */
	examine_book();
	reset_flag = TRUE;

	break;
    case 'c':
	/* c == close */
	closeobject();

	break;
    case 'd':
	/* d == drop */
	drop();

	break;
    case 'e':
	/* e == equipment */
	/* Free move */
	reset_flag = TRUE;

	if(inven_command('e', 0, 0)) {
	    draw_cave();
	}

	break;
    case 't':
	/* t == throw */
	throw_object();

	break;
#if 0
    case 104:
	/* h == moria help */
	moria_help("");
	draw_cave();

	/* Free move */
	reset_flag = TRUE;

	break;
#endif
    case 'i':
	/* i == inventory */
	/* Free move */
	reset_flag = TRUE;

	if(inven_command('i', 0, 0)) {
	    draw_cave();
	}

	break;
    case 'S':
	/* S == jam */
	jamdoor();

	break;
    case 'x':
	/* x == look */
	look();

	/* Free move */
	reset_flag = TRUE;
	
	break;
    case 'm':
	/* m = magick */
	cast();

	break;
    case 'o':
	/* o == open */
	openobject();

	break;
    case 'p':
	/* p == pray */
	pray();

	break;
    case 'q':
	/* q == quaff */
	quaff();

	break;
    case 'r':
	/* r == read */
	read_scroll();

	break;
    case 's':
	/* s == search */
	if(py.flags.blind > 0) {
	    msg_print("You are incapable of searching while blind.");
	}
	else {
	    search(char_row, char_col, py.misc.srh);
	}

	break;
    case 'T':
	/* T == unwear */
	reset_flag = TRUE;

	if(inven_command('t', 0, 0)) {
	    draw_cave();
	}

	break;
    case 'Z':
	/* Z == use staff */
	use();

	break;
    case 'v':
	/* v == version */
	game_version();
	reset_flag = TRUE;

	break;
    case 'w':
	/* w == wear */
	reset_flag = TRUE;

	if(inven_command('w', 0, 0)) {
	    draw_cave();
	}

	break;
    case 'X':
	/* X == exchange */
	reset_flag = TRUE;

	if(inven_command('x', 0, 0)) {
	    draw_cave();
	}

	break;
    default:
	if(wizard1) {
	    /* Wizard commands are free moves */
	    reset_flag = TRUE;

	    switch(*com_val) {
	    case 1:
		/* ^A == cure all */
		remove_curse();
		cure_blindness();
		cure_confusion();
		cure_poison();
		remove_fear();
		s_ptr = &py.stats;
		s_ptr->cstr = s_ptr->str;
		s_ptr->cint = s_ptr->intel;
		s_ptr->cwis = s_ptr->wis;
		s_ptr->cdex = s_ptr->dex;
		s_ptr->ccon = s_ptr->con;
		s_ptr->cchr = s_ptr->chr;
		f_ptr = &py.flags;

		if(f_ptr->slow > 1) {
		    f_ptr->slow = 1;
		}

		if(f_ptr->image > 1) {
		    f_ptr->image = 1;
		}

		/* put_qio(); */

		/* Adjust misc stats */
		py_bonuses(blank_treasure, 0);

		break;
	    case 15:
		/* ^O == objects */
		print_objects();

		break;
	    case 4:
		/* ^D == up/down */
		prt("Go to which level (0-1200)? ", 0, 0);
		i = -1;

		if(get_string(tmp_str, 0, 30, 10)) {
		    sscanf(tmp_str, "%d", &i);
		}

		if(i > -1) {
		    dun_level = i;

		    if(dun_level > 1200) {
			dun_level = 1200;
		    }

		    moria_flag = TRUE;
		}
		else {
		    erase_line(MSG_LINE, 0);
		}

		break;
	    case 127: /* ^? DEL */
		/* DEL == wizard help */
		rogue_like_wizard_help();

		break;
	    case 9:
		/* ^I == identify */
		ident_spell();

		break;
	    case 13:
		/* ^M == monster map */
		print_monsters();

		break;
	    case '*':
		/* * == wizard light */
		wizard_light();

		break;
	    case 20:
		/* ^T == teleport */
		teleport(100);

		break;
	    case 22:
		/* ^V == restore */
		restore_char();
		prt_stat_block();
		moria_flag = TRUE;

		break;
	    default:
		if(wizard2) {
		    switch(*com_val) {
		    case 5:
			/* ^E == wizard character */
			change_character();

			break;
		    case 6:
			/* ^F == genocide */
			mass_genocide();
			/* put_qio(); */

			break;
		    case 7:
			/* ^G == treasure */
			alloc_object(set_floor, 5, 10);
			/* put_qio(); */

			break;
		    case '+':
			/* + == gain experience */
			if(py.misc.exp == 0) {
			    py.misc.exp == 1;
			}
			else {
			    py.misc.exp = py.misc.exp * 2;
			}

			prt_experience();

			break;
		    case 19:
			/* ^S == summon */
			y = char_row;
			x = char_col;
			summon_monster(&y, &x, TRUE);
			creatures(FALSE);

			break;
		    case '@':
			/* ^Q == create */
			wizard_create();

			break;
		    default:
			prt("Type '?' or DELETE for help...", 0, 0);

			break;
		    }
		}
		else {
		    prt("Type '?' or DELETE for help...", 0, 0);
		}
	    }
	}
	else {
	    prt("Type '?' for help...", 0, 0);
	    reset_flag = TRUE;
	}
    }
}
