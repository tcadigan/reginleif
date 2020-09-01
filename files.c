#include "files.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "config.h"
#include "constants.h"
#include "desc.h"
#include "externs.h"
#include "io.h"
#include "misc1.h"
#include "misc2.h"
#include "types.h"

/* Correct SUN stupidity in the stdio.h file */
#ifdef sun
char *sprintf();
#endif

#if defined(ultrix) || defined(USG)
void exit();
#endif

/*
 * init_scorefile 
 * Open the score file while we still have the setuid privileges. Later when 
 * the score is being written out, you must be sure to flock the file so we 
 * don't have multiple people trying to write to it at the same time.
 * Craig Norborg (doc)    Mon Aug 10 16:41:59 EST 1987
 */
void init_scorefile()
{
    highscore_fd = open(MORIA_TOP, O_RDWR | O_CREAT, 0644);

    if(highscore_fd < 1) {
	fputs("Can't open score file!\n", stderr);

	exit(1);
    }
}

/*
 * Attempt to open the intro file    -RAK-
 * This routine also checks the hours file vs. what time it is    -Doc
 */
void intro(char *finam)
{
    int xpos;
    int i;
    vtype in_line;
    FILE *file1;
    char *string;

    /*
     * Attempt to read hours.dat. If it does not exist, inform user so he can
     * tell the wizard about it
     */
    file1 = fopen(MORIA_HOU, "r");

    if(file1 != NULL) {
	while(fgets(in_line, 80, file1) != NULL) {
	    if(strlen(in_line) > 3) {
		if(!strncpy(in_line, "SUN:", 4)) {
		    strcpy(days[0], in_line);
		}
		else if(!strncmp(in_line, "MON:", 4)) {
		    strcpy(days[1], in_line);
		}
		else if(!strncmp(in_line, "TUE:", 4)) {
		    strcpy(days[2], in_line);
		}
		else if(!strncmp(in_line, "WED:", 4)) {
		    strcpy(days[3], in_line);
		}
		else if(!strncmp(in_line, "THU:", 4)) {
		    strcpy(days[4], in_line);
		}
		else if(!strncmp(in_line, "FRI:", 4)) {
		    strcpy(days[5], in_line);
		}
		else if(!strncmp(in_line, "SAT:", 4)) {
		    strcpy(days[6], in_line);
		}
	    }
	}

	fclose(file1);
    }
    else {
	fprintf(stderr, "There is no hours file.\nPlease inform the wizard, %s, so he can correct this!\n", WIZARD);
	exit_game();
    }

    /* Check the hours, if closed require password */
    string = index(finam, '^');

    if(string) {
	xpos = strlen(finam) - strlen(string);
    }
    else {
	xpos = -1;
    }

    if(xpos >= 0) {
	if(check_paswd()) {
	    insert_str(finam, "^", "");
	}
    }

    if(!check_time()) {
	if(!wizard1) {
	    file1 = fopen(MORIA_HOU, "r");

	    if(file1 != NULL) {
		clear_screen(0, 0);

		for(i = 0; fgets(in_line, 80, file1) != NULL; ++i) {
		    prt(in_line, i, 0);
		}

		fclose(file1);
	    }

	    exit_game();
	}
    }

    /* Print the introduction message, news, etc... */
    file1 = fopen(MORIA_MOR, "r");

    if(file1 != NULL) {
	clear_screen(0, 0);

	for(i = 0; fgets(in_line, 80, file1) != NULL; ++i) {
	    prt(in_line, i, 0);
	}

	pause_line(23);
	fclose(file1);
    }
}

