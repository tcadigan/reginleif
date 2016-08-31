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

#ifdef SYSV
#define TERMIO
#endif

#ifdef CTIX
#define signal sigset
#endif

#ifdef USE_INDEX
extern char *index(const char *, int);
extern char *rindex(const char *, int);
extern void bcopy(const void *, void *, int);
extern void bzero(void *, int);

#define strchr(s, c)      index((s), (int)(c))
#define strrchr(s, c)     rindex((s), (int)(c))
#define memcpy(s1, s2, n) bcopy((s2), (s1), (n))
#define memset(s, l)      bzero((s), 0, (l))

#else

extern char *strchr(const char *, int);
extern char *strrchr(const char *, int);
#endif /* USE_INDEX */

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
    int parse_var;
    int echo_command;
    int send_to_socket;
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
    struct node *prev;
    struct node *prev;
} Node;

extern int action_match_suppress;
extern struct morestruct move_val;
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
extern Game *find_game(char *nick);
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
extern char *race_colors[];
