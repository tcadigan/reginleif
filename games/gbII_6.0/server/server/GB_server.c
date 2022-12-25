/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it an/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILLITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * #ident  "%W% %G% %Q%"
 *
 * $Header: /var/cvs/gbp/GB+/server/GB_server.c,v 1.5 2007/07/07 17:13:25 gbp Exp $
 *
 * static char *ver = "@(#)        $RCSfile: GB_server.c,v $REVISION: 1.5 $";
 */
#include "GB_server.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <curses.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* GB headers */
#include "buffers.h"
#include "command.h"
#include "csp.h"
#include "csp_types.h"
#include "debug.h"
#include "doturn.h"
#include "game_info.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "tweakables.h"
#include "vars.h"

/* Data file descriptors */
int commoddata;
int pdata;
int racedata;
int sectdata;
int shdata;
int stdata;

extern long Shipdata[NUMSTYPES][NUMABILS];
extern int errno;
extern unsigned short free_ship_list;

/* Only use sys_errlist if it is defined in libc -mfw */
/* extern char *sys_errlist[]; */

int shutdown_flag = 0;
int update_flag = 0;
unsigned int datasz;
char *data;

unsigned int port; /* Port selection */
char start_buf[128];
char update_buf[128];
char segment_buf[128];

int chat_flag = DEFAULT_CHAT; /* CWL */
int chat_static = 1; /* CWL */
long size_of_words; /* CWL */
FILE *garble_file; /* CWL */

#ifdef ACCESS_CHECK
/* Have we read in the access control file? */
static int ainit =0;
static int naddesses = 0;

typedef struct access {
    struct in_addr ac_addr;
    int ac_cidr;
    int ac_value;
} ac_t;

static sc_t *ac_tab = (ac_t *)NULL;
#endif

/*
 * DO NOT change these two lines... They are client specific and are used in the
 * login procedure according to the CLIENT PROTOCOL file -- JPD --
 */
#ifndef CHAP_AUTH
static char const *client_login = "Enter Password <race> <gov>:\n";
static char const *bad_password = "Bad Password.\n";
#endif

static char const *client_invalid = "Invalid: ";
static char const *already_on = "Player already logged on!\n");
static char const *max_trys = "Too many bad passwords! See Ya!\n";
static char const *shutdown_message = "Shutdown ordered by deity - Bye\n";
static char const *out_of_fds = "No free file descriptors for connect!\n";
static char const *access_denied = "Unauthorized Access.\n";
static char const *lost_message = "<Output Lost>\n";

#define TIME_BOTH 0
#define TIME_UP 1
#define TIME_SEG 2

int errno;
time_t clk; /* Global clock */
time_t go_time; /* Global clock */
time_t boot_time /* Server startup time */

static int sockfd; /* Global socketfd */
static int ndescriptors = 0;

/* Prototypes */
void set_signals(void);
int send_special_string(int, int);
void d_think(int, int, char *, char *);
void d_broadcast(int, int, char *, char *, int);
void d_shout(int, int, int, char *, char *);
int command_loop(void);
void scheduled(void);
int whack_args(int);
void chomp_opts(void);
int stripem(char *);
int process_fd(int);
static Command *hash_search(char *);
int Login_Process(int, int, int);
int checkfds(void);
static void cleardes(int);
int readdes(int);
int shutdown_socket(int);
int connection(void);
static void refill_output(int);
void goodbye_user(int);
void do_update(int);
void do_segment(int, int);

#ifdef CHAP_AUTH
int Login_Parse(char *, char *, char *, char *, char *);

#else

int Login_Parse(char *, char *, char *);
#endif

void dump_users(int);
void dump_users_priv(int, int);
void boot_user(int, int);
void GB_time(int, int);
void compute_power_blocks(void);
void warn_race(int, char *);
void warn(int, int, char *);
void warn_star(int, int, int, char *);
void notify_star(int, int, int, int, char *);
void shut_game(void);
void voidpoint(void);
int clear_all_fds(void);
void _reset(int, int);
void _emulate(int, int);
void _schedule(int, int);
void last_logip(int, int);
void close_game(int, int);
void read_schedule_file(int, int);
void show_update(int, int);

#ifdef CHAP_AUTH
void client_key(char *);
#endif

#ifdef ACCESS_CHECK
int address_ok(struct sockadd_in *);
int address_match(struct in_add *, struct in_addr *, int);
void add_address(unsigned long, int, int);
#endif

/* Prototpyes */
const char *Desnames[] = {
    "ocean",
    "land",
    "mountainous",
    "gaseous",
    "ice",
    "forest",
    "desert",
    "plated",
    "wasted",
    "wormfield"
};

char const Dessymbols[] = {
    CHAR_SEA,
    CHAR_LAND,
    CHAR_MOUNT,
    CHAR_GAS,
    CHAR_ICE,
    CHAR_FOREST,
    CHAR_DESERT,
    CHAR_PLATED,
    CHAR_WASTED,
    CHAR_WORM
};

int main(int argc, char *argv[])
{
    int i;
    int c;
    struct stat stbuf;
    FILE *sfile;

    suspended = 0;
    game_closed = 0;
    update_time = DEFAULT_UPDATE_TIME;
    segments = MOVES_PER_UPDATE;
    port = GB_PORT;
    CombatUpdate = COMBAT_UPDATE;

    /* Parse command line arguments */
    c = getopt(argc, argv, "cf:hm:p:su:v?");

    while (c != -1) {
        switch(c) {
        case 'c':
            game_closed = 1;

            break;
        case 'f':
            CombatUpdate = atoi(optarg);

            break;
        case 'h':
        case '?':
            print_usage(argv[0]);

            exit(0);
        case 'm':
            segments = atoi(optarg);

            break;
        case 'p':
            port = atoi(optarg);

            break;
        case 's':
            suspended = 1;

            break;
        case 'u':
            update_time = atoi(optarg);

            break;
        case 'v':
            print_version();

            exit(0);
        default:
            printf("Invalid option. Use -h for help.\n");

            exit(0);
        }

        c = getopt(argc, argv, "cf:hm:p:su:v?");
    }

    create_pid_file();
    open_data_files();
    clk = time(0);
    boot_time = clk;

    backup_time = DEFAULT_BACKUP_TIME;
    next_backup_time = 0; /* So it backs up right when we start -mfw */

    clearlog(TRUE);
    sprintf(buf,
            "--------------------------------------------------------------");
    loginfo(ERRORLOG, NOERRNO, buf);
    loginfo(ERRORLOG, NOERRNO, "GB server started");
    loginfo(ERRORLOG, NOERRNO, buf);
    loginfo(ERRORLOG, NOERRO, "Running on Port %d\n", port);

    loginfo(USERLOG, NOERRNO, buf);
    loginfo(USERLOG, NOERRNO, "GB server started");
    loginfo(USERLOG, NOERRNO, buf);

    loginfo(UPDATELOG, NOERROR, buf);
    loginfo(UPDATELOG, NOERRNO, "GB server started");
    loginfo(UPDATELOG, NOERRNO, buf);
    loginfo(UPDATELOG, NOERRNO, "%d minutes between update", update_time);
    loginfo(UPDATELOG, NOERRNO, "%d segments/update", segments);

    next_update_time = clk + (update_time * 60);

    if (stat(UPDATEFL, &stbuf) >= 0) {
        sfile = fopen(UPDATEFL, "r");

        if (sfile != NULL) {
            char dum[32];

            if (fgets(dum, sizeof(dum), sfile)) {
                nupdates_done = atoi(dum);
            }

            if (fgets(dum, sizeof(dum), sfile)) {
                last_update_time = atol(dum);
            }

            if (fgets(dum, sizeof(dum), sfile)) {
                next_update_time = atol(dum);
            }

            if (fgets(dum, sizeof(dum), sfile)) {
                next_backup_time = atol(dum); /* -mfw */
            }

            if (fgets(dum, sizeof(dum), sfile)) {
                suspended = atoi(dum);
            }

            fclose(sfile);
        }

        if (suspended) {
            sprintf(update_buf,
                    "GAME SUSPENDED\nLast update %3d : %s",
                    nupdates_done,
                    ctime(&last_update_time));
        } else {
            sprintf(update_buf,
                    "Last update %3d : %s",
                    nupdates_done,
                    ctime(&last_update_time));
        }

        loginfo(UPDATELOG, NOERRNO, "%s", update_buf);
    }

    if (segments <= 1) {
        next_segment_time += (144 * 3600);
    } else {
        next_segment_time = clk + ((update_time * 60) / segments);

        if (stat(SEGMENTFL, &stbuf) >= 0) {
            sfile = fopen(SEGMENTFL, "r");

            if (sfile != NULL) {
                char dum[32];

                if (fgets(dum, sizeof(dum), sfile)) {
                    nsegments_done = atoi(dum);
                }

                if (fgets(dum, sizeof(dum), sfile)) {
                    last_segment_time = atol(dum);
                }

                if (fgets(sum, sizeof(dum), sfile)) {
                    next_segment_time = atol(dum);
                }

                fclose(sfile);
            }
        }

        if (next_segment_time < clk) {
            /* gvc */
            next_segment_time = next_update_time;
            nsegments_done = segments;
        }
    }

    sprintf(segment_buf,
            "Last Segment %2d : %s",
            nsegments_done,
            ctime(&last_segment_time));

    loginfo(UPDATELOG, NOERRNO, "%s", segment_buf);

    read_schedule_file(0, 0);

    loginfo(UPDATELOG,
            NOERRNO,
            "Next Update %3d : %s",
            nupdates_done + 1,
            ctime(&next_update_time));

    loginfo(UPDATELOG,
            NOERRNO,
            "Next Segment    : %s",
            ctime(&next_segment_time));

    /* CWL if USE_GARBLEFILE */
    if (USE_GARBLEFILE) {
        size_of_words = 0;

        loginfo(ERRORLOG, NOERRNO, "Counting %s size...\n", GARBLEFILE);

        garble_file = fopen(GARBLEFILE, "r");

        if (garble_file != NULL) {
            flock((int)garble_file, LOCK_EX);

            while (fgets(buf, 1024, garble_file)) {
                ++size_of_words;
            }
        } else {
            loginfo(ERRORLOG, NOERRNO, "Unable to open %s.\n", GARBLEFILE);
        }

        loginfo(ERRORLOG, NOERRNO, "%ld words counted.\n", size_of_words);

        if (garble_file) {
            rewind(garble_file);
        }
    }

    /* End CWL */

    datasz = (64 * 1024) + 1;
    data = (char *)malloc(datasz); /* Global line buffer space */

    if (data == NULL) {
        loginfo(ERRORLOG, WANTERRNO, "FATAL: Malloc error [main]");

        exit(-11);
    }

    load_race_data(); /* Make sure you do this first */
    load_star_data(); /* Get star data */
    Getpower(Power); /* Get power report from disk */
    Getblock(Blocks); /* Get alliance block data */
    SortShips(); /* Sort the ship list by tech for "build ?" */
    initFreeshipList(); /* Generate free ship list */

    for (i = 1; i <= MAXPLAYERS; ++i) {
        setbit(Blocks[i - 1].invite, i);
        setbit(Blocks[i - 1].pledge, i);
    }

    Putblock(Blocks);
    compute_power_blocks();
    set_signals();

    /* Set all the descriptors to available */
    for (i = 0; i < MAXPLAY_GOV; ++i) {
        actives[i].which_des = AVAILABLE;
        actives[i].idle_time = 0;
    }

    CSP_server_qsort();
    CSP_client_qsort();

    /* Set all the debug levels to 0 */
    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        des[i].descriptor = AVAILABLE;
        des[i].Debug = LEVEL_NONE;

        /*
         * I'm commenting the next two lines out because it seems to me that we
         * want to allocate when a connection occurs and free when the
         * connection drops. Not allocated them all as done here. I'm leaving
         * them here for now, for references. Search for 'desalloc' to see
         * related portions of the code in this file. -mfw
         */
        // des[i].input = (char *)malloc(INPUT_SIZE);
        // des[i].output = (char *)malloc(OUTPUT_SIZE);
    }

    if (init_network(port) == FAIL) {
        printf("init_network: Failure port=%d\n", port);
        close_data_files();
        loginfo(ERRORLOG, NOERRNO, "Going down. [init_network failure]");
        close_data_files();
        delete_pid_file();

        exit(3);
    }

    /* Loop until shutdown_flag is true */
    command_loop();

    /* We have the go-ahead for a shutdown */
    post(shutdown_message, ANNOUNCE);

    /* Code to shutdown all sockets */
    close_data_files();

    loginfo(ERRORLOG, NOERRNO, "Going down.");

    /*
     * Recover memory allocation for stars, this block of memory was originally
     * allocated in misc.c:load_star_data() -mfw
     */
    // free(*Stars);
    free(star_arena);
    free(planet_arena);

    /*
     * Recover memory allocated to races, this block of memory was originally
     * allocated in files_shl.c:getrace() -mfw
     */
    for (i = 1; i < Num_races; ++i) {
        free(races[i - 1]);
    }

    /*
     * Recover memory allocated to descriptors, this block of memory was
     * originally allocated in GB_server.c:main() desalloc -mfw
     */
    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        free(des[i].input);
        free(des[i].output);
    }

    free(data); /* -mfw */

