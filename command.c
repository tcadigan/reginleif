// Read and execute the user commands
//
// @(#)command.c 3.45 (Berkeley) 6/15/81

#include "command.h"

#include "armor.h"
#include "daemon.h"
#include "fight.h"
#include "io.h"
#include "list.h"
#include "main.h"
#include "misc.h"
#include "move.h"
#include "newlevel.h"
#include "options.h"
#include "pack.h"
#include "passages.h"
#include "potions.h"
#include "rings.h"
#include "rogue.h"
#include "save.h"
#include "scrolls.h"
#include "sticks.h"
#include "things.h"
#include "rip.h"
#include "weapons.h"
#include "wizard.h"

#include <ctype.h>
#include <sgtty.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

struct h_list helpstr[] = {
    {'?',	"	prints help"},
    {'/',	"	identify object"},
    {'h',	"	left"},
    {'j',	"	down"},
    {'k',	"	up"},
    {'l',	"	right"},
    {'y',	"	up & left"},
    {'u',	"	up & right"},
    {'b',	"	down & left"},
    {'n',	"	down & right"},
    {'H',	"	run left"},
    {'J',	"	run down"},
    {'K',	"	run up"},
    {'L',	"	run right"},
    {'Y',	"	run up & left"},
    {'U',	"	run up & right"},
    {'B',	"	run down & left"},
    {'N',	"	run down & right"},
    {'t',	"<dir>	throw something"},
    {'f',	"<dir>	forward until find something"},
    {'p',	"<dir>	zap a wand in a direction"},
    {'z',	"	zap a wand or staff"},
    {'>',	"	go down a staircase"},
    {'s',	"	search for trap/secret door"},
    {'{',	"	(space) rest for a while"},
    {'i',	"	inventory"},
    {'I',	"	inventory single item"},
    {'q',	"	quaff potion"},
    {'r',	"	read paper"},
    {'e',	"	eat food"},
    {'w',	"	wield a weapon"},
    {'W',	"	wear armor"},
    {'T',	"	take armor off"},
    {'P',	"	put on ring"},
    {'R',	"	remove ring"},
    {'d',	"	drop object"},
    {'c',	"	call object"},
    {'o',	"	examine/set options"},
    {CTRL('L'),	"	redraw screen"},
    {CTRL('R'),	"	repeat last message"},
    {ESCAPE,	"	cancel command"},
    {'v',	"	print program version number"},
    {'!',	"	shell escape"},
    {'S',	"	save game"},
    {'Q',	"	quit"},
    {0, 0}
};

