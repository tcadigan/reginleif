/*
 * types.h: Used to define the needed #defines as well as to set up structures
 *          used throughout the client. They get declared by gb.c (with the
 *          trick of including types.h first) and then are forever after 
 *          declared as externs by included vars.h
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1991-1993
 *
 * See the COPYRIGHT file.
 */
#ifndef _CLIENT_TYPES_H_
#define _CLIENT_TYPES_H_
#endif

#include "csp_types.h"

/* A yes macro from promptfor() */
#define YES(A) (((A) == 'Y') || ((A) == 'y'))

/* Bit functions for the user options */
#define SET_BIT(A, I) ((A)[(I) / 32] |= ((I) < 32 ? (1 << (I)) : (1 << ((I) % 32))))
#define CLR_BIT(A, I) ((A)[(I) / 32] &= ~((I) < 32 ? (1 << (I)) : (1 << ((I) % 32))))
#define GET_BIT(A, I) ((A)[(I) / 32] & ((I) < 32 ? (1 << (I)) : (1 << ((I) % 32))))
#define GET_BITx(A, I) ((A) & (1 << (I)))

/* Various defines to make my life easier */
#define REPEAT_SLEEP_TIME 60 /* Time to sleep if doing repeats */
#define MORE_DELAY        15 /* In secs, time to trigger more */

/*
 * Number of loops through selest() until exiting. To allow the server
 * to catch up and display a good bye message
 */
#define GB_CLOSE_SOCKET_DELAY 15

#define MAX_NUM_PLAYERS      65 /* 1-64, and 1 for 0 is 65 */
#define MAX_NUM_STARS        256
#define MAX_NUM_PLANETS_STAR 10
#define MAX_NUM_PLANETS      (MAX_NUM_STARS * MAX_NUM_PLANETS_STAR)
#define MAX_POST_LEN         500

/* rwho stuff */
#define RWHO_DELAY    60
#define RWHO
#define RWHO_OFF      0
#define RWHO_ON       1
#define RWHO_NOTIFIED 2

/* Which type of game have we logged into */
#define GAME_NOTGB   -1
#define GAME_UNKNOWN 0
#define GAME_GB      1
#define GAME_HAP     2
#define GAME_GBDT    3

#define NOTGB()   (game_type == GAME_NOTGB)
#define UNKNOWN() (game_type == GAME_UNKNOWN)
#define GB()      (game_type == GAME_GB)
#define HAP()     (game_type == GAME_HAP)
#define GBDT()    (game_type == GAME_GBDT)

/* Types of end prompts */
#define INTERNAL_PROMPT -1 /* For internal workings */
#define NOT_PROMPT      0 /* None */
#define END_PROMPT      1 /* Generic, connect refused, help, doing */
#define PASS_PROMPT     2 /* Please enter ... */
#define DOING_PROMPT    3 /* Doing */
#define ENDDOING_PROMPT 4
#define NODIS_PROMPT    5
#define FINISHED_PROMPT 6 /* Finished */
#define LEVEL_PROMPT    7 /* ( [*] / * ) */

/* Commands we are processing */
#define C_DONEINIT -2 /* Init and re-init */
#define C_DONEPROC -1 /* Any internal icomm process */
#define C_NONE     0
#define C_RNEWS    1
#define C_RWHO     2
#define C_PROFILE  3
#define C_RELATION 4
#define C_STATUS   5
#define C_TELEGRAM 6
#define C_DISPATCH 7
#define C_READ     8

/* State of processing */
#define S_NONES 0 /* Not doing any processing */
#define S_WAIT  1 /* Waiting for the start sequence */
#define S_PROC  2 /* Processing */
#define S_DONE  3 /* Done, waiting to cleanup/shutdown */

/* Levels for scope */
#define LEVEL_ERROR       -2
#define LEVEL_NONE        -1
#define LEVEL_UNIV        0
#define LEVEL_STAR        1
#define LEVEL_PLANET      2
#define LEVEL_SHIP        3 /* Use this for if x > LEVEL_SHIP */
#define LEVEL_MOON        4