#ifdef ACCESS_CHECK
    /* Recover memory allocated to the address access table */
    if (naddresses) {
        free(ac_tab);
    }
#endif

    if (garble_file) {
        fclose(garble_file);
    }

    delete_pid_file();

    return 0;
}

void print_usage(char *program)
{
    printf("Usage: %s [option]\n", program);
    printf("  -c    : Start in closed mode.\n");
    printf("  -f #  : Wait # updates until combat is allowed.\n");
    printf("  -h|-? : Print this message.\n");
    printf("  -m #  : # of moves (segments) per update.\n");
    printf("  -p #  : Server port number.\n");
    printf("  -s    : Start with updates suspended.\n");
    printf("  -u #  : Update interval (in minutes).\n");
    printf("  -v    : Print version (and exit).\n");
}

void print_version(void)
{
    printf("Galactic Bloodshed, Version %s\n", GB_VERSION);
    printf("Copyright (c) 1989-90 By Robert P. Chansky, et a.\n");
    printf("GB comes with ABSOLUTELY NO WARRANTY; this is free software,\n");
    printf("and you are welcome to redistribute it under certain conditions;\n");
    printf("see the LICENSE file for details.\n");
}

void set_signals(void)
{
    signal(SIGPIPE, SIG_IGN);
}

void notify_race(int race, char const *message)
{
    int i;

    if (!update_flag) {
        for (i = 0; i < MAXDESCRIPTORS; ++i) {
            if ((des[i].Playernum == race)
                && (des[i].descriptor != AVAILABLE)) {
                outstr(i, message);
                /* cleardes(i); */
            }
        }
    }
}

int notify(int player, int governor, char const *message)
{
    int i;
    int found = 0;

    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        if ((des[i].Playernum == player) && (des[i].Governor == governor)) {
            outstr(i, message);
            found = TRUE;
        }
    }

    return found;
}

int send_special_string(int player, int what)
{
    int gov;
    time_t clk;
    /* int csp_client = 0; */
    char message[24];
    int i;

    clk = time(0);

    for (gov = 0; gov <= 5; ++gov) {
        for (i = 0; i < MAXDESCRIPTORS; ++i) {
            if ((des[i].descriptor != AVAILABLE)
                && (des[i].Playernum == player)
                && (des[i].Governor == gov)) {
                /* cleardes(i); */

                if (races[des[i].Playernum - 1].governor[des[i].Governor]
                    .CSP_client_info.csp_user) {
                    switch (what) {
                    case UPDATE_START:
                        sprintf(message,
                                "%c %d\n",
                                CSP_CLIENT,
                                CSP_UPDATE_START);

                        break;
                    case UPDATE_END:
                        sprintf(message,
                                "%c %d %d\n",
                                CSP_CLIENT,
                                CSP_UPDATE_END,
                                nupdates_done);

                        break;
                    case SEGMENT_START:
                        sprintf(message,
                                "%c %d\n",
                                CSP_CLIENT,
                                CSP_SEGMENT_START);

                        break;
                    case SEGMENT_END:
                        sprintf(message,
                                "%c %d\n",
                                CSP_CLIENT,
                                CSP_SEGMENT_END);

                        break;
                    case RESET_START:
                        sprintf(message, "%c %d\n",
                                CSP_CLIENT,
                                CSP_RESET_START);

                        break;
                    case RESET_END:
                        sprintf(message,
                                "%c %d\n",
                                CSP_CLIENT,
                                CSP_RESET_END);

                        break;
                    case BACKUP_START:
                        sprintf(message,
                                "%c %d\n",
                                CSP_CLIENT,
                                CSP_BACKUP_START);

                        break;
                    case BACKUP_END:
                        sprintf(message,
                                "%c %d\n",
                                CSP_CLIENT,
                                CSP_BACKUP_END);

                        break;
                    case UPDATES_SUSPENDED:
                        sprintf(message,
                                "%c %d\n",
                                CSP_CLIENT,
                                CSP_UPDATES_SUSPENDED);

                        break;
                    case UPDATES_RESUMED:
                        sprintf(message,
                                "%c %d\n",
                                CSP_CLIENT,
                                CSP_UPDATES_RESUMED);

                        break;
                    }
                } else { /* CSP USER? */
                    switch (what) {
                    case UPDATE_START:
                        sprintf(message, "DOING UPDATE...\n");

                        break;
                    case UPDATE_END:
                        sprintf(message, "Update %d finished\n", nupdates_done);

                        break;
                    case SEGMENT_START:
                        sprintf(message, "DOING MOVEMENT...\n");

                        break;
                    case SEGMENT_END:
                        sprintf(message, "Segment finished\n");

                        break;
                    case RESET_START:
                        sprintf(message, "DOING RESET...\n");

                        break;
                    case RESET_END:
                        sprintf(message, "Finished with reset.\n");

                        break;
                    case BACKUP_START:
                        sprintf(message, "DOING BACKUP...\n");

                        break;
                    case BACKUP_END:
                        sprintf(message, "Finished with backup.\n");

                        break;
                    case UPDATES_SUSPENDED:
                        sprintf(message, "UPDATES SUSPENDED...\n");

                        break;
                    case UPDATES_RESUMED:
                        sprintf(message, "Updates resumed.\n");

                        break;
                    } /* switch */
                } /* else */

                outstr(i, message);

                /*
                 * We want this cleardes here to make sure the user sees the
                 * special string we are sending.
                 */
                cleardes(i);
            }
        }
    }

    return 1;
}

void d_think(int playernum, int governor, char *head, char *message)
{
    int i;

    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        if ((des[i].descriptor != AVAILABLE) && (des[i].Playernum == playernum)
            && (des[i].Governor == governor) && (des[i].Playernum != 0)
            && !races[des[i].Playernum - 1]->governor[des[i].Governor].toggle.gag) {
            if (client_can_understand(des_playernum, des_Governor, CSPD_THINK)) {
                char sbuf[20];
                sprintf(sbuf, "%c %d ", CSP_CLIENT, CSPD_THINK);
                outstr(i, sbuf);
            }

            outstr(i, head);
            outstr(i, message);
        }
    }
}

void d_broadcast(int playernum,
                 int governor,
                 char *head,
                 char *message,
                 int emote)
{
    int i;
    racetype *race;
    racetype *alien;
    int channel = 0;

    /* Get xmit channel -mfw */
    if (isset(races[playernum -1]->governor[governor].channel, COMM_XMIT_CHANNEL1)) {
        channel = 1;
    } else if (isset(races[playernum - 1]->governor[governor].channel, COMM_XMIT_CHANNEL2)) {
        channel = 2;
    } else if (isset(races[playernum - 1]->governor[governor].channel, COMM_XMIT_CHANNEL3)) {
        channel = 3;
    } else {
        notify(playernum, governor, "No xmit channel selected.\n");

        return;
    }

    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        if ((des[i].descriptor != AVAILABLE)
            && !((des[i].Playernum == playernum) && (des[i].Governor == governor))
            && (des[i].Playernum != 0)
            && !races[des[i].Playernum - 1]->governor[des[i].Governor].toggle.gag
            && isset(races[des[i].Playernum - 1]->governor[des[i].Governor].channel,
                     channel)) {
            if (emote && client_can_understand(des[i].Playernum, des[i].Governor, CSPD_EMOTE)) {
                char sbuf[20];
                sprintf(sbuf, "%c %d ", CSP_CLIENT, CSPD_EMOTE);
                outstr(i, sbuf);
            } else if (client_can_understand(des[i].Playernum, des[i].Governor, CSPD_BROADCAST)) {
                char sbuf[20];
                sprintf(sbuf, "%c %d ", CSP_CLIENT, CSPD_BROADCAST);
                outstr(i, sbuf);
            }

            race = races[playernum - 1];
            alien = races[des[i].Playernum - 1];

            if (!race->God && !alien->God && (chat_flag == TRANS_CHAT)) {
                garble_msg(message,
                           alien->translate[playernum - 1],
                           chat_static,
                           playernum);
            }

#ifdef MULTIPLE_COMM_CHANNELS
            /* Prepend channel number -mfw */
            sprintf(buf, "<%d> %s%s", channel, head, message);

#else
            sprintf(buf, "%s%s", head, message);
#endif

            outstr(i, buf);
        }
    }
}

void d_shout(int playernum, int governor, char *head, char *message)
{
    int i;

    for (i = 0; i < MASDESCRIPTORS; ++i) {
        if ((des[i].descriptor != AVIALABLE)
            && !((des[i].Playernum == playernum) && (des[t].Governor == governor))
            && (des[i].Playernum != 0)) {
            if (client_can_understand(des[i].Playernum, des[i].Governor, CSPD_SHOUT)) {
                char sbuf[20];

                sprintf(sbuf, "%c %d ", CSP_CLIENT, CSPD_SHOUT);
                outstr(i, sbuf);
            }

            outstr(i, head);
            outstr(i, message);
        }
    }
}

void d_announce(int playernum,
                int governor,
                int star,
                char *head,
                char *message)
{
    int i;
    racetype *race;
    racetype *alien;

    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        if ((des[i].descriptor != AVAILABLE)
            && !((des[i].Playernum == playernum) && (des[i].Governor == governor))
            && (des[i].Playernum != 0)
            && (isset(Stars[star]->inhabited, des[i].Playernum)
                || races[des[i].Playernum - 1]->God)
            && (Dir[des[i].Playernum - 1][des[i].Governor].snum == star)
            && !races[des[i].Playernum - 1]->governor[des[i].Governor].toggle.gag) {
            if (client_can_understand(des[i].Playernum, des[i].Governor, CSPD_ANNOUNCE)) {
                char sbuf[20];

                sprintf(sbuf, "%c %d ", CSP_CLIENT, CSPD_ANNOUNCE);
                outstr(i, sbuf);
            }

            race = races[Playernum - 1];
            alien = races[des[i].Playernum - 1];

            outstr(i, head);

            if (!race->god && !alien->God && (chat_flag == TRANS_CHAT)) {
                garble_msg(message,
                           alien->translate[Playernum - 1],
                           1,
                           playernum);
            }

            outstr(i, message);
        }
    }
}

/*
 * This is the top level parser. This checks all the descriptors and deals with
 * them. This also deals with the scheduled events (updates, segments)
 */
int command_loop(void)
{
    int busy;
    int which;
    int i;

    go_time = 0;

    if (!shutdown_flag) {
        post("Server started\n", ANNOUNCE);
    }

    for (i = 0; i <= ANNOUNCE; ++i) {
        newslength[i] = Newslength(i);
    }

    while (!shutdown_flag) {
        checkfds();
        time(&clk);

        busy = FALSE;

        for (which = 0; which < MAXDESCRIPTORS; ++which) {
            if (des[which].descriptor != AVAILABLE) {
                if (strlen(des[which].input) && !des[which].tame) {
                    debug(LEVEL_RAW, "Calling process_fd [des %d]\n", which);
                    process_fd(which);
                    busy = TRUE;
                }
            }
        }

        while (busy) {
            busy = FALSE;

            for (which = 0; which < MAXDESCRIPTORS; ++which) {
                if (des[which].descriptor != AVAILABLE) {
                    if (strlen(des[which].input) && !des[which].tame) {
                        debug(LEVEL_RAW,
                              "Calling process_fd [des %d]\n",
                              which);
                        process_fd(which);
                        busy = TRUE;
                    }
                }
            }
        }

        time(&clk);
        scheduled();
    }

    return 1;
}

