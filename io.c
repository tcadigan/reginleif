/*
 * Various input/output functions
 *
 * @(#)io.c	3.10 (Berkeley) 6/15/81
 *
 *		Revision History
 *		================
 *	23 Dec 81	DPK	Added the Howard Walter Memorial Kludge
 *				for Braindamaged Terminals.  This kludge
 *				prints a squashed status line on screens
 *				where the line length is < 80.
 */

#include "io.h"

#include "rogue.h"

#include <ctype.h>
#include <ncurses.h>
#include <stdio.h>

/*
 * msg:
 *	Display a message at the top of the screen.
 */

static char msgbuf[BUFSIZ];
static int newpos = 0;

/*VARARGS1*/
int msg(char *fmt, void *args)
{
    /*
     * if the string is "", just clear the line
     */
    if (*fmt == '\0')
    {
	wmove(cw, 0, 0);
	wclrtoeol(cw);
	mpos = 0;
	return 0;
    }
    /*
     * otherwise add to the message and flush it out
     */
    doadd(fmt, args);
    endmsg();

    return 0;
}

/*
 * add things to the current message
 */
int addmsg(char *fmt, void *args)
{
    doadd(fmt, args);

    return 0;
}

/*
 * Display a new msg (giving him a chance to see the previous one if it
 * is up there with the --More--)
 */
int endmsg()
{
    strcpy(huh, msgbuf);
    if (mpos)
    {
	wmove(cw, 0, mpos);
	waddstr(cw, "--More--");
        wrefresh(cw);
	wait_for(' ');
    }
    mvwaddstr(cw, 0, 0, msgbuf);
    wclrtoeol(cw);
    mpos = newpos;
    newpos = 0;
    wrefresh(cw);

    return 0;
}

int doadd(char *fmt, void *args)
{
    FILE *junk = tmpfile();
    fprintf(junk, fmt, args);
    putc('\0', junk);
    
    rewind(junk);

    char str[BUFSIZ];

    fgets(str, sizeof(str), junk);

    strcat(msgbuf, str);
    newpos = strlen(msgbuf);

    fclose(junk);
    
    return 0;
}

/*
 * step_ok:
 *	returns true if it is ok to step on ch
 */
int step_ok(char ch)
{
    switch (ch)
    {
	case ' ':
	case '|':
	case '-':
	case SECRETDOOR:
	    return FALSE;
	default:
	    return (!isalpha(ch));
    }
}

/*
 * readchar:
 *	flushes stdout so that screen is up to date and then returns
 *	getchar.
 */
char readchar()
{
    char c;

    fflush(stdout);
    while (read(0, &c, 1) < 0)
	continue;
    return c;
}

/*
 * status:
 *	Display the important stats line.  Keep the cursor where it was.
 */