#define LEVEL_USHIP       5
#define LEVEL_SSHIP       6
#define LEVEL_PSHIP       7
#define LEVEL_MOTHERSHIP  8 /* Use for if x > LEVEL_MOTHERSHIP */
#define LEVEL_USSHIP      9 /* Ship on ship on ship...at univ */
#define LEVEL_SSSHIP     10 /* Ship on ship on ship...at star */
#define LEVEL_PSSHIP     11 /* Ship on ship on ship...at planet */

/* Buffer sizes */
#define SMABUF   200
#define NORMSIZ 1024
#define MAXSIZ  8192

/* Modes of output */
#define NONE            0
#define SCREEN_ONLY     1
#define DO_SCREEN_ONLY  2
#define SERVER_ONLY     3
#define USER_DEFINED    4
#define ENCRYPTED       5
#define SERVER          6
#define NORM_ANNOUNCE   7
#define NORM_BROADCAST  8
#define GB_ANNOUNCE     9
#define GB_BROADCAST    10
#define GB_THINK        11
#define GB_EMOTE        12
#define HAP_ANNOUNCE    13
#define HAP_BROADCAST   14
#define HAP_THINK       15

/* Modes for cursor */
#define EDIT_OVERWRITE 1
#define EDIT_INSERT    2

/* Race types (these are cross referenceed to CSP_RELATION) */
#define RACE_UNKNOWN 0
#define RACE_MESO    1
#define RACE_NORM    2

/* Defines for logging in and at which step */
#define L_NOTHING      0 /* Pre-init and not ready yet */
#define L_BOOTED       1 /* gbII has started itself after init */
#define L_CONNECTED    2 /* Socket connection established */
#define L_PASSWORD     3 /* At password prompt */
#define L_LOGGEDIN     4 /* Password entered and accepted */
#define L_SEGMENT      5 /* Moveseg */
#define L_UPDATE       6 /* Update */
#define L_INTERNALINIT 7 /* Doing initial start commands */
#define L_REINIT       8 /* After update/segment reinit */
#define L_ACTIVE       9 /* Ripping and rarin' to go */

/* wait_status flags */
#define WAIT_NONE     L_NOTHING
#define WAIT_BOOTED   L_BOOTED
#define WAIT_CONNECT  L_CONNECTED
#define WAIT_PASSWORD L_PASSWORD
#define WAIT_LOGIN    L_LOGGEDIN
#define WAIT_SEGMENT  L_SEGMENT
#define WAIT_UPDATE   L_UPDATE

/* Number of allowed queued Icommands */
#define MAX_ICOMMANDS 10

/* Macros for manipulating Icommands */
#define ICOMM_DOING_COMMAND   (icomm.num)
#define ICOMM_INITIALIZE()    (icomm.num = 0)
#define ICOMM_IGNORE          icomm.list[0].ignore
#define ICOMM_PROMPT          icomm.list[0].prompt
#define ICOMM_STATE           icomm.list[0].state
#define ICOMM_COMM            icomm.list[0].comm
#define ICOMM_SIGIGNORE(n, x) (icomm.list[(n)].ignore = (x))
#define ICOMM_SPROMPT(n, x)   (icomm.list[(n)].prompt = (x))
#define ICOMM_SSTATE(n, x)    (icomm.list[(n)].state = (x))
#define ICOMM_SCOMM(n, x)     (icomm.list[(n)].comm = (x))

/* Types of relations (there are cross referenced to CSP_RELATION) */
#define RELATION_UNKNOWN 0
#define RELATION_ALLIED  1
#define RELATION_NEUTRAL 2
#define RELATION_ENEMY   3

/* Types of bind modes */
#define BIND_ERR   -1
#define BIND_NORM  0
#define BIND_ESC   1
#define BIND_IMAP  2
#define BIND_MORE  3
#define BIND_ARROW 4
#define BIND_FUNC  5

/* Default chars for more chars */
#define MORE_DEFAULT_QUITCH    'q'
#define MORE_DEFUALT_CLEARCH   'c'
#define MORE_DEFAULT_CANCELCH  'k'
#define MORE_DEFAULT_NONSTOPCH 'n'
#define MORE_DEFAULT_FORWARDCH 'f'
#define MORE_DEFAULT_ONELINECH '\n'

/* Values prompt_for take on in input_mode */
#define PROMPT_OFF    0
#define PROMPT_CHAR   1
#define PROMPT_STRING 2