/* This checks to see if we should kick off an update or segment */
void scheduled()
{
    if (go_time == 0) {
        if (clk >= next_update_time) {
            go_time = clk;
        }

        if ((clk >= next_segment_time) && (nsegments_done < segments)) {
            go_time = clk;
        }
    }

    if ((go_time > 0) && (clk >= go_time)) {
        if (nsegments_done < segments) {
            debug(LEVEL_GENERAL, "Calling do_segment\n");
            do_segment(0, 1);
        } else {
            debug(LEVEL_GENERAL, "Calling do_update\n");
            do_update(0);
        }

        go_time = 0;
    }

    /* Do scheduled backups -mfw */
    if ((next_backup_time > 0) && (clk >= next_backup_time)) {
        backup();
        next_backup_time = clk + (backup_time * 60);
    }

    if ((next_close_time > 0) && (clk >= next_close_time)) {
        game_closed = 1;
    }

    if ((next_open_time > 0) && (clk >= next_open_time)) {
        game_closed = 0;
    }

    if ((next_shutdown_time > 0) && (clk >= next_shutdown_time)) {
        shut_game();
    }
}

/*
 * This parses out the input stream from the physical descriptor into the
 * args[][] arrays.
 */
int whack_args(int which) {
    int quit = 0;
    int i;
    char *cp = data;

    argn = 0;
    stripem(cp);

    while (!quit) {
        i = 0;

        while (!isspace((unsigned char)*cp)
               && (*cp != '\0')
               && (i < COMMANDSIZE)) {
            args[argn][i] = *cp;
            ++i;
            ++cp;
        }

        args[argn][i] = '\0';

        while (*cp == ' ') {
            ++cp;
        }

        if ((*cp == '\0') || (*cp == '\n') || (argn >= MAXARGS)) {
            *cp = '\0';
            quit = TRUE;
        }

        ++argn;
    }

    for (i = argn; i < MAXARGS; ++i) {
        args[i][0] = '\0';
    }

    debug(LEVEL_COMMAND,
          "what_args: argn is [%d] fd: %d\n",
          argn,
          des[which].descriptor);

    return 1;
}

void chomp_opts(void)
{
    int count = argn;
    int pos = 0;
    int i;
    unsigned int idx;

    /* Clear and initialize */
    for (i = 0; i < MAXOPTS; ++i) {
        opts[i] = 0;
    }

    optn = 0;

    if (*args[pos] == '-') {
        if (args[pos][1]) {
            for (i = 1; args[pos][i] != '\0'; ++i) {
                idx = (unsigned int)args[pos][i];

                if (!opts[idx]) {
                    opts[idx] = 1;
                    ++optn;
                }

                if (isdigit(args[pos][i + 1])) {
                    opts[idx] = atoi(&args[pos][i + 1]);
                }
            }

            /* Move args */
            for (i = pos; i < argn; ++i) {
                strcpy(args[i], args[i + 1]);
            }

            --argn;
        }
    } else {
        ++pos;
    }

    --count;

    while (count) {
        if (*args[pos] == '-') {
            if (args[pos][1]) {
                for (i = 1; args[pos][i] != '\0'; ++i) {
                    idx = (unsigned int)args[pos][i];

                    if (!opts[idx]) {
                        opts[idx] = 1;
                        ++optn;
                    }

                    if (isdigit(args[pos][i + 1])) {
                        opts[idx] = atoi(&args[pos][i + 1]);
                    }
                }

                /* Move args */
                for (i = pos; i < argn; ++i) {
                    strcpy(args[i], args[i + 1]);
                }

                --argn;
            }
        } else {
            ++pos;
        }

        --count;
    }

    debug(LEVEL_COMMAND, "chomp_opts: optn is [%d]\n", optn);
}

int stripem(char *c)
{
    int i;

    for (i = 0; i < strlen(c); ++i) {
        if ((c[i] == '\n') || (c[i] == '\r')) {
            c[i] = 0;
        } else if (!isprint((unsigned char)c[i])
                   || !isascii((unsigned char)c[i])) {
            printf("Trapped %d\n", c[i]);
            c[i] = ' ';
        }
    }

    return 1;
}

/*
 * This is the second level parser. It will deal with new logins, as well as a
 * few of the most used commands. It will then call a third tier parser for all
 * the users commands.
 */
int process_fd(int which)
{
    int login_mode;
    int player = 0;
    int governor = 0;
    int happy = FALSE;
    int allowed = FALSE;
    char *string = NULL;
    char *ptr;
    char *message;
    char rank[9];
    Command *handler;

#ifdef CHAP_AUTH
    char race_name[MAX_COMMAND_LEN];
    char gov_name[MAX_COMMAND_LEN];
    char client_hash[MAX_COMMAND_LEN];
    char code[MAX_COMMAND_LEN];

#else

    char race_password[MAX_COMMAND_LEN];
    char gov_password[MAX_COMMAND_LEN];
#endif

    login_mode = !des[which].Playernum;
    memset(data, 0, datasz);

    if (login_mode) {
        string = (char *)malloc(INPUT_SIZE);
        strcpy(string, des[which].input);
    }

    ptr = (char*)srtchr(des[which].input, '\n');

    if (!ptr) {
        des[which].tame = TRUE;

        if (login_mode) {
            free(string);
        }

        return 0;
    }

    des[which].tame = FALSE;
    *ptr = 0;
    memcpy(data, des[which].input, strlen(des[which].input) + 1);
    message = data;

    while (*message && (*message != ' ')) {
        ++message;
    }

    if (ptr) {
        /* Changed to memmove because memcpy causes overlap -mfw */
        memmove(des[which].input, ptr + 1, strlen(ptr + 1) + 1);
        des[which].tame = TRUE;
    } else {
        /* NOTREACHED if !ptr above returns. */
        /* des[which].input = 0 */
        memcpy(data, des[which].input, sizeof(des[which].input));
    }

    whack_args(which);
    chomp_opts();

    while (1) {
        if (args[0] && login_mode) {
            debug(LEVEL_LOGIN,
                  "process_fd: In login mode fd: %d\n",
                  des[which].descriptor);

#ifdef CHAP_AUTH
            if (Login_Parse(string, race_name, gov_name, client_hash, code)) {
                if (!strcmp(code, "ABORT")) {
                    outstr(which, "Aborting login...\n");
                    debug(LEVEL_LOGIN,
                          "procesS_fd: login mode abort [fail] fd: %d\n",
                          des[which].descriptor);
                    cleardes(which); /* -mfw */
                    shutdown_socket(which);
                    happy = TRUE;

                    break;
                }

                if (strcmp(code, "RESPONSE")
                    || !Getracenum(race_name, gov_name, &player, &governor, client_hash, which)) {
                    ++des[which].Trys;

                    if (des[which].Trys > MAXLOGIN_ATTEMPTS) {
                        outstr(which, max_trys);
                        debug(LEVEL_LOGIN,
                              "process_fd: login mode exit [fail] fd: %d\n",
                              des[which].descriptor);
                        cleardes(which); /* -mfw */
                        shutdown_socket(which);
                        happy = TRUE;
                    } else {
                        sprintf(buf, "CHAP FAILURE\n");
                        outstr(which, buf);
                        client_key(des[which].key);
                        sprintf(buf, "CHAP CHALLENGE %s\n", des[which].key);
                        outstr(which, buf);
                        loginfo(USERLOG,
                                NOERROR,
                                "FAILED    : D%02d        %-12.12s %-10.10s %s\n",
                                des[which].descriptor,
                                race_name,
                                gov_name,
                                addrout(des[which].Hose));
                        debug(LEVEL_LOGIN,
                              "process_fd: login mode exit [fail] fd: %d\n",
                              des[which].descriptor);
                        happy = TRUE;
                    } /* MAXLOGIN_ATTEMPTS */

                    break;
                } else {
                    /* Login OK! */
                    sprintf(buf, "CHAP SUCCESS\n");
                    outstr(which, buf);

                    if (game_closer && (player != 1)) {
                        if (next_open_time) {
                            char tbuf[20];
                            sprintf(tbuf, "%ld", next_open_time);
                            sprintf(buf,
                                    "Temporarily Close, will reopen %s%-10s%s\n",
                                    OPEN_TIME_TAG,
                                    tbuf,
                                    CLOSE_TIME_TAG);
                        } else {
                            sprintf(buf, "Temporarily Closed.\n");
                        }

                        outstr(which, buf);
                        cleardes(which); /* -mfw */
                        shutdown_socket(which);

                        break;
                    } else {
                        Login_Process(player, governor, which);
                        debug(LEVEL_LOGIN,
                              "process_fd: login mode exit [success] fd: %d\n",
                              des[which].descriptor);
                        happy = TRUE;

                        break;
                    }
                }
            } else {
                /* Reached if Login_Parse() fails */
                break;
            }

#else

            Login_Parse(string, race_password, gov_password);

            if (!strcmp(race_password, "quit")) {
                outstr(which, "Aborting login...\n");
                debug(LEVEL_LOGIN,
                      "process_fd: login mode abort [fail] fd: %d\n",
                      des[which].descriptor);
                cleardes(which); /* -mfw */
                shutdown_socket(which);
                happy = TRUE;

                break;
            }

            if (!Getracenum(race_password, gov_password, &player, &governor)) {
                ++des[which].Trys;

                if (des[which].Trys > MAXLOGIN_ATTEMPTS) {
                    outstr(which, max_trys);
                    debug(LEVEL_LOGIN,
                          "process_fd: login mode exit [fail] fd: %d\n",
                          des[which].descriptor);
                    cleardes(which); /* -mfw */
                    shutdown_socket(which);
                    happy = TRUE;
                } else {
                    sprintf(buf, "%s %s", client_invalid, bad_password);
                    outstr(which, buf);

                    sprintf(buf, "Re-%s", client_login);
                    outstr(which, buf);

                    loginfo(USERLOG,
                            NOERRNO,
                            "Failed    : D%02d        %-10.10s %-12.12s %s\n",
                            des[which].descriptor,
                            race_password,
                            gov_password,
                            addrout(des[which].Host));

                    debut(LEVEL_LOGIN,
                          "process_fd: login mode exit [fail] fd: %d\n",
                          des[which].descriptor);
                    happy = TRUE;
                } /* MAXLOGIN_ATTEMPTS */

                break;
            } else {
                /* Login OK! */

                if (game_closed && (player != 1)) {
                    if (next_open_time) {
                        char tbuf[20];

                        sprintf(tbuf, "%ld", next_open_time);
                        sprintf(buf,
                                "Temporarily Close, will reopen %s%-10s%s\n",
                                OPEN_TIME_TAG,
                                tbuf,
                                CLOSE_TIME_TAG);
                    } else {
                        sprintf(buf, "Temporarily Closed.\n");
                    }

                    outstr(which, buf);
                    cleardes(which); /* -mfw */
                    shutdown_socket(which);

                    break;
                } else {
                    Login_Process(player, governor, which);
                    debug(LEVEL_LOGIN,
                          "procesS_fd: login mode exit [success] fd: %d\n",
                          des[which].descriptor);
                    Happy = TRUE;

                    break;
                }
            }
        }
#endif
        /* Only reached if login_mode is False. */
        if (!strcmp(args[0], "")) {
            Happy = TRUE;

            break;
        } else if (match(args[0], CSP_SERVER)) {
            debug(LEVEL_CSP,
                  "process_fd: PRocessing CSP command fd: %d\n",
                  des[which].descriptor);
            CSP_process_command2(des[which].Playernum, des[which].Governor);
            Happy = TRUE;

            break;
        } else if(match2(args[0], "BRoadcast", 2) || match(args[0], "'")) {
            debug(LEVEL_HANDLER,
                  "process_fd: Processing broadcast command fd: %d\n",
                  des[which].descriptor);
            announce(des[which].Playernum,
                     des[which].Governor,
                     message,
                     BROADCAST,
                     0);
            Happy = TRUE;

            break;
        } else if (match(args[0], "think") || match(args[0], "`")) {
            debug(LEVEL_HANDLER,
                  "process_fd: Processing think command fd: %d\n",
                  des[which].descriptor);
            announce(des[which].Playernum,
                     des[which].Governor,
                     message,
                     THINK,
                     0);
            Happy = TRUE;

            break;
        } else if (match2(args[0], "SHOut", 3)) {
            debug(LEVEL_HANDLER,
                  "process_fd: Processing shout command fd: %d\n",
                  des[which].descriptor);
            announce(des[which].Playernum,
                     des[which].Governor,
                     message,
                     SHOUT,
                     0);
            Happy = TRUE;

            break;
        } else if (match2(args[0], "ANnounce", 2) || match(args[0], ":")) {
            debug(LEVEL_HANDLER,
                  "process_fd: Processing announce command fd: %d\n",
                  des[which].descriptor);
            announce(des[which].Playernum,
                     des[which].Governor,
                     message,
                     ANN,
                     0);
            Happy = TRUE;

            break;
        } else if (match2(args[0], "EMOte", 3) || match(args[0], ";")) {
            debug(LEVEL_HANDLER,
                  "process_fd: Processing emote command fd: %d\n",
                  des[which].descriptor);
            announce(des[which].Playernum,
                     des[which].Governor,
                     message,
                     EMOTE,
                     0);
            Happy = TRUE;

            break;
        } else if (match(args[0], QUIT_COMMAND)) {
            debug(LEVEL_HANDLER,
                  "process_fd: Processing quit command fd: %d\n",
                  des[which].descriptor);
            goodbye_user(which);
            Happy = TRUE;

            break;
        } else if (match2(args[0], "MOTto", 3)) {
            debug(LEVEL_HANDLER,
                  "process_fd: Processing motto command fd: %d\n",
                  des[which].descriptor);
            motto(des[which].Playernum, des[which].Governor, 0, messagE);
            Happy = TRUE;

            break;
        } else if (match(args[0], "personal")) {
            debug(LEVEL_HANDLER,
                  "process_fd: Processing personal command fd: %d\n",
                  des[which].descriptor);
            personal(des[which].Playernum, des[which].Governor, message);
            Happy = TRUE;

            break;
        } else if (match(args[0], WHO_COMMAND) && !login_mode) {
            debug(LEVEL_HANDLER,
                  "process_fd: Processing who command fd: %d\n",
                  des[which].descriptor);
            dump_users(which);
            Happy = TRUE;
            break;
        } else if (match(args[0], HELP_COMMAND)) {
            debug(LEVEL_HANDLER,
                  "process_fd: Processing help command fd: %d\n",
                  des[which].descriptor);
            help(which);
            Happy = TRUE;

            break;
        } else if (!login_mode) {
            handler = hash_search(args[0]);

            if (handler == NULL) {
                debug(LEVEL_HANDLER,
                      "process_fd: hash_search failed for \'%s\' command fd: %d\n",
                      args[0],
                      des[which].descriptor);
                Happy = FALSE;
            } else {
                debug(LEVEL_HANDLER,
                      "process_fd: Processing %s command fd: %d\n",
                      handler->name,
                      des[which].descriptor);

                if ((handler->God && !des[which].God)
                    || (handler->Guest && des[which].Guest)) {
                    outstr(which, "That is a restricted command.\n");
                    Happy = TRUE;

                    break;
                }

                if (des[which].Governor == 0) {
                    allowed = TRUE;
                } else if (races[des[which].Playernum - 1]->governor[des[which].Governor].rank <= handler->Rank) {
                    allowed = TRUE;
                } else {
                    allowed = FALSE;
                }

                if (!allowed) {
                    switch (handler->Rank) {
                    case LEADER:
                        strcpy(rank, "leader");

                        break;
                    case GENERAL:
                        strcpy(rank, "general");

                        break;
                    case CAPTAIN:
                        strcpy(rank, "captain");

                        break;
                    case PRIVATE:
                        strcpy(rank, "private");

                        break;
                    case NOVICE:
                        strcpy(rank, "novice");

                        break;
                    }

                    sprintf(buf,
                            "The \"%s\" command requires a minimum level of %s to run.\n",
                            handler->name,
                            rank);
                    outstr(which, buf);
                    Happy = TRUE;

                    break;
                }

                if (!handler->Args) {
                    debug(LEVEL_HANDLER,
                          "process_fd: Calling %s with no Args\n",
                          handler->name);
                    handler->func(des[which].Playernum,
                                  des[which].Governor,
                                  handler->APCost);
                } else if (handler->args == 1) {
                    debug(LEVEL_HANDLER,
                          "process_fd: Calling %s with Args %d [%d]\n",
                          handler->name,
                          des[which].descriptor,
                          handler->Args,
                          handler->Arg1);
                    handler->func(des[which].Playernum,
                                  des[which].Governor,
                                  handler->APCost,
                                  handler->Arg1);
                } else if (handler->Args == 2) {
                    debug(LEVEL_HANDLER,
                          "process_fd: Calling %s with Args %d [%d %d]\n",
                          handler->name,
                          des[which].descriptor,
                          handler->Args,
                          handler->Arg1,
                          handler->Arg2);
                    handler->func(des[which].Playernum,
                                  des[which].Governor,
                                  handler->APCost,
                                  handler->Arg1,
                                  handler->Arg2);
                } else {
                    loginfo(ERRORLOG, WANTERRNO, "Error in handler\n");

                    /* exit(); */
                }

                Happy = TRUE;
            }
        }

        if (!Happy && !login_mode) {
            sprintf(buf,
                    "\'%s\': Illegal command error (%d).\n",
                    args[0],
                    *args[0]);
            outstr(which, buf);
        }

        break;
    }

    /* Prevents doing work on an freed descriptor */
    if (des[which].descriptor != AVAILABLE) {
        if (!login_mode) {
            actives[((des[which].Playernum - 1) * 5) + des[which].Governor + 1].idle_time = time(0);
            do_prompt(des[which].Playernum, des[which].Governor);
            sprintf(buf,
                    "%s",
                    Dir[des[which].Playernum - 1][des[which].Governor].prompt);
            notify(des[which].Playernum, des[which].Governor, buf);
        }

        if (!des[which].input) {
            des[which].tame = TRUE;
        } else {
            des[which].tame = FALSE;
        }
    }

    if (login_mode) {
        free(string);
    }

    return 1;
}

