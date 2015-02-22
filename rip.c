// File for fun ends, death or a total win
//
// @(#)rip.c 3.13 (Berkeley) 6/16/81
//
// Revision History
// ================
// 28 Dec 81  DPK  Added code to use RAND locking open on scorefile

#include "rip.h"

#include "io.h"
#include "machdep.h"
#include "main.h"
#include "misc.h"
#include "rogue.h"
#include "save.h"
#include "things.h"

#include <ctype.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

static char *rip[] = {
    "                       __________                    ",
    "                      /          \\                  ",
    "                     /    REST    \\                 ",
    "                    /      IN      \\                ",
    "                   /     PEACE      \\               ",
    "                  /                  \\              ",
    "                  |                  |               ",
    "                  |                  |               ",
    "                  |   killed by a    |               ",
    "                  |                  |               ",
    "                  |       1981       |               ",
    "                 *|     *  *  *      | *             ",
    "         ________)/\\\\_//(\\/(/\\)/\\//\\/|_)_______",
    0
};

// death:
//     Do something really fun when he dies
int death(char monst)
{
    char **dp = rip;
    char *killer;
    struct tm *lt;
    time_t date;
    char buf[80];

    time(&date);
    lt = localtime(&date);
    clear();
    move(8, 0);
    while(*dp) {
    	printw("%s\n", *dp++);
    }
    mvaddstr(14, 28 - ((strlen(whoami) + 1) / 2), whoami);
    purse -= (purse / 10);
    sprintf(buf, "%d Au", purse);
    mvaddstr(15, 28 - ((strlen(buf) + 1) / 2), buf);
    killer = killname(monst);
    mvaddstr(17, 28 - ((strlen(killer) + 1) / 2), killer);
    mvaddstr(16, 33, vowelstr(killer));
    sprintf(prbuf, "%4d", lt->tm_year + 1900);
    mvaddstr(18, 26, prbuf);
    mvaddstr(LINES - 1, 0, "--Press space to continue--");
    wrefresh(stdscr);
    wait_for(' ');
    score(purse, 0, &monst);
    endwin();
    exit(0);
}

