/*
 * gb.c: Main routine, as well as the handling of i/o choices.
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1993
 *
 * See the COPYRIGHT file.
 */
#include "gb.h"

#include <ctype.h>
#include <memory.h>
#include <pwd.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#ifdef __hpux
#include <unistd.h>
#endif

#include "ansi.h"
#include "proto.h"
#include "str.h"
#include "terms.h"
#include "types.h"
#include "vars.h"

#ifdef XMAP
#include "xmap.h"
#endif

#define REFRESH_TIME_IMAP 30000 /* When in imap */
#define REFRESH_TIME_NORM 80000
#define REFRESH_TIME_LONG 150000
#define MAX_QUEUE_COMMANDS 10
#define MAX_SOCKET_COMMANDS 3

#ifndef FD_ZERO
#define DESCR_MASK int
#define FD_ZERO(A) (*(A) = 0)
#define FD_SET(A, B) (*(B) |= (1 << (A)))
#define FD_ISSET(A, B) (*(B) & (1 << (A)))

#else

#define DESCR_MASK fd_set

#endif

extern char last_prompt[];
extern long last_no_logout_time; /* For no_logout */

#ifdef IMAP
extern long map_time;
#endif

int errno;
static jmp_buf int_jmp;
static int nfds;
struct logstruct logfile = { (FILE *)NULL, "", FALSE, FALSE, LOG_OFF };
struct scopestruct scope;
struct waitforstruct wait_csp = { 0, 0, FALSE, "" };

struct morestruct more_val = {
    MORE_DEFAULT_QUITCH,
    MORE_DEFAULT_CLREACH,
    MORE_DEFAULT_CANCELCH,
    MORE_DEFAULT_NONSTOPCH,
    MORE_DEFAULT_FORWARDCH,
    MORE_DEFAULT_ONELINECH,
    21,
    MORE_DELAY,
    FALSE,
    0,
    0,
    FALSE,
    FALSE,
    FALSE
};

struct statusstruct status = {
    0,
    "-",
    "Not Connected",
    DEFAULT_STATUS_BAR,
    DEFAULT_STATUS_BAR_CHAR
};

struct profilestruct profile;
struct racestruct races[MAX_NUM_PLAYERS];
struct input_modestruct input_mode = { EDIT_INSERT, FALSE, FALSE, FALSE, PROMPT_OFF, 0 };
struct sector_typestruct sector_type[SECTOR_MAX];
ServInfo servinfo = { 0, 0 };
CurGame cur_game;
Icomm icomm;
Info info = { 0, 0, 0, 0 };
char macro_char;
char gbrc_path[BUFSIZ + 1];

char *Discoveries[] = {
    "Hyperdrive ",
    "Crystal ",
    "Atmosphere ",
    "Laser ",
    "Wormhole ",
    "VN ",
    "CEW ",
    "Cloak ",
    "AVPM ",
    "Tractor ",
    "Transporter "
};

char *PlanetTypes[] = {
    "Class M",
    "Asteroid",
    "Airless",
    "Iceball",
    "Jovian",
    "Waterball",
    "Forest",
    "Desert"
};

char *RaceType[] = { "Unknown", "Morphic", "Normal" };
char *RaceTypePad[] = { "", "", " " };
char *Relation[] = { "Unknown", "Allied", "neutral", "WAR" };

char *SectorTypes[] = {
    /* sea, lnd, mtn, gas, ice,  for, des, plt, was, unk, wrm, swp */
       ".", "*", "^", "~", "#", "\"", "-", "o", "%", "?", " ", "("
};

char SectorTypesChar[] = {
    /* sea, lnd, mtn, gas, ice, for, des, plt, was, unk, wrm, swp */
       '.', '*', '^', '~', '#', '"', '-', 'o', '%', '?', ' ', '('
};

char *help_client; /* Where client help file is */
char *progname;
char *shell; /* User's preferred shell */
char *shell_flags;
char **refresh_line;
char **race_colors[MAX_RCOLORS];