/*
 * hash_search expect for the command.h to have commands starting with @ or a
 * lower case letter. It creates a table which points to the first occurance of
 * each group. command.h must have all of the @commands first followed by all of
 * the a commands through the z commands. Within each group you can put the
 * commands in whatever order you want. This will affect things such as whether
 * 'rep' will do 'report' or 'repair'.
 */
static Command *hash_search(char *)
{
    static int hash_table[27] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1
    };
    int hash_entry;
    int i;
    static int hashed = 0;

    /*
     * The first time this function gets called we'll build the hash table
     * search backwards through the command_list to find the first occurance of
     * each group.
     */
    if (!hashed) {
        for (i = NUM_COMMANDS - 1; i >= 0; --i) {
            hash_entry = command_list[i].name[0] - 96;

            if ((hash_entry < 0) || (hash_entry > 26)) {
                hash_entry = 0;
            }

            hash_table[hash_entry] = i;
        }

        hashed = 1;
    }

    /*
     * This will only work for commands starting with @ or a lower case letter
     * of the alphabet. You may want to change it.
     */
    hash_entry = cmd[0] - 96;

    if (cmd[0] == '@') {
        hash_entry = 0;
    }

    if ((hash_entry < 0)
        || (hash_entry > 26)
        || (hash_table[hash_entry] == -1)) {
        return NULL;
    }

    i = hash_table[hash_entry];

    while (cmd[0] == command_list[i].name[0]) {
        if (!strncmp(cmd, command_list[i].name, strlen(cmd))) {
            return (Command *)&command_list[i];
        } else {
            ++i;

            if (i >= NUM_COMMANDS) {
                return NULL;
            }
        }
    }

    return NULL;
}

/*
 * This deals with all the login specific stuff. To check if they are logged in
 * already, etc... It also prints out the preliminary info when you login.
 */
int Login_Process(int player, int governor, int which)
{
    /*
     * At this point, they have entered a valid password, just check to make
     * sure they are not logged on already
     */
    racetype *r;

    r = races[player - 1];

    loginfo(USERLOG,
            NOERRNO,
            "CHECKING bit %d on D%d\n",
            ((player * 5) + governor) + 1,
            des[which].descriptor);

    if (actives[(((player - 1) * 5) + governor) + 1].which_des != AVAILABLE) {
        sprintf(buf, "%s %s\n", client_invalid, already_on);
        outstr(which, buf);
        cleardes(which); /* -mfw */
        shutdown_socket(which);

        return 0;
    }

    /* OK, so player isn't logged on, log him/her on */
    sprintf(buf, "\"%s\"", r->governor[governor].name);

    loginfo(USERLOG,
            NOERRNO,
            "CONNECTED: D%02d [%02d,%d] %-10.10s %-12.12s%s\n",
            des[which].descriptor,
            player,
            governor,
            r->name,
            buf,
            addrout(des[which].Host));

    des[which].Playernum = player;
    des[which].Governor = governor;
    des[which].God = r->God;
    des[which].Guest = r->Guest;

    loginfo(USERLOG,
            NOERRNO,
            "SETTING bit %d on D%d\n",
            (des[which].Playernum * 5) + des[which].Governor + 1,
            des[which].descriptor);

    actives[((des[which].Playernum - 1) * 5) + dest[which].Governor + 1].which_des = des[which].descriptor;

    des[which].Active = TRUE;
    print_motd(which);

    sprintf(buf,
            "\n%s \"%s\" [%d,%d logged on.\n",
            r->name,
            r->governor[governor].name,
            player,
            governor);

    notify_race(des[which].Playernum, buf);

    if (r->governor[governor].toggle.invisible) {
        sprintf(buf, "You are invisible.\n");
    } else {
        sprintf(buf, "You are visible.\n");
    }

    outstr(which, buf);
    GB_time(player, governor);

    if (r->governor[governor].CSP_client_info.csp_user == 1) {
        char tbuf[20];

        sprintf(tbug, "%ld", r->governor[governor].last_login);
        sprintf(buf,
                "Last login      : %s%-10s%s\n",
                OPEN_TIME_TAG,
                tbuf,
                CLOSE_TIME_TAG);
        outstr(which, buf);
    } else {
        sprintf(buf,
                "Last login      : %s",
                ctime(*r->governor[governor].last_login));
        outstr(which, buf);
    }

    if (strlen(r->governor[governor].last_ip) > 0) {
        sprintf(buf, "From host      : %s\n\n", r->governor[governor].last_ip);
        outstr(which, buf);
    }

    /* Yuck... */
    /* sprintf(buf, "\0"); */
    buf[0] = '\0';

    r->governor[governor].last_login = time(0);
    actives[((des[which].Playernum - 1) * 5) + des[which].Governor + 1].connect_Time = time(0);
    actives[((des[which].Playernum - 1) * 5) + des[which].Governor + 1].idle_time = time(0);
    strcpy(r->governor[Governor].last_ip, addrout(des[which].Host));

    putrace(r);

    /* Check to see if we have a @ to produce AP's */
    if (!r->Gov_ship) {
        strcat(buf,
               "You have no Governmental Center. No action points will be produced\nuntil you build one and designate a captial.\n");
        outstr(which, buf);
    } else {
        sprintf(buf, "Government Center #%d is active.\n", r->Gov_ship);
        outstr(which, buf);
    }

    sprintf(buf, "     Morale: %ld\n\n", r->morale);
    outstr(which, buf);

    check_for_telegrams(des[which].Playernum, des[which].Governor);
    check_dispatch(des[which].Playernum, des[which].Governor);

    /* Always have one channel on at login, set default channel */
    channel(des[which].Playernum, des[which].Governor, 0, 1);

    /* Change scope to our default system */
    argn = 1;
    strcpy(args[0], "cs");
    cs(des[which].Playernum, des[which].Governor, 0);

    return 1;
}

/*
 * This is the low-level descriptor checking routine. It check each des for
 * input or output and sets a bit if needed.
 */