/* Special characters */
#define SEND_QUOTE_CHAR       '^'
#define SEND_QUOTE_PHRASE     "GBCS"
#define SEND_OLD_QUOTE_PHRASE "gbII_client_special"
#define BELL_CHAR             '\007'
#define BOLD_CHAR             '\002'
#define INVERSE_CHAR          '\022'
#define UNDERLINE_CHAR        '\025'
#define BELL_CHAR_STR         "\007"
#define BOLD_CHAR_STR         "\002"
#define INVERSE_CHAR_STR      "\022"
#define UNDERLINE_CHAR_STR    "\025"
#define VAR_CHAR              '$' /* Variable char denote */

/* Message type levels */
#define MSG_BROADCAST      1
#define MSG_ANNOUNCE       2
#define MSG_THINK          3
#define MSG_TELEGRAMS      4
#define MSG_COMMUNICATION  5 /* Cut off point */
#define MSG_NEWS           6
#define MSG_EXTRA          7
#define MSG_PLANET         8
#define MSG_ORBIT         10 /* Can't use currently */
#define MSG_ALL           11
#define MSG_NONE          12

/* Log levels */
#define LOG_OFF           0
#define LOG_COMMUNICATION MSG_COMMUNICATION
#define LOG_ALL           MSG_ALL

/* Modes for imap settings */
#define IMAP_GEO     1
#define IMAP_NORMAL  2
#define IMAP_INVERSE 3

/* Parsing modes */
#define PARSE_SLASH       1
#define PARSE_SLASH_NOTNL 2
#define PARSE_VARIABLES   3
#define PARSE_LOOP        4
#define PARSE_FOR_LOOP    5
#define PARSE_ALL         6

/* Sector indexes */
#define SECTOR_OCEAN   1
#define SECTOR_GAS     2
#define SECTOR_ICE     3
#define SECTOR_MTN     4
#define SECTOR_LAND    5
#define SECTOR_DESERT  6
#define SECTOR_FOREST  7
#define SECTOR_PLATED  8
#define SETTOR_WORM    9
#define SECTOR_MAX    10

/* check_for_special_formatting types */
#define FORMAT_NORMAL  1
#define FROMAT_DEFAULT 1
#define FORMAT_HELP    2 /* Server help format */

/* Defines for the setable options */
#define OP_NONE                0
#define AUTOLOGIN              1
#define AUTOLOGIN_STARTUP      2
#define BEEP                   3
#define BRACKETS               4
#define CONNECT                5
#define CONNECT_STARTUP        6
#define DISPLAYING             7
#define DISPLAY_TOP            8
#define ENCRYPT                9
#define HIDE_END_PROMPT       10
#define LOGINSUPPRESS         11
#define LOGINSUPPRESS_STARTUP 12
#define NO_LOGOUT             13
#define MAP_DOUBLE            14
#define MAP_SPACE             15
#define PARTIAL_LINES         16
#define RAWMODE               17
#define SCROLL_CLR            18
#define SHOW_MAIL             19
#define SHOW_CLOCK            20
#define SLASH_COMMANDS        21
#define DO_BELLS              22
#define SHOW_ACTIONS          23
#define NOCLOBBER             24
#define BOLD_COMM             25
#define FULLSCREEN            26
#define SCROLL                27
#define QUIT_ALL              28
#define ACTIONS               29
#define NUM_BITOPTIONS        30
#define DISP_ANSI             31 /* Display ansi colors -mfw */

/* Object types for CSP orbit -mfw */
#define TYPE_UNKNOWN 0
#define TYPE_STAR    1
#define TYPE_PLANET  2
#define TYPE_MOON    3
#define TYPE_SHIP    4

#define set_display(A) toggle((int *)(A), DISPLAYING, "display")

/* Macro for secret input */
#define SECRET(B, S, T) {                       \
        ++hide_input;                           \
        promptfor((B), (S), (T));               \
        hide_input = 0;                         \
    }

/* imap/popn defines */
#define MAX_ShIPS_IN_SURVEY 10

/*
 * For time tags. The strlen of these plus the strlen of a unix
 * timestamp should be greater than what is returned by ctime (or
 * it'll segfault)
 */
#define OPEN_TIME_TAG  ">T>T>T>T>"
#define CLOSE_TIME_TAG "<T<T<T<T<"

