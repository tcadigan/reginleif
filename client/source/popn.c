/*
 * popn.c: Does moving of population on a planet
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1993
 *
 * See the COPYRIGHT file.
 */
#include "popn.h"

#include "str.h"
#include "types.h"

/* Defines for popn movement */
#define IGNORE 0
#define EMPTY 1
#define MY_SECT 2

Map popn_map = { 0, 0, "", "", 0, 0, 0, 0, 0, 0, 0, 0, 0, true, false, false, (Sector *)NULL };

struct popn_infostruct {
    int doing;
    int aps_second;
    int ymin;
    int ymax;
    int xmin;
    int xmax;
    int apcost;
    int do_use_compat;
    int comandcnt;
};

struct popn_infostruct popn_info = { false, 0, -1, -1, -1, -1, 0, true, 0 };

extern char pbuf[];
extern int end_msg;
extern int hide_msg;
extern int kill_socket_output;

extern int atoi(const char *);
extern int sscanf(const char *, const char *, ...);
void handle_popn(void);

#ifdef POPN
void popn_input(int comm_num, char *procbuf)
{
    switch (comm_num) {
    case CSP_SURVEY_INTRO:
        if (popn_map.ptr) {
            free(popn_map.ptr);
        }

        process_client_survey(comm_num, procbuf, &popn_map);
        popn_map.ptr = (Sector *)malloc(sizeof(Sector) * popn_map.maxx * popn_map.maxy);

        if (popn_map.ptr == (Sector *)NULL) {
            msg("-- Popn: could not malloc map space. Aborting.");
            popn_info.doing = false;

            return;
        }

        break;
    case CSP_SURVEY_SECTOR:
        if (!popn_map.ptr) {
            return;
        }

        process_client_survey(comm_num, procbuf, &popn_map);

        break;
    case CSP_SURVEY_END:
        if (popn_info.doing) {
            handle_popn();
        }

        break;
    default:
        msg("-- Unknown popn lcient_survey #%d", comm_num);

        break;
    }
}

int doing_popn_command(void)
{
    return popn_info.doing;
}

#endif

void cmd_popn(char *args)
{
#ifdef POPN
    char rest[MAXSIZ];
    char extra[MAXSIZ];
    char intc[SMABUF];
    int int1;
    int int2;
    char ch;

    if (scope.level != LEVEL_PLANET) {
        msg("-- Popn: You are NOT at planet scope.");

        return;
    }

    if (profile.raceinfo.sexes <= 0) {
        msg("-- WARNING: Client does not know number of sexes. Please input it now.");
        int1 = true;

        while (int1) {
            promptfor("# of sexes? ", pbuf, PROMPT_STRING);

            if (isdigit(*pbuf)) {
                profile.raceinfo.sexes = atoi(pbuf);

                if (profile.raceinfo.sexes > 0) {
                    int1 = false;
                }
            }
        }
    }

    if (profile.raceinfo.sexes == 1) {
        popn_info.apcost = 1;
    } else if (profile.raceinfo.sexes > 6) {
        popn_info.apcost = 3;
    } else { /* Sexes is in range 2 - 6 */
        popn_info.apcost = 2;
    }

    if (scope.aps < popn_info.apcost) {
        msg("-- Popn: You do not have enough APs.");

        return;
    }

    /* No ars, do whole planet */
    if (!*args) {
        popn_info.ymin = 0;
        popn_info.ymax = 10000;
        popn_info.xmin = 0;
        popn_info.xmax = 10000;
        popn_info.aps_spend = scope.aps;
    } else {
        split(args, intc, rest);
        popn_info.aps_spend = atoi(intc);

        if (!*rest) {
            popn_info.ymin = 0;
            popn_info.ymax = 10000;
            popn_info.xmin = 0;
            popn_info.xmax = 10000;
        } else {
            split(rest, intc, extra);

            if (sscanf(intc, "%d,%d", &int1, &int2) == 2) {
                popn_info.xmin = int1;
                popn_info.xmax = int2;
            } else if (sscanf(intc, "%d", &int1) == 1) {
                popn_info.xmin = int1;
                popn_info.xmax = 10000;
            } else {
                popn_info.xmin = 0;
                popn_info.xmax = 10000;
            }
        }

        /* No negative ranges */
        if (popn_info.xmin < 0) {
            popn_info.xmin = 0;
        }

        if (popn_info.xmax < 0) {
            popn_info.xmax = 10000;
        }

        if (*extra) {
            split(extra, intc, rest);

            if (sscanf(intc, "%d,%d", &int1, &int2) == 2) {
                popn_info.ymin = int1;
                popn_info.ymax = int2;
            } else if (sscanf(intc, "%d", &int1) == 1) {
                popn_info.ymin = int1;
                popn_info.ymax = 10000;
            } else {
                popn_info.ymin = 0;
                popn_info.ymax = 10000;
            }
        }

        /* No negative ranges */
        if (popn_info.ymin < 0) {
            popn_info.ymin = 0;
        }

        if (popn_info.ymax < 0) {
            popn_info.ymax = 10000;
        }
    }

    msg("-- Popn: Avoid incompatible sectors (y/n)?");
    promptfor("(y/n)? ", &ch, PROMPT_CHAR);

    if ((ch == 'y') || (ch == 'Y')) {
        popn_info.do_use_compat = true;
    } else {
        popn_info.do_use_compat = false;
    }

    if (game_type == GAME_GBDT) {
        csp_send_request(CSP_SURVEY_COMMAND, "-");
    } else {
        send_gb("client_survey -\n", 16);
    }

    popn_info.doing = true;
    msg("-- Popn: Gathering data. Please hold.");

#else

    msg("-- Popn: The client must be compiled with the POPN option.");
#endif
}

