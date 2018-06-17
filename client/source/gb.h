/*
 * gb.h: Meta header for the software
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1993
 *
 * See the COPYRIGHT file.
 */
#ifndef GB_H_
#define GB_H_

/*
 * The VERSION is used to track differences between client versions. This should
 * only be changed if you make a mod, and should reflect your changes. For
 * example: 2.4.2-mymod2.2 or something. NOT the same format that I use. The
 * GBSAYING is just the log off message which I periodically use for what ever
 * philosophical statement I have for the current client release. It does not
 * serve any important function.
 */
/* #define VERSION "2.5.4 12/03/93" -mfw */
/* #define VERSION "2.6.0 01/25/05" */
#define VERSION "2.6.1 09/15/06"

/* #define GBSAYING "-- GB and reality are mutually exclusive!" */
#define GBSAYING "-- We gladly feast on those who would subdue us -- Meso Motto"

/*
 * DEFAULT_GBPORT is the default port for connecting to games. GBSERVER_VERS was
 * used in the past but currently serves no important function, but it may in
 * the future.
 */
#define DEFAULT_GBPORT 2010
#define GBSERVER_VERS 60

/*
 * These lines are set by the setup script and can NOT be removed
 *
 * LINUX a unix that's become hugely popular (esp. in opensource communities)
 * XMAP compiles the client with the XMAP functions
 * IMAP compiles the client with the Imap functions
 * OPTTECH compiles the client with the code for opttech included
 * POPN compiles the client with the auto mover ability (popn) enacted
 * NOTE: not including xmap and/or imap will reduce client size.
 * RESTRICTED causes the client to limit access. See the Help file.
 * SMART_CLIENT is current a beta test and to a normal user server no purpose
 * CLIENT_DEVEL causes extra warning to be displayed, for anyone developing the
 * client. This has to be manually set or change the author variable in the
 * setup script so it will be set automatically.
 */
#define TERMIO

// #define XMAP

#define ARRAY
#define IMAP
#define OPTTECH
#define POPN
#define RESTRICTED_ACCESS
#define SMART_CLIENT

#define CLIENT_DEVEL

/* Locations of help files, for the client and server. */
#define HELP_CLIENT "/usr/games/lib/Help"
#define HELP_SERVER "/usr/games/lib/Help.server"

/* Path to pre-init file */
#define DEFAULT_GBRC_PATH "~/.gbrc"

/* Num to keep in history */
#define DEFAULT_HISTORY 50

/* Num to keep for recall */
#define DEFAULT_RECALL 100

/*
 * CLIENT_PROMPT is output generated from the client (like proc_commands)
 * INPUT_PROMPT is in the input window.
 * OUTPUT_PROMPT is when you hit return and it is displayed in the output window
 */
#define DEFAULT_CLIENT_PROMPT "-> "
#define DEFAULT_INPUT_PROMPT  "command> "
#define DEFAULT_OUTPUT_PROMPT "> "

/* The format for the status bar. See the help file about the parameters */
#define DEFAULT_STATUS_BAR "$c$S$c$t$M$c$P$R$E$c$m$c$c$T$c"

/* Must be a string */
#define DEFAULT_STATUS_BAR_CHAR "-"

/* Path to mail file */
#define MAILPATH "/var/mail"

/* Mail seperator */
#define MAIL_DELIMITER "From "

/* For strncmp check */
#define MAIL_DELIMITER_LEN 5

/* Use for current sector in Imap */
#define DEFAULT_CURSOR_SECTOR "$"

#define DEFAULT_SHELL "/usr/bin/bash"

#ifndef ERROR
#define ERROR -1
#endif

#ifdef CLIENT_DEVEL
extern int client_devel;
#endif

#define ROBONAME "Robby"
#define MAXSTARS 300

#define HYPER_DIST_FACTOR 200.0
#define HYPER_DRIVE_FUEL_USE 5.0

#ifdef RWHO
struct rwhostruct rwho;
#endif

int main(int, char *argv[]);
void gbs(void);
void test_client(char ch);
void init_race_colors(void);

#endif // GB_H_