/* Prints dungeon map to external file    -RAK- */
void print_map()
{
    int i;
    int j;
    int m;
    int n;
    int k;
    int l;
    int i7;
    int i8;
    char dun_line[MAX_WIDTH];
    char *dun_ptr;
    vtype filename1;
    char tmp_str[80];
    FILE *file1;
    int page_width = OUTPAGE_WIDTH;
    int page_height = OUTPAGE_HEIGHT;

    /*
     * This allows us to strcat each character in the inner loop, instead of
     * using the expensive sprintf
     */
    prt("File name: ", 0, 0);

    if(get_string(filename1, 0, 11, 64)) {
	if(strlen(filename1) == 0) {
	    strcpy(filename1, "MORIAMAP.DAT");
	}

	file1 = fopen(filename1, "w");

	if(file1 == NULL) {
	    sprintf(dun_line, "Cannot open file %s", filename1);
	    prt(dun_line, 0, 0);
	    put_qio();

	    return;
	}

	sprintf(tmp_str, "section width (default = %d char):", page_width);
	prt(tmp_str, 0, 0);
	get_string(tmp_str, 0, strlen(tmp_str), 10);
	sscanf(tmp_str, "%d", &page_width);

	if(page_width < 10) {
	    page_width = 10;
	}

	sprintf(tmp_str, "Section height (default = %d lines):", page_height);
	prt(tmp_str, 0, 0);
	get_string(tmp_str, 0, strlen(tmp_str), 10);
	sscanf(tmp_str, "%d", &page_height);

	if(page_height < 10) {
	    page_height = 10;
	}

	prt("Writing Moria Dungon Map...", 0, 0);
	put_qio();

	i = 0;
	i7 = 0;
	j = 0;
	k = i + page_height - 1;

	if(k >= cur_height) {
	    k = cur_height - 1;
	}

	++i7;
	i8 = 0;
	l = j + page_width - 1;

	if(l >= cur_width) {
	    l = cur_width - 1;
	}

	++i8;
	fprintf(file1, "%c\n", 12);
	fprintf(file1, "Section[%d,%d];     ", i7, i8);
	fprintf(file1, "Depth : %d (feet)\n\n   ", dun_level * 50);

	for(m = j; m <= l; ++m) {
	    n = m / 100;
	    fprintf(file1, "%d", n);
	}

	fputs("\n   ", file1);
	
	for(m = j; m <= l; ++m) {
	    n = (m / 10) - ((m / 100) * 10);
	    fprintf(file1, "%d", n);
	}

	fputs("\n   ", file1);

	for(m = j; m <= l; ++m) {
	    n = m - ((m / 10) * 10);
	    fprintf(file1, "%d", n);
	}

	fprintf(file1, "\n");

	for(m = i; m <= k; ++m) {
	    sprintf(dun_line, "%2d ", m);
	    dun_ptr = &dun_line[3];

	    for(n = j; n <= l; ++n) {
		if(test_light(m, n)) {
		    loc_symbol(m, n, dun_ptr);
		    ++dun_ptr;
		}
		else {
		    *dun_ptr = ' ';
		    ++dun_ptr;
		}
	    }

	    *dun_ptr = '\n';
	    ++dun_ptr;
	    fputs(dun_line, file1);
	}

	j += page_width;

	while(j < cur_width) {
	    l = j + page_width - 1;

	    if(l >= cur_width) {
		l = cur_width - 1;
	    }

	    ++i8;
	    fprintf(file1, "%c\n", 12);
	    fprintf(file1, "Section[%d,%d];     ", i7, i8);
	    fprintf(file1, "Depth : %d (feet)\n\n   ", dun_level * 50);

	    for(m = j; m <= l; ++m) {
		n = m / 100;
		fprintf(file1, "%d", n);
	    }

	    fputs("\n   ", file1);

	    for(m = j; m <= l; ++m) {
		n = (m / 10) - ((m / 100) * 10);
		fprintf(file1, "%d", n);
	    }

	    fputs("\n   ", file1);

	    for(m = j; m <= l; ++m) {
		n = m - ((m / 10) * 10);
		fprintf(file1, "%d", n);
	    }

	    fprintf(file1, "\n");

	    for(m = i; m <= k; ++m) {
		sprintf(dun_line, "%2d ", m);
		dun_ptr = &dun_line[3];

		for(n = j; n <= l; ++n) {
		    if(test_light(m, n)) {
			loc_symbol(m, n, dun_ptr);
			++dun_ptr;
		    }
		    else {
			*dun_ptr = ' ';
			++dun_ptr;
		    }
		}

		*dun_ptr = '\n';
		++dun_ptr;
		fputs(dun_line, file1);
	    }

	    j += page_width;
	}

	i += page_height;

	while(i < cur_height) {
	    j = 0;
	    k = i + page_height - 1;

	    if(k >= cur_height) {
		k = cur_height - 1;
	    }

	    ++i7;
	    i8 = 0;
	    l = j + page_width - 1;

	    if(l >= cur_width) {
		l = cur_width - 1;
	    }

	    ++i8;
	    fprintf(file1, "%c\n", 12);
	    fprintf(file1, "Section[%d,%d];     ", i7, i8);
	    fprintf(file1, "Depth : %d (feet)\n\n   ", dun_level * 50);

	    for(m = j; m <= l; ++m) {
		n = m / 100;
		fprintf(file1, "%d", n);
	    }

	    fputs("\n   ", file1);

	    for(m = j; m <= l; ++m) {
		n = (m / 10) - ((m / 100) * 10);
		fprintf(file1, "%d", n);
	    }

	    fputs("\n   ", file1);

	    for(m = j; m <= l; ++m) {
		n = m - ((m / 10) * 10);
		fprintf(file1, "%d", n);
	    }

	    fprintf(file1, "\n");

	    for(m = i; m <= k; ++m) {
		sprintf(dun_line, "%2d ", m);
		dun_ptr = &dun_line[3];

		for(n = j; n <= l; ++n) {
		    if(test_light(m, n)) {
			loc_symbol(m, n, dun_ptr);
			++dun_ptr;
		    }
		    else {
			*dun_ptr = ' ';
			++dun_ptr;
		    }
		}

		*dun_ptr = '\n';
		++dun_ptr;
		fputs(dun_line, file1);
	    }

	    j += page_width;

	    while(j < cur_width) {
		l = j + page_width - 1;

		if(l >= cur_width) {
		    l = cur_width - 1;
		}

		++i8;
		fprintf(file1, "%c\n", 12);
		fprintf(file1, "Section[%d,%d];     ", i7, i8);
		fprintf(file1, "Depth : %d (feet)\n\n   ", dun_level * 50);

		for(m = j; j <= l; ++m) {
		    n = m / 100;
		    fprintf(file1, "%d", n);
		}

		fputs("\n   ", file1);

		for(m = j; m <= l; ++m) {
		    n = (m / 10) - ((m / 100) * 10);
		    fprintf(file1, "%d", n);
		}

		fputs("\n   ", file1);

		for(m = j; m <= l; ++m) {
		    n = m - ((m / 10) * 10);
		    fprintf(file1, "%d", n);
		}

		fprintf(file1, "\n");

		for(m = i; m <= k; ++m) {
		    sprintf(dun_line, "%2d ", m);
		    dun_ptr = &dun_line[3];

		    for(n = j; n <= l; ++n) {
			if(test_light(m, n)) {
			    loc_symbol(m, n, dun_ptr);
			    ++dun_ptr;
			}
			else {
			    *dun_ptr = ' ';
			    ++dun_ptr;
			}
		    }

		    *dun_ptr = '\n';
		    ++dun_ptr;
		    fputs(dun_line, file1);
		}

		j += page_width;
	    }

	    i += page_height;
	}

	fclose(file1);
	prt("Completed.", 0, 0);
    }
}