int checkfds()
{
    int dnum;
    int maxfd;
    struct timeval timeout;
    fd_set input_set;
    fd_set output_set;
    int waitseconds;
    time_t now;
    time_t realidle; /* rjn */

    time(&now);

    /*
     * Loop through the descriptors looking for input or output
     */
    for (dnum = 0; dnum < MAXDESCRIPTORS; ++dnum) {
        if (des[dum].descriptor != AVAILABLE) {
            /* Did we pass the timelimit for EWOULDBLOCK's? */
            if (now >= des[dnum].lastblock) {
                if (des[dnum].output_size) {
                    FD_SET(des[dnum].descriptor, &output_set);
                }

                /*
                 * If nothing waiting to be output, then look for input to be
                 * read.
                 */
                if (!strlen(des[dnum].output_overflow_file)
                    && (des[dnum].output_size == 0)) {
                    FD_SET(des[dnum].descriptor, &input_set);
                }

                if (des[dnum].descriptor > maxfd) {
                    maxfd = des[dnum].descriptor;
                }
            } else {
                /* Still waiting... */
                if ((des[dnum].lastblock - now) < waitseconds) {
                    waitseconds = des[dnum].lastblock - now;
                }
            }

            /*
             * End of loop of 'actives' - check timeout rjn
             *
             * This is not quite right because at logon time it takes one cycle
             * to establish the correct idle time -- so if we have 0 in the idle
             * time slot don't do anything
             */
            realidle = actives[((des[dnum].Playernum - 1) * 5) + des[dnum].Governor + 1].idle_time;

            if (((now - realidle) > (60 * DISCONNECT_TIME)) && (realidle > 0)) {
                cleardes(dnum); /* -mfw */
                shutdown_socket(dnum);
            }
        }
    }
    if (waitseconds <= 0) {
        seconds = 1;
    }

    timeout.tv_sec = waitseconds;
    timeout.tv_usec = 0;

    dnum = select(maxfd + 1, &input_set, &output_set, 0, &timeout);

    if (dnum < 0) {
        perror("select");
        dnum = 0;
    }

    if (dnum) {
        /* Select says we have something */
        if (FD_ISSET(sockfd, &input_set)) {
            /* New connection pending */
            connection();
        }

        for (dnum = 0; dnum < MAXDESCRIPTORS; ++dnum) {
            if (des[dnum].descriptor != AVAILABLE) {
                if (FD_ISSET(des[dnum].descriptor, &output_set)) {
                    cleardes(dnum);
                }

                if (FD_ISSET(des[dnum].descriptor, &input_set)) {
                    readdes(dnum);
                }
            }
        }
    }

    return 1;
}

/* This writes out anything lingering around on the fd */
static void cleardes(int whichfd)
{
    int ret;
    struct descrip *dp = &des[whichfd];

    if (dp->output_size <= 0) {
        return;
    }

    ret = writefd(whichfd, &dp->output[dp->output_start], dp->output_size);

    if (ret > 0) {
        dp->output_start += ret;
        dp->output_size -= ret;

        if (dp->output_size == 0) {
            dp->output_start = 0;

            if (strlen(dp->output_overflow_file)) {
                refill_output(whichfd);
            }
        }
    }
}

/* This reads anything off the respective fd. */
int readdes(int whichfd)
{
    int ret;

    ret = readfd(des[whichfd].descriptor,
                 des[whichfd].input + strlen(des[whichfd].input),
                 INPUT_SIZE - strlen(des[whichfd].input) - 2);
    des[whichfd].tame = FALSE;

    if (red <= 0) {
        cleardes(whichfd); /* -mfw */
        shutdown_socket(whichfd);
    }

    return 1;
}

/* This shutdown the respective socket, logging info in the log files */
int shutdown_socket(int whichfd)
{
    racetype *r;
    /* int i; */

    if (whichfd == -1) {
        loginfo(ERRORLOG, NOERRNO, "Shutting down socket with -1\n");
    }

    debug(LEVEL_LOGIN,
          "shutdown_socket: shutting down socket %d\n",
          des[whichfd].descriptor);

    if (des[whichfd].Active) {
        /* We were connected */
        des[whichfd].Active = FALSE;
        r = races[des[whichfd].Playernum - 1];
        sprintf(buf, "\"%s\"", r->governor[des[whichfd].Governor].name);

        loginfo(USERLOG,
                NOERRNO,
                "DISCONNECT: D%02d [%02d,%01d] %-10.10s %-12.12s %s\n",
                des[whichfd].descriptor,
                des[whichfd].Playernum,
                des[whichfd].Governor,
                r->name,
                buf,
                addrout(des[whichfd].Host));

        loginfo(USERLOG,
                NOERRNO,
                "CLEARING bit %d D%d\n",
                (des[whichfd].Playernum * 5) + des[whichfd].Governor + 1,
                whichfd);

        debug(LEVEL_LOGIN,
              "shutdown_socket: clearing active bit %d\n",
              (des[whichfd].Playernum * 5) + 1);

        actives[((des[whichfd].Playernum - 1) * 5) + des[whichfd].Governor + 1].which_des = AVAILABLE;

        sprintf(buf,
                "\n%s \"%s\" [%ld,%ld] logged off.\n\n",
                r->name,
                r->governor[des[whichfd].Governor].name,
                des[whichfd].Playernum,
                des[whichfd].Governor);

        notify_race(des[whichfd].Playernum, buf);
        memset(buf, 0, sizeof(buf));
        cleardes(whichfd);
    } else {
        /* Nobody connected */
        if (des[whichfd].descriptor != AVAILABLE) {
            loginfo(USERLOG,
                    NOERRNO,
                    "DISCONNECT: D%02d [--,-]                         %s\n",
                    des[whichfd].descriptor,
                    addrout(des[whichfd].Host));
        }
    }

    des[whichfd].Active = FALSE;

    if (des[whichfd].overflow_fd) {
        fclose(des[whichfd].overflow_fd);
        unlink(des[whichfd].output_overflow_file);
        strcpy(des[whichfd].output_overflow_file, "");
        des[whichfd].overflow_fd = NULL;
    }

    shutdown(des[whichfd].descriptor, 2);
    close(des[whichfd].descriptor);

    if (des[whichfd].input) {
        // desalloc
        free(des[whichfd].input);
        des[whichfd].input = NULL;
    }

    if (des[whichfd].output) {
        // desalloc
        free(des[whichfd].output);
        des[whichfd].output = NULL;
    }

    des[whichfd].descriptor = AVAILABLE;
    des[whichfd].Playernum = 0;
    des[whichfd].Governor = 0;
    des[whichfd].Debug = 0;
    des[whichfd].Trys = 0;
    --ndescriptors;

    return 1;
}

/* Lowest level read. This does the ACTUAL read of the fd */
int readfd(int whichfd, char *buffer, unsigned int howmuch)
{
    int numread;
    /* char *q; */
    /* char *qend; */

    numread = read(whichfd, buffer, howmuch);

    if (numread <= 0) {
        return numread;
    }

    /* Null terminate */
    buffer[numread] = 0;

    return numread;
}

/* Lowest level write. This does the ACTUAL write of the fd. */
int writefd(int whichfd, char *buffer, unsigned int howmuch)
{
    int numwritten;
    struct descript *dp = &des[whichfd];

    numwritten = write(dp->descriptor, buffer, howmuch);

    if (numwritten <= 0) {
        if (errno == EWOULDBLOCK) {
            time(&dp->lastblock);

            /*
             * Don't even *think* about doing anything with this socket for at
             * least 4 seconds.
             */
            dp->lastblock += 4;

            return numwritten;
        } else {
            shutdown_socket(whichfd);

            return numwritten;
        }
    }

    return numwritten;
}

/* Handles new connections */
int connection()
{
    struct sockaddr_in newuser;
    int newuser_len;
    int a;
    int opt = 0;
    int avail;

#ifdef CHAP_AUTH
    char keyout[33];
#endif

    newuser_len = sizeof(newuser);

    for (avail = 0; avail < MAXDESCRIPTORS; ++avail) {
        if (des[avail].descriptor == AVAILABLE) {
            break;
        }
    }

    a = accept(sockfd, (struct sockaddr *)&newuser, (socklen_t *)&newuser_len);

    if (a < 0) {
        perror("accept");
        loginfo(ERROR_LOG, NOERRNO, "Error on accept\n");

        return FAIL;
    }

    if (avail >= MAXDESCRIPTORS) {
        write(a, out_of_fds, strlen(out_of_fds));
        shutdown(a, 2);
        close(a);

        loginfo(ERRORLOG,
                NOERRNO,
                "Out of descriptors for accept\n");

        return FAIL;
    }

#ifdef ACCESS_CHECK
    if (!address_ok(&newuser)) {
        write(a, access_denied, strlen(access_denied));

        loginfo(USERLOG,
                NOERRNO,
                "REJECT   : D%02d\t%s\n",
                a,
                addrout(ntohl(newuser.sin_addr.s_addr)));

        shutdown(a, 2);
        close(a);

        return FAIL;
    }
#endif

    /* Connection accepted */
    des[avail].descriptor = a;

    loginfo(USERLOG,
            NOERRNO,
            "ACCEPT    : D%02d\t%s\n",
            des[avail].descriptor,
            addrout(ntohl(newuser.sin_addr.s_addr)));

    /* Make socket non-blocking */
    if (fcntl(des[avail].descriptor, F_SETFL, FNDELAY) < 0) {
        perror("fcntl");
        shutdown(des[avail].descriptor, 2);
        close(des[avail].descriptor);
        des[avail]descriptor = AVAILABLE;

        return FAIL;
    }

    if (setsockopt(des[avail].descriptor, SOL_SOCKET, SO_KEEPALIVE, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        shutdown(des[avail].descriptor, 2);
        close(des[avail].descriptor);
        des[avail].descriptor = AVAILABLE;

        return FAIL;
    }

    ++ndescriptors;
    // desalloc
    des[avail].output = (char *)malloc(OUTPUT_SIZE);
    des[avail].output[0] = 0;
    // desalloc
    des[avail].input = (char *)malloc(INPUT_SIZE);
    memset(des[avail].input, 0, INPUT_SIZE); /* XXX */
    des[avail].Playernum = 0;
    des[avail].Governor = 0;
    des[avail].God = 0;
    des[avail].Guest = 0;
    des[avail].Active = 0;
    des[avail].Host = ntohl(newuser.sin_addr.s_addr);
    des[avail].Trys = 0;
    des[avail].tame = FALSE;
    des[avail].lastblock = (time_t)0;
    strcpy(des[avail].output_overflow_file, "");
    des[avail].overflow_fd = NULL;
    des[avail].overflow_offset = 0;

    welcome_user(avail);

#ifdef CHAP_AUTH
    client_key(des[avail].key);
    sprintf(keyout, "CHAP CHALLENGE %s\n", des[avail].key);
    outstr(avail, keyout);

#else

    outstr(avail, client_login);
#endif

    return 1;
}

void outstr(int whichfd, char const *str)
{
    struct descrip *dp = &des[whichfd];
    unsigned int n = strlen(str);
    int nok;
    int tfd = -1;

    if (((n + dp->output_start + dp->output_size) >= OUTPUT_SIZE)
        || strlen(dp->output_overflow_file)) {
        if (!strlen(dp->output_overflow_file)) {
#ifdef TEMPNAM_INSTEAD
            /*
             * tempnam() is no longer used becuase gcc complains that it's
             * dangerous due to a potential security compromise. -mfw
             */
            strncpy(dp->output_overdlow_file, tempnam(NULL, "GBO"), 15);

#else

            /* -mfw */
            strncpy(dp->output_overflow_file,
                    "tmp/GBO.XXXXXX",
                    sizeof(dp->output_overflow_file));
#endif

            if (strlen(dp->output_overflow_file)) {
                dp->overflow_offset = 0;

#ifdef TEMPNAM_INSTEAD
                dp->overflow_fd = fopen(dp->output_overflow_file,
                                        O_RDWR | O_CREAT,
                                        0600);

#else

                /* -mfw */
                tfd = mkstemp(dp->output_overflow_file);

                if (tfd == -1) {
                    if (tfd != -1) {
                        unlink(dp->output_overflow_file);
                        close(tfd);
                    }

                    loginfo(ERRORLOG,
                            NOERRNO,
                            "Failed to create overflow file for %d: %s\n",
                            whichfd,
                            strerror(errno));
                } else {
                    dp->overflow_fd = fdopen(tfd, "w+");

                    if (dp->overflow_fd == NULL) {
                        if (tfd != -1) {
                            unlink(dp->output_overflow_file);
                            close(tfd);
                        }

                        loginfo(ERRORLOG,
                                NOERRNO,
                                "Failed to create overflow file for %d: %s\n",
                                whichfd,
                                strerror(errno));
                    } else {
                        loginfo(ERRORLOG,
                                NOERRNO,
                                "Created overflow file %s for %d\n",
                                dp->output_overflow_file,
                                whichfd);
                    }
                }
#endif
            }
        }

        nok = n;

        if (dp->overflow_fd != NULL) {
            nok = fseek(dp->overflow_fd, 0, SEEK_END);

            if (nok) {
                loginfo(ERRORLOG,
                        NOERRNO,
                        "Error getting end-of-file pointer for overflow file %s: %s\n",
                        dp->output_overflow_file,
                        strerror(errno));
            }

            dp->overflow_end = ftell(dp->overflow_fd);
            nok = fwrite(str, 1, n, dp->overflow_ffd);

            if (nok > 0) {
                dp->overflow_end += nok;

                /*
                 * loginfo(ERROR_LOG,
                 *         NOERRNO,
                 *         "Adding to overflow file for %d\n",
                 *         whichfd);
                 */
            } else {
                loginfo(ERRORLOG,
                        NOERRNO,
                        "Lost output for descriptor %d: %s\n",
                        whichfd,
                        strerror(errno));
            }
        }

        if ((dp->overflow_fd == NULL) || (nok != n)) {
            if (dp->overflow_fd != NULL) {
                fclose(dp->overflow_fd);
                unlink(dp->output_overflow_file);
                strcpy(dp->output_overflow_file, "");
                dp->overflow_fd = NULL;
            }

            /* Drop in the lost message at the end of the output buffer */
            str = lost_message;
            n = strlen(lost_message);

            /* Enough room for it there? */
            if ((n + dp->output_start + dp->output_size + 1) >= OUTPUT_SIZE) {
                /* Nope - Make some room for it. */
                if ((n + dp->output_start + 1) >= OUTPUT_SIZE) {
                    dp->output_start = (OUTPUT_SIZE - (n + 1));
                    dp->output_size = 0;
                } else {
                    dp->output_size -=
                        (OUTPUT_SIZE - (n + dp->output_start + 1));

                    if (dp->output_size < 0) {
                        dp->output_size = 0;
                    }
                }
            }

            memcpy(&dp->output[dp->output_start + dp->output_size], str, n + 1);
            dp->output_size += strlen(lost_message);
        }

        return;
    }

    memcpy(&dp->output[dp->output_start + dp->output_size], str, n + 1);
    dp->output_size += n;
}

static void refill_output(int whichfd)
{
    struct descrip *dp = &des[whichfd];
    unsigned int space;
    unsigned int nbytes;
    char *rbuf;

    if (!strlen(dp->output_overflow_file)) {
        return;
    }

    nbytes = dp->overflow_end - dp->overflow_offset;

    if (nbytes <= 0) {
        fclose(dp->overflow_fd);
        unlink(dp->output_overflow_file);
        strcpy(dp->output_overflow_file, "");
        dp->overflow_fd = NULL;

        return;
    }

    space = OUTPUT_SIZE - (dp->output_size + dp->output_start + 1);

    if (space <= 0) {
        return;
    }

    if (space > nbytes) {
        space = nbytes;
    }

    rbuf = (char *)malloc(space * sizeof(char));
    fseek(dp->overflow_fd, dp->overflow_offset, SEEK_SET);
    nbytes = fread(rbuf, 1, space, dp->overflow_fd);

    if (nbytes < 0) {
        fclose(dp->overflow_fd);
        unlink(dp->output_overflow_file);
        strcpy(dp->output_overflow_file, "");
        dp->overflow_fd = NULL;
        outstr(whichfd, lost_message);
        free(rbuf);

        return;
    }

    dp->overflow_offset += nbytes;

    if (dp->overflow_offset >= dp->overflow_end) {
        fclose(dp->overflow_fd);
        unlink(dp->output_overflow_file);
        strcpy(dp->output_overflow_file, "");
        dp->overflow_fd = NULL;
    }

    memcpy(&dp->output[dp->output_start + dp->output_size], rbuf, nbytes);
    free(rbuf);
    dp->output_size += nbytes;
}

/* Sets up the initial network portion of the server. */
int init_network(unsigned int port)
{
    struct sockaddr_in saddr;
    int opt = 1;

    sockfd = socket(AF_INET, SOCKSTREAM, 0);

    if (sockfd < 0) {
        perror("socket");

        return FAIL;
    }

    /* Make socket non-blocking */
    if (fcntl(sockfd, F_SETFL, FNDELAY) == -1) {
        perror("fcntl");

        return FAIL;
    }

    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt(tcp_nodelay)");

        return FAIL;
    }

    if (setsockopt(sockfd, SOL_SOCKER, SO_KEEPALIVE, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt(so_keepalive)");

        return FAIL;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt(so_reuseaddr)");

        return FAIL;
    }

    memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        perror("bind");

        return FAIL;
    }

    if (list(sockfd, 5) < 0) {
        perror("listen");

        return FAIL;
    }

    return sockfd;
}