/* Structs and other variable declarations */

/* Extern declared in vars.h, initialized in gb.c */
struct logstruct {
    FILE *fd;
    char name[BUFSIZ];
    int on;
    int redirect;
    int level;
};

typedef struct icommunitstruct {
    int comm; /* Which command are we doing */
    int csp_start;
    int csp_end;
    int state; /* Waiting/processing */
    int prompt; /* What prompt to wait for */
    int ignore; /* Don't print line or not */
    int flag; /* A flag for passing extra info */
} IcommUnit;

typedef struct icommstruct {
    int num;
    IcommUnit list[MAX_ICOMMANDS];
} Icomm;

typedef struct shipstruct {
    int shipno;
    char ltr;
    int owner;
    struct shipstruct *next;
    struct shipstruct *prev;
} Ship;

struct scopestruct {
    int aps;
    int starnum;
    int planetnum;
    int numships;
    int ship;
    Ship *motherlist;
    char star[200];
    char planet[200];
    char shipc[20]; /* For old GB games */
    char mothership[20]; /* For old GB games */
    /* int level; */
    enum LOCATION level;
};

#ifdef RWHO
struct rwhoplayers {
    char name[20];
    int id;
    long last_on;
    long last_spoke;
    char last_name[200];
    long changed_names;
    int on;
    int watch4;
};

struct wrhostruct {
    long last_time;
    int on;
    struct rwhoplayers info[MAX_NUM_PLAYERS];
};
#endif

struct morestruct {
    char k_quit;
    char k_clear;
    char k_cancel;
    char k_nonstop;
    char k_forward;
    char k_oneline;
    int num_rows;
    int delay;
    int on;
    long last_line_time;
    int num_lines_scrolled;
    int non_stop;
    int forward;
    int doing;
};

struct statusstruct {
    long last_time;
    char last_buf[BUFSIZ];
    char current_buf[BUFSIZ];
    char format[BUFSIZ];
    char schar[2];
};

struct ranges {
    int guns;
    int space;
    int ground;
};

struct raceinfo {
    enum RACE_TYPE racetype;
    int morale;
    int fert;
    double birthrate;
    double mass;
    int fight;
    double metab;
    int sexes;
    int explore;
    double tech;
    double iq;
};

struct planetinfo {
    int temp;
    int methane;
    int oxygen;
    int helium;
    int nitrogen;
    int co2;
    int hydrogen;
    int sulfur;
    int other;
};

struct sectorinfo {
    int ocean;
    int gas;
    int ice;
    int mtn;
    int land;
    int desert;
    int forest;
    int plated;
};

typedef struct profilestruct {
    enum PLAYER_TYPE player_type; /* Normal/guest/diety */
    char defscope[SMABUF];
    char personal[BUFSIZ];
    char sect_pref[SMABUF];
    char racename[SMABUF];
    char govname[SMABUF];
    int raceid;
    int govid;
    int capital;
    int updates_active;
    int know;
    char discovery[SMABUF];
    struct ranges rages;
    struct raceinfo raceinfo;
    struct planetinfo planet;
    struct sectorinfo sector;
} Profile;

struct sector_typestruct {
    char setc;
    int compat;
};

struct racestruct {
    char name[200];
    int type;
    enum RELATION you_to_them;
    enum RELATION them_to_you;
};

/* For world/game construct */
typedef struct gamestruct {
    char *nick;
    char *host;
    char *port;
    char *type; /* CHAP -mfw */
    char *racename; /* CHAP -mfw */
    char *pripassword;
    char *govname; /* CHAP -mfw */
    char *secpassword;
    int indx;
    struct gamestruct *next;
    struct gamestruct *prev;
} Game;

typedef struct curgamestruct {
    Game game;
    int maxplayers;
} CurGame;

struct input_modestruct {
    int edit;
    int map;
    int say;
    int post;
    int promptfor;
    int offset;
};

/* csp waitfor */
struct waitforstruct {
    int lo;
    int hi;
    int have;
    char buf[MAXSIZ];
};