/*
 * Prints a list of random objects to a file.    -RAK- 
 * Note that the objects produced is a sampling of objects which are expected
 * to appear on that level.
 */
void print_objects()
{
    int i;
    int nobj;
    int j;
    int level;
    vtype filename1;
    vtype tmp_str;
    FILE *file1;
    treasure_type *i_ptr;

    prt("Produce objects on what level?: ", 0, 0);
    level = 0;

    if(get_string(tmp_str, 0, 32, 10)) {
	sscanf(tmp_str, "%d", &level);
    }

    prt("Produce how many objects?: ", 0, 0);
    nobj = 0;

    if(get_string(tmp_str, 0, 26, 10)) {
	sscanf(tmp_str, "%d", &nobj);
    }

    if((nobj > 0) && (level > -1) && (level < 1201)) {
	if(nobj > 9999) {
	    nobj = 9999;
	}

	prt("File name: ", 0, 0);

	if(get_string(filename1, 0, 11, 64)) {
	    if(strlen(filename1) == 0) {
		strcpy(filename1, "MORIAOBJ.DAT");
	    }

	    file1 = fopen(filename1, "w");

	    if(file1 != NULL) {
		sprintf(tmp_str, "%d", nobj);
		prt(strcat(tmp_str, " random objects being produced..."), 0, 0);
		put_qio();
		fprintf(file1, "*** Random Objects Sampling:\n");
		fprintf(file1, "*** % d objects\n", nobj);
		fprintf(file1, "*** for Level %d\n", level);
		fprintf(file1, "\n");
		fprintf(file1, "\n");
		popt(&j);

		for(i = 0; i < nobj; ++i) {
		    t_list[j] = object_list[get_obj_num(level)];
		    magic_treasure(j, level);
		    inventory[INVEN_MAX] = t_list[j];
		    i_ptr = &inventory[INVEN_MAX];
		    unquote(i_ptr->name);
		    known1(i_ptr->name);
		    known2(i_ptr->name);
		    objdes(tmp_str, INVEN_MAX, TRUE);
		    fprintf(file1, "%s\n", tmp_str);
		}

		pusht(j);
		fclose(file1);
		prt("Completed.", 0, 0);
	    }
	    else {
		prt("File could not be opened.", 0, 0);
	    }
	}
    }
}