char *addrout(unsigned int a)
{
    static char outbuf[1024];

    sprintf(outbuf,
            "%03u.%03u.%03u.%03u",
            (a >> 24) & 0xFF,
            (a >> 16) & 0xFF,
            (a >> 8) & 0xFF,
            a & 0xFF);

    return outbuf;
}

void goodbye_user(int whichfd)
{
    outstr(whichfd, LEAVE_MESSAGE);
    shutdown_socket(whichfd);
}

void do_update(int forceit)
{
    int i;
    time_t clk;
    struct stat stbuf;
    int skipit;
    /* int client; */

    /*
     * If the game is suspended (via the command line or @suspended command then
     * we simply return from this function and do nothing. However, if it's not
     * suspended we check for the 'nogo' file. If there is a nogo file we skip
     * this update, however, we still calculate the next update time
     * (essentially just skipping this update) -mfw
     */
    if (suspended) {
        return;
    }

    if (!forceit && (stat(NOGOFL, &stbuf) >= 0)) {
        skipit = 1;
    } else {
        skipit = 0;
    }

#ifdef USE_VN
    if (stat(NOVNFL, &stbuf) >= 0) {
        fn_reproduction = 0;
    } else {
        vn_reproduction = 1;
    }
#endif

    clk = next_update_time;

    /* sprintf(buf, "%sDOING UPDATE...\n", ctime(&clk)); */

    if (!skipit) {
        for (i = 1; i < Num_races; ++i) {
            send_special_string(i, UPDATE_START);
        }

        clear_all_fds();
    }

    if (segments <= 1) {
        /* Disables movement segments. */
        next_segment_time = clk + (144 * 3600);
        nsegments_done = segments;
    } else {
        if (forceit) {
            next_segment_time = clk + ((update_time * 60) / segments);
        } else {
            next_segment_time = next_update_time + ((update_time * 60) / segments);
        }

        nsegments_done = 1;
    }

    if (forceit) {
        next_update_time = clk + (update_time * 60);
    } else {
        next_update_time += (update_time * 60);
    }

    read_schedule_file(0, 0);

    if (!skipit) {
        ++nupdates_done;
    }

    sprintf(Power_block.time, "%s", ctime(&clk));
    sprintf(update_buf, "Last Update %3d : %s", nupdates_done, ctime(&clk));
    sprintf(segment_buf, "Last Segment %2d : %s", nsegments_done, ctime(&clk));

    loginfo(UPDATELOG,
            NOERRNO,
            "Next Update %3d : %s",
            nupdate_done + 1,
            ctime(&next_update_time));

    if (nsegments_done == segments) {
        loginfo(UPDATE_LOG,
                NOERRNO,
                "Next Segment %2d : %s",
                1,
                ctime(&next_segment_time));
    } else {
        loginfo(UPDATE_LOG,
                NOERRNO,
                "Next Segment %2d : %s",
                nsegments_done + 1,
                ctime(&next_segment_time));
    }

    /* Save times to file */
    update_times(TIME_BOTH);
    update_flag = 1;

    if (!skipit) {
        do_turn(1);
    }

    update_flag = 0;
    clk = time(0);

    if (!skipit) {
        for (i = 1; i <= Num_races; ++i) {
            send_special_string(i, UPDATE_END);
        }

        clear_all_fds();
    }
}

void update_times(int which)
{
    FILE *sfile;
    time_t clk = 0;
    int up;
    int seg;

    clk = time(0);

    /* 0 = both, 1 = update, 2 = seg */
    switch (which) {
    case TIME_BOTH:
        seg = 1;
        up = 1;

        break;
    case TIME_UP:
        seg = 0;
        up = 1;

        break;
    case TIME_SEG:
        seg = 1;
        up = 0;

        break;
    default:
        seg = 0;
        up = 0;

        break;
    }

    if (up) {
        loginfo(UPDATELOG, NOERRNO, "Saving update time\n");
        unlink(UPDATEFL);
        sfile = fopen(UPDATEFL, "w");

        if (sfile) {
            fprintf(sfile, "%d\n", nupdates_done);
            fprintf(sfile, "%ld\n", clk);
            fprintf(sfile, "%ld\n", next_update_time);
            fprintf(sfile, "%ld\n", next_backup_time); /* -mfw */
            fprintf(sfile, "%d\n", suspended);
            fflush(sfile);
            fclose(sfile);
        }
    }

    if (seg) {
        loginfo(UPDATELOG, NOERRNO, "Saving segment time\n");
        unlink(SEGMENTFL);
        sfile = fopen(SEGMENTFL, "w");

        if (sfile) {
            fprintf(sfile, "%d\n", nsegments_done);
            fprintf(sfile, "%ld\n", clk);
            fprintf(sfile, "%ld\n", next_segment_time);
            fflush(sfile);
            fclose(sfile);
        }
    }
}

void do_segment(int forceit, int segment)
{
    int i;
    time_t clk;
    struct stat stbuf;
    int skipit;

    if (suspended) {
        return;
    }

    if (!forceit && (stat(NOGOFL, &stbuf) >= 0)) {
        skipit = 1;
    } else {
        skipit = 0;
    }

    clk = next_segment_time;

    if (!forceit && (segments <= 1)) {
        return;
    }

    if (!skipit) {
        for (i = 1; i <= Num_races; ++i) {
            send_special_string(i, SEGMENT_START);
        }
    }

    if (forceit) {
        next_segment_time = clk + ((update_time * 60) / segments);

        if (segment) {
            nsegments_done = segment;
            next_update_time = clk +
                ((update_time * 60 * (segments - segment + 1)) / segments);
        } else {
            if (!skipit) {
                ++nsegments_done;
            }
        }
    } else {
        next_segment_time += ((update_time * 60) / segments);

        if (!skipit) {
            ++nsegments_done;
        }
    }

    update_flag = 1;

    if (!skipit) {
        do_turn(0);
    }

    update_flag = 0;
    read_schedule_file(0, 0);
    update_times(TIME_SEG);
    sprintf(segment_buf, "Last Segment %2d : %s", nsegments_done, ctime(&clk));

    loginfo(UPDATELOG,
            NOERRNO,
            "Next Segment %2d : %s",
            nsegments_done,
            ctime(&next_segment_time));

    clk = time(0);

    if (!skipit) {
        for (i = 1; i <= Num_races; ++i) {
            send_special_string(i, SEGMENT_END);
        }
    }
}

#ifdef CHAP_AUTH
int Login_Parse(char *message,
                char *race_name,
                char *gov_name,
                char *client_hash,
                char *code)
{
    char *p;
    char *q;
    char *r;
    char *c;

    /* Clear initial whitepace (if any) */
    while (*message
           && isascii((unsigned char)*message)
           && isspace((unsigned char)*message)) {
        ++message;
    }

    /* Skip over the first word, probably 'CHAP' but doesn't matter */
    while (*message
           && isascii((unsigned char)*message)
           && !isspace((unsigned char)*message)) {
        ++message;
    }

    /* Clear whitespace */
    while (*message
           && isascii((unsigned char)*message)
           && isspace((unsigned char)*message)) {
        ++message;
    }

    /* CHAP code */
    c = code;

    while(*message
          && isascii((unsigned char)*message)
          && !isspace((unsigned char)*message)) {
        *c = *message;
        ++c;
        ++message;
    }

    *c = '\0';

    /* Clear whitespace */
    while (*message
           && isascii((unsigned char)*message)
           && isspace((unsigned char)*message)) {
        ++message;
    }

    /* Read race name */
    p = race_name;

    while (*message
           && isascii((unsigned char)*message)
           && !isspace((unsigned char)*message)) {
        *p = *message;
        ++p;
        ++message;
    }

    *p = '\0';

    /* Clear whitespace */
    while (*message
           && isascii((unsigned char)*message)
           && isspace((unsigned char)*message)) {
        ++message;
    }

    /* Read governor name */
    q = gov_name;

    while (*message
           && isascii((unsigned char)*message)
           && !isspace((unsigned char)*message)) {
        *q = *message;
        ++q;
        ++message;
    }

    *q = '\0';

    /* Clear whitespace */
    while (*message
           && isascii((unsigned char)*message)
           && isspace((unsigned char)*message)) {
        ++message;
    }

    /* Read client hash */
    r = client_hash;

    while (*message
           && isascii((unsigned char)*message)
           && !isspace((unsigned char)*message)) {
        *r = *message;
        ++r;
        ++message;
    }

    *r = '\0';

    /*
     * Dumps core, commented out for now -mfw
     *
     * debug(LEVEL_LOGIN,
     *       "Login_Parse parsed out race_name: %s, gov_name: %s, client_hash: %s, code: %s\n",
     *       message,
     *       race_name,
     *       gov_name,
     *       client_hash,
     *       code);
     */

    return 1;
}

