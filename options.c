/*
 * This file has all the code for the option command.
 * I would rather this command were not necessary, but
 * it is the only way to keep the wolves off of my back.
 *
 * @(#)options.c	3.3 (Berkeley) 5/25/81
 */

#include "options.h"

#include "io.h"

#include <ctype.h>

/*
 * description of an option and what to do with it
 */
struct optstruct {
    char	*o_name;	/* option name */
    char	*o_prompt;	/* prompt for interactive entry */
    int		*o_opt;		/* pointer to thing to set */
    int		(*o_putfunc)();	/* function to print value */
    int		(*o_getfunc)();	/* function to get value interactively */
};

struct optstruct optlist[] = {
    {"terse",	 "Terse output: ",
		 (int *) &terse,	put_bool,	get_bool	},
    {"flush",	 "Flush typeahead during battle: ",
		 (int *) &fight_flush,	put_bool,	get_bool	},
    {"jump",	 "Show position only at end of run: ",
		 (int *) &jump,		put_bool,	get_bool	},
    {"step",	"Do inventories one line at a time: ",
		(int *) &slow_invent,	put_bool,	get_bool	},
    {"askme",	"Ask me about unidentified things: ",
		(int *) &askme,		put_bool,	get_bool	},
    {"name",	 "Name: ",
		 (int *) whoami,	put_str,	get_str		},
    {"fruit",	 "Fruit: ",
		 (int *) fruit,		put_str,	get_str		},
    {"file",	 "Save file: ",
		 (int *) file_name,	put_str,	get_str		}
};

/*
 * print and then set options from the terminal
 */
int option()
{
    struct optstruct *op;
    int retval;

    wclear(hw);
    touchwin(hw);
    /*
     * Display current values of options
     */
    for (op = optlist; op < &optlist[sizeof(optlist) / sizeof(struct optstruct)]; op++)
    {
	waddstr(hw, op->o_prompt);
	(*op->o_putfunc)(op->o_opt);
	waddch(hw, '\n');
    }
    /*
     * Set values
     */
    wmove(hw, 0, 0);
    for (op = optlist; op < &optlist[sizeof(optlist) / sizeof(struct optstruct)]; op++)
    {
	waddstr(hw, op->o_prompt);
	if((retval = (*op->o_getfunc)(op->o_opt, hw))) {
	    if(retval == QUIT) {
		break;
            }
	    else if(op > optlist) {	/* MINUS */
		wmove(hw, (op - optlist) - 1, 0);
		op -= 2;
	    }
	    else {	/* trying to back up beyond the top */
		putchar('\007');
		wmove(hw, 0, 0);
		op--;
	    }
        }
    }
    /*
     * Switch back to original screen
     */
    mvwaddstr(hw, LINES-1, 0, "--Press space to continue--");
    wrefresh(hw);
    wait_for(' ');
    clearok(cw, TRUE);
    touchwin(cw);
    after = FALSE;

    return 0;
}

/*
 * put out a boolean
 */
int put_bool(bool *b)
{
    if(*b) {
        waddstr(hw, "True");
    }
    else {
        waddstr(hw, "False");
    }

    return 0;
}

/*
 * put out a string
 */
int put_str(char *str)
{
    waddstr(hw, str);

    return 0;
}

/*
 * allow changing a boolean option and print it out
 */

int get_bool(bool *bp, WINDOW *win)
{
    register int oy, ox;
    register bool op_bad;

    op_bad = TRUE;
    getyx(win, oy, ox);

    if(*bp) {
        waddstr(win, "True");
    }
    else {
        waddstr(win, "False");
    }
    
    while(op_bad) {
	wmove(win, oy, ox);
        wrefresh(win);
	switch (readchar())
	{
	    case 't':
	    case 'T':
		*bp = TRUE;
		op_bad = FALSE;
		break;
	    case 'f':
	    case 'F':
		*bp = FALSE;
		op_bad = FALSE;
		break;
	    case '\n':
	    case '\r':
		op_bad = FALSE;
		break;
	    case '\033':
	    case '\007':
		return QUIT;
	    case '-':
		return MINUS;
	    default:
		mvwaddstr(win, oy, ox + 10, "(T or F)");
	}
    }
    
    wmove(win, oy, ox);

    if(*bp) {
        waddstr(win, "True");
    }
    else {
        waddstr(win, "False");
    }

    waddch(win, '\n');
    
    return NORM;
}