// score:
//     Figure out score and post it
int score(int amount, int flags, char *monst)
{
    static struct sc_ent {
	int sc_score;
	char sc_name[80];
	int sc_flags;
	int sc_level;
	int sc_uid;
	char sc_monster;
    } top_ten[10];
    struct sc_ent *scp;
    int i;
    struct sc_ent *sc2;
    FILE *outf;
    char *killer;
    int prflags = 0;
    int fd;
    static char *reason[] = {
	"killed",
	"quit",
	"A total winner",
    };

    if(flags != -1) {
	endwin();
    }

    for(scp = top_ten; scp < &top_ten[10]; ++scp) {
	scp->sc_score = 0;
	for(i = 0; i < 80; ++i) {
	    scp->sc_name[i] = rnd(255);
        }
	scp->sc_flags = rand();
	scp->sc_level = rand();
	scp->sc_monster = rand();
	scp->sc_uid = rand();
    }

    signal(SIGINT, SIG_DFL);
    char *input_line = NULL;
    if(flags != -1) {
	printf("[Press return to continue]\n");
	fflush(stdout);

	size_t read_length = 0;
	ssize_t line_length = getline(&input_line, &read_length, stdin);

	while(line_length == -1) {
	    line_length = getline(&input_line, &read_length, stdin);
	}
	
	input_line[line_length - 1] = '\0';
    }

    // Open file and read list
#ifndef BRL
    fd = open(SCOREFILE, O_RDWR);
    if(fd < 0) {
	return 0;
    }
#else
    // Use the RAND cooperative locking open to prevent plastering
    // the scorefile. (BRL 6.144)
    fd = open(SCOREFILE, O_RDWR);
    while(fd < 0) {
	if(errno != ETXTBSY) {
	    return 0;
        }
        
	sleep (1);

        fd = open(SCOREFILE, 6);
    }
#endif
    outf = fdopen(fd, "w");

    if(wizard) {
	if(strcmp(input_line, "names") == 0) {
	    prflags = 1;
        }
	else if(strcmp(input_line, "edit") == 0) {
	    prflags = 2;
        }
    }

    free(input_line);

    encread((char *)top_ten, sizeof(top_ten), fd);

    // Insert him in the list if need be
    if(!waswizard) {
	for(scp = top_ten; scp < &top_ten[10]; ++scp) {
	    if(amount > scp->sc_score) {
		break;
            }
        }
	if(scp < &top_ten[10]) {
	    for(sc2 = &top_ten[9]; sc2 > scp; --sc2) {
		*sc2 = *(sc2 - 1);
            }
	    scp->sc_score = amount;
	    strcpy(scp->sc_name, whoami);
	    scp->sc_flags = flags;
	    if(flags == 2) {
		scp->sc_level = max_level;
            }
	    else {
		scp->sc_level = level;
            }
	    scp->sc_monster = *monst;
	    scp->sc_uid = getuid();
	}
    }

    // Print the list
    printf("\nTop Ten Adventurers:\nRank\tScore\tName\n");
    for(scp = top_ten; scp < &top_ten[10]; ++scp) {
	if(scp->sc_score) {
	    printf("%ld\t%d\t%s: %s on level %d",
                   scp - top_ten + 1,
                   scp->sc_score,
                   scp->sc_name,
                   reason[scp->sc_flags],
                   scp->sc_level);
	    if(scp->sc_flags == 0) {
		printf(" by a");
		killer = killname(scp->sc_monster);
		if((*killer == 'a')
                   || (*killer == 'e')
                   || (*killer == 'i')
                   || (*killer == 'o')
                   || (*killer == 'u')) {
                    putchar('n');
                }
		printf(" %s", killer);
	    }
	    if(prflags == 1) {
		struct passwd *pp;

                pp = getpwuid(scp->sc_uid);
		if(pp == NULL){
		    printf(" (%d)", scp->sc_uid);
                }
		else {
		    printf(" (%s)", pp->pw_name);
                }
		putchar('\n');
	    }
	    else if(prflags == 2) {
		fflush(stdout);

		char *more_input = NULL;
		size_t read_length = 0;
		ssize_t line_length = getline(&more_input, &read_length, stdin);

		while(line_length == -1) {
		    line_length = getline(&more_input, &read_length, stdin);
		}

		more_input[line_length - 1] = '\0';

		if(more_input[0] == 'd') {
		    for(sc2 = scp; sc2 < &top_ten[9]; ++sc2) {
			*sc2 = *(sc2 + 1);
                    }
		    top_ten[9].sc_score = 0;
		    for(i = 0; i < 80; ++i) {
			top_ten[9].sc_name[i] = rnd(255);
                    }
		    top_ten[9].sc_flags = rand();
		    top_ten[9].sc_level = rand();
		    top_ten[9].sc_monster = rand();
		    --scp;
		}

		free(more_input);
	    }
	    else {
		printf(".\n");
            }
	}
    }
    fseek(outf, 0L, 0);

    // Update the list file
    encwrite((char *) top_ten, sizeof top_ten, outf);

    fclose(outf);

    return 0;
}

