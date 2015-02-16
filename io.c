// Various input/output functions
//
// @(#)io.c 3.10 (Berkeley) 6/15/81
//
// Revision History
// ================
// 23 Dec 81  DPK  Added the Howard Walter Memorial Kludge
//                 for Brain Damaged Terminals. This kludge
//                 prints a squashed status line on screens
//                 where the line length is < 80.

#include "io.h"

#include "rogue.h"

#include <ctype.h>
#include <stdio.h>

/* TC_DEBUG: Start */
#include "debug.h"
#include <stdio.h>
/* TC_DEBUG: Finish */

static char msgbuf[BUFSIZ];
static int newpos = 0;

// msg:
//     Display a message at the top of the screen.
int msg(char *fmt, ...)
{
    // If the string is "", just clear the line
    if(*fmt == '\0') {
	wmove(cw, 0, 0);
	wclrtoeol(cw);
	mpos = 0;
	return 0;
    }

    // Otherwise add to the message and flush it out
    va_list args;
    va_start(args, fmt);
    doadd(fmt, args);
    va_end(args);
    
    endmsg();

    return 0;
}

// addmsg:
//     Add things to the current message
int addmsg(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    doadd(fmt, args);
    va_end(args);

    return 0;
}

// endmsg:
//     Display a new msg (giving him a chance to see the previous one
//     if it is up there with the --More--)
int endmsg()
{
    strcpy(huh, msgbuf);
    if(mpos) {
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

// doadd:
//     Something...
int doadd(char *fmt, va_list args)
{
    int num_written = vsprintf(msgbuf + newpos, fmt, args);
    msgbuf[newpos + num_written] = '\0';
    newpos += num_written;
    
    return 0;
}

// step_ok:
//     Returns true if it is ok to step on ch
int step_ok(char ch)
{
    switch(ch) {
	case ' ':
	case '|':
	case '-':
	case SECRETDOOR:
	    return FALSE;
	default:
	    return (!isalpha(ch));
    }
}

// readchar:
//     Flushes stdout so that screen is up to date and then
//     returns getchar
char readchar()
{
    char c;

    fflush(stdout);
    while(read(0, &c, 1) < 0) {
	continue;
    }
    
    return c;
}

// status:
//     Display the important stats line. Keep the cursor where it was
int status()
{
    int ox;
    int temp;
    int oy;
    char *pb;
    static char buf[80];
    static int hpwidth = 0;
    static int s_hungry = -1;
    static int s_lvl = -1;
    static int s_pur;
    static int s_hp = -1;
    static int s_str;
    static int s_add;
    static int s_ac = 0;
    static long s_exp = 0;

    // If nothing has changed since the last status, don't bother
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
    if(s_hp != max_hp) {
        s_hp = max_hp;
        temp = max_hp;

	for(hpwidth = 0; temp; ++hpwidth) {
	    temp /= 10;
        }
    }

    int maxx = getmaxx(stdscr);
    
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

    maxx = getmaxx(stdscr);
    
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

    // Save old status
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

// wait_for:
//     Sit around until the guy types the right key
int wait_for(char ch)
{
    char c;

    if(ch == '\n') {
        c = readchar();
        while((c != '\n') && (c != '\r')) {
            c = readchar();
        }
    }
    else {
        while(readchar() != ch) {
	    continue;
        }
    }

    return 0;
}

// show_win:
//     Function used to display a window and wait before returning
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