#ifdef POPN
void handle_popn(void)
{
    Sector *p;
    Sector *q;
    int x;
    int y;
    int x2;
    int y2;
    int dx;
    int dir;
    int i;
    int empty_secs;
    int xtra_civs;
    int moving_civs;
    int empty_adj_sects = 0;
    int max_move;
    int changes_made = true;
    char movebuf[NORMSIZ];

    static int movement_direction_table[9] = {
        7, 8, 9, /* nw, n, ne */
        4, 0, 6, /* w , -,  e */
        1, 2, 3, /* sw, s, se */
    };

    if (popn_info.xmax > popn_max.maxx) {
        pop_info.xmax = popn_map.maxx - 1;
    }

    if (popn_info.ymax > popn_map.maxy) {
        popn_info.ymax = popn_map.maxy - 1;
    }

    msg("-- Popn: Analyzing: X range (%d,%d), Y range (%d,%d), Aps to utilize: %d",
        popn_info.xmin,
        popn_info.xmax,
        popn_info.ymin,
        popn_info.ymax,
        popn_info.aps_spend);

    popn_info.doing = false;

    /* Mark which ones for moving? */
    p = popn_map.ptr;

    for (y = 0; y < popn_map.maxy; ++y) {
        for (x = 0; x < popn_map.maxx; ++x) {
            p = popn_map.ptr + x + (y * popn_map.maxx);

            if (p->own == profile.raceid) {
                p->sect_status = MY_SECT;
            } else if (p->own && (p->civ || p->mil)) {
                p->sect_status = IGNORE;
            } else if (!p->own) {
                p->sect_status = EMPTY;
            } else { /* Should never get here */
                p->sect_status = IGNORE;
            }

            /*
             * If we care whether or not we are compat with the
             * sector, check if we are, and make to ignore the sector
             * if we are not compat with it.
             */
            if ((p->sect_status == EMPTY) && popn_info.do_use_compat) {
                for (i = 0; i < SECTOR_MAX; ++i) {
                    if ((p->sectc == sector_type[i].sectc) && !sector_type[i].compat) {
                        p->sect_status = IGNORE;
                    }
                }
            }
        } /* For x */
    } /* For y */

    kill_socket_output = true;
    popn_info.commandcnt = 0;

    /* Repeat until we don't move any more civs */
    while (changes_made) {
        changes_made = false;

        /* Count the number of empty sects adj to an owned sector */
        for (x = popn_info.xmin; x < popn_map.maxx; ++x) {
            for (y = popn_info.ymin; y < popn_map.maxy; ++y) {
                p = popn_map.ptr + x + (y * popn_map.maxx);

                if (p->sect_status != EMPTY) {
                    continue;
                }

                for (x2 = (x - 1); x2 < (x + 2); ++x2) {
                    for (y2 = (y - 1); y2 < (y + 2); ++y2) {
                        if ((y2 < popn_info.ymin) || (y2 > popn_info.ymax)) {
                            continue;
                        }

                        if (x2 < popn_info.xmin) {
                            dx = popn_info.xmax;
                        } else if (x2 > popn_info.xmax) {
                            dx = popn_info.xmin;
                        } else {
                            dx = x2;
                        }

                        q = popn_map.ptr + dx + (y2 * popn_map.maxx);

                        if (q->sect_status == MY_SECT) {
                            ++empty_adj_sects;
                            x2 = x + 2;
                            y2 = y + 2;
                        }
                    } /* For y2 */
                } /* For x2 */
            } /* For y */
        } /* For x */

        /* Move civs into unused sectors */
        for (y = popn_info.ymin; y <= popn_map.maxy; ++y) {
            for(x = popn_info.xmin; x < popn_map.maxx; ++x) {
                p = popn_map.ptr + x + (y * popn_map.maxx);

                /* If we don't own the sector, skip it */
                if (p->sect_status != MY_SECT) {
                    continue;
                }

                /* Not enough civs in this sector to move? */
                if (p->civ < (profile.raceinfo.sexes * 2)) {
                    continue;
                }

                /*
                 * If we are a 1 sex race, and there are more empty
                 * sectors that are adjacent to our sector, move one
                 * cive to them first, else move as many as we can up
                 * to 6
                 */
                if ((profile.raceinfo.sexes == 1)
                    && (empty_adj_sects >= popn_info.aps_spend)) {
                    max_move = 1;
                } else {
                    max_move = 6;
                }

                empty_secs = 0;
                xtra_civs = 0;

                /*
                 * Count the number of empty sectors adjacent to this
                 * sector
                 */
                for (x2 = (x - 1); x2 < (x + 2); ++x2) {
                    for (y2 = (y - 1); y2 < (y + 2); ++y2) {
                        if ((y2 < popn_info.ymin) || (y2 > popn_info.ymax)) {
                            continue;
                        }

                        if (x2 < 0) {
                            dx = popn_info.xmax;
                        } else if(x2 > popn_info.xamx) {
                            dx = 0;
                        } else {
                            dx = x2;
                        }

                        q = popn_map.ptr + dx + (y2 * popn_map.maxx);

                        if (q->sect_status == EMPTY) {
                            ++empty_secs;
                        }
                    } /* For y2 */
                } /* For x2 */

                /*
                 * If there are more civs then needed to fill each
                 * surrounding empty sector with your min sexes, then
                 * we have some extra to move around too
                 */
                xtra_civs = p->civ - (profile.raceinfo.sexes * (empty_secs + 1));

                /*
                 * If we don't have enough civs to fill all
                 * surrounding empty sectors, then we don't have an
                 * xtra civs
                 */
                if (xtra_civs < 0) {
                    xtra_civs = 0;
                }

                for (x2 = (x - 1); x2 < (x + 2); ++x2) {
                    for (y2 = (y - 1); y2 < (y + 2); ++y2) {
                        if ((y2 < popn_info.ymin) || (y2 > popn_info.ymax)) {
                            continue;
                        }

                        if (x2 < 0) {
                            dx = popn_info.xmax;
                        } else if (x2 > popn_info.xmax) {
                            dx = 0;
                        } else {
                            dx = x2;
                        }

                        q = popn_map.ptr + dx + (y2 * popn_map.maxx);

                        if (q->sect_status != EMPTY) {
                            continue;
                        }

                        if ((xtra_civs + profile.raceinfo.sexes) >= max_move) {
                            moving_civs = max_move;
                            xtra_civs -= (max_move - profile.raceinfo.sexes);
                        } else {
                            if (p->civ >= (2 * profile.raceinfo.sexes)) {
                                moving_civs = xtra_civs + profile.raceinfo.sexes;
                                xtra_civs = 0;
                            } else {
                                moving_civs = 0;
                            }
                        }

                        if ((moving_civs > 0) && ((p->civ - moving_civs) >= profile.raceinfo.sexes)) {
                            changes_made = true;
                        } else {
                            continue;
                        }

                        dir = movement_direction_table[(3 * (y2 - y + 1)) + x2 - x + 1];

                        if (!dir) {
                            continue;
                        }

                        sprintf(movebuf,
                                "move %d,%-d %d %d\n",
                                x,
                                y,
                                dir,
                                moving_civs);

                        send_gb(movebuf, strlen(movebuf));
                        ++popn_info.commandcnt;
                        ++hide_msg;
                        ++end_msg;
                        q->civ += moving_civs;
                        p->civ -= moving_civs;
                        q->sect_status = MY_SECT;

                        if (moving_civs == 1) {
                            popn_info.aps_spend -= 1;
                        } else {
                            popn_info.aps_spend -= 2;
                        }

                        --empty_adj_sects;

                        if (popn_info.aps_spend <= popn_info.apcost) {
                            --end_msg;

                            if (!hide_msg) {
                                kill_socket_output = false;
                                end_msg = false;
                            }

                            msg("-- Popn: Sending %d commands. Please hold.",
                                popn_info.command_cnd);

                            return;
                        }
                    } /* y2 loop */
                } /* x2 loop */
            } /* x loop */
        } /* y loop */
    } /* while changes_made */

    --end_msg;

    if (!hide_msg) {
        kill_socket_output = false;
        end_msg = false;
    }

    msg("-- Popn: Sending %d commands. Please hold.",
        popn_info.commandcnt);

    add_queue("echo -- Popn: Commands executed. Done.", 0);
}

#endif
