/*
 * vars.h: Contains a few generic changes for various systems and how they
 *         like to handle functions. (I.e. names and parameter listing).
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1991-1993
 *
 * See the COPYRIGHT file.
 */
#ifndef VARS_H_
#define VARS_H_

#include "ansi.h"
#include "types.h"

#include <stdbool.h>
#include <string.h>

#ifdef SYSV
#define TERMIO
#endif

/* Maximum amount of arguments in parsing line arguments */
#define MAX_NUM_ARGS 20

typedef struct actionstruct {
    char *pattern;
    char *action;
    int indx;
    int nooutput; /* Don't print output */
    int quiet; /* No activation message */
    int notify; /* Print command responses */
    int active; /* Active or not */
    struct actionstruct *next;
    struct actionstruct *prev;
} Action;

typedef struct assignstruct {
    char *name;
    char *str;
    int mode;
    struct assignstruct *prev;
    struct assignstruct *next;
} Assign;

typedef struct ptarraystruct {
    char arr[MAX_NUM_ARGS][SMABUF];
    int arg_cnt;
} ArrStruct;

/* For binary search */
typedef struct command_struct {
    char *name;
    void (*func)();
    bool parse_var;
    bool echo_command;
    bool send_to_socket;
    int cnt;
} Command;

typedef struct gags {
    char *name;
    int indx;
    struct gags *next;
    struct gags *prev;
} Gag;

typedef struct macro {
    char *name;
    char *action;
    int flag;
    int indx;
    struct macro *next;
    struct macro *prev;
} Macro;

typedef struct rnode {
    char *line;
    char *date;
    int count;
    char ftime[10];
    char ltime[10];
    struct rnode *next;
    struct rnode *prev;
} RNode;

typedef struct node {
    char *line;
    int type; /* For queueing. Do we wait for prompt */
    int indx;
    struct node *next;
    struct node *prev;
} Node;

extern int action_match_suppress;
extern struct morestruct more_val;
extern Info info;

#ifdef RWHO
extern struct rwhostruct rwho;
#endif

extern int game_type;
extern struct input_modestruct input_mode;
extern Icomm icomm;
extern struct profilestruct profile;
extern struct racestruct races[MAX_NUM_PLAYERS];
extern struct logstruct logfile;
extern int quit_all;
extern int gb_close_socket;
extern Orbit orbit;
extern CurGame cur_game;
extern int detached;
extern int options[2];
extern struct scopestruct scope;
extern struct sector_typestruct sector_type[SECTOR_MAX];
extern struct statusstruct status;
extern struct waitforstruct wait_csp;
extern ServInfo servinfo;
extern int client_stats;
extern int csp_server_vers;
extern int end_prompt;
extern int hide_prompt;
extern int input_file;
extern int msg_type;
extern int paused;
extern int racegen;
extern int wait_status;
extern long boot_time;
extern long connect_time;
extern char macro_char;
extern char *Discoveries[];
extern char *PlanetTypes[];
extern char *RaceType[];
extern char *RaceTypePad[];
extern char *Relation[];
extern char *SectorTypes[];
extern char SectorTypesChar[];
extern char gbrc_path[];
extern char *help_client;
extern char *progname;
extern char *shell;
extern char *shell_flags;
extern char **refresh_line;
extern char race_name[];
extern char govn_name[];
extern char race_pass[];
extern char govn_pass[];
extern int password_failed;
extern char *race_colors[MAX_RCOLORS];

#endif // VARS_H_