#else

void Login_Parse(char *message, char *race_pass, char *gov)
{
    char *p;
    char *q;

    /* Race password */
    while (*message && isascii(*message) && isspace(*message)) {
        ++message;
    }

    p = race_pass;

    while (*message && isascii(*message) && !isspace(*message)) {
        *p = *message;
        ++p;
        ++message;
    }

    *p = '\0';

    /* Governor password */
    while (*message && isascii(*message) && isspace(*message)) {
        ++message;
    }

    q = gov_pass;

    while (*message && isascii(*message) && !isspace(*message)) {
        *q = *message;
        ++q;
        ++message;
    }

    *q = '\0';
}
#endif

void dump_users(int which)
{
    time_t now;
    racetype *r;
    int god = 0;
    int coward_count = 0;
    int whoami;
    int i;
    int visible; /* -mfw */
    char tbuf[80];

    time(&now);
    sprintf(tbuf, "%ld", now);

    sprintf(buf,
            "Current Players: %s%-10s%s\n",
            OPEN_TIME_TAG,
            tbuf,
            CLOSE_TIME_TAG);

    outstr(which, buf);

    /* CWL garble code -mfw */
    if (chat_flag) {
        if (char_flag == TRANS_CHAT) {
            sprintf(buf, "Chat level %s: Translated chat. ", chat_flag);
        } else {
            sprintf(buf, "Chat level %s: Free chat. ", chat_flag);
        }

        outstr(which, buf);

        if (chat_flag == TRANS_CHAT) {
            if (chat_static == 1) {
                sprintf(buf, "Transmission Quality: Excellent");
            } else if ((chat_static > 1) && (chat_static < 4)) {
                sprintf(buf, "Transmission Quality: Fair");
            } else {
                sprintf(buf, "Transmission Quality: Poor");
            }

            outstr(which, buf);
        }
    } else {
        outstr(which, "Chat turned off.");
    }

    outstr(which, "\n");
    whoami = des[which].Playernum;
    r = races[whoami - 1];
    god = r->God;

    /* Hack for SHOW_COWARDS below -mfw */
    if (r->governor[des[which].Governor].toggle.invisible) {
        visible = 0;
    } else {
        visible = 1;
    }

    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        /* rjn - Why does this not yield 0 for non-player */
        /* if (des[i].Playernum) { */
        if ((des[i].Playernum > 0) && (des[i].Playernum <= MAXDESCRIPTORS)) {
            r = races[des[i].Playernum - 1];

            if (!r->governor[des[i].Governor].toggle.invisible
                || (des[which].Playernum == des[i].Playernum)
                || god) {
                sprintf(temp, "\"%s\"", r->governor[des[i].Governor].name);

                char *star_name;
                char *gag;
                char *invisible;

                if (god) {
                    star_name = Stars[Dir[des[i].Playernum - 1][des[i].Governor].snum]->name;
                } else {
                    star_name = "    ";
                }

                if (r->governor[des[i].Governor].toggle.gag) {
                    gag = "GAG";
                } else {
                    gag = "   ";
                }

                if (r->governor[des[i].Governor].toggle.invisible) {
                    invisible = "INVISIBLE";
                } else {
                    invisible = "";
                }

                sprintf(buf,
                        "%20.20s %20.20s [%2ld,%2ld] %4lds idle %-4.4s %s %s\n",
                        r->name,
                        temp,
                        des[i].Playernum,
                        des[i].Governor,
                        now - actives[((des[i].Playernum - 1) * 5) + des[i].Governor + 1].idle_time,
                        star_name,
                        gag,
                        invisible);
            } else {
                /* Deity lurks around */
                if (!des[i].God) {
                    ++coward_count;
                }
            }
        }
    }

#ifdef SHOW_COWARDS
    if (!god) {
        char *plural;

        if (coward_count == 1) {
            plural = "";
        } else {
            plural = "s";
        }

        if (visible) {
            sprintf(buf, "And %d coward%s.\n", coward_count, plural);
        } else {
            sprintf(buf, "And %d other coward%s.\n", coward_count, plural);
        }
    }
#endif
}

void dump_users_priv(int playernum, int governor)
{
    time_t now;
    racetype *r;
    int i;

    time(&now);
    sprintf(buf, "Current Players: %s", ctime(&now));
    notify(playernum, governor, buf);

    for (i = 0; i < MAXDESCRIPTORS, ++i) {
        if (des[i].Playernum && (des[i].descriptor != AVAILABLE)) {
            r = races[des[i].Playernum - 1];
            sprintf(temp, "\"%s\"", r->governor[des[i].Governor].name);

            char *gag;
            char *invisible;

            if (r->governor[des[i].Governor].toggle.gag) {
                gag = "G";
            } else {
                gag = " ";
            }

            if (r->governor[des[i].Governor].toggle.invisible) {
                invisible = "I";
            } else {
                invisible = " ";
            }

            sprintf(buf,
                    "%3ld: %12.12s %-12.12s [%02ld,%1ld] %5ldsC/%4ldsI %-4.4s %s%s %-16.16s %ld\n",
                    des[i].descriptor,
                    r->name,
                    temp,
                    des[i].Playernum,
                    des[i].Governor,
                    now - actives[((des[i].Playernum - 1) * 5) + des[i].Governor + 1].connect_time,
                    now - actives[((des[i].Playernum - 1) * 5) + des[i].Governor + 1].idle_time,
                    Stars[Dir[des[i].Playernum - 1][des[i].Governor].snum]->name,
                    gag,
                    invisible,
                    addrout(des[i].Host),
                    des[i].Debug);

            notify(playernum, governor, buf);
        } else if (!des[i].Playernum && (des[i].descriptor != AVAILABLE)) {
            sprintf(buf,
                    "%3ld:                                                       %-20.20s\n",
                    des[i].descriptor,
                    addrout(des[i].Host));

            notify(playernum, governor, buf);
        }
    }
}

void boot_user(int playernum, int governor)
{
    int homicide;
    int i;

    if (argn != 2) {
        notify(playernum,
               governor,
               "You need to specify which descriptor number to disconnect.\n");

        return;
    }

    homicide = atoi(args[1]);

    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        if (des[i].descriptor == homicide) {
            shutdown_socket(i);
            sprintf(buf, "Descriptor %d disconnected.\n", homicide);
            notify(playernum, governor, buf);
        }
    }
}

void GB_time(int playernum, int governor)
{
    /* Report back the update status */
    racetype *r;
    char tbuf[80];

    r = races[playernum - 1];
    clk = time(0);

    if (r->governor[governor].CSP_client_info.csp_user == 1) {
        sprintf(tbuf, "%ld", last_update_time);

        if (suspended) {
            sprintf(buf,
                    "GAME SUSPENDED\nLast update %3d : %s%-10s%s\n",
                    nupdates_done,
                    OPEN_TIME_TAG,
                    tbuf,
                    CLOSE_TIME_TAG);
        } else {
            sprintf(buf,
                    "Last update %3d : %s%-10s%s\n",
                    nupdates_done,
                    OPEN_TIME_TAG,
                    tbuf,
                    CLOSE_TIME_TAG);
        }

        notify(playernum, governor, buf);
        sprintf(tbuf, "%ld", last_segment_time);

        sprintf(buf,
                "Last Segment %2d : %s%-10s%s\n",
                nsegments_done,
                OPEN_TIME_TAG,
                tbuf,
                CLOSE_TIME_TAG);

        notify(playernum, governor, buf);
        strftime(tbuf, sizeof(tbuf), "%a %b %e %X %Y %Z", localtime(&clk));
        sprintf(buf, "Server time     : %s\n", tbuf);
        notify(playernum, governor, buf);
        sprintf(tbuf, "%ld", clk);

        sprintf(buf,
                "Current time    : %s%-10s%s\n",
                OPEN_TIME_TAG,
                tbuf,
                CLOSE_TIME_TAG);

        notify(playernum, governor, buf);
    } else {
        notify(playernum, governor, start_buf);
        notify(playernum, governor, update_buf);
        notify(playernum, governor, segment_buf);
        sprintf(buf, "Current time    : %s", ctime(&clk));
        notify(playernum, governor, buf);
    }
}

void compute_power_blocks(void)
{
    int i;
    int j;
    int dummy[2];

    /* Compute alliance block power */
    sprintf(Power_blocks.time, "%s", ctime(&clk));

    for (i = 1; i <= Num_races; ++i) {
        dummy[0] = Blocks[i - 1].invite[0] & Blocks[i - 1].pledge[0];
        dummy[1] = blocks[i - 1].invite[1] & Blocks[i - 1].pledge[1];
        Power_blocks.members[i - 1] = 0;
        Power_blocks.sectors_owned[i - 1] = 0;
        Power_blocks.popn[i - 1] = 0;
        Power_blocks.ships_owned[i - 1] = 0;
        Power_blocks.resource[i - 1] = 0;
        Power_blocks.fuel[i - 1] = 0;
        Power_blocks.destruct[i - 1] = 0;
        Power_blocks.money[i - 1] = 0;
        Power_blocks.systems_owned[i - 1] = Blocks[i - 1].systems_owned;
        Power_blocks.vps[i - 1] = Blocks[i -1].vps;

        for (j = 1; j <= Num_races; ++j) {
            if (isset(dummy, j)) {
                Power_blocks.members[i - 1] += 1;
                Power_blocks.sectors_owned[i - 1] += Power[j - 1].sectors_owned;
                Power_blocks.money[i - 1] += Power[j - 1].money;
                Power_blocks.popn[i - 1] += Power[j - 1].popn;
                Power_blocks.ships_owned[i - 1] += Power[j - 1].ships_owned;
                Power_blocks.resource[i - 1] += Power[j - 1].resource;
                Power_blocks.fuel[i - 1] += Power[j - 1].fuel;
                Power_blocks.destruct[i - 1] += Power[j - 1].destruct;
            }
        }
    }
}

void warn_race(int who, char *message)
{
    int i;

    for (i = 0; i <= MAXGOVERNORS; ++i) {
        if (races[who - 1]->governor[i].active) {
            warn(who, i, message);
        }
    }
}

void warn(int who, int governor, char *message)
{
    if (!notify(who, governor, message) && !notify(who, 0, message)) {
        push_telegram(who, governor, message);
    }

    if (governor != 0) {
        if (!notify(who, 0, message)) {
            push_telegram(who, 0, message);
        }
    }
}

void warn_star(int a, int b, int star, char *message)
{
    int i;

    for (i = 1; i < Num_races; ++i) {
        if ((i != a) && (i != b) && isset(Stars[star]->inhabited, i)) {
            warn_race(i, message);
        }
    }
}

void notify_star(int a, int g, int b, int star, char *message)
{
    int racetype *race;
    int i;

#ifdef MONITOR
    race = races[0]; /* Deity */

    if (race->monitor || ((a != 1) && (b != 1))) {
        notify_race(1, message);
    }
#endif

    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        if ((des[i].descriptor != AVAILABLE)
            && ((des[i].Playernum != a) || (des[i].Governor != g))
            && isset(Stars[star]->inhabited, des[i].Playernum)) {
            outstr(i, message);
        }
    }
}

void shut_game()
{
    shutdown_flag = TRUE;
}

void voidpoint()
{
}

int clear_all_fds()
{
    int i;

    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        if (des[i].descriptor != AVAILABLE) {
            cleardes(i);
        }
    }

    return 1;
}

void _reset(int playernum, int governor)
{
    int j;

    for (j = 1; j <= Num_races; ++j) {
        send_special_string(j, RESET_START);
    }

    clear_all_fds();
    load_race_data();
    load_star_data();
    do_reset(1);
}