// command:
//     Process the user commands
int command()
{
    char ch;
    // Number of player moves
    int ntimes = 1;
    static char countch;
    static char direction;
    static char newcount = FALSE;

    if((player.t_flags & ISHASTE) != 0) {
        ++ntimes;
    }

    // Let the daemons start up
    do_daemons(BEFORE);
    do_fuses(BEFORE);
    
    while(ntimes--) {
	look(TRUE);
        
	if(!running) {
	    door_stop = FALSE;
        }
        
	status();
	lastscore = purse;
	wmove(cw, player.t_pos.y, player.t_pos.x);
        
	if(!((running || count) && jump)) {
            // Draw the screen
            wrefresh(cw);
        }

	take = 0;
	after = TRUE;

        // Read command or continue run
	if(wizard) {
	    waswizard = TRUE;
        }
        
	if(!no_command) {
	    if(running) {
                ch = runch;
            }
	    else if(count) {
                ch = countch;
            }
	    else {
		ch = readchar();
                // Erase message if its there
		if((mpos != 0) && !running) {
		    msg("");
                }
	    }
	}
	else {
            ch = ' ';
        }
        
	if(no_command) {
            --no_command;
            
	    if(no_command == 0) {
		msg("You can move again.");
            }
	}
	else {
            // Check for prefixes
	    if(isdigit(ch)) {
		count = 0;
		newcount = TRUE;

		while(isdigit(ch)) {
		    count = count * 10 + (ch - '0');
		    ch = readchar();
		}
                
		countch = ch;

                // Turn off count for command which don't make sense
                // to repeat
		switch (ch) {
                case 'h':
                case 'j':
                case 'k':
                case 'l':
                case 'y':
                case 'u':
                case 'b':
                case 'n':
                case 'H':
                case 'J':
                case 'K':
                case 'L':
                case 'Y':
                case 'U':
                case 'B':
                case 'N':
                case 'q':
                case 'r':
                case 's':
                case 'f':
                case 't':
                case 'C':
                case 'I':
                case ' ':
                case 'z':
                case 'p':
                    break;
                default:
                    count = 0;
		}
	    }
            
	    switch(ch) {
            case 'f':
                if((player.t_flags & ISBLIND) == 0) {
                    door_stop = TRUE;
                    firstmove = TRUE;
                }
                
                if(count && !newcount) {
                    ch = direction;
                }
                else {
                    ch = readchar();
                }
                
                switch(ch) {
                case 'h':
                case 'j':
                case 'k':
                case 'l':
                case 'y':
                case 'u':
                case 'b':
                case 'n':
                    ch = toupper(ch);
                }
                
                direction = ch;
	    }
            
	    newcount = FALSE;

            // Execute a command
	    if(count && !running) {
		--count;
            }
            
	    switch(ch) {
            case '!':
                shell();
                
                break;
            case 'h':
                do_move(0, -1);
                
                break;
            case 'j':
                do_move(1, 0);
                
                break;
            case 'k':
                do_move(-1, 0);
                
                break;
            case 'l':
                do_move(0, 1);
                
                break;
            case 'y':
                do_move(-1, -1);
                
                break;
            case 'u':
                do_move(-1, 1);
                
                break;
            case 'b':
                do_move(1, -1);
                
                break;
            case 'n':
                do_move(1, 1);
                
                break;
            case 'H':
                do_run('h');
                
                break;
            case 'J':
                do_run('j');
                
                break;
            case 'K':
                do_run('k');
                
                break;
            case 'L':
                do_run('l');
                
                break;
            case 'Y':
                do_run('y');
                
                break;
            case 'U':
                do_run('u');
                
                break;
            case 'B':
                do_run('b');
                
                break;
            case 'N':
                do_run('n');
                
                break;
            case 't':
                if(!get_dir()) {
                    after = FALSE;
                }
                else {
                    missile(delta.y, delta.x);
                }
                
                break;
            case 'Q':
                after = FALSE;
                
                quit(0);
            case 'i':
                after = FALSE;
                inventory(player.t_pack, 0);
                
                break;
            case 'I':
                after = FALSE;
                picky_inven();
                
                break;
            case 'd':
                drop();
                
                break;
            case 'q':
                quaff();
                
                break;
            case 'r':
                read_scroll();
                
                break;
            case 'e':
                eat();
                
                break;
            case 'w':
                wield();
                
                break;
            case 'W':
                wear();
                
                break;
            case 'T':
                take_off();
                
                break;
            case 'P':
                ring_on();
                
                break;
            case 'R':
                ring_off();
                
                break;
            case 'o':
                option();
                
                break;
            case 'c':
                call();
                
                break;
            case '>':
                after = FALSE;
                d_level();
                
                break;
            case '<' :
                after = FALSE;
                u_level();
                
                break;
            case '?':
                after = FALSE;
                help();
                
                break;
            case '/':
                after = FALSE;
                identify();
                
                break;
            case 's':
                search();
                
                break;
            case 'z':
                do_zap(FALSE);
                
                break;
            case 'p':
                if(get_dir()) {
                    do_zap(TRUE);
                }
                else {
                    after = FALSE;
                }
                
                break;
            case 'v':
                msg("Rogue version %s. (mctesq was here)", release);
                
                break;
            case CTRL('L'):
                after = FALSE;
                clearok(curscr,TRUE);
                wrefresh(curscr);
                
                break;
            case CTRL('R'):
                after = FALSE;
                msg(huh);
                
                break;
            case 'S': 
                after = FALSE;
                
                if(save_game()) {
                    wmove(cw, LINES-1, 0); 
                    wclrtoeol(cw);
                    wrefresh(cw);
                    endwin();
                    exit(0);
                }
                
                break;
            case ' ':
                
                // Rest command
                break;
            case CTRL('P'):
                after = FALSE;
                
                if(wizard) {
                    wizard = FALSE;
                    msg("Not wizard any more");
                }
                else {
                    if(wizard == passwd()) {
                        msg("You are suddenly as smart as Ken Arnold in dungeon #%d", dnum);
                        waswizard = TRUE;
                    }
                    else {
                        msg("Sorry");
		    }
                }
                
                break;
            case ESCAPE:
                door_stop = FALSE;
                count = 0;
                after = FALSE;
                
                break;
            default:
                after = FALSE;
                
                if(wizard) {
                    switch(ch) {
                    case '@':
                        msg("@ %d,%d", player.t_pos.y, player.t_pos.x );
                        
                        break;
                    case 'C':
                        create_obj();
                        
                        break;
                    case CTRL('I'):
                        inventory(lvl_obj, 0);
                        
                        break;
                    case CTRL('W'):
                        whatis();
                        
                        break;
                    case CTRL('D'):
                        ++level;
                        new_level();
                        
                        break;
                    case CTRL('U'):
                        --level;
                        new_level();
                        
                        break;
                    case CTRL('F'):
                        show_win(stdscr, "--More (level map)--");
                        
                        break;
                    case CTRL('X'):
                        show_win(mw, "--More (monsters)--");

                        break;
                    case CTRL('T'):
                        teleport();
                        
                        break;
                    case CTRL('E'):
                        msg("food left: %d", food_left);
                        
                        break;
                    case CTRL('A'):
                        msg("%d things in your pack", inpack);
                        
                        break;
                    case CTRL('C'):
                        add_pass();
                        
                        break;
                    case CTRL('N'):
			{
                            struct linked_list *item;
                            
                            item = get_item("charge", STICK);
			    if(item != NULL) {
				((struct object *)item->l_data)->o_ac = 10000;
                            }
			}
                        
                        break;
                    case CTRL('H'):
			{
			    int i;
			    struct linked_list *item;
			    struct object *obj;

			    for(i = 0; i < 9; ++i) {
				raise_level();
                            }

                            // Give the rogue a sword (+1, +1)
			    item = new_item(sizeof *obj);
			    obj = (struct object *)item->l_data;
			    obj->o_type = WEAPON;
			    obj->o_which = TWOSWORD;
			    init_weapon(obj, SWORD);
			    obj->o_hplus = 1;
			    obj->o_dplus = 1;
			    add_pack(item, TRUE);
			    cur_weapon = obj;

                            // And his suite of armor
			    item = new_item(sizeof *obj);
			    obj = (struct object *)item->l_data;
			    obj->o_type = ARMOR;
			    obj->o_which = PLATE_MAIL;
			    obj->o_ac = -5;
			    obj->o_flags |= ISKNOW;
			    cur_armor = obj;
			    add_pack(item, TRUE);
			}
                        
                        break;
                    default:
			    msg("Illegal command '%s'.", unctrl(ch));
			    count = 0;
		    }
                }
                else {
                    msg("Illegal command '%s'.", unctrl(ch));
                    count = 0;
                }
	    }
            
            // Turn of flags if no longer needed
	    if(!running) {
		door_stop = FALSE;
            }
	}

        // If he ran into something to take, let him pick it up
	if(take != 0) {
	    pick_up(take);
        }
        
	if(!running) {
	    door_stop = FALSE;
        }
    }

    // Kick off the rest of the daemons and fuses
    if(after) {
	look(FALSE);

	do_daemons(AFTER);
	do_fuses(AFTER);

        if((cur_ring[LEFT] != NULL) && (cur_ring[LEFT]->o_which == R_SEARCH)) {
	    search();
        }
        else if(((cur_ring[LEFT] != NULL) && (cur_ring[LEFT]->o_which == R_TELEPORT))
                && (rnd(100) < 2)) {
	    teleport();
        }

        if((cur_ring[RIGHT] != NULL) && (cur_ring[RIGHT]->o_which == R_SEARCH)) {
	    search();
        }
        else if(((cur_ring[RIGHT] != NULL) && (cur_ring[RIGHT]->o_which == R_TELEPORT))
                && (rnd(100) < 2)) {
	    teleport();
        }
    }

    return 0;
}