// total_winner:
//     Something...
int total_winner()
{
    struct linked_list *item;
    struct object *obj;
    int worth;
    char c;
    int oldpurse;

    clear();
    standout();
    addstr("                                                               \n");
    addstr("  @   @               @   @           @          @@@  @     @  \n");
    addstr("  @   @               @@ @@           @           @   @     @  \n");
    addstr("  @   @  @@@  @   @   @ @ @  @@@   @@@@  @@@      @  @@@    @  \n");
    addstr("   @@@@ @   @ @   @   @   @     @ @   @ @   @     @   @     @  \n");
    addstr("      @ @   @ @   @   @   @  @@@@ @   @ @@@@@     @   @     @  \n");
    addstr("  @   @ @   @ @  @@   @   @ @   @ @   @ @         @   @  @     \n");
    addstr("   @@@   @@@   @@ @   @   @  @@@@  @@@@  @@@     @@@   @@   @  \n");
    addstr("                                                               \n");
    addstr("     Congratulations, you have made it to the light of day!    \n");
    standend();
    addstr("\nYou have joined the elite ranks of those who have escaped the\n");
    addstr("Dungeons of Doom alive.  You journey home and sell all your loot at\n");
    addstr("a great profit and are admitted to the fighters guild.\n");
    mvaddstr(LINES - 1, 0, "--Press space to continue--");
    refresh();
    wait_for(' ');
    clear();
    mvaddstr(0, 0, "   Worth  Item");
    oldpurse = purse;
    for(c = 'a', item = player.t_pack; item != NULL; ++c, item = item->l_next) {
	obj = (struct object *)item->l_data;
        
	switch(obj->o_type) {
        case FOOD:
            worth = 2 * obj->o_count;
            break;
        case WEAPON:
            switch(obj->o_which) {
            case MACE:
                worth = 8;
                break;
            case SWORD:
                worth = 15;
                break;
            case BOW:
                worth = 75;
                break;
            case ARROW:
                worth = 1;
                break;
            case DAGGER:
                worth = 2;
                break;
            case ROCK:
                worth = 1;
                break;
            case TWOSWORD:
                worth = 30;
                break;
            case SLING:
                worth = 1;
                break;
            case DART:
                worth = 1;
                break;
            case CROSSBOW:
                worth = 15;
                break;
            case BOLT:
                worth = 1;
                break;
            case SPEAR:
                worth = 2;
                break;
            default:
                worth = 0;
            }
            
            worth *= (1 + (10 * obj->o_hplus + 10 * obj->o_dplus));
            worth *= obj->o_count;
            obj->o_flags |= ISKNOW;
            break;
        case ARMOR:
            switch(obj->o_which) {
            case LEATHER:
                worth = 5;
                break;
            case RING_MAIL:
                worth = 30;
                break;
            case STUDDED_LEATHER:
                worth = 15;
                break;
            case SCALE_MAIL:
                worth = 3;
                break;
            case CHAIN_MAIL:
                worth = 75;
                break;
            case SPLINT_MAIL:
                worth = 80;
                break;
            case BANDED_MAIL:
                worth = 90;
                break;
            case PLATE_MAIL:
                worth = 400;
                break;
            default:
                worth = 0;
            }
            
            worth *= (1 + (10 * (a_class[obj->o_which] - obj->o_ac)));
            obj->o_flags |= ISKNOW;
            break;
        case SCROLL:
            s_know[obj->o_which] = TRUE;
            worth = s_magic[obj->o_which].mi_worth;
            worth *= obj->o_count;
            break;
        case POTION:
            p_know[obj->o_which] = TRUE;
            worth = p_magic[obj->o_which].mi_worth;
            worth *= obj->o_count;
            break;
        case RING:
            obj->o_flags |= ISKNOW;
            r_know[obj->o_which] = TRUE;
            worth = r_magic[obj->o_which].mi_worth;
            if((obj->o_which == R_ADDSTR)
               || (obj->o_which == R_ADDDAM)
               || (obj->o_which == R_PROTECT)
               || (obj->o_which == R_ADDHIT)) {
                if(obj->o_ac > 0) {
                    worth += (obj->o_ac * 20);
                }
                else {
                    worth = 50;
                }
            }
            break;
        case STICK:
            obj->o_flags |= ISKNOW;
            ws_know[obj->o_which] = TRUE;
            worth = ws_magic[obj->o_which].mi_worth;
            worth += 20 * obj->o_ac;
            break;
	default:
            worth = 1000;
	}
        
	mvprintw(c - 'a' + 1, 0, "%c) %5d  %s", c, worth, inv_name(obj, FALSE));
	purse += worth;
    }
    
    mvprintw(c - 'a' + 1, 0,"   %5d  Gold Peices          ", oldpurse);
    refresh();
    score(purse, 2, NULL);
    exit(0);
}

// killname:
//     Something...
char *killname(char monst)
{
    if(isupper(monst)) {
	return monsters[monst - 'A'].m_name;
    }
    else {
	switch(monst) {
        case 'a':
            return "arrow";
        case 'd':
            return "dart";
        case 'b':
            return "bolt";
	}
    }
    
    return "";
}
