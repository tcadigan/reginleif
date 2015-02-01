// This file has all the code for the option command.
//
// I would rather this command were not necessary, but
// it is the only way to keep the wolves off of my back
//
// @(#)options.c 3.3 (Berkeley) 5/25/81

#include "options.h"

#include "io.h"

#include <ctype.h>

// Description of an option and what to do with it
struct optstruct {
    // Option name
    char	*o_name;
    // Prompt for interactive entry
    char	*o_prompt;
    // Pointer to thing to set
    int		*o_opt;
    // Function to print value
    int		(*o_putfunc)();
    // Function to get value interactively
    int		(*o_getfunc)();
};

struct optstruct optlist[] = {
    {"terse", "Terse output: "                     , (int *) &terse      , put_bool, get_bool },
    {"flush", "Flush typeahead during battle: "    , (int *) &fight_flush, put_bool, get_bool },
    {"jump" , "Show position only at end of run: " , (int *) &jump       , put_bool, get_bool },
    {"step" , "Do inventories one line at a time: ", (int *) &slow_invent, put_bool, get_bool },
    {"askme", "Ask me about unidentified things: " , (int *) &askme      , put_bool, get_bool },
    {"name" , "Name: "                             , (int *) whoami      , put_str , get_str  },
    {"fruit", "Fruit: "                            , (int *) fruit       , put_str , get_str  },
    {"file" , "Save file: "                        , (int *) file_name   , put_str , get_str  }
};

// option:
//     Print and then set options from the terminal
int option()
{
    struct optstruct *op;
    int retval;

    wclear(hw);
    touchwin(hw);

    // Display current values of options
    for(op = optlist; op < &optlist[sizeof(optlist) / sizeof(struct optstruct)]; ++op) {
	waddstr(hw, op->o_prompt);
	(*op->o_putfunc)(op->o_opt);
	waddch(hw, '\n');
    }

    // Set values
    wmove(hw, 0, 0);
    for(op = optlist; op < &optlist[sizeof(optlist) / sizeof(struct optstruct)]; ++op) {
	waddstr(hw, op->o_prompt);
        retval = (*op->o_getfunc)(op->o_opt, hw);
	if(retval) {
	    if(retval == QUIT) {
		break;
            }
	    else if(op > optlist) {
                // MINUS
		wmove(hw, (op - optlist) - 1, 0);
		op -= 2;
	    }
	    else {
                // Trying to back up beyond the top
		putchar('\007');
		wmove(hw, 0, 0);
		--op;
	    }
        }
    }

    // Switch back to original screen
    mvwaddstr(hw, LINES-1, 0, "--Press space to continue--");
    wrefresh(hw);
    wait_for(' ');
    clearok(cw, TRUE);
    touchwin(cw);
    after = FALSE;

    return 0;
}

// put_bool:
//     Put out a boolean
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

// put_str:
//     Put out a string
int put_str(char *str)
{
    waddstr(hw, str);

    return 0;
}

// get_bool:
//     Allow changing a boolean option and print it out
int get_bool(bool *bp, WINDOW *win)
{
    int oy;
    int ox;
    bool op_bad;

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
	switch(readchar()) {
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

// get_str:
//     Set a string option
int get_str(char *opt, WINDOW *win)
{
    char *sp;
    int c;
    int oy;
    int ox;
    char buf[80];

    wrefresh(win);
    getyx(win, oy, ox);

    // Loop reading in the string, and put it in a temporary buffer
    c = readchar();
    for(sp = buf;
        (c != '\n') && (c != '\r') && (c != '\033') && (c != '\007');
        wclrtoeol(win), wrefresh(win)) {
	if(c == -1) {
	    continue;
        }
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

        c = readchar();
    }
    *sp = '\0';
    // Only change an option if something has been typed
    if(sp > buf) {
	strucpy(opt, buf, strlen(buf));
    }
    wmove(win, oy, ox);
    waddstr(win, opt);
    waddch(win, '\n');
    wrefresh(win);
    if(win == cw) {
	mpos += (sp - buf);
    }
    if(c == '-') {
	return MINUS;
    }
    else if((c == '\033') || (c == '\007')) {
	return QUIT;
    }
    else {
	return NORM;
    }
}

// parse_opts:
//     Parse options from a string, usually taken from the environment.
//     The string is a series of command separated values, with booleans
//     being stated as "name" (true) or "noname" (false), and strings
//     being "name=...", with the string being defined up to a comma
//     or the end of the entire option string.
int parse_opts(char *str)
{
    char *sp;
    struct optstruct *op;
    int len;

    while(*str) {
        // Get the option name
	for(sp = str; isalpha(*sp); ++sp) {
	    continue;
        }
	len = sp - str;

        // Look it up and deal with it
	for(op = optlist; op < &optlist[sizeof(optlist) / sizeof(struct optstruct)]; ++op) {
	    if(strncmp(str, op->o_name, len) == 0) {
                // If the option is a boolean
		if (op->o_putfunc == put_bool) {
		    *(bool *)op->o_opt = TRUE;
                }
		else {
                    // String option
		    char *start;

                    // Skipt to the start of the string value
		    for(str = sp + 1; *str == '='; ++str) {
			continue;
                    }
		    if(*str == '~') {
			strcpy((char *) op->o_opt, home);
			start = (char *) op->o_opt + strlen(home);
			while(*++str == '/') {
			    continue;
                        }
		    }
		    else {
			start = (char *) op->o_opt;
                    }

                    // Skip to the end of string value
		    for(sp = str + 1; *sp && *sp != ','; ++sp) {
			continue;
                    }
		    strucpy(start, str, sp - str);
		}
		break;
	    }
	    else if((op->o_putfunc == put_bool)
                     && (strncmp(str, "no", 2) == 0)
                     && (strncmp(str + 2, op->o_name, len - 2) == 0)) {
                // Check for "noname" for boolean
		*(bool *)op->o_opt = FALSE;
		break;
	    }
        }

        // Skip to start of next option name
	while(*sp && !isalpha(*sp)) {
	    sp++;
        }
        
	str = sp;
    }

    return 0;
}

// strucpy:
//     Copy string using unctrl for things
int strucpy(char *s1, char *s2, int len)
{
    char *sp;

    while(len--) {
	strcpy(s1, (sp = unctrl(*s2++)));
	s1 += strlen(sp);
    }
    *s1 = '\0';

    return 0;
}