/* Prints a listing of monsters    -RAK- */
void print_monsters()
{
    int i;
    int j;
    int xpos;
    int attype;
    int adesc;
    FILE *file1;
    vtype out_val;
    vtype filename1;
    vtype attstr;
    vtype attx;
    dtype damstr;
    creature_type *c_ptr;
    char *string;

    prt("File name: ", 0, 0);
    if(get_string(filename1, 0, 11, 64)) {
	if(strlen(filename1) == 0) {
	    strcpy(filename1, "MORIAMON.DAT");
	}

	file1 = fopen(filename1, "w");

	if(file1 != NULL) {
	    prt("Writing Monster Dictionary...", 0, 0);
	    put_qio();

	    for(i = 0; i < MAX_CREATURES; ++i) {
		c_ptr = &c_list[i];

		/* Begin writing to file */
		fprintf(file1, "--------------------------------------------\n");
		strcpy(out_val, c_ptr->name);
		strcat(out_val, "                              ");
		fprintf(file1, "%d  %s     (%c)\n", i, out_val, c_ptr->cchar);
		fprintf(file1, "     Speed ==%d  Level     ==%d  Exp ==%d\n", c_ptr->speed, c_ptr->level, (int)c_ptr->mexp);
		fprintf(file1, "     AC    ==%d  Eye-sight ==%d  HD  ==%s\n", c_ptr->ac, c_ptr->aaf, c_ptr->hd);

		if(0x80000000 & c_ptr->cmove) {
		    fprintf(file1, "     Creature is a ***Win Creature***\n");
		}

		if(0x00080000 & c_ptr->cmove) {
		    fprintf(file1, "     Creature eats/kills other creatures.\n");
		}

		if(0x0001 & c_ptr->cdefense) {
		    fprintf(file1, "     Creature is a dragon.\n");
		}

		if(0x0002 & c_ptr->cdefense) {
		    fprintf(file1, "     Creature is a monster.\n");
		}

		if(0x0004 & c_ptr->cdefense) {
		    fprintf(file1, "     Creature is evil.\n");
		}

		if(0x0008 & c_ptr->cdefense) {
		    fprintf(file1, "     Creature is undead.\n");
		}

		if(0x0010 & c_ptr->cdefense) {
		    fprintf(file1, "     Creature is harmed by cold.\n");
		}

		if(0x0020 & c_ptr->cdefense) {
		    fprintf(file1, "     Creature is harmed by fire.\n");
		}

		if(0x0040 & c_ptr->cdefense) {
		    fprintf(file1, "     Creature is harmed by poison.\n");
		}

		if(0x0080 & c_ptr->cdefense) {
		    fprintf(file1, "     Creature is harmed by acid.\n");
		}

		if(0x0100 & c_ptr->cdefense) {
		    fprintf(file1, "     Creature is harmed by blue light.\n");
		}

		if(0x0200 & c_ptr->cdefense) {
		    fprintf(file1, "     Creature is harmed by Stone-to-Mud.\n");
		}

		if(0x1000 & c_ptr->cdefense) {
		    fprintf(file1, "     Creature cannot be charmed or slept.\n");
		}

		if(0x2000 & c_ptr->cdefense) {
		    fprintf(file1, "     Creature seen with Infra-Vision.\n");
		}

		if(0x4000 & c_ptr->cdefense) {
		    fprintf(file1, "     Creature has MAX hit points.\n");
		}

		if(0x00010000 & c_ptr->cmove) {
		    fprintf(file1, "     Creature is invisible.\n");
		}

		if(0x00100000 & c_ptr->cmove) {
		    fprintf(file1, "     Creature picks up objects.\n");
		}

		if(0x00200000 & c_ptr->cmove) {
		    fprintf(file1, "     Creature multiplies.\n");
		}

		if(0x01000000 & c_ptr->cmove) {
		    fprintf(file1, "     Carries object(s).\n");
		}

		if(0x02000000 & c_ptr->cmove) {
		    fprintf(file1, "     Carries gold, gems, etc.\n");
		}

		if(0x04000000 & c_ptr->cmove) {
		    fprintf(file1, "       Has object/gold 60%% of time.\n");
		}

		if(0x08000000 & c_ptr->cmove) {
		    fprintf(file1, "       Has object/gold 90%% of time.\n");
		}

		if(0x10000000 & c_ptr->cmove) {
		    fprintf(file1, "       Has 1d2 object(s)/gold.\n");
		}

		if(0x20000000 & c_ptr->cmove) {
		    fprintf(file1, "       Has 2d2 object(s)/gold.\n");
		}

		if(0x40000000 & c_ptr->cmove) {
		    fprintf(file1, "       Has 4d2 object(s)/gold.\n");
		}

		/* Creature casts spells/breathes dragon breath... */
		if(c_ptr->spells != 0) {
		    fprintf(file1, "   --Spells/Dragon breath ==\n");
		    fprintf(file1, "       Casts spells 1 out of %d turns.\n", (int)(0xF & c_ptr->spells));

		    if(0x00000010 & c_ptr->spells) {
			fprintf(file1, "       Can teleport short.\n");
		    }

		    if(0x00000020 & c_ptr->spells) {
			fprintf(file1, "       Can teleport long.\n");
		    }

		    if(0x00000040 & c_ptr->spells) {
			fprintf(file1, "       Teleport player to itself.\n");
		    }

		    if(0x00000080 & c_ptr->spells) {
			fprintf(file1, "       Cause light wounds.\n");
		    }

		    if(0x00000100 & c_ptr->spells) {
			fprintf(file1, "       Cause serious wounds.\n");
		    }

		    if(0x00000200 & c_ptr->spells) {
			fprintf(file1, "       Hold person.\n");
		    }

		    if(0x00000400 & c_ptr->spells) {
			fprintf(file1, "       Cause blindness.\n");
		    }

		    if(0x00000800 & c_ptr->spells) {
			fprintf(file1, "       Cause confusion.\n");
		    }

		    if(0x00001000 & c_ptr->spells) {
			fprintf(file1, "       Cause fear.\n");
		    }

		    if(0x00002000 & c_ptr->spells) {
			fprintf(file1, "       Summon a monster.\n");
		    }

		    if(0x00004000 & c_ptr->spells) {
			fprintf(file1, "       Summon an undead.\n");
		    }

		    if(0x00008000 & c_ptr->spells) {
			fprintf(file1, "       Slow person.\n");
		    }

		    if(0x00010000 & c_ptr->spells) {
			fprintf(file1, "       Drains mana for healing.\n");
		    }

		    if(0x00020000 & c_ptr->spells) {
			fprintf(file1, "       **Unknown spell value**\n");
		    }

		    if(0x00040000 & c_ptr->spells) {
			fprintf(file1, "       **Unknown spell value**\n");
		    }

		    if(0x00080000 & c_ptr->spells) {
			fprintf(file1, "       Breathes Lightning Dragon breath.\n");
		    }

		    if(0x00100000 & c_ptr->spells) {
			fprintf(file1, "       Breathes Gas Dragon breath.\n");
		    }

		    if(0x00200000 & c_ptr->spells) {
			fprintf(file1, "       Breathes Acid Dragon breath.\n");
		    }

		    if(0x00400000 & c_ptr->spells) {
			fprintf(file1, "       Breathes Frost Dragon breath.\n");
		    }

		    if(0x00800000 & c_ptr->spells) {
			fprintf(file1, "       Breathes Fire Dragon breath.\n");
		    }
		}

		/* Movement for creature */
		fprintf(file1, "   --Movement ==\n");

		if(0x00000001 & c_ptr->cmove) {
		    fprintf(file1, "       Move only to attack.\n");
		}

		if(0x00000002 & c_ptr->cmove) {
		    fprintf(file1, "       Move and attack normally.\n");
		}

		if(0x00000008 & c_ptr->cmove) {
		    fprintf(file1, "       20%% random movement.\n");
		}

		if(0x00000010 & c_ptr->cmove) {
		    fprintf(file1, "       40%% random movement.\n");
		}

		if(0x00000020 & c_ptr->cmove) {
		    fprintf(file1, "       75%% random movement.\n");
		}

		if(0x00020000 & c_ptr->cmove) {
		    fprintf(file1, "       Can open doors.\n");
		}

		if(0x00040000 & c_ptr->cmove) {
		    fprintf(file1, "       Can phase through walls.\n");
		}

		fprintf(file1, "   --Creature attacks ==\n");
		strcpy(attstr, c_ptr->damage);

		while(strlen(attstr) > 0) {
		    string = index(attstr, '|');

		    if(string) {
			xpos = strlen(attstr) - strlen(string);
		    }
		    else {
			xpos = -1;
		    }

		    if(xpos >= 0) {
			strncpy(attx, attstr, xpos);
			attx[xpos] = '\0';
			strcpy(attstr, &attstr[xpos + 1]);
		    }
		    else {
			strcpy(attx, attstr);
			attstr[0] = '\0';
		    }

		    sscanf(attx, "%d%d%s", &attype, &adesc, damstr);
		    out_val[0] = '\0';

		    switch(adesc) {
		    case 1:
			strcpy(out_val, "       Hits for ");

			break;
		    case 2:
			strcpy(out_val, "       Bites for ");

			break;
		    case 3:
			strcpy(out_val, "       Claws for ");

			break;
		    case 4:
			strcpy(out_val, "       Stings for ");

			break;
		    case 5:
			strcpy(out_val, "       Touches for ");

			break;
		    case 6:
			strcpy(out_val, "       Kicks for ");

			break;
		    case 7:
			strcpy(out_val, "       Gazes for ");

			break;
		    case 8:
			strcpy(out_val, "       Breathes for ");

			break;
		    case 9:
			strcpy(out_val, "       Spits for ");

			break;
		    case 10:
			strcpy(out_val, "       Wails for ");

			break;
		    case 11:
			strcpy(out_val, "       Embraces for ");

			break;
		    case 12:
			strcpy(out_val, "       Crawls on you for ");

			break;
		    case 13:
			strcpy(out_val, "       Shoots spores for ");

			break;
		    case 14:
			strcpy(out_val, "       Begs for money for ");

			break;
		    case 15:
			strcpy(out_val, "       Slimes you for ");

			break;
		    case 16:
			strcpy(out_val, "       Crushes you for ");

			break;
		    case 17:
			strcpy(out_val, "       Tramples you for ");

			break;
		    case 18:
			strcpy(out_val, "       Drools on you for ");

			break;
		    case 19:
			strcpy(out_val, "       Insults you for ");

			break;
		    case 99:
			strcpy(out_val, "       Is repelled...");

			break;
		    default:
			strcpy(out_val, "     **Unknown value** ");

			break;
		    }

		    switch(attype) {
		    case 1:
			strcat(out_val, "normal damage.");

			break;
		    case 2:
			strcat(out_val, "lowering strength.");

			break;
		    case 3:
			strcat(out_val, "confusion.");

			break;
		    case 4:
			strcat(out_val, "fear.");

			break;
		    case 5:
			strcat(out_val, "fire damage.");

			break;
		    case 6:
			strcat(out_val, "acid damage.");

			break;
		    case 7:
			strcat(out_val, "cold damage.");

			break;
		    case 8:
			strcat(out_val, "lightning damage.");

			break;
		    case 9:
			strcat(out_val, "corrosion damage.");

			break;
		    case 10:
			strcat(out_val, "blindness.");

			break;
		    case 11:
			strcat(out_val, "paralyzation.");

			break;
		    case 12:
			strcat(out_val, "stealing money.");

			break;
		    case 13:
			strcat(out_val, "stealing object.");

			break;
		    case 14:
			strcat(out_val, "poison damage.");

			break;
		    case 15:
			strcat(out_val, "lose dexterity.");

			break;
		    case 16:
			strcat(out_val, "lose constitution.");

			break;
		    case 17:
			strcat(out_val, "lose intelligence.");

			break;
		    case 18:
			strcat(out_val, "lose wisdom.");

			break;
		    case 19:
			strcat(out_val, "lose experience.");

			break;
		    case 20:
			strcat(out_val, "aggravates monsters.");

			break;
		    case 21:
			strcat(out_val, "disenchants objects.");

			break;
		    case 22:
			strcat(out_val, "eating food.");

			break;
		    case 23:
			strcat(out_val, "eating light source.");

			break;
		    case 24:
			strcat(out_val, "absorbing charges.");

			break;
		    case 99:
			strcat(out_val, "blank message.");

			break;
		    default:
			strcat(out_val, "**Unknown value**");

			break;
		    }

		    fprintf(file1, "%s (%s)\n", out_val, damstr);
		}

		for(j = 0; j < 2; ++j) {
		    fprintf(file1, "\n");
		}
	    }

	    /* End writing to file */
	    fclose(file1);
	    prt("Completed.", 0, 0);
	}
    }
}