#ifdef RWHO
struct rwhostruct rwho;
#endif

int action_match_suppress = FALSE; /* String is user(keyboard) definition */
int client_stats = L_NOTHING; /* Stats of the client */
int detached = FALSE;
int do_queue = TRUE; /* For the queue */
int end_prompt = NOT_PROMPT;
int exit_now = FALSE;
int game_type = GAME_NOTGB;
int gb = -1; /* Socket descriptor */
int gb_close_socket = FALSE;
int hide_input = FALSE; /* Secret/password typing */
int input_file = FALSE; /* -e flag for redirected file */
int msg_type = MSG_NONE;
int prompt_return = FALSE;
int paused = FALSE;
int queue_wait = TRUE; /* Wait only when a %l or %b */
int quit_all = FALSE;
int racegen = FALSE;
int reconnect_delay = REPEAT_SLEEP_TIME;
int socket_return = FALSE; /* waitfor_csp return flag */
int csp_server_vers = 0;
int robo = FALSE;
int wait_status = WAIT_NONE;
long boot_time = -1;
long connect_time = -1;
long now;
long then = -1; /* Initialize */

#ifdef CLIENT_DEVEL
int client_devel = FALSE;
#endif

#ifdef XMAP
int xmap_active = FALSE;

extern void mwin_event_loop();
extern void makw_mwin();

extern mwin *win;
extern Display *mdpy;
extern widget *trv;
#endif

extern char *getenv(const char *);
extern char *getlogin(void);
extern char *string();
extern Game *find_game();
extern uid_t getuid(void);
extern int atoi(const char *);
extern time_t time(time_t *);
extern int getdtablesize(void);
extern int fflush(FILE *);

/* Variables used with CHAP */
char race_name[32];
char govn_name[32];
char race_pass[32];
char govn_pass[32];
int password_failed;

/*
 * Sets screen and connects socket. Upon quitting, closes screen inits and
 * socket.
 */
