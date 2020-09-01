#include "signals.h"

#include <curses.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "constants.h"
#include "death.h"
#include "externs.h"
#include "io.h"
#include "moria1.h"
#include "save.h"
#include "types.h"

extern int total_winner;
extern int moria_flag;
extern int search_flag;

/* No local special characters */

int error_sig;
int error_code;

/* This signal package was brought to you by -JEW- */

/* No suspend signal */

/*
 * Only allow one signal handler to execute, otherwise can get multiple save
 * files be sending multiple signals
 */
static int caught_signal = 0;

void init_signals()
{
    signal(SIGHUP, signal_save_no_core);
    signal(SIGINT, signal_ask_quit);
    signal(SIGQUIT, signal_save_core);
    signal(SIGILL, signal_save_core);
    signal(SIGTRAP, signal_save_core);
    signal(SIGIOT, signal_save_core);
    signal(SIGFPE, signal_save_core);
    signal(SIGKILL, signal_save_core);
    signal(SIGBUS, signal_save_core);
    signal(SIGSEGV, signal_save_core);
    signal(SIGTERM, signal_save_core);
}

void signal_save_core(int sig)
{
    if(caught_signal) {
	return;
    }

    caught_signal = 1;
    panic_save = 1;
    error_sig = sig;
    error_code = 0;

    if(!character_saved) {
	prt("OH NO!!!!!!!!!! Attempting panic save.", 23, 0);
	save_char(FALSE, FALSE);
    }

    signal(SIGQUIT, SIG_DFL);

    /* Restore terminal settings */
#ifndef BUGGY_CURSES
    nl();
#endif

#if defined(ultrix)
    nocrmode();

#else
    
    nocbreak();
#endif
    
    echo();
    resetterm();

    /* Restore the saved values of the local special chars */
    /* No local special characters */
    /* Allow QUIT signal */
    /* Nothing needs to be done here */
    kill(getpid(), 3);

    exit_game();
}

void signal_save_no_core(int sig)
{
    if(caught_signal) {
	return;
    }

    caught_signal = 1;
    panic_save = 1;
    error_sig = sig;
    error_code = 0;

    if(!character_saved) {
	save_char(FALSE, TRUE);
    }

    exit_game();
}

void signal_ask_quit(int sig)
{
    char command;

    /* Reset signal handler */
    signal(sig, signal_ask_quit);
    find_flag = FALSE;

    if(search_flag) {
	search_off();
    }

    if(py.flags.rest > 0) {
	rest_off();

	return;
    }

    if(get_com("Do you really want to quit?", &command)) {
	switch(command) {
	case 'y':
	case 'Y':
	    if(character_generated) {
		strcpy(died_from, "Quitting.");
		upon_death();
	    }
	    else {
		exit_game();
	    }

	    break;
	}
    }

    erase_line(MSG_LINE, 0);

    /* In case control-c typed during msg_print */
    if(wait_for_more) {
	put_buffer(" -more-", MSG_LINE, 0);
	put_qio();
    }
}


void no_controlz()
{
    /* No suspend signal */
}

void controlz()
{
    /* No suspend signal */
}

void ignore_signals()
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
}

void default_signals()
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}

void restore_signals()
{
    signal(SIGINT, signal_ask_quit);
    signal(SIGQUIT, signal_save_core);
}