typedef struct sectorstruct {
    int x;
    int y;
    char setc;
    char des;
    int wasted;
    int own;
    int eff;
    int frt;
    int frt;
    int mob;
    int xtal;
    int res;
    int civ;
    int mil;
    int mpopn;
    int numships;
    Ship ships[MAX_SHIPS_IN_SURVEY];
    int sect_status; /* Used by popn */
} Sector;

typedef struct mapstruct {
    int maxx;
    int maxy;
    char star[200];
    char planet[200];
    int res;
    int fuel;
    int des;
    int popn;
    int mpopn;
    int tox;
    double compat;
    int enslaved;
    int map;
    int inverse; /* Inverse on/off */
    int geo; /* Geo on/off */
    int ansi; /* Ansi on/off */
    Sector *ptr;
} Map;

/* Buffer output structs */
typedef struct bufferstruct {
    char *buf;
    struct bufferstruct *next;
    struct bufferstruct *prev;
} Buffer;

typedef struct bufferinfostruct {
    Buffer *head;
    Buffer *tail;
    int partial;
    int is_partial;
} BufferInfo;

typedef struct infostruct {
    unsigned long bytes_read;
    unsigned long bytes_sent;
    unsigned long lines_sent;
    unsigned long lines_read;
} Info;

typedef struct serverinfo {
    int update_suspended;
    int version;
} ServInfo;

typedef struct orbstar {
    int snum;
    double x;
    double y;
    int explored;
    int inhabited;
    int numplan;
    int stability;
    int novastage;
    int tempclass;
    double gravity;
    char name[SMABUF];
} OrbStar;

typedef struct orbplanet {
    int snum;
    int pnum;
    double x;
    double y;
    int explored;
    int type;
    double compat;
    int owned;
    char name[SMABUF];
} OrbPlanet;

typedef struct orbship {
    int num;
    int owner;
    char type;
    double x;
    double y;
    double st;
    double yt;
    int array;
} OrbShip;

typedef struct orbit {
    int scope;
    int univsize;
    int syssize;
    int plorbsize;
    int inverse;
    int scnt;
    int pcnt;
    int bcnt;
    int color;
    int scale;
    float lastx;
    float lasty;
    float zoom;
    int nostars;
    int noplanets;
    int noships;
    int stars;
    char gltype[SMABUF];
    char glname[SMABUF];
    int type;
    OrbStar star;
    OrbPlanet planet;
    OrbShip ship;
    int position;
} Orbit;

typedef struct pmap {
    int geo; /* Remove soon */
    int inverse; /* Remove soon */
    int double_digits; /* Remove soon */
    int color; /* For maps with ansi -mfw */
    int snum;
    char sname[SMABUF];
    int pnum;
    char pname[SMABUF];
    double compat;
    int tox;
    int enslaved;
    int x;
    int y;

    /* Dynamic1 */
    int type;
    int sects;
    int guns;
    int mobptr;
    int res;
    int des;
    int fuel;
    int xtals;

    /* Dynamic2 */
    int mob;
    int actmob;
    int popn;
    int mpopn;
    int totpopn;
    int mil;
    int totmil;
    int tax;
    int acttax;
    int deposits;
    int estprod;

    char aliens[BUFSIZ];
} Pmap;

typedef struct cspsendval_struct {
    char *name;
    int comm_num;
    char *(*func)();
    int cnt;
    int know;
} CSPSendVal;

typedef struct cspreceiveval_struct {
    int comm_num;
    void (*func)();
    int cnt;
    int know;
} CSPReceiveVal;

typedef struct cryptrecallstruct {
    char *key;
    struct cryptrecallstruct *next;
    struct cryptrecallstruct *prev;
} CryptRecall;

/*
 * Crypt: The crypt list structure, consists of the nuckname, and the
 *        encryption key
 */
typedef struct cryptstruct {
    char *nick;
    char *key;
    int indx;
    struct cryptstruct *next;
    struct cryptstruct *prev;
} Crypt;

/* For binary search */
typedef struct commandset_struct {
    char *name;
    int value;
    void (*func)();
    int has_changed;
} CommandSet;

typedef struct loopstruct {
    long time; /* Time to wait in seconds */
    char *cmd; /* cmd to pass to the process_key */
    ing user_defn; /* User defined or client */
    long last_time; /* Last time cmd done */
    int indx;
    struct loopstruct *next;
    struct loopstruct *prev;
} Loop;