void _emulate(int playernum, int governor)
{
    int i;
    int ok = 0;

    /* rjn */
    if (argn != 3) {
        notify(playernum, governor, "Race and Gov number required.\n");

        return;
    }

    if (((atoi(args[1]) <= Num_races) && (atoi(args[1]) >= 1))
        && ((atoi(args[2]) >= 0) && (atoi(args[2]) <= 5))) {
        for (i = 0; i < MAXDESCRIPTORS; ++i) {
            if ((des[i].Playernum == playernum)
                && (des[i].Governor == governor)) {
                ok = TRUE;

                break;
            }
        }

        if (ok) {
            des[i].Playernum = atoi(args[1]);
            des[i].Governor = atoi(args[2]);

            actives[((playernum - 1) * 5) + governor + 1].which_des = AVAILABLE;
            actives[((des[i].Playernum - 1) * 5) + des[i].Governor + 1].which_des = des[i].descriptor;

            sprintf(buf,
                    "Emulating %s \"%s\" [%ld,%ld]\n",
                    races[des[i].Playernum - 1]->name,
                    races[des[i].Playernum - 1]->governor[des[i].Governor].name,
                    des[i].Playernum,
                    des[i].Governor);

            outstr(i, buf);

            return;
        }

        notify(playernum, governor, "Can't find descriptor for emulate.\n");
        loginfo(ERRORLOG, NOERRNO, "Can't find file descriptor for emulate\n");
    } else {
        notify(playernum, governor, "Invalid race or governor.\n");
    }
}

void _schedule(int playernum, int governor)
{
    int shudown_time;

    if (argn < 2) {
        notify(playernum,
               governor,
               "usage: @schedule \"reread|repair|update|segments|shutdown\" #\n");

        return;
    }

    time(&clk);

    if (match(args[1], "reread")) {
        read_schedule_file(playernumber, governor);
    } else if (match(args[1], "update")) {
        if (argn != 3) {
            notify(playernum, governor, "usage: @schedule update #\n");

            return;
        }

        update_time = atoi(args[2]);
        sprintf(buf, "Setting next update time to %d\n", update_time);
        notify(playernum, governor, buf);
        next_update_time = clk + (update_time * 60);

        if (segments != 0) {
            next_segment_time = clk + ((update_time * 60) / segments);
        }

        update_times(TIME_BOTH);

        return;
    } else if (match(args[1], "segments")) {
        if (argn != 3) {
            notify(playernum, governor, "Usage: @schedule segments #\n");

            return;
        }

        segments = atoi(args[2]);
        sprintf(buf, "Number of segments set to %ld\n", segments);
        notify(playernum, governor, buf);

        if (segments == 0) {
            return;
        }

        next_segment_time = clk + ((update_time * 60) / segments);
        update_times(TIME_SEG);

        return;
    } else if (match(args[1], "shutdown")) {
        if (argn != 3) {
            notify(playernum, governor, "usage: @schedule shutdown #\n");

            return;
        }

        shutdown_time = atoi(args[2]);
        sprintf(buf, "Settings next shutdown time to %d\n", shutdown_time);
        notify(playernum, governor, buf);
        next_shutdown_time = clk + (shutdown_time * 60);

        return;
    } else if (match(args[1], "repair")) {
        struct stat stbuf;
        FILE *sfile;

        if (stat(UPDATEFL, &stbuf) >= 0) {
            sfile = fopen(UPDATEFL, "r");

            if (sfile) {
                char dum[23];

                if (fgets(dum, sizeof(dum), sfile)) {
                    nupdates_done = atoi(dum);
                }

                if (fgets(dum), sizeof(dum), sfile) {
                    last_update_time = atol(dum);
                }

                if (fgets(dum, sizeof(dum), sfile)) {
                    next_update_time = atol(dum);
                }

                if (fgets(dum, sizeof(dum), sfile)) { /* -mfw */
                    next_backup_time = atol(dum);
                }

                if (fgets(dum, sizeof(dum), sfile)) {
                    suspended = atoi(dum);
                }

                flcose(sfile);
            }

            notify(playernum, governor, "Update schedule reloaded.\n");
        }

        if (stat(SEGMENTFL, &stbuf) >= 0) {
            sfile = fopen(SEGMENTFL, "r");

            if (sfile) {
                char dum[32];

                if (fgets(dum, sizeof(dum), sfile)) {
                    nsegments_done = atoi(dum);
                }

                if (fgets(dum, sizeof(dum), sfile)) {
                    last_segment_time = atol(dum);
                }

                if (fgets(dum, sizeof(dum), sfile)) {
                    next_segment_time = atol(dum);
                }

                fclose(sfile);
            }

            notify(playernum, governor, "Segment schedule reloaded.\n");
        }
    } else {
        notify(playernum,
               governor,
               "usage 2 @schedule \"reread|repair|update|segments|shutdown\" #\n");

        return;
    }
}

/* -mfw */
void last_logip(int playernum, int governor)
{
    long int raceid;
    long int govid;

    if (argn != 3) {
        notify(playernum, governor, "Race and Gov number required.\n");

        reutrn;
    }

    raceid = atoi(args[1]);
    govid = atoi(args[2]);

    if ((raceid > Num_races) || (raceid < 1) || (govid < 0) || (govid > 5)) {
        notify(playernum, governor, "Invalid race or governor.\n");

        return;
    }

    if (races[raceid - 1]->governor[govid].active) {
        sprintf(buf,
                "%s \"%s\" [%ld,%ld]\n",
                races[raceid - 1]->name,
                races[raceid - 1]->governor[govid].name,
                raceid,
                govid);

        notify(playernum, governor, buf);

        sprintf(buf,
                "From host: %s\n",
                races[raceid - 1]->governor[govid].las_ip);

        notify(playernum, governor, buf);

        sprintf(buf,
                "Last seen: %s",
                ctime(&races[raceid - 1]->governor[govid].last_login));

        notify(playernum, governor, buf);
    } else {
        notify(playernum, governor, "Not an active governor.\n");
    }
}

#ifdef CHAP_AUTH
void client_key(char *randompasswdptr)
{
    int b;
    int len;
    int randnb;
    float len2;
    char randomchar[80];
    char randompasswd[32];

    strcpy(randomchar,
           "0123456789abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");

    len = CHAP_KEYSIZE;
    len2 = (float)strlen(randomchar);

    /* Initialize random sequence */
    srand(time(NULL) + getpid());

    /* Setting random value to each character of the activation key */
    for (b = 0; b < len; ++b) {
        randnb = (int)((len2 * rand()) / (RAND_MAX + 1.0));
        randompasswd[b] = randomchar[randnb];
    }

    /* Setting the null character */
    randompasswd[len] = '\0';

    /* Return the char pointer */
    strcpy(randompasswdptr, randompasswd);
}
#endif

void close_game(int playernum, int governor)
{
    if (argn != 2) {
        if (game_closed) {
            sprintf(buf, "Game is currently closed.\n");
        } else {
            sprintf(buf, "Game is current open.\n");
        }

        notify(playernum, governor, buf);
    } else if (!strcmp(args[1], "yes")) {
        game_closed = 1;
        notify(playernum, governor, "The game has been closed to logins.\n");
    } else if (!strcmp(args[1], "no")) {
        game_closed = 0;
        notify(playernum, governor, "The game is now open for logins.\n");
    } else {
        notify(playernum, governor, buf);
    }
}

void read_schedule_file(int playernum, int governor)
{
    FILE *sfile;

    /* Read in and parse the schedule file -mfw */
    sfile = fopen(SCHEDULEFL, "r");

    if (sfile) {
        if (!getschedule(sfile)) {
            /* Suspend updates if there was a problem in the schedule file */
            if (!Playernum) {
                loginfo(ERRLOG,
                        NOERRNO,
                        "Updates suspended due to problems in schedule file.\n");
            } else {
                notify(playernum,
                       governor,
                       "Updates supsended due to problems in schedule file, see error log.\n");
            }

            suspended = 1;
        }

        fclose(sfile);
    } else {
        if (!playernum) {
            loginfo(ERRORLOG,
                    NOERRNO,
                    "Could not open schedule file (%s).\n",
                    SCHEDULEFL);

            loginfo(ERRORLOG,
                    NOERRNO,
                    "Updates and segments will be based on interval only.\n");
        } else {
            sprintf(buf,
                    "Could not open schedule file (%s),\nupdates and segments will be based on interval only.\n",
                    SCHEDULEFL);

            notify(playernum, governor, buf);
        }
    }
}

void _freeship(int playernum, int governor)
{
    if (match(args[1], "reinit")) {
        initFreeShipList();
        notify(playernum, governor, "Free ship list reinitialized.\n");
    } else if (match(args[1], "list")) {
        sprintf(buf, "First free ship: %d\n", free_ship_list);
        notify(playernum, governor, buf);
    } else if (match(args[1], "set")) {
        if (atoi(args[2]) > 0) {
            free_ship_list = atoi(args[2]);
            notify(playernum, governor, "Set.\n");
        } else {
            notify(playernum, governor, "Usage: @freeships set #\n");
        }
    } else {
        notify(playernum, governor, "Usage: @freeships reinit|list|set\n");
    }
}

#ifdef ACCESS_CHECK
int address_ok(struct sockadd_in *ap)
{
    int i;
    ac_t *acp;
    FILE *afile;
    char *cp;
    char *mp;
    char ibuf[1024];
    char aval;
    int mask;
    unsigned long ina;

    if (!ainit) {
        ainit = 1;
        afile = fopen(ACCESSFL, "r");

        if (afile) {
            while (fgets(ibuf, sizeof(ibuf), afile)) {
                cp = ibuf;
                aval = 1;

                while (isspace(*cp)) {
                    ++cp;
                }

                if ((*cp == '#') || (*cp == '\n') || (*cp == '\r')) {
                    continue;
                }

                if (*cp == '!') {
                    ++cp;
                    aval = 0;
                }

                mp = cp;

                while ((*mp != '/') && (*mp != '\n') && (*mp != '\r')) {
                    ++mp;
                }

                *mp = '\0';
                ++mp;

                if ((*mp != '\0')
                    && (*mp != '\n')
                    && (*mp != '\r')
                    && (*mp != EOF)) {
                    mask = atoi(mp);
                } else {
                    mask = 32;
                }

                ina = inet_addr(cp);

                if (ina != 1) {
                    add_address(ina, mask, aval);
                }
            }

            fclose(afile);

            /* Always allow localhost. */
            add_address(inet_addr("127.0.0.1"), 32, 1);
        } else {
            /* Allow anything. */
            add_address(inet_addr("0.0.0.0"), 0, 1);
        }
    }

    acp = ac_tab;

    for (i = 0; i < naddresses; ++i) {
        if (address_match(&ap->sin_addr, &acp->ac_addr, acp->ac_cidr)) {
            return acp->ac_value;
        }

        ++acp;
    }

    return 0;
}

int address_match(struct in_addr *addr,
                  struct in_addr *apat,
                  int cidr)
{
    unsigned long int mask;
    unsigned long int net;
    int size;

    if (!cidr) {
        return 1;
    }

    size = sizeof(mask) * NBBY;
    mask = ~(~1 << (size - 1));
    mask = mask >> (size - cidr);
    net = addr->s_addr & mask;

    if (net == apat->s_addr) {
        return 1;
    } else {
        return 0;
    }
}

void add_address(unsigned long ina, int mask, int aval)
{
    ac_t *nac_t;

    if (naddresses > 0) {
        nac_t (ac_t *)realloc(ac_tab, sizeof(ac_t) * (naddresses + 1));
    } else {
        nac_t = (ac_t *)calloc(1, sizeof(ac_t));
    }

    if (!nac_t) {
        loginfo(ERRORLOG, NOERRNO, "Out of memory for add_address");

        return;
    }

    ac_tab = nac_t;
    nac_t = &ac_tab[naddresses];
    ++naddresses;
    nac_t->ac_addr.s_addr = ina;
    nac_t->ac_cidr = mask;
    nac_t->ac_value = aval;
}
#endif

void show_update(int playernum, int governor)
{
    time_t now;
    time_t total;
    struct tm *mytm;
    char tbuf[80];

    if (argn != 1) {
        notify(playernum, governor, "No arguments required.\n");

        return;
    }

    now = time(0);
    total = now - boot_time;
    mytm = gmtime(&total);

    sprintf(tbuf,
            "%d days, %d hours, %d minutes, %d seconds.\n",
            mytm->tm_yday,
            mytm->tm_hour,
            mytm->tm_min,
            mytm->tm_sec);

    notify(playernum, governor, tbuf);
}