/* Print the character to a file or device    -RAK- */
void file_character()
{
    int i;
    int j;
    int xbth;
    int xbthb;
    int xfos;
    int xsrh;
    int xstl;
    int xdis;
    int xsave;
    int xdev;
    vtype xinfra;
    FILE *file1;
    vtype out_val;
    vtype filename1;
    vtype prt1;
    vtype prt2;
    stat_type out_str;
    stat_type out_int;
    stat_type out_wis;
    stat_type out_dex;
    stat_type out_con;
    stat_type out_chr;
    struct misc *p_ptr;
    treasure_type *i_ptr;

    prt("File name: ", 0, 0);

    if(get_string(filename1, 0, 11, 64)) {
	if(strlen(filename1) == 0) {
	    strcpy(filename1, "MORIACHR.DAT");
	}

	file1 = fopen(filename1, "w");

	if(file1 != NULL) {
	    prt("Writing character sheet...", 0, 0);
	    put_qio();
	    fprintf(file1, "%c", 12);
	    cnv_stat(py.stats.cstr, out_str);
	    cnv_stat(py.stats.cint, out_int);
	    cnv_stat(py.stats.cwis, out_wis);
	    cnv_stat(py.stats.cdex, out_dex);
	    cnv_stat(py.stats.ccon, out_con);
	    cnv_stat(py.stats.cchr, out_chr);
	    fprintf(file1, "\n");
	    fprintf(file1, "\n");
	    fprintf(file1, "\n");
	    fprintf(file1, "  Name  :%s", pad(py.misc.name, " ", 25));
	    fprintf(file1, "  Age         :%4d", (int)py.misc.age);
	    fprintf(file1, "     Strength     :%s\n", out_str);
	    fprintf(file1, "  Race  :%s", pad(py.misc.race, " ", 25));
	    fprintf(file1, "  Height      :%4d", (int)py.misc.ht);
	    fprintf(file1, "     Intelligence :%s\n", out_int);
	    fprintf(file1, "  Sex   :%s", pad(py.misc.sex, " ", 25));
	    fprintf(file1, "  Weight      :%4d", (int)py.misc.wt);
	    fprintf(file1, "     Wisdom       :%s\n", out_wis);
	    fprintf(file1, "  Class :%s", pad(py.misc.tclass, " ", 25));
	    fprintf(file1, "  Social Class:%4d", py.misc.sc);
	    fprintf(file1, "     Dexterity    :%s\n", out_dex);
	    fprintf(file1, "  Title :%s", pad(py.misc.title, " ", 25));
	    fprintf(file1, "                   ");
	    fprintf(file1, "     Constitution :%s\n", out_con);
	    fprintf(file1, "                                  ");
	    fprintf(file1, "                   ");
	    fprintf(file1, "     Charisma     :%s\n", out_chr);
	    fprintf(file1, "\n");
	    fprintf(file1, "\n");
	    fprintf(file1, "\n");
	    fprintf(file1, "\n");
	    fprintf(file1, "  + To Hit    :%6d", py.misc.dis_th);
	    fprintf(file1, "     Level      :%6d", (int)py.misc.lev);
	    fprintf(file1, "     Max Hit Points :%6d\n", py.misc.mhp);
	    fprintf(file1, "  + To Damage :%6d", py.misc.dis_td);
	    fprintf(file1, "     Experience :%6d", py.misc.exp);
	    fprintf(file1, "     Cur Hit Points :%6d\n", (int)py.misc.chp);
	    fprintf(file1, "  + To AC     :%6d", py.misc.dis_tac);
	    fprintf(file1, "     Gold       :%6d", py.misc.au);
	    fprintf(file1, "     Max Mana       :%6d\n", py.misc.mana);
	    fprintf(file1, "    Total AC  :%6d", py.misc.dis_ac);
	    fprintf(file1, "                       ");
	    fprintf(file1, "     Cur Mana       :%6d\n", (int)py.misc.cmana);
	    fprintf(file1, "\n");
	    fprintf(file1, "\n");
	    p_ptr = &py.misc;
	    xbth = p_ptr->bth + (p_ptr->lev * BTH_LEV_ADJ) + (p_ptr->ptohit * BTH_PLUS_ADJ);
	    xbthb = p_ptr->bthb + (p_ptr->lev * BTH_LEV_ADJ) + (p_ptr->ptohit * BTH_PLUS_ADJ);

	    /* This results in a range from 0 to 29 */
	    xfos = 40 - p_ptr->fos;

	    if(xfos < 0) {
		xfos = 0;
	    }

	    xsrh = p_ptr->srh + int_adj();

	    /* This results in a range from 0 to 9 */
	    xstl = p_ptr->stl + 1;
	    xdis = p_ptr->disarm + p_ptr->lev + (2 * todis_adj()) + int_adj();
	    xsave = p_ptr->save + p_ptr->lev + wis_adj();
	    xdev = p_ptr->save + p_ptr->lev + int_adj();
	    sprintf(xinfra, "%d feet", py.flags.see_infra * 10);
	    fprintf(file1, "(Miscellaneous Abilities)\n");
	    fprintf(file1, "\n");
	    fprintf(file1, "  Fighting    : %s", pad(likert(xbth, 12), " ", 10));
	    fprintf(file1, "  Stealth     : %s", pad(likert(xstl, 1), " ", 10));
	    fprintf(file1, "  Perception  : %s\n", pad(likert(xfos, 3), " ", 10));
	    fprintf(file1, "  Throw/Bows  : %s", pad(likert(xbthb, 12), " ", 10));
	    fprintf(file1, "  Disarming   : %s", pad(likert(xdis, 8), " ", 10));
	    fprintf(file1, "  Searching   : %s\n", pad(likert(xsrh, 6), " ", 10));
	    fprintf(file1, "  Saving Throw: %s", pad(likert(xsave, 6), " ", 10));
	    fprintf(file1, "  Magic Device: %s", pad(likert(xdev, 6), " ", 10));
	    fprintf(file1, "  Infra-Vision: %s\n", pad(xinfra, " ", 10));

	    /* Write out the character's history */
	    fprintf(file1, "\n");
	    fprintf(file1, "\n");
	    fprintf(file1, "Character Background\n");

	    for(i = 0; i < 5; ++i) {
		fprintf(file1, "%s\n", pad(py.misc.history[i], " ", 71));
	    }

	    /* Write out the equipment list... */
	    j = 0;
	    fprintf(file1, "\n");
	    fprintf(file1, "\n");
	    fprintf(file1, "  [Character's Equipment List]\n");
	    fprintf(file1, "\n");

	    if(equip_ctr == 0) {
		fprintf(file1, "  Character has no equipment in use.\n");
	    }
	    else {
		for(i = 22; i < INVEN_MAX; ++i) {
		    i_ptr = &inventory[i];

		    if(i_ptr->tval != 0) {
			switch(i) {
			case 22:
			    strcpy(prt1, ") You are wielding   : ");

			    break;
			case 23:
			    strcpy(prt1, ") Worn on head       : ");

			    break;
			case 24:
			    strcpy(prt1, ") Worn around neck   : ");

			    break;
			case 25:
			    strcpy(prt1, ") Worn on body       : ");

			    break;
			case 26:
			    strcpy(prt1, ") Worn on shield arm : ");

			    break;
			case 27:
			    strcpy(prt1, ") Worn on hands      : ");

			    break;
			case 28:
			    strcpy(prt1, ") Right ring finger  : ");

			    break;
			case 29:
			    strcpy(prt1, ") Left ring finger   : ");

			    break;
			case 30:
			    strcpy(prt1, ") Worn on feet       : ");

			    break;
			case 31:
			    strcpy(prt1, ") Worn about body    : ");

			    break;
			case 32:
			    strcpy(prt1, ") Light source is    : ");

			    break;
			case 33:
			    strcpy(prt1, ") Secondary weapon   : ");

			    break;
			default:
			    strcpy(prt1, ") *Unknown value*    : ");

			    break;
			}

			objdes(prt2, i, TRUE);
			sprintf(out_val, "  %c%s%s", j + 97, prt1, prt2);
			fprintf(file1, "%s\n", out_val);
			++j;
		    }
		}
	    }

	    /* Write out the character's inventory... */
	    fprintf(file1, "%c", 12);
	    fprintf(file1, "\n");
	    fprintf(file1, "\n");
	    fprintf(file1, "\n");
	    fprintf(file1, "  [General Inventory List]\n");
	    fprintf(file1, "\n");

	    if(inven_ctr == 0) {
		fprintf(file1, "  Character has no objects in inventory.\n");
	    }
	    else {
		for(i = 0; i < inven_ctr; ++i) {
		    objdes(prt1, i, TRUE);
		    sprintf(out_val, "%c) %s", i + 97, prt1);
		    fprintf(file1, "%s\n", out_val);
		}
	    }

	    fprintf(file1, "%c", 12);
	    fclose(file1);
	    prt("Completed.", 0, 0);
	}
    }
}
		
	    