// quit:
//     Have player make certain, then exit
void quit(int parameter)
{
    // Reset the signal in case we got here via an interrupt
    if(signal(SIGINT, quit) != quit) {
	mpos = 0;
    }
    
    msg("Really quit?");
    wrefresh(cw);
    
    if(readchar() == 'y') {
	clear();
	move(LINES - 1, 0);
        wrefresh(stdscr);
	score(purse, 1, NULL);
	exit(0);
    }
    else {
	signal(SIGINT, quit);
	wmove(cw, 0, 0);
	wclrtoeol(cw);
	status();
        wrefresh(cw);
	mpos = 0;
	count = 0;
    }
}

// Search:
//     Player gropes about him to find hidden things.
int search()
{
    int x;
    int y;
    char ch;

    // Look all around the hero, if there is something hidden there,
    // give him a change to find it. If it's found, display it.
    if((player.t_flags & ISBLIND) != 0) {
	return 0;
    }
    
    for(x = player.t_pos.x - 1; x <= player.t_pos.x + 1; ++x) {
	for(y = player.t_pos.y - 1; y <= player.t_pos.y + 1; ++y) {
            if(mvwinch(mw, y, x) == ' ') {
                ch = mvwinch(stdscr, y, x);
            }
            else {
                ch = winch(mw);
            }
            
	    switch(ch) {
            case SECRETDOOR:
                if(rnd(100) < 20) {
                    mvaddch(y, x, DOOR);
                    count = 0;
                }
                
                break;
            case TRAP:
                {
                    struct trap *tp;
                    
                    if(mvwinch(cw, y, x) == TRAP) {
                        break;
                    }
                    
                    if(rnd(100) > 50) {
                        break;
                    }
                    
                    tp = trap_at(y, x);
                    tp->tr_flags |= ISFOUND;
                    mvwaddch(cw, y, x, TRAP);
                    count = 0;
                    running = FALSE;
                    msg("%s", tr_name(tp->tr_type));
                }
                    
                break;
	    }
	}
    }

    return 0;
}