int main(int argc, char *argv[])
{
    char *env;
    Game *game;
    int allow_init_file = TRUE;
    int editclient = FALSE;

    /* Set up game struct */
    game = (Game *)NULL;
    progname = *argv;
    cur_game.game.host = NULL;

    /*
     * We have a default port, but no host since that varies using last_prompt
     * since it gets init right afterwards
     */
#ifdef DEFAULT_GBPORT
    sprintf(last_prompt, "%d", DEFAULT_GBPORT);
    cur_game.game.port = string(last_prompt);
#endif

#ifdef GBSERVER_VERS
    servinfo.version = GBSERVER_VERS; /* Server version */
#else
    servinfo.version = 0;
#endif

    cur_game.game.pripassword = string("");
    cur_game.game.secpassword = string("");
    cur_game.game.nick = string("");
    strcpy(las_prompt, "Not Connected");

    /* Check user's environment */
    env = getenv("GBHOST");

    if(env != NULL) {
        strfree(cur_game.game.host);
        cur_game.game.host = string(env);
    }

    env = getenv("GBPORT");

    if(env != NULL) {
        strfree(cur_game.game.port);
        cur_game.game.port = string(env);
    }

    /* Prevent null pointers later in the program by initializing */
#ifdef DEFAULT_CLIENT_PROMPT
    client_prompt = string(DEFAULT_CLIENT_PROMPT);
#else
    client_prompt = string("");
#endif

#ifdef DEFAULT_INPUT_PROMPT
    input_prompt = string(DEFAULT_INPUT_PROMPT);
#else
    input_prompt = string("");
#endif

    update_input_prompt(input_prompt);

#ifdef DEFAULT_OUTPUT_PROMPT
    output_prompt = string(DEFAULT_OUTPUT_PROMPT);
#else
    output_prompt = string("");
#endif

#ifdef DEFAULT_MACRO_CHAR
    macro_char = DEFAULT_MACRO_CHAR;
#else
    macro_char = '/';
#endif

    /* Get user's shell for shell escapes */
    shell = getenv("SHELL");

    if(shell != NULL) {
        struct passwd *pwd;
        pwd = getpwuid(getuid());

        if(pwd) {
            shell = string(pwd->pw_shell);
        }
        else {
#ifdef DEFAULT_SHELL
            shell = string(DEFAULT_SHELL);
#else
            shell = string("/bin/sh");
#endif
        }
    }

#ifdef DEFUALT_SHELL_FLAGS
    shell_flags = string(DEFAULT_SHELL_FLAGS);
#else
    shell_flags = string("-cf");
#endif

#ifdef HELP_CLIENT
    help_client = string(HELP_CLIENT);
#else
    help_client = string("./Help");
#endif

    setjmp(int_jmp);
    init_key();

    /* Fixed inits */
    init_refresh_lines();
    init_crypt();
    init_csp();
    init_endprompt_connect();
    init_race_colors();

    /* 
     * Dynamic inits
     * Pre-defined assigns so $vars can be used in macros and to maintain
     * backward compatability
     */
    init_assign(TRUE);
    ICOMM_INITIALIZE();

    term_clear_screen();
    term_move_cursor(0, num_rows - 2);

    /* Load up the initialization file */
    env = getenv("GBRC");

    if(env != NULL) {
        strcpy(gbrc_path, env);
    }
    else {
#ifdef DEFAULT_GBRC_PATH
        strcpy(gbrc_path, DEFAULT_GBRC_PATH);

#else
        
        strcpy(gbrc_path, "~/.gbrc");
#endif
    }

    set_display("off");

    while((--argc > 0) && ((*++argv)[0] == '-')) {
        while(*++(*argv)) {
            switch(**argv) {
            case 'a':
                toggle(options, AUTOLOGIN_STARTUP, "pre-init al");

                break;
            case 'd':
                toggle(options, DISPLAY_TOP, "pre-init dt");

                break;
            case 'e':
                input_file = TRUE;

                break;
            case 'f':
                allow_init_file = FALSE;

                break;
            case 'i':
#ifndef RESTRICTED_ACCESS
                load_predefined(*argv + 1);
                
                while(*++(*argv)) {
                    ;
                }

                --(*argv);
#endif

                break;
            case 'l':
                CLR_BIT(options, DISPLAY_TOP);
                set_display("on");
                cmd_listgame();
                quit_gb(0, "", NULL, NULL);

                break;
            case 'p':
                toggle(options, PARTIAL_LINES, "pre-init pl");

                break;
            case 'r':
                toggle(options, CONNECT_STARTUP, "pre-init rc");

                break;
            case 's':
                toggle(options, LOGINSUPPRESS_STARTUP, "pre-init ls");

                break;
            case 'v':
                CLR_BIT(options, DISPLAY_TOP);
                set_display("on");
                cmd_version("");
                quit_gb(-1, "", NULL, NULL);

                /* Never get here */
                break;
#ifdef XMAP
            case 'x':
                xmap_active = TRUE;

                break;
#endif
            case 'E':
                editclient = TRUE;

                break;
            case 'R':
                toggle(options, RAWMODE, "pre-init raw");

                break;
#ifdef CLIENT_DEVEL
            case 'D':
                client_devel = TRUE;

                break;
#endif
            case 'V':
                servinfo.version = atoi(*argv + 1);

                while(*++(*argv)) {
                    ;
                }

                --(*argv);

                break;
            case 'h':
            case '?':
            default:
                CLR_BIT(options, DISPLAY_TOP);
                set_display("on");
                msg("-- Usage: %s [-adehlrsvER?] [-e<filename>] [-i<filename>] [-V<version#>] [host | nick] [port]", progname);
                msg("  -a      toggles autologin");
                msg("  -d      toggles display_from_top");
                msg("  -f      fast startup. Do not read GBRC(~/.gbrc be default");
                msg("  -h      this help screen");
                msg("  -l      does a listgame command and exits");
                msg("  -r      toggles repeat_connect");
                msg("  -s      toggle login_suppress");
                msg("  -?      this help screen");
                msg("  -v      prints out client version and exits");
#ifdef XMAP
                msg("  -x      starts client with Xmap");
#endif
                msg("  -E      edit mode. Enters %s, without connecting to a game", progname);
                msg("  -R      toggles raw mode");
#ifdef CLIENT_DEVEL
                msg("  -D      eneable developer informative output");
                msg("  -e<filename>  will use <filename> as a script for reading input");
                msg("  -i<filename>  will start %s with filename as an init file", progname);
                msg("  -V<version#>  for backwards compatibility");
                msg("  host    host site to connect to");
                msg("  nick    use this game entry to find for connection");
                msg("  port    port to connect on");
                msg("-- All of these command line arguments are optional.");
                quit_gb(-1, "", NULL, NULL);
            }
        }
    }

    /* Needs to be her in case of -f argument */
    if(allow_init_file) {
        set_display("on");
        load_predefined(gbrc_path);
        set_display("off");
    }

    while((argc > 0) && *argv) {
        if(**argv == '-') {
            continue;
        }

        env = strchr(*argv, '.');

        if(env == NULL) {
            /* It's a port */
            if(isdigit(**argv)) {
                strfree(cur_game.game.port);
                cur_game.game.port = string(*argv);
            }
            else {
                game = find_game(*argv);

                if(!game) {
                    strfree(cur_ganme.game.host);
                    cur_game.game.host = string(*argv);
                }
                else {
                    strfree(cur_game.game.port);
                    strfree(cur_game.game.host);
                    cur_game.game.host = string(game->host);
                    cur_game.game.port = string(game->port);

                    cur_game.game.type = string(game->type);
                    add_assign("type", cur_game.game.type);
                    cur_game.game.racename = string(game->racename);
                    add_assign("racename", cur_game.game.racename);
                    cur_game.game.govname = string(game->govname);
                    add_assign("govname", cur_game.game.govname);

                    strfree(cur_game.game.pripassword);
                    cur_game.game.pripassword = string(game->pripassword);
                    add_assign("pripassword", game->password);

                    strfree(cur_game.game.secpassword);
                    cur_game.game.secpassword = string(game->secpassword);
                    add_assign("secpassword", game->secpassword);

                    strfree(cur_game.game.nick);
                    cur_game.game.nick = string(game->nick);
                    add_assign("game_nick", game->nick);
                    SET_BIT(options, AUTOLOGIN);
                }
            }
        }
        else {
            /* It's a host */
            strfree(cur_game.game.host);
            cur_game.game.host = string(*argv);
        }

        --argc;
        ++argv;
    }

    /* No game found on command line, try defaults */
    if(!game && (cur_game.game.host == (char *)NULL)) {
        game = find_game("default");

        if(!game) {
            /* Nope, no game, just enter in edit mode */
            editclient = TRUE;
            strfree(cur_game.game.host);
            cur_game.game.host = string("");
        }
        else {
            strfree(cur_game.game.host);
            strfree(cur_game.game.port);

            cur_game.game.host = string(game->host);
            cur_game.game.port = string(game->port);

            cur_game.game.type = string(game->type);
            add_assign("type", cur_game.game.type);
            cur_game.game.racename = string(game->racename);
            add_assign("racename", cur_game.game.racename);
            cur_game.game.govname = string(game->govname);
            add_assign("govname", cur_game.game.govname);

            strfree(cur_game.game.pripassword);
            cur_game.game.pripassword = string(game->pripassword);
            add_assign("pripassword", game->pripassword);

            strfree(cur_game.game.secpassword);
            cur_game.game.secpassword = string(game->secpassword);
            add_assign("secpassword", game->secpassword);

            strfree(curgame.game.nick);
            cur_game.game.nick = string(game->nick);
            add_assign("game_nick", game->nick);
            SET_BIT(options, AUTOLOGIN);
        }
    }

    /* Set up the duplicate options so as to preserve GBRC info */
    if(GET_BIT(options, AUTOLOGIN_STARTUP)) {
        SET_BIT(options, AUTOLOGIN);
    }
    else {
        CLR_BIT(options, AUTOLOGIN);
    }

    if(GET_BIT(options, LOGINSUPPRESS_STARTUP)) {
        SET_BIT(options, LOGINSUPPRESS);
    }
    else {
        CLR_BIT(options, LOGINSUPPRESS);
    }

    /* We are initialized and booted, so let's go on! */
    set_display("on");
    client_stats = L_BOOTED;
    boot_time = time(0);

    /* Status bar */
    force_update_status();

    /* Clean input line */
    cancel_input();

#ifdef XMAP
    if(xmap_active) {
        /* XMAP set up the map window... */
        make_mwin();
    }
#endif

    /***** THE ABOVE IS INITIALIZATION. BELOW START THE MAIN I/O LOOP *****/
    msg("-- Galactic Bloodshed Client II version %s --", VERSION);
    msg("--            type 'helpc' for client help %s --", "           ");

#ifdef RESTRICTED_ACCESS
    msg("-- THE CLIENT IS RUNNING WITH RESTRICTED ACCESS        --");
    msg("-- type 'helpc restricted access' for more information --");
#endif

    /* Default to hide encrypted messages (confuses newbies) -mfw */
    SET_BIT(options, ENCRYPT);

    if(editclient) {
        msg("-- client running in editing mode.");
        msg("-- Use 'connect' to connect with a server.");
    }

#ifdef CLIENT_DEVEL
    if(client_devel) {
        msg(":: Developer output will be displayed.");
    }
#endif

#ifdef __hpux
    /* nfds = FOPEN_MAX */
    nfds = sysconf(_SC_OPEN_MAX);

#else

    nfds = getdtablesize();
#endif

    /*
     * Need this to accomodate some machines. Currently the client does not use
     * more than 9 fds at any one time.
     */
    if(nfds > 20) {
        nfds = 20;
    }

    if(!editclient) {
        add_assign("host", cur_game.game.host);
        add_assign("port", cur_game.game.port);
        msg("-- Connecting to: %s %s", cur_game.game.host, cur_game.game.port);

        /*
         * There was a problem if someone used a 'connect' directive in their
         * .gbrc file it would create an additional socket descriptor. This 'if'
         * statement makes sure we don't connect again if we're already
         * connected. (3/15/05 -mfw)
         */
        if(gb < 1) {
            gb = connectgb(cur_game.game.host, cur_game.game.port, "Connect: ");
        }

        debug(1, "main(): gbII connected, descriptor value: %d", gb);

        if(gb > 0) {
            add_assign("connected", "TRUE");
        }
    }
    else {
        /* Flagged for repeat_connect in gbs() */
        gb = -2;
    }

    last_no_logout_time = time(0);
    more_val.last_line_time = last_no_logout_time;
    gbs();

    if(gb >= 0) {
        close_gb();
    }

    quite_gb(0, GBSAYING, NULL, NULL);

    return 0;
}