/*
 * set a string option
 */
int get_str(char *opt, WINDOW *win)
{
    register char *sp;
    register int c, oy, ox;
    char buf[80];

    wrefresh(win);
    getyx(win, oy, ox);
    /*
     * loop reading in the string, and put it in a temporary buffer
     */
    for (sp = buf;
         (c = readchar()) != '\n' && c != '\r' && c != '\033' && c != '\007';
         wclrtoeol(win), wrefresh(win)) {
	if (c == -1)
	    continue;
	/* else if (c == _tty.sg_erase)	/\* process erase character *\/ */
	/* { */
	/*     if (sp > buf) */
	/*     { */
	/* 	register int i; */

	/* 	sp--; */
	/* 	for (i = strlen(unctrl(*sp)); i; i--) */
	/* 	    waddch(win, '\b'); */
	/*     } */
	/*     continue; */
	/* } */
	/* else if (c == _tty.sg_kill)	/\* process kill character *\/ */
	/* { */
	/*     sp = buf; */
	/*     wmove(win, oy, ox); */
	/*     continue; */
	/* } */
	else if(sp == buf) {
	    if(c == '-') {
		break;
            }
	    else if(c == '~') {
		strcpy(buf, home);
		waddstr(win, home);
		sp += strlen(home);
		continue;
	    }
        }
	*sp++ = c;
	waddstr(win, unctrl(c));
    }
    *sp = '\0';
    if (sp > buf)	/* only change option if something has been typed */
	strucpy(opt, buf, strlen(buf));
    wmove(win, oy, ox);
    waddstr(win, opt);
    waddch(win, '\n');
    wrefresh(win);
    if (win == cw)
	mpos += sp - buf;
    if (c == '-')
	return MINUS;
    else if (c == '\033' || c == '\007')
	return QUIT;
    else
	return NORM;
}

/*
 * parse options from string, usually taken from the environment.
 * the string is a series of comma seperated values, with booleans
 * being stated as "name" (true) or "noname" (false), and strings
 * being "name=....", with the string being defined up to a comma
 * or the end of the entire option string.
 */

int parse_opts(char *str)
{
    char *sp;
    struct optstruct *op;
    int len;

    while (*str)
    {
	/*
	 * Get option name
	 */
	for (sp = str; isalpha(*sp); sp++)
	    continue;
	len = sp - str;
	/*
	 * Look it up and deal with it
	 */
	for (op = optlist; op < &optlist[sizeof(optlist) / sizeof(struct optstruct)]; op++)
	    if(strncmp(str, op->o_name, len) == 0) {
		if (op->o_putfunc == put_bool)	/* if option is a boolean */
		    *(bool *)op->o_opt = TRUE;
		else				/* string option */
		{
		    register char *start;
		    /*
		     * Skip to start of string value
		     */
		    for (str = sp + 1; *str == '='; str++)
			continue;
		    if (*str == '~')
		    {
			strcpy((char *) op->o_opt, home);
			start = (char *) op->o_opt + strlen(home);
			while (*++str == '/')
			    continue;
		    }
		    else
			start = (char *) op->o_opt;
		    /*
		     * Skip to end of string value
		     */
		    for (sp = str + 1; *sp && *sp != ','; sp++)
			continue;
		    strucpy(start, str, sp - str);
		}
		break;
	    }
	    /*
	     * check for "noname" for booleans
	     */
	    else if((op->o_putfunc == put_bool)
                     && (strncmp(str, "no", 2) == 0)
                     && (strncmp(str + 2, op->o_name, len - 2) == 0)) {
		*(bool *)op->o_opt = FALSE;
		break;
	    }

	/*
	 * skip to start of next option name
	 */
	while (*sp && !isalpha(*sp))
	    sp++;
	str = sp;
    }

    return 0;
}

/*
 * copy string using unctrl for things
 */
int strucpy(char *s1, char *s2, int len)
{
    register char *sp;

    while (len--)
    {
	strcpy(s1, (sp = unctrl(*s2++)));
	s1 += strlen(sp);
    }
    *s1 = '\0';

    return 0;
}