// help:
//     Give single character help, or the whole mess if he wants it
int help()
{
    struct h_list *strp = helpstr;
    char helpch;
    int cnt;

    msg("Character you want help for (* for all): ");
    helpch = readchar();
    mpos = 0;

    // If it's not a '*', print the right help string
    // or an error if he typed a funny character.
    if(helpch != '*') {
	wmove(cw, 0, 0);
        
	while(strp->h_ch) {
	    if(strp->h_ch == helpch) {
		msg("%s%s", unctrl(strp->h_ch), strp->h_desc);
		break;
	    }
            
	    strp++;
	}
        
	if(strp->h_ch != helpch) {
	    msg("Unknown character '%s'", unctrl(helpch));
        }
        
	return 0;
    }

    // Here we print help for everything.
    // Then wait before we return to command mode
    wclear(hw);
    cnt = 0;

    while(strp->h_ch) {
        if(cnt > 22) {
            mvwaddstr(hw, cnt % 23, 40, unctrl(strp->h_ch));
        }
        else {
            mvwaddstr(hw, cnt %23, 0, unctrl(strp->h_ch));
        }

	waddstr(hw, strp->h_desc);
	++cnt;
	++strp;
    }
    
    wmove(hw, LINES - 1, 0);
    wprintw(hw, "--Press space to continue--");
    wrefresh(hw);
    wait_for(' ');
    wclear(hw);
    wrefresh(hw);
    wmove(cw, 0, 0);
    wclrtoeol(cw);
    status();
    touchwin(cw);

    return 0;
}

// identify:
//     Tell the player what a certain thing is
int identify()
{
    char ch;
    char *str;

    msg("What do you want identified? ");
    ch = readchar();
    mpos = 0;
    
    if(ch == ESCAPE) {
	msg("");
	return 0;
    }
    
    if(isalpha(ch) && isupper(ch)) {
	str = monsters[ch-'A'].m_name;
    }
    else {
        switch(ch) {
	case '|':
	case '-':
	    str = "wall of a room";
            
            break;
	case GOLD:
            str = "gold";
            
            break;
        case STAIRS:
            str = "passage leading down";
            
            break;
        case DOOR:
            str = "door";
            
            break;
        case FLOOR:
            str = "room floor";
            
            break;
        case PLAYER:
            str = "you";
            
            break;
        case PASSAGE:
            str = "passage";
            
            break;
        case TRAP:
            str = "trap";
            
            break;
	case POTION:
            str = "potion";
            
            break;
        case SCROLL:
            str = "scroll";
            
            break;
        case FOOD:
            str = "food";
            
            break;
        case WEAPON:
            str = "weapon";
            
            break;
        case ' ':
            str = "solid rock";
            
            break;
	case ARMOR:
            str = "armor";
            
            break;
	case AMULET:
            str = "The Amulet of Yendor";
            
            break;
        case RING:
            str = "ring";
            
            break;
	case STICK:
            str = "wand or staff";
            
            break;
        default:
            str = "unknown character";
        }
    }
    
    msg("'%s' : %s", unctrl(ch), str);

    return 0;
}