/* Nitty gritty. Handles the choices between reading keyboard and socket */
void gbs(void)
{
    int count;
    DESCR_MASK rd;
    DESCR_MASK except;
    struct timeval refresh_time;
    struct timeval *timev;
    char buf[MAXSIZ];
    int num_commands;
    int q_cnt;

    while(!exit_now) {
        if(gb_close_socket) {
            ++gb_close_socket;
        }

#ifdef XMAP
        if(xmap_active) {
            /* XMAP code: check even queue */
            mwin_event_loop();
        }
#endif

        FD_ZERO(&rd);
        FD_ZERO(&except);
        FD_SET(0, &rd);
        num_commands = 0;

        if(gb > 0) {
            FD_SET(gb, &rd);
        }
        else if(gb != -2) {
            now = time(0);

            if(then == -1) {
                then = now;
            }
            else if(!exit_now
                    && GET_BIT(options, CONNECT)
                    && (now > (then + reconnect_delay))) {
                if(GBDT()) {
                    if(GET_BIT(options, AUTOLOGIN_STARTUP)) {
                        SET_BIT(options, AUTOLOGIN);
                    }

                    if(GET_BIT(options, LOGINSUPPRESS_STARTUP)) {
                        SET_BIT(options, LOGINSUPPRESS);
                    }
                }

                msg("-- Trying to reconnect.");
                gb = connectgb(cur_game.game.host, cur_game.game.port, "Reconnect: ");
                then = now;

                if(gb > 0) {
                    msg("-- Connection re-established");
                    then = -1;
                }
            }
        }

        /* Get time for various loops below */
        now = time(0);

        if(client_stats == L_LOGGEDIN) {
            init_start_commands(0);
            client_stats = L_INTERNALINIT;
            more_val.num_lines_scrolled = 0;
        }

#ifdef RWHO
        if((client_stats == L_ACTIVE)
           && rwho.on
           && ((rwho.last_time + RWHO_DELAY) < now)) {
            if(start_command(C_RWHO, 0)) {
                /* A small delay */
                rwho.last_time = now + 20;
            }
            else {
                rwho.last_time += 90;
            }
        }
#endif

        handle_loop();

        if((status.last_time + 60) < now) {
            force_update_status();
        }

#ifdef IMAP
        if((map_time != -1) && (map_time < now)) {
            redraw_sector();
            ping_current_sector();
        }

        if(input_mode.map) {
            refresh_time.tv_usec = REFRESH_TIME_IMAP;
        }
        else {
            refresh_time.tv_usec = REFRESH_TIME_NORM;
        }
#else

        refresh_time.tv_usec = REFRES_TIME_NORM;

#endif

        refresh_time.tv_sec = 0;
        timev = &refresh_time;

        count = select(nfds, &rd, NULL, &except, timev);

        if(count == -1) {
            if(errno != EINTR) {
                msg_error("-- Error in select:");
            }
        }
        else if(count > 0) {
            if((gb > 0) && FD_ISSET(gb, &rd)) {
                read_socket();
            }

            if(FD_ISSET(0, &rd) && !detached) {
                get_key();

                if(prompt_return) {
                    return;
                }
            }

            read_process(rd);
            check_process();
            fflush(stdout);
        }

        q_cnt = 0;

        if(have_socket_output() && !paused && (q_cnt < MAX_SOCKET_COMMANDS)) {
            get_socket();

            if(socket_return) {
                return;
            }

            ++q_cnt;
        }

        q_cnt = 0;

        while(check_queue() && (q_cnt <= MAX_QUEUE_COMMANDS)) {
            remove_queue(buf);
            process_queue(buf);

            if(do_clear_queue()) {
                clear_queue();
            }

            ++q_cnt;
        }

        check_no_logout();

        if(gb_close_socket == GB_CLOSE_SOCKET_DISPLAY) {
            close_gb();
            gb_close_socket = FALSE;

            if(quit_all) {
                exit_now = TRUE;
            }
        }
        else if((gb_close_socket == FALSE) && quit_all) {
            exit_now = TRUE;
        }

        fflush(stdout);
    } /* End while */
} /* End gb */

/*
 * This is a testing routine, which can be reached by binding a key to the bind
 * function client_test (i.e. bind ESC-t client-test). It is used for internal
 * testing and aiding in debugging.
 */
void test_client(void)
{
    extern void crypt_test(void);

    crypt_test();
    msg("-- done with client test.");
}

void init_race_colors(void)
{
    race_colors[0] = ANSI_BAC_RED;
    race_colors[1] = ANSI_BAC_GREEN;
    race_colors[2] = ANSI_BAC_YELLOW;
    race_colors[3] = ANSI_BAC_BLUE;
    race_colors[4] = ANSI_BAC_MAGENTA;
    race_colors[5] = ANSI_BAC_CYAN;
    race_colors[6] = ANSI_BAC_WHITE;
}