int status()
{
    register int oy, ox, temp;
    register char *pb;
    static char buf[80];
    static int hpwidth = 0, s_hungry = -1;
    static int s_lvl = -1, s_pur, s_hp = -1, s_str, s_add, s_ac = 0;
    static long s_exp = 0;

    /*
     * If nothing has changed since the last status, don't
     * bother.
     */
    if(cur_armor != NULL) {
        if ((s_hp == player.t_stats.s_hpt)
            && (s_exp == player.t_stats.s_exp)
            && (s_pur == purse)
            && (s_ac == cur_armor->o_ac)
            && (s_str == player.t_stats.s_str.st_str)
            && (s_add == player.t_stats.s_str.st_add)
            && (s_lvl == level)
            && (s_hungry == hungry_state)) {
	    return 0;
        }
    }
    else {
        if ((s_hp == player.t_stats.s_hpt)
            && (s_exp == player.t_stats.s_exp)
            && (s_pur == purse)
            && (s_ac == player.t_stats.s_arm)
            && (s_str == player.t_stats.s_str.st_str)
            && (s_add == player.t_stats.s_str.st_add)
            && (s_lvl == level)
            && (s_hungry == hungry_state)) {
	    return 0;
        }
    }
	
    getyx(cw, oy, ox);
    if (s_hp != max_hp)
    {
	temp = s_hp = max_hp;
	for (hpwidth = 0; temp; hpwidth++)
	    temp /= 10;
    }

    int maxx;
    int maxy;
    getmaxyx(stdscr, maxy, maxx);
    
    if(maxx > 79) {
        sprintf(buf,
                "Level: %d  Gold: %-5d  Hp: %*d(%*d)  Str: %-2d",
                level,
                purse,
                hpwidth,
                player.t_stats.s_hpt,
                hpwidth,
                max_hp,
                player.t_stats.s_str.st_str);
    }
    else {
        sprintf(buf,
		"L:%d G:%-5d H:%*d(%*d) S:%-2d",
                level,
                purse,
                hpwidth,
                player.t_stats.s_hpt,
                hpwidth,
                max_hp,
                player.t_stats.s_str.st_str);
    }
    
    if(player.t_stats.s_str.st_add != 0) {
	pb = &buf[strlen(buf)];
	sprintf(pb, "/%d", player.t_stats.s_str.st_add);
    }

    pb = &buf[strlen(buf)];

    getmaxyx(stdscr, maxy, maxx);
    
    if(maxx > 79) {
        if(cur_armor != NULL) {
            sprintf(pb,
                    "  Ac: %-2d  Exp: %d/%ld",
                    cur_armor->o_ac,
                    player.t_stats.s_lvl,
                    player.t_stats.s_exp);
        }
        else {
            sprintf(pb,
                    "  Ac: %-2d  Exp: %d/%ld",
                    player.t_stats.s_arm,
                    player.t_stats.s_lvl,
                    player.t_stats.s_exp);
        }
    }
    else {
        if(cur_armor != NULL) {
            sprintf(pb,
                    " A:%-2d E:%d/%ld",
                    cur_armor->o_ac,
                    player.t_stats.s_lvl,
                    player.t_stats.s_exp);
        }
        else {
            sprintf(pb,
                    " A:%-2d E:%d/%ld",
                    player.t_stats.s_arm,
                    player.t_stats.s_lvl,
                    player.t_stats.s_exp);
        }
    }
    
    /*
     * Save old status
     */
    s_lvl = level;
    s_pur = purse;
    s_hp = player.t_stats.s_hpt;
    s_str = player.t_stats.s_str.st_str;
    s_add = player.t_stats.s_str.st_add;
    s_exp = player.t_stats.s_exp;

    if(cur_armor != NULL) {
        s_ac = cur_armor->o_ac;
    }
    else {
        s_ac = player.t_stats.s_arm;
    }

    mvwaddstr(cw, LINES - 1, 0, buf);
    
    switch(hungry_state) {
    case 0:
        break;
    case 1:
        waddstr(cw, "  Hungry");
        break;
    case 2:
        waddstr(cw, "  Weak");
        break;
    case 3:
        waddstr(cw, "  Fainting");
        break;
    }
    wclrtoeol(cw);
    s_hungry = hungry_state;
    wmove(cw, oy, ox);

    return 0;
}

/*
 * wait_for
 *	Sit around until the guy types the right key
 */

int wait_for(char ch)
{
    register char c;

    if (ch == '\n')
        while ((c = readchar()) != '\n' && c != '\r')
	    continue;
    else
        while (readchar() != ch)
	    continue;

    return 0;
}

/*
 * show_win:
 *	function used to display a window and wait before returning
 */

int show_win(WINDOW *scr, char *message)
{
    mvwaddstr(scr, 0, 0, message);
    touchwin(scr);
    wmove(scr, player.t_pos.y, player.t_pos.x);
    wrefresh(scr);
    wait_for(' ');
    clearok(cw, TRUE);
    touchwin(cw);

    return 0;
}

/*
 * extra functions by DM
 */
/* int noecho() */
/* { */
/* 	struct sgttyb data; */

/* 	gtty(_tty_ch, &data); */
/* 	data.sg_flags &= ~ECHO; */
/* 	stty(_tty_ch, &data); */

/*         return 0; */
/* } */

/* int noraw() */
/* { */
/* 	struct sgttyb data; */

/* 	gtty(_tty_ch, &data); */
/* 	data.sg_flags &= ~RAW; */
/* 	stty(_tty_ch, &data); */

/*         return 0; */
/* } */

/* int crmode() */
/* { */
/* 	struct sgttyb data; */

/* 	gtty(_tty_ch, &data); */
/* 	data.sg_flags |= CBREAK; */
/* 	stty(_tty_ch, &data); */

/*         return 0; */
/* } */

/* int raw() */
/* { */
/* 	struct sgttyb data; */

/* 	gtty(_tty_ch, &data); */
/* 	data.sg_flags |= RAW; */
/* 	stty(_tty_ch, &data); */

/*         return 0; */
/* } */