// d_level:
//     He wants to go down a level
int d_level()
{
    if(mvwinch(mw, player.t_pos.y, player.t_pos.x ) == ' ') {
        if(mvwinch(stdscr, player.t_pos.y, player.t_pos.x) != STAIRS) {
            msg("I see no way down.");
        }
        else {
            ++level;
            new_level();
        }
    }
    else {
        if(winch(mw) != STAIRS) {
            msg("I see no way down.");
        }
        else {
            ++level;
            new_level();
        }
    }
    
    return 0;
}

// u_level:
//     He wants to go up a level
int u_level()
{
    if(mvwinch(mw, player.t_pos.y, player.t_pos.x) == ' ') {
        if(mvwinch(stdscr, player.t_pos.y, player.t_pos.x) == STAIRS) {
            if(amulet) {
                --level;
                
                if(level == 0) {
                    total_winner();
                }
                
                new_level();
                msg("You feel a wrenching sensation in your gut.");
            }
        }
    }
    else {
        if(winch(mw) == STAIRS) {
            if(amulet) {
                --level;
                
                if(level == 0) {
                    total_winner();
                }
                
                new_level();
                msg("You feel a wrenching sensation in your gut.");
            }
        }
    }

    msg("I see no way up.");
    
    return 0;
}

// Let him escape for a while
int shell()
{
    int pid;
    char *sh;
    int ret_status;

    // Set the terminal back to original mode
    sh = getenv("SHELL");
    wclear(hw);
    wmove(hw, LINES - 1, 0);
    wrefresh(hw);
    endwin();
    in_shell = TRUE;
    fflush(stdout);

    // Fork and do a shell
    while((pid = fork()) < 0) {
	sleep(1);
    }
    
    if(pid == 0) {
        // Set back to original user, just in case
	setuid(getuid());
	setgid(getgid());

        if(sh == NULL) {
            execl("/bin/sh", "shell", "-i", NULL);
        }
        else {
            execl(sh, "shell", "-i", NULL);
        }

	perror("No shelly");
	exit(-1);
    }
    else {
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	while(wait(&ret_status) != pid) {
	    continue;
        }
        
	signal(SIGINT, endit);
	signal(SIGQUIT, endit);
	printf("\n[Press return to continue]");
	noecho();
	crmode();
	in_shell = FALSE;
	wait_for('\n');
	clearok(cw, TRUE);
	touchwin(cw);
    }

    return 0;
}

// call:
//     Allow a user to call a potion, scroll, or ring something
int call()
{
    struct object *obj;
    struct linked_list *item;
    char **guess;
    char *elsewise;
    bool *know;

    item = get_item("call", CALLABLE);

    // Make certain that it is something that we want to wear
    if(item == NULL) {
	return 0;
    }
    
    obj = (struct object *)item->l_data;
    
    switch(obj->o_type) {
    case RING:
        guess = r_guess;
        know = r_know;

        if(r_guess[obj->o_which] != NULL) {
            elsewise = r_guess[obj->o_which];
        }
        else {
            elsewise = r_stones[obj->o_which];
        }

        break;
    case POTION:
        guess = p_guess;
        know = p_know;

        if(p_guess[obj->o_which] != NULL) {
            elsewise = p_guess[obj->o_which];
        }
        else {
            elsewise = p_colors[obj->o_which];
        }

        break;
    case SCROLL:
        guess = s_guess;
        know = s_know;

        if(s_guess[obj->o_which] != NULL) {
            elsewise = s_guess[obj->o_which];
        }
        else {
            elsewise = s_names[obj->o_which];
        }

        break;
    case STICK:
        guess = ws_guess;
        know = ws_know;

        if(ws_guess[obj->o_which] != NULL) {
            elsewise = ws_guess[obj->o_which];
        }
        else {
            elsewise = ws_made[obj->o_which];
        }

        break;
    default:
        msg("You can't call that anything");
        return 0;
    }
    
    if(know[obj->o_which]) {
	msg("That has already been identified");
	return 0;
    }
    
    if(terse) {
	addmsg("C");
    }
    else {
	addmsg("Was c");
    }
    
    msg("alled \"%s\"", elsewise);

    if(terse) {
	msg("Call it: ");
    }
    else {
	msg("What do you want to call it? ");
    }
    
    if(guess[obj->o_which] != NULL) {
	free(guess[obj->o_which]);
    }
    
    strcpy(prbuf, elsewise);

    if(get_str(prbuf, cw) == NORM) {
	guess[obj->o_which] = malloc((unsigned int) strlen(prbuf) + 1);
	strcpy(guess[obj->o_which], prbuf);
    }

    return 0;
}
