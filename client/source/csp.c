/*
 * csp.c: Handles special server/client communication.
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1993
 *
 * See the COPYRIGHT file.
 */
#include "csp.h"

#include <ctype.h>
#include <malloc.h>
#include <math.h>
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "ansi.h"
#include "arrays.h"
#include "csparr.h"
#include "gh.h"
#include "proto.h"
#include "str.h"
#include "term.h"
#include "types.h"
#include "vars.h"

#ifdef XMAP
#include "xmap.h"

char xtring[BUFSIZ];
#endif

#define INIT 0
#define NEXT 1

extern int notify;
extern int socket_return;

extern int atoi(const char *);
extern int icomm_valid_csp(int);
extern int icomm_valid_csp_end(int);
extern int sscanf(const char *, const char *, ...);
extern void icomm_command_done(void);
extern void process_socket(char *);
void csp_msg(char *, ...);
void csp_profile_output(Profile *prof);

static int csp_kill_output = FALSE;
static int csp_last_comm_num = 0;
static Pmap map;
Orbit orbit;

void process_special(char *s)
{
    int comm_num;
    char temp[10];
    char line[MAXSIZ];
    static CSPReceiveVal *handler = NULL;

    split(s, temp, line);
    ocmm_num = atoi(temp);

    if(wait_csp.lo && (wait_csp.lo <= comm_num) && (wait_csp.hi >= comm_num)) {
        strcpy(wait_csp.buf, line);
        wait_csp.have = TRUE;
        socket_return = TRUE;
        debug(2, "proc_spec, wait_csp found!");

        return;
    }

    if(!handler || (handler->comm_num != comm_num)) {
        handler = cspr_binary_search(comm_num);
    }

    if(handler == NULL) {
#ifdef CLIENT_DEVEL
        if(client_devel) {
            msg(":: Unknown CSP command #%d - %s", comm_num, line);
        }
        else {
            msg("Unknown CPS(rec): %d %s", comm_num, line);
        }
#endif

        return; /* (FALSE) */
    }

    csp_kill_output = FALSE;
    debug(1, "checking icomm/csp");

    if(icomm_valid_csp(comm_num)) {
        debug(1, "CSP and icomm say to kill output");
        csp_kill_output = TRUE;
    }

    ++handler->cnt;
    handler_func(comm_num, line);

    if(icomm_valid_csp_end(comm_num)) {
        debug(1, "CSP and icomm say to do done");
        icomm_command_done();
    }

    return; /* (TRUE) */
}

int cspr_qsort_cmp(const void *a, const void *b)
{
    return (((const CSPReceiveVal *)a)->comm_num - ((const CSPReceiveVal *)b)->comm_num);
}

int csps_qsort_cmp(const void *a, const void *b)
{
    return (((const CSPSendVal *)a)->comm_num - ((const CSPSendVal *)b)->comm_num);
}

void init_csp(void)
{
    extern void qsort(void *, size_t, size_t, int(*)(const void *, const void *));

    qsort(csp_send_table, NUM_SEND_COMMANDS, sizeof(CSPSendVal), csps_qsort_cmp);
    qsort(csp_receive_table, NUM_RECEIVE_COMMANDS, sizeof(CSPReceiveVal), cspr_qsort_cmp);
}

/* CSP Receive functions */

/* Update/Segment/Reset/Backup */
void cspr_backup_end(int cnum, char *line)
{
    csp_msg("BACKUP DONE... %s", line);
}

void cpsr_update_end(int cnum, char *line)
{
    if(notify > 0) {
        term_beep(notify);
    }

    init_start_commands(1);
    client_stats = L_REINIT;
    csp_msg("UPDATE DONE... %s", line);
}

void cspr_segment_end(int cnum, char *line)
{
    if(notify > 0) {
        term_beep(notify);
    }

    init_start_commands(2);
    client_status = L_REINIT;
    csp_msg("SEGMENT DONE... %s", line);
}

void cspr_reset_end(int cnum, char *line)
{
    if(notify > 0) {
        term_beep(notify);
    }

    init_start_commands(0);
    client_stats = L_REINIT;
    csp_msg("RESET DONE... %s", line);
}

void cspr_map(int cnum, char *line)
{
    char nums[3];
    char outbuf1[BUFSIZ];
    char outbuf2[BUFSIZ];
    char fmt[SMABUF];
    int pad;
    int i;
    static int y = 0;
    char *p;
    char *q;

    struct sect {
        int geo_type;
        int symbol;
        int owner;
    } sect;

    int inverse; /* Currently on or not */
    int colored = -1; /* Currently on or not */

    switch(cnum) {
    case CSP_MAP_INTRO: /* 601 */
        sscanf(line,
               "%d %s %d %s %lf, %d %d %d %d %d %d %d %d",
               &map.snum,
               map.sname,
               &map.pnum,
               map.pname,
               &map.compat,
               &map.tox,
               &map.enslaved,
               &map.x,
               &map.y,
               &map.geo,
               &map.inverse,
               &map.double_digits,
               &map.color);

        y = 0;

#ifdef XMAP
        if(xmap_active) {
            xmap_plot_surface(line);
        }
#endif

        /*
         * Moved to "print header" below
         *
         * (void)get_map_info_buf(INIT, &map);
         * p = get_map_info_buf(NEXT, &map);
         * sprintf(outbuf1, "   %s(%d)/%s(%d)", map.sname, map.snum, map.pname, map.pnum);
         * csp_msg("");
         * csp_msg("%-40s%s", outbuf1, p);
         */

        break;
    case CSP_MAP_DYNAMIC: /* 602 */
#ifdef XMAP
        if(xmap_active) {
            break;
        }
#endif

        sscanf(line,
               "%d %d %d %d %d %d %d %d",
               &map.type,
               &map.sects,
               &map.guns,
               &map.mobptr,
               &map.res,
               &map.des,
               &map.fuel,
               &map.xtals);

        /* Print header */
        (void)get_map_info_buf(INIT, &map);
        p = get_map_info_buf(NEXT, &map);
        sprintf(outbuf1, "   %s(%d)/%s(%d)", map.sname, map.snum, map.pname, map.pnum);
        csp_msg("");
        csp_msg("%-40s%s", outbuf1, p);
        strcpy(outbuf1, "   ");

        for(i = 0; i < map.x; ++i) {
            sprintf(outbuf2, "%d", i / 10);
            strcat(outbuf1, outbuf2);
        }

        csp_msg("%-40s", outbuf1);
        strcpy(outbuf1, "   ");

        for(i = 0; i < map.x; ++i) {
            sprintf(outbuf2, "%d", i %10);
            strcat(outbuf1, outbuf2);
        }

        p = get_map_info_buf(NEXT, &map);
        csp_msg("%-40s%s", outbuf1, p);

        if(GET_BIT(options, MAP_SPACE)) {
            p = get_map_info_buf(NEXT, &map);
            csp_msg("%-40s%s", "", p);
        }

        break;
    case CSP_MAP_DYNAMIC_2: /* 603 */
#ifdef XMAP
        if(xmap_active) {
            break;
        }
#endif

        sscanf(line,
               "%d %d %d %d %d %d %d %d %d %d %d",
               &map.mob,
               &map.actmob,
               &map.popn,
               &map.mpopn,
               &map.totpopn,
               &map.mil,
               &map.totmil,
               &map.acttax,
               &map.tax,
               &map.deposits,
               &map.estprod);

        break;
    case CSP_MAP_ALIENS: /* 604 */
#ifdef XMAP
        if(xmap_active) {
            break;
        }
#endif

        strcpy(map.aliens, line);

        if(!*map.aliens || (*map.aliens == '0')) {
            strcpy(map.aliens, "none");
        }

        break;
    case CSP_MAP_DATA: /* 605 */
#ifdef XMAP
        if(xmap_active) {
            break;
        }
#endif

        inverse = FALSE;
        pad = 0;
        *outbuf = '\0';
        p = line;

        for(i = 0; i < map.x; ++i) {
            q = strchr(p, ';');
            *q = '\0';
            sect.geotype = *p++ - '0';
            sect.symbol = *p++;
            sect.owner = atoi(p);

            /* Determine sector output */
            if(map.color) {
                if(sect.owner > 0) {
                    colored = (sect.owner % MAX_RCOLORS);
                    strcat(outbuf1, ANSI_TRUNCATE);
                    strcat(outbuf1, race_colors[colored]);
                    strcat(outbuf1, ANSI_FOR_BLACK);
                    pad += 15;
                }

                sprintf(outbuf2, "%c", sect.symbol);
                strcat(outbuf1, outbuf2);

                if(colored > -1) {
                    strcat(outbuf1, ANSI_NORMAL);
                    colored = -1;
                    pad += 4;
                }
            }
            else if(map.geo) {
                if(map.inverse && (sect.owner == profile.raceid)) {
                    if(!inverse) {
                        strcat(outbuf1, "");
                        inverse = TRUE;
                        ++pad;
                    }
                }
                else if(inverse) {
                    strcat(outbuf1, "");
                    inverse = FALSE;
                    ++pad;
                }

                strcat(outbuf1, SectorTypes[sect.geo_type]);
            }
            else { /* !map.geo */
                if(sect.owner == profile.raceid) {
                    if(map.inverse) {
                        if(!inverse) {
                            strcat(outbuf1, "");
                            inverse = TRUE;
                            ++pad;
                        }
                        
                        sprintf(outbuf2, "%c", sect.symbol);
                    }
                    else {
                        if(!map.double_digits || (sect.owner < 10)) {
                            sprintf(outbuf2, "%d", sect.owner % 10);
                        }
                        else {
                            if(i % 2) {
                                sprintf(outbuf2, "%d", sect.owner / 10);
                            }
                            else {
                                sprintf(outbuf2, "%d", sect.owner % 10);
                            }
                        }
                    }
                }
                else { /* Not owned by us */
                    if(inverse) {
                        strcat(outbuf1, "");
                        inverse = FALSE;
                        ++pad;
                    }

                    if(sect.owner == 0) {
                        sprintf(outbuf2, "%c", sect.symbol);
                    }
                    else if(sect.symbol == SectorTypesChar[sect.geo_type]) {
                        /* sector type == symbol type (i.e., geo) so display digits */
                        if(!map.double_digits || (sect.owner < 10)) {
                            sprintf(outbuf2, "%d", sect.owner % 10);
                        }
                        else {
                            if(i %2) {
                                sprintf(outbuf2, "%d", sect.owner / 10);
                            }
                            else {
                                sprintf(outbuf2, "%d", sect.owner % 10);
                            }
                        }
                    }
                    else { /* Non civ - display symbol */
                        sprintf(outbuf2, "%c", sect.symbol);
                    }
                }

                strcat(outbuf1, outbuf2);
            } /* !map.geo */

            p = q + 1;
        } /* for loop */

        if(inverse) {
            strcat(outbuf, "");
            inverse = FALSE;
            ++pad;
        }

        sprintf(nums, "%d%d", y / 10, y % 10);
        ++y;

        sprintf(outbuf2,
                "%s%s%s%s",
                nums,
                outbuf1,
                (GET_BIT(options, MAP_DOUBLE) && GET_BIT(options, MAP_SPACE) ? " " : ""),
                (GET_BIT(options, MAP_DOUBLE) ? nums : ""));

        p = get_map_info_buf(NEXT, &map);

        if(p) {
            sprintf(fmt, "%%-%ds%%s", 40 + pad);
            csp_msg(fmt, outbuf2, p);
        }
        else {
            csp_msg("%-40s", outbuf2);
        }

        break;
    case CSP_MAP_END:
#ifdef XMAP
        if(xmap_active) {
            break;
        }
#endif

        if(GET_BIT(options, MAP_DOUBLE)) {
            if(GET_BIT(options, MAP_SPACE)) {
                p = get_map_info_buf(NEXT, &map);

                if(p) {
                    csp_msg("%-40s%s", "", p);
                }
                else {
                    csp_msg("");
                }
            }

            strcpy(outbuf1, "   ");

            for(i = 0; i < map.x; ++i) {
                sprintf(outbuf2, "%d", i / 10);
                strcat(outbuf1, outbuf2);
            }

            p = get_map_info(NEXT, &map);

            if(p) {
                csp_msg("%-40s%s", outbuf1, p);
            }
            else {
                csp_msg("%-40s", outbuf1);
            }

            strcpy(outbuf1, "   ");

            for(i = 0; i < map.x; ++i) {
                sprintf(outbuf2, "%d", i % 10);
                strcat(outbuf1, outbuf2);
            }

            p = get_map_info_buf(NEXT, &map);

            if(p) {
                csp_msg("%-40s%s", outbuf1, p);
            }
            else {
                csp_msg("%-40s", outbuf1);
            }
        }

        p = get_map_info_buf(NEXT, &map);

        while(p) {
            csp_msg("%-40s%s", "", p);
            p = get_map_info_buf(NEXT, &map);
        }

        break;
    default:
#ifdef CLIENT_DEVEL
        if(client_devel) {
            msg(":: Bad CSP# in map: %d - %s", cnum, line);
        }
#endif

        break;
    }
}

char *get_map_info_buf(int cnt, Pmap *map)
{
    static int pos = 0;
    static char xtrabuf[40];
    char enslavebuf[SMABUF];

    if(cnt == INIT) {
        pos = 0;

        /* To hush compiler */
        return NULL;
    }

    ++pos;

    switch(pos) {
    case 1:
        sprintf(xtrabuf,
                " %s    Tox:%3d Compat:%3.0f",
                PlanetTypes[map->type],
                map->tox,
                map->compat);

        break;
    case 2:
        sprintf(xtrabuf,
                " R:%7d   Sects:%3d (%3d) x=%-2d",
                map->res,
                map->sects,
                map->x * map->y,
                map->xtals);

        break;
    case 3:
        sprintf(xtrabuf,
                " D:%7d    Guns:%5d (%5d)",
                map->des,
                map->guns,
                map->mobpts);

        break;
    case 4:
        sprintf(xtrabuf,
                " F:%7d     Mil:%5d (%5d)",
                map->fuel,
                map->mil,
                map->totmil);

        break;
    case 5:
        sprintf(xtrabuf,
                " Mob:%3d/%-3d   Dep:%6d(%5d)",
                map->actmob,
                map->mob,
                map->deposits,
                map->estprod);

        break;
    case 6:
        sprintf(xtrabuf, " Tax:%3d/%-3d", map->acttax, map->tax);

        break;
    case 7:
        sprintf(xtrabuf,
                " Pop:%7d/%-8d (%8d)",
                map->popn,
                map->mpopn,
                map->totpopn);

        break;
    case 8:
        if(map->enslaved) {
            sprintf(enslavebuf, "ENSLAVED(%d)", map->enslaved);
        }
        else {
            *enslavebuf = '\0';
        }

        sprintf(xtrabuf, " %sAliens: %s", enslavebuf, map->aliens);

        break;
    default:

        return NULL;
    }

    return xtrabuf;
}

void cspr_pint(int cnum, char *line)
{
    csp_send_request(CSP_PING, "");

#ifdef CLIENT_DEVEL
    if(client_devel) {
        msg(":: Ping!!!");
    }
#endif
}

void cspr_pause(int cnum, char *line)
{
    char c;

    paused = TRUE;
    promptfor("-- paused --", &c, PROMPT_CHAR);
    paused = FALSE;
}

void cspr_survey(int cnum, char *line)
{
#ifdef POPN
    if(doing_popn_command()) {
        popn_input(cnum, line);

        return;
    }
#endif

#ifdef IMAP
    if(doing_imap_command()) {
        imap_input(cnum, line);

        return;
    }
#endif

#ifdef XMAP
    if(doing_xmap_command()) {
        xmap_input(cnum, line);

        return;
    }
#endif
}

void cspr_updates_suspended(int cnum, char *line)
{
    csp_msg("== Updates/Segments have been suspended.");
    servinfo.updates_suspended = TRUE;
}

void cspr_updates_resumed(int cnum, char *line)
{
    csp_msg("== Updates/Segmets have resumed.");
    servinfo.updates_suspended = FALSE;
}

/* Receiving a list of those commands that we can _send_ */
void cspr_knowledge(int cnum, char *line)
{
    char *p;
    char *q = line;
    CSPSendVal *handler;
    char known[MAXISZ];
    char unknown[MAXSIZ];
    extern char *entry_quote;

    /* Send entry quote now so data will be ready when we leave */
    if(entry_quote && *entry_quote) {
        send_gb(entry_quote, strlen(entry_quote));
    }

    *known = '\0';
    *unknown = '\0';
    p = strchr(q, ' ');

    while(p) {
        *p = '\0';
        handler = csps_binary_search(atoi(q));

        if(handler) {
            handler->know = TRUE;
            strcat(known, handler->name);
            strcat(known, " ");
        }
        else {
            strcat(unknown, q);
            strcat(unknown, " ");
        }
        
        q = p + 1;
        p = strchr(q, ' ');
    }

    handler = csps_binary_search(atoi(q));

    if(handler) {
        handler->know = TRUE;
        strcat(known, handler->name);
        strcat(known, " ");
    }
    else {
        strcat(unknown, q);
        strcat(unknown, " ");
    }

#ifdef CLIENT_DEVEL
    if(client_devel) {
        msg(":: Server knows the following CSP commands:");
        msg(":: %s", known);
        msg(":: Unknown: %s", unknown);
    }
#endif
}

void cspr_err(int cnum, char *line)
{
    int err;

    err = atoi(line);

    /* Old server */
    if((err == CSP_ERR_UNKNOWN_COMMAND) && (csp_last_comm_num == CSP_KNOWLEDGE)) {
#ifdef CLIENT_DEVEL
        if(client_devel) {
            msg(":: Old style server. Does not know CSP_KNOWLEDGE!");
        }
#endif

        return;
    }

    msg("CSP Error: %d %s", cnum, line);
}

/* CSP Send Functions */
CSPSendVal *csps_binary_search(int cnum)
{
    int bottom = 0;
    int top = NUM_SEND_COMMANDS - 1;
    int mid;
    int value;

    while(bottom <= top) {
        mid = bottom + ((top - bottom) / 2);
        value = cnum - csp_send_table[mid].comm_num;

        if(value == 0) {
            return &csp_send_table[mid];
        }
        else if(value < 0) {
            top = mid - 1;
        }
        else {
            bottom = mid + 1;
        }
    }

    return NULL;
}

CSPReceiveVal *cspr_binary_search(int cnum)
{
    int bottom = 0;
    int top = NUM_RECEIVE_COMMANDS - 1;
    int mid;
    int value;

    while(bottom <= top) {
        mid = bottom + ((top - bottom) / 2);
        value = cnum - csp_receive_tabl[mid].comm_num;

        if(value == 0) {
            return &csp_receive_table[mid];
        }
        else if(value < 0) {
            top = mid - 1;
        }
        else {
            bottom = mid + 1;
        }
    }

    return NULL;
}

void waitfor(char *buf, int lo, int hi)
{
    if(!csp_server_vers) {
        debug(2, "waitfor (csp off however): %s %d %d", buf, lo, hi);
        *buf = '\0';

        return;
    }

    wait_csp.lo = lo;
    wait_csp.hi = hi;
    gbs();
    strcpy(buf, wait_csp.buf);
    wait_csp.lo = 0;
    wait_csp.have = FALSE;
    socket_return = FALSE;
}

int csp_send_request(int comm_num, char *buf)
{
    CSPSendVal *handler;
    char *p;
    char str[MAXSIZ];

    /* If not active, then return with a 0 (fail) status */
    if(!csp_server_vers && (comm_num != CSP_LOGIN_COMMAND)) {
        debug(2, "send_csp (off however): %d %s", comm_num, buf);

#ifdef CLIENT_DEVEL
        if(client_devel) {
            msg(":: send_csp (off however): %d %s", comm_num, buf);
        }
#endif

        return 0;
    }

    handler = csps_binary_search(comm_num);

    if(handler == NULL) {
#ifdef CLIENT_DEVEL
        if(client_devel) {
            msg(":: Unknown CSP(send) %s %d %s", CSP_SERVER, comm_num, buf);
        }
        else {
            msg("-- CSP(send) unknown: %s %d %s", CSP_SERVER, comm_num, buf);
        }
#endif

        return 0; /* (FALSE) */
    }

    ++handler->cnt;
    p = (char *)handler->func(buf);

    if(p) {
        sprintf(str, "%s %d %s\n", CSP_SERVER, comm_num, p);
    }
    else {
        sprintf(str, "%s %d\n", CSP_SERVER, comm_num);
    }

    send_gb(str, strlen(str));
    csp_las_comm_num = handler_comm_num;

    return 1; /* (TRUE) */
}

void cspr_profile(int cnum, char *line)
{
    static Profile prof;
    int rt;
    int pt;

    switch(cnum) {
    case CSP_PROFILE_INTRO: /* 301 */
        strcat(line, "|");
        sscanf(line, "%d %d %[^|]|", &prof.raceid, &pt, prof.racename);
        prof.player_type = pt;

        break;
    case CSP_PROFILE_PERSONAL: /* 302 */
        strcpy(prof.personal, line);

        break;
    case CSP_PROFILE_DYNAMIC: /* 303 */
        sscanf(line,
               "%d %d %d %d %d %d %d %s",
               &prof.updates_active,
               &prof.know,
               &prof.capital,
               &prof.raceinfo.morale,
               &prof.ranges.guns,
               &prof.ranges.space,
               &prof.ranges.ground,
               prof.defscope);

        break;
    case CSP_PROFILE_DYNAMIC_OTHER: /* 304 */
        sscanf(line,
               "%d %d %d %d %d %s",
               &prof.know,
               &prof.raceinfo.morale,
               &prof.ranges.guns,
               &prof.ranges.space,
               &prof.ranges.ground,
               prof.sect_pref);

        break;
    case CSP_PROFILE_RACE_STATS: /* 305 */
        sscanf(line,
               "%i %d %lf %lf %d %lf %d %d %lf %lf",
               &rt,
               &prof.raceinfo.fert,
               &prof.raceinfo.birthrate,
               &prof.raceinfo.mass,
               &prof.raceinfo.fight,
               &prog.raceinfo.metab,
               &prof.raceinfo.sexes,
               &prof.raceinfo.explore,
               &prof.raceingo.tech,
               &prog.raceinf.iq);

        prof.raceinfo.racetype = rt;

        break;
    case CSP_PROFILE_PLANET: /* 306 */
        sscanf(line,
               "%d %d %d %d %d %d %d %d %d",
               &prof.planet.temp,
               &prof.planet.methane,
               &prof.planet.oxygen,
               &prof.planet.co2,
               &prof.planet.hydrogen,
               &prof.planet.nitrogen,
               &prof.planet.sulfur,
               &prof.planet.helium,
               &prof.planet.other);

        break;
    case CSP_PROFILE_SECTOR: /* 307 */
        sscanf(line,
               "%d %d %d %d %d %d %d %d",
               &prof.sector.ocean,
               &prof.sector.land,
               &prof.sector.mtn,
               &prof.sector.gas,
               &prof.sector.ice,
               &prof.sector.forest,
               &prof.sector.desert,
               &prof.sector.plated);

        break;
    case CSP_PROFILE_DISCOVERY: /* 308 */
        strcpy(prof.discovery, line);

        break;
    case CSP_PROFILE_END: /* 309 */
        csp_profile_output(&prof);

        /* Update our race profile */
        if(profile.raceid == prof.raceid) {
            profile = prof;
        }

        break;
    default:
#ifdef CLIENT_DEVEL
        if(client_devel) {
            msg(":: Bad CSP# in profile: %d - %s", cnum, line);
        }
#endif

        break;
    }
}

void csp_profile_output(Profile *prof)
{
    char buf[MAXSIZ];
    char *p;
    int i;

    csp_msg("   Racial Profile for %s[%d]%s",
            prof->racename,
            prof->raceid,
            (prof->player_type == CSPD_GUEST ? "  **GUEST**" :
             (prof->player_type == CSPD_DEITY ? "  **DEITY**" : "")));

    csp_msg("");
    csp_msg("Personal: %s", prof->personal);

    if((prof->raceid == profile.raceid) || (profile.player_type == CSPD_DEITY)) {
        csp_msg("Default Scope: %-20s Capital: #%-d",
                prof->defscope,
                prof->capital);

        csp_msg("Morale: %-28d  Ranges:   guns   space   ground",
                prof->raceinfo.moral);

        csp_msg("Updates Active: %-18d            %6d %7d %8d",
                prof->updates_active,
                prof->ranges.guns,
                prof->ranges.space,
                prof->ranges.ground);

        csp_msg("");
        csp_msg("%s Race%s            Planet Conditions         Sector Preferences",
                Racetype[prof->raceinfo.racetype],
                RaceTypePad[prof->raceinfo.racetype]);

        csp_msg("Fert: %6d%%           Temp: %9d",
                prof.raceinfo.fert,
                prof->planet.temp);

        csp_msg("Rate: %6.1lf            methane %6d%%           ocean    . %3d%%",
                prof->raceinfo.birthrate,
                prof->planet.methane,
                prof->second.ocean);

        csp_msg("Mass: %6.2lf            oxygen  %6d%%           gaseous  ~ %3d%%",
                prof->raceinfo.mass,
                prof->planet.oxygen,
                prof->sector.gas);

        csp_msg("Fight: %5d            helium %7%%            ice      # %3d%%",
                prof->raceinfo.fight,
                prof->planet.helium,
                prof->second.ice);

        csp_msg("Metab: %5.2lf            nitrogen %5d%%           mountain ^ %3d%%",
                prof->raceinfo.metab,
                prof->planet.nitrogen,
                prof->sector.mtn);

        csp_msg("Sexes: %5d            CO2 %10d%%           land     * %3d%%",
                prof->raceinfo.sexes,
                prof->planet.co2,
                prof->sector.land);

        csp_msg("Explore: %3d%%           hydrogen %5d%%           desert  - %3d%%",
                prof->raceinfo.explore,
                prof->planet.hydrogen,
                prof->sector.desert);

        csp_msg("Avg Int:%4.0lf            sulfer %7d%%           forest  \ %3d%%",
                prof->raceinfo.iq,
                prof->planet.sulfur,
                prof->sector.forest);

        csp_msg("Tech: %6.2lf            other %8d%%           plated   o %3d%%",
                prof->raceinfo.tech,
                prof->planet.other,
                prof->sector.plated);

        i = 0;
        *buf = '\0';
        p = strtok(prof->discovery, " ");

        while(p) {
            if(atoi(p)) {
                strcat(buf, Discoveries[i]);
            }

            ++i;
            p = strtok(NULL, " ");
        }

        csp_msg("Discoveries: %s", buf);
    }
    else {
        csp_msg("");
        csp_msg("%s Race%s            Planet Conditions",
                RaceType[prof->raceinfo.racetype],
                RaceTypePad[prof->raceinfo.racetype]);

        csp_msg("Fert: %6d%%           Temp: %9d",
                prof->raceinfo.fert,
                prof->planet.temp);

        csp_msg("Rate: %6.1lf           methane %6d%%           Ranges:",
                prof->raceinfo.birthrate,
                prof->planet.methane,
                prof->sector.ocean);

        csp_msg("Mass: %6.2lf           oxygen  %6d%%             guns:%8d",
                prof->raceinfo.mass,
                prof->planet.oxygen,
                prof->ranges.guns);

        csp_msg("Fight: %5d           helium %7d%%             space:%7d",
                prof->raceinfo.fight,
                prof->planet.helium,
                prof->ranges.space);

        csp_msg("Metab: %5.2lf           nitrogen %5%%              ground:%6d",
                prof->raceinfo.metab,
                prof->planet.nitrogen,
                prof->ranges.ground);

        csp_msg("Sexes: %5d           CO2 %10d%%",
                prof->raceinfo.sexes,
                prof->planet.co2);

        csp_msg("Explore: %3d%%          hydrogen %5d%%",
                prof->raceinfo.explore,
                prof->planet.hydrogen);

        csp_msg("Avg Int:%4.0lf           sulfur %7d%%",
                prof->raceinfo.iq,
                prof->planet.sulfur);

        csp_msg("Tech: %6.2lf           other %8d%%           Morale: %7d",
                prof->raceinfo.tech,
                prof->planet.other,
                prof->raceinfo.morale);

        csp_msg("Sector Type Preference: %s", prof->sect_pref);
    } /* else */
} /* csp_profile_output */

void cspr_relation(int cnum, char *line)
{
    char *p;
    char name[SMABUF];
    int id;
    int know;
    int race_type;
    int them_to_you;
    int you_to_them;

    switch(cnum) {
    case CSP_RELATION_INTRO:
        /* First word is race id */
        p = strchr(line, ' ');

        /* Null term, and move to char after */
        *p++ = '\0';

        id = atoi(line);

        /* Just updating the name since we have it here */
        if(id == profile.raceid) {
            strcpy(profile.racename, p);
        }
        else {
            strcpy(races[id].name, p);
        }

        csp_msg("");
        csp_msg("Race Relations for %s[%d]", p, id);
        csp_msg("");

        csp_msg("%2s        %-7s   %-35s %-10s %-10s",
                " #",
                " know",
                "Race Name",
                "Yours",
                "Theirs");

        csp_msg("%2s        %-7s   %-35s %-10s %-10s",
                "--",
                "------",
                "---------",
                "-------",
                "-------");

        break;
    case CSP_RELATION_DATA:
        strcat(line, "|");

        /* First word is race id */
        p = strchr(line, ' ');

        /* Null term, and move to char after */
        *p++ = '\0';

        id = atoi(line);

        sscanf(p,
               "%d %d %d %d %[^|]|",
               &race_type,
               &know,
               &you_to_them,
               &them_to_you,
               name);

        cur_game.maxplayers = id;

        csp_msg("%2d %-6s (%3%%)    %-35s %-10s %-10s",
                id,
                RaceType[race_type],
                know,
                name,
                Relation[you_to_them],
                Relation[them_to_you]);

        break;
    case CSP_RELATION_END:

        break;
    default:
#ifdef CLIENT_DEVEL
        if(client_devel) {
            msg(":: Bad CSP# in relation: %d - %s", cnum, line);
        }
#endif

        break;
    }
}

void cspr_print(int cnum, char *line)
{
    msg("%s", line);
}

void cspr_client_on(int cnum, char *line)
{
    int rc;

    rc = sscanf(line,
                "%d %d %d",
                &profile.raceid,
                &profile.govid,
                &servinfo.updates_suspended);

    if(rc < 3) {
        servinfo.updates_suspended = 0;
    }

#ifdef CLIENT_DEVEL
    if(client_devel) {
        msg(":: Server recognizes CSP.");
        msg(":: You are [%d, %d].", profile.raceid, profile.govid);
    }
#endif

    if(servinfo.updates_suspended) {
        msg("== Updates/Segments are currently suspended.");
    }

    csp_server_vers = TRUE;
    csp_send_request(CSP_KNOWLEDGE, NULL);
    game_type = GAME_GBDT;
}

void cspr_client_off(int cnum, char *line)
{
    csp_server_vers = 0;
}

void cspr_event(int cnum, char *line)
{
    msg("DOING %s... %s",
        (cnum == CSP_UPDATE_START ? "UPDATE" :
         (cnum == CSP_SEGMENT_START ? "SEGMENT" :
          (cnum == CSP_RESET_START ? "RESET" :
           (cnum == CSP_BACKUP_START ? "BACKUP" : "UNKNOWN")))),
        line);

    end_prompt = DOING_PROMPT;
}

/* CSP_SCOPE_PROMPT */
void cspr_scope_prompt(int cnum, char *line)
{
    char buf[SMABUF];
    char rest[MAXSIZ];
    int i;
    char *p;
    Ship *mptr;

    scope.starnum = -1;
    *scope.start = '\0';
    scope.planetnum = '\0';
    *scope.planet = '\0';
    scope.ship = 0;

    strcpy(rest, line);
    p = strtok(rest, " ");
    scope.level = atoi(p);
    p = strtok(NULL, " ");
    scop.numships = atoi(p);
    p = strtok(NULL, " ");
    scope.aps = atoi(p);

    if((scope.level == CSPD_STAR) || (scope.level == CSPD_PLAN)) {
        p = strtok(NULL, " ");
        scope.starnum = atoi(p);
        strcpy(scope.start, strtok(NULL, " "));
    }

    if(scope.level == CSPD_PLAN) {
        p = strtok(NULL, " ");
        scope.planetnum = atoi(p);
        p = strtok(NULL, " ");
        strcpy(scope.planet, p);
    }

    if(scope.numships) {
        /* We have at least 1 ship */
        p = strtok(NULL, " ");
        scope.ship = atoi(p);

        /*
         * Now to get mother ships in reverse order and push them on the list,
         * overwriting old data and malloc'ing new nodes as needed
         */
        for(i = 1; i < scope.numships; ++i) {
            if(i == 1) {
                if(!scope.motherlist) {
                    scope.motherlist = (Ship *)malloc(sizeof(Ship));
                    scope.motherlist->next = NULL;
                    scope.motherlist->prev = NULL;
                }

                mptr = scope.motherlist;
            }
            else {
                if(!mptr->next) {
                    mptr->next = (Ship *)malloc(sizeof(Ship));
                    mptr->next->next = NULL;
                    mptr->next->prev = mptr;
                }

                mptr = mptr->next;
            }
        }

        for(i = 1; i < scope.numships; mptr = mptr->prev, ++i) {
            p = strtok(NULL, " ");

            /* Being cautious here */
            if(!p) {
                break;
            }

            mptr->shipno = atoi(p);
        }
    }

    end_prompt = LEVEL_PROMPT;
    add_assign("scope", build_scope());
    sprintf(buf, "%d", scope.aps);
    add_assign("aps", buf);
    sprintf(buf, "%d", scope.level);
    add_assign("scope_level", buf);
    add_assign("star", scope.star);
    add_assign("planet", scope.planet);
    sprintf(buf, "%d", scope.ship);
    add_assign("ship", buf);

    if(scope.motherlist) {
        sprintf(buf, "%d", scope.motherlist->shipno);
        add_assign("mothership", buf);
    }

    if(on_endprompt(end_prompt)) {
        return;
    }

    /* Else display the scope prompt */
    msg(" %s", build_scope_prompt());
}

void cspr_explore(int cnum, char *line)
{
    char name[SMABUF];
    char nbuf[SMABUF];
    static char aliens[MAXSIZ];
    int compat;
    int aps;
    int autorep;
    int enslaved;
    int explored;
    int id;
    int nsects;
    int stab;
    int tox;
    int totsects;
    int deposits;
    int type;

    switch(cnum) {
    case CSP_EXPLORE_INTRO: /* 501 */
        aps = atoi(line);
        csp_msg("         ========== Exploration Report ==========");
        csp_msg("Global APs: [%d]", aps);
        csp_msg("#    Star[AP](stability)");
        csp_msg("     #  Planet             PlanType Deposit(Compat) [ Explored? Inhab(#seect) Race#]");

        break;
    case CSP_EXPLORE_STAR: /* 502 */
        sscanf(line, "%d %s %d %d", &id, name, &stab, &aps);
        csp_msg("");
        csp_msg("#%-3d %s[%3d](%d)", id, name, aps, stab);

        break;
    case CSP_EXPLORE_START_ALIENS: /* 503 */
        if(*line) {
            strcpy(aliens, line);
        }
        else {
            *aliens = '\0';
        }

        break;
    case CSP_EXPLORE_STAR_DATA: /* 504 */
        sscanf(line,
               "%d %s %d %d %d %d %d %d %d %d %d",
               &id,
               name,
               &explored,
               &nsects,
               &autorep,
               &enslaved,
               &tox,
               &compat,
               &totsects,
               &deposits,
               &type);

        if(explored) {
            sprintf(nbuf, "Inhab(%d) ", nsects);
            csp_msg("    %2d) %-18s %-9s %6d (%3%%) [ Ex %s%s]",
                    id,
                    name,
                    PlanetTypes[type],
                    deposits,
                    compat,
                    (nsects ? nbuf : ""),
                    aliens);
        }
        else {
            csp_msg("    %2d) %-18s ????????? ?????? (???%%) [ No Data ]", id, name);
        }

        break;
    case CSP_EXPLORE_END:

        break;
    default:
#ifdef CLIENT_DEVEL
        if(client_devel) {
            msg(":: Bad CSP# in explore: %d - %s", cnum, line);
        }
#endif

        break;
    }
}

void cspr_who(int cnum, char *line)
{
    char rname[SMABUF];
    char gname[SMABUF];
    int rid;
    int gid;
    int idle;
    int gag;
    int invis;
    int rd;
    char scope[SMABUF];

    switch(cnum) {
    case CSP_WHO_INTRO:
        csp_msg("Current Players: %s", line);

        break;
    case CSP_WHO_DATA:
        *scope = '\0';
        rc = sscanf(line,
                    "%[^\"] \"%[^\"]\" %d %d %d %d %d %s",
                    rname,
                    gname,
                    &rid,
                    &gid,
                    &idle,
                    &gag,
                    &invis,
                    scope);

        if(rc == 8) {
            scope[MAX_SCOPE_LTRS] = '\0';
        }

        remove_space_at_end(rname);
        csp_msg("        %s \"%s\" [%2d,%2d] %d seconds idle %4s%s%s",
                rname,
                gname,
                rid,
                gid,
                idle,
                (*scope ? scope : ""),
                (gag ? " GAG" : ""),
                (invis ? " INVISIBLE" : ""));

        break;
    case CSP_WHO_COWARDS:
        idle = atoi(line);

        if(idle != -1) {
            cps_msg("And %d coward%s",
                    idle,
                    (((idle == 0) || (idle > 1)) ? "s" : ""));

        }

        break;
    default:
#ifdef CLIENT_DEVEL
        if(client_devel) {
            msg(":: Bad CSP# in who: %d - %s", cnum, line);
        }
#endif

        break;
    }
}

/* CSP Send functions */
char *csps_relation(char *s)
{
    return s;
}

char *csps_knowledge(char *s)
{
    static char mbuf[MAXSIZ];
    char num[10];
    int i;

    *mbuf = '\0';

    /* We are sending the list of those commands we can _receive_ */
    for(i = 0; i < NUM_RECEIVE_COMMANDS; ++i) {
        sprintf(num, "%d ", csp_receive_table[i].comm_num);
        strcat(mbuf, num);
    }

#ifdef CLIENT_DEVEL
    if(client_devel) {
        msg(":: CSP List: %s", mbuf);
    }
#endif

    return mbuf;
}

char *csps_login(char *s)
{
    return "0";
}

char *csps_map(char *s)
{
    return NULL;
}

char *csps_ping(char *s)
{
    return NULL;
}

char *csps_survey(char *s)
{
    return NULL;
}

char *csps_null(char *s)
{
    static char buf[BUFSIZ];

    if(s == NULL) {
        return NULL;
    }

    strcpy(buf, s);

    return buf;
}

void csp_msg(char *fmt, ...)
{
    va_list vargs;
    char buf[MAXSIZ];

    if(csp_kill_output) {
        return;
    }

    va_start(vargs, fmt);
    (void)vsprintf(buf, fmt, vargs);
    process_socket(buf);
    va_end(vargs);
}

void cspr_orbit(int cnum, char *line)
{
    char colbuf[SMABUF];
    double dx;
    double dy;
    double dt;

    switch(cnum) {
    case CSP_ORBIT_OUTPUT_INTRO:
        sscanf(line,
               "%d %d %d %d %d %f %f %f %d %d %d %d %d %d %s %s",
               &orbit.scope,
               &orbit.univsize,
               &orbit.syssize,
               &orbit.plorbsize,
               &orbit.scale,
               &orbit.lastx,
               &orbit.lasty,
               &orbit.zoom,
               &orbit.nostars,
               &orbit.noplanets,
               &orbit.noships,
               &orbit.inverse,
               &orbit.color,
               &orbit.stars,
               orbit.gltype,
               orbit.glname);

        // Do we care about this?
        orbit.bcnt = 0;
        orbit.pcnt = obrit.bcnt;
        orbit.scnt = orbit.pcnt;

#ifdef XMAP
        if(xmap_active) {
            strcpy(xstring, "#");
        }
        else {
            term_clear_screen();
        }
#else

        term_clear_screen();
#endif

        orbit.position = 0;

        // Print survey infor in a nice little box
        term_pove_cursor(num_columns - 20, orbit.position++);
        sprintf(colbuf, "+-------------------");
        term_puts(colbuf, strlen(colbuf));

        if(orbit.scope == LEVEL_UNIV) {
            orbit_info_box();
        }

        break;
    case CSP_ORBIT_STAR_DATA:
        if(orbit.nostars) {
            break;
        }

        orbit.type = TYPE_STAR;
        ++orbit.scnt;

        sscanf(line,
               "%d %lf %lf %s %s %s %s %s %s %lf %s",
               &orbit.star.snum,
               &orbit.star.x,
               &orbit.star.y,
               &orbit.star.explored,
               &orbit.star.inhabited,
               &orbit.star.numplan,
               &orbit.star.stability,
               &orbit.star.novastage,
               &orbit.star.tempclass,
               &orbit.star.gravity,
               orbit.star.name);

        plot_orbit_object();

        if(orbit.scope == LEVEL_STAR) {
            orbit_info_box();
        }

        break;
    case CSP_ORBIT_UNEXP_PL_DATA:
        if(orbit.noplanets) {
            break;
        }

        orbit.type = TYPE_PLANET;
        orbit.planet.explored = 0;
        ++orbit.pcnt;

        sscanf(line,
               "%d %d %lf %lf %s",
               &orbit.planet.snum,
               &orbit.planet.pnum,
               &orbit.planet.x,
               &orbit.planet.y,
               orbit.planet.name);

        plot_orbit_object();

        if(orbit.scope == LEVEL_STAR) {
            dx = pow(orbit.planet.x - orbit.star.y, 2);
            dy = pow(orbit.planet.y - orbit.star.y, 2);
            dt = sqrt(dx + dy);
            term_move_cursor(num_columns - 20, orbit.position++);
            sprintf(colbuf, "| %5.0f %s", dt, orbit.planet.name);
            term_puts(colbuf, strlen(colbuf));
        }
        else {
            orbit_info_box();
        }

        break;
    case CSP_ORBIT_EXP_PL_DATA:
        if(orbit.noplanets) {
            break;
        }

        orbit.type = TYPE_PLANET;
        orbit.planet.explored = 1;
        ++orbit.pcnt;

        sscanf(line,
               "%d %d %lf %lf %d %lf %d %s",
               &orbit.planet.snum,
               &orbit.planet.pnum,
               &orbit.planet.x,
               &orbit.planet.y,
               &orbit.planet.type,
               &orbit.planet.compat,
               &orbit.planet.owned,
               orbit.planet.name);

        plot_orbit_object();

        if(orbit.scope == LEVEL_STAR) {
            dx = pow(orbit.planet.x - orbit.star.x, 2);
            dy = pow(orbit.planet.y - orbit.star.y, 2);
            dt = sqrt(dx + dy);
            term_move_cursor(num_columns - 20, orbit.position++);
            sprintf(colbuf, "| %5.0f %s", orbit.planet.name);
            term_puts(colbuf, strlen(colbuf));
        }
        else {
            orbit_info_box();
        }

        break;
    case CSP_ORBIT_SHIP_DATA:
        if(orbit.noships) {
            break;
        }

        orbit.type = TYPE_SHIP;
        ++orbit.bcnt;

        sscanf(line,
               "%d %d %c %lf %lf %lf %lf",
               &orbit.ship.num,
               &orbit.ship.owner,
               &orbit.ship.type,
               &orbit.ship.x,
               &orbit.ship.y,
               &orbit.ship.xt,
               &orbit.ship.yt);

        plot_orbit_object();

        break;
    case CSP_ORBIT_OUTPUT_END:
#ifdef XMAP
        if(xmap_active) {
            // xmap_plot_orbit(&xstring); -mfw
            xmap_plot_orbit(xstring);

            return;
        }
#endif

        term_move_cursor(num_cols - 20, orbit.position);
        sprintf(colbuf, "+---------------");
        term_puts(colbuf, strlen(colbuf));

        break;
    default:
#ifdef CLIENT_DEVEL
        if(client_devel) {
            msg(":: Bad CSP# in orbit: %d - %s", cnum, line);
        }
#endif

        break;
    }
}

void plot_orbit_object(void)
{
    int x;
    int y;

    /* Is our ansi bit set? */
    int want_color = -1;

    /* Currently on or not */
    int colored = -1;

    char colbuf[SMABUF];
    char ptype[] = "@oO#~.\"-0(";

    want_color = GET_BIT(options, DISP_ANSI);

    if((orbit.type == TYPE_STAR) && (orbit_scope == LEVEL_UNIV)) {
        x = (int)(orbit.scale + ((orbit.scale * (orbit.star.x - orbit.lastx)) / (orbit.univsize * orbit.zoom)));
        y = (int)(orbit.scale + ((orbit.scale * (orbit.star.y - orbit.lasty)) / (orbit.univsize * orbit.zoom)));
    }
    else if((orbit_type == TYPE_STAR) && (orbit.scope == LEVEL_STAR)) {
        x = (int)(orbit.scale + ((orbit.scale * -orbit.lastx) / (orbit.syssize * orbit.zoom)));
        y = (int)(orbit.scale + ((orbit.scale * -orbit.lasty) / (orbit.syssize * orbit.zoom)));
    }
    else if((orbit.type == TYPE_PLANET) && (orbit.scope == LEVEL_STAR)) {
        x = (int)(orbit.scale + ((orbit.scale * (orbit.planet.x - orbit.lastx)) / (orbit.syssize * orbit.zoom)));
        y = (int)(orbit.scale + ((orbit.scale * (orbit.planet.y - orbit.lasty)) / (orbit.syssize * orbit.zoom)));
    }
    else if((orbit.type == TYPE_PLANET) && (orbit.scope == LEVEL_PLANET)) {
        x = (int)(orbit.scale + ((orbit.scale * -orbit.lastx) / (orbit.plorbsize * orbit.zoom)));
        y = (int)(orbit.scale + ((orbit.scale * -orbit.lasty) / (orbit.plorbsize * orbit.zoom)));
    }
    else if((orbit.type == TYPE_SHIP) && (orbit.scope == LEVEL_UNIV)) {
        x = (int)(orbit.scale + ((orbit.scale * (orbit.ship.x - orbit.lastx)) / (orbit.univsize * orbit.zoom)));
        y = (int)(orbit.scale + ((orbit.scale * (orbit.ship.y - orbit.lasty)) / (orbit.univsize * orbit.zoom)));
    }
    else if((orbit.type == TYPE_SHIP) && (orbit.scope == LEVEL_STAR)) {
        x = (int)(orbit.scale + ((orbit.scale * (orbit.ship.x - orbit.star.x - orbit.lastx)) / (orbit.syssize * orbit.zoom)));
        y = (int)(orbit.scale + ((orbit.scale * (orbit.ship.y - orbit.star.y - orbit.lasty)) / (orbit.syssize * orbit.zoom)));
    }
    else if((orbit.type == TYPE_SHIP) && (orbit.scope == LEVEL_PLANET)) {
        x = (int)(orbit.scale + ((orbit.scale * (orbit.ship.x - orbit.planet.x - orbit.lastx)) / (orbit.plorbsize * orbit.zoom)));
        y = (int)(orbit.scale + ((orbit.scale * (orbit.ship.y - orbit.planet.y - orbit.lasty)) / (orbit.plorbsize * orbit.zoom)));
    }
    else {
        /* Unknown type or scope, don't plot it */
        return;
    }

#ifdef XAMP
    if(xamp_active) {
        if(orbit.type == TYPE_STAR) {
            sprintf(colbuf,
                    "%d %d %d %d %c %d %s;",
                    orbit.star.explored,
                    x,
                    y,
                    orbit.star.novastage,
                    '*',
                    orbit.star.inhabited,
                    orbit.star_name);
        }
        else if(orbit.type == TYPE_PLANET) {
            sprintf(colbuf,
                    "%d %d %d %d %c %d %s;",
                    orbit.planet.explored,
                    x,
                    y,
                    0,
                    ptypes[orbit.planet.type],
                    orbit.planet.owned,
                    orbit.planet.name);
        }
        else if(orbit.type == TYPE_SHIP) {
            sprintf(colbuf,
                    "%d %d %d %d %c %d #%d;",
                    (orbit.ship.owner == profile.raceid),
                    x,
                    y,
                    0,
                    orbit.ship.type,
                    0,
                    orbit.ship.num);
        }

        strcat(xstring, colbuf);

        return;
    }
#endif

    x *= Midx;
    y *= Midy;

    if((x >= 0) && (y >= 1) && (x <= S_X) && (y <= S_Y)) {
#ifdef ARRAY
        if((orbit.type == TYPE_STAR)
           && (orbit.scope == LEVEL_STAR)
           && (orbit.star.novastage > 0)
           && (orbit.star.novastage <= 16)) {
            /* Nova */
            DispArray(x, y, 11, 7, Novae[orbit.star.novastage - 1], 1.0);
        }

        /*
         * The array variable is not sent from the server, so this won't
         * work. -mfw
         */
        if((orbit.type == TYPE_SHIP)
           && (orbit.scope == LEVEL_PLANET)
           && (orbit.ship_type == 'M')
           && (orbit.ship.array <= 8)
           && (orbit.ship.array > 0)) {
            /* Mirror */
            DispArray(x, y, 9, 5, Mirror[orbit.ship.array - 1], 1.0);
        }
#endif

        term_move_cursor(x, y);

        if(orbit.type == TYPE_STAR) {
            if(orbit.star.explored && orbit.inverse) {
                term_standout_on();
                term_putchar('*');
                term_standout_off();
            }
            else {
                term_putchar('*');
            }
        }
        else if(orbit.type == TYPE_PLANET) {
            if(orbit.planet.explored && orbit.inverse) {
                term_standout_on();
                term_putchar(ptyes[orbit.planet.type]);
                term_standout_off();
            }
            else {
                term_putchar('?');
            }
        }
        else if(orbit.type == TYPE_SHIP) {
            if(orbit.color && want_color) {
                colored = orbit.ship.owner % MAX_RCOLORS;
                sprintf(colbuf,
                        "%s%s%c%s",
                        race_colors[colored],
                        ANSI_FOR_BLACK,
                        orbit.ship.type,
                        ANSI_NORMAL);

                term_puts(colbuf, strlen(colbuf));
            }
            else if((orbit.ship.owner == profile.raceid) && orbit.inverse) {
                term_standout_on();
                term_putchar(orbit.ship.type);
                term_standout_off();
            }
            else {
                term_putchar(orbit.ship.type);
            }
        }

        term_putchar(' ');

        if(orbit.type == TYPE_STAR) {
            sprintf(colbuf, "%s", orbit.star.name);

            if(orbit.star.inhabited && orbit.inverse) {
                term_standout_on();
                term_puts(colbuf, strlen(colbuf));
                term_standout_off();
            }
            else {
                term_puts(colbuf, strlen(colbuf));
            }
        }
        else if(orbit.type == TYPE_PLANET) {
            sprintf(colbuf, "%s", orbit.planet.name);

            if(orbit.planet.owned && orbit.inverse) {
                term_standout_on();
                term_puts(colbuf, strlen(colbuf));
                term_standout_off();
            }
            else {
                term_puts(colbuf, strlen(colbuf));
            }
        }
        else if(orbit.type == TYPE_SHIP) {
            sprintf(colbuf, "#%d", orbit.ship.num);
            term_puts(colbuf, strlen(colbuf));
        }
    }
}

void orbit_info_box(void)
{
    char colbuf[SMABUF];

    if(orbit.scope == LEVEL_UNIV) {
        term_move_cursor(num_columns - 20, orbit.position++);
        sprintf(colbuf, "| Galaxy %.10s", orbit.glname);
        term_puts(colbuf, strlen(colbuf));

        term_move_cursor(num_columns - 20, orbit.position++);
        sprintf(colbuf, "| Type: %.10s", orbit.gltype);
        term_puts(colbuf, strlen(colbuf));

        term_move_cursor(num_columns - 20, orbit.position++);
        sprintf(colbuf, "| Size: %d", orbit.univsize);
        term_puts(colbuf, strlen(colbuf));

        term_move_cursor(num_columns - 20, orbit.position++);
        sprintf(colbuf, "| Stars: %d", orbit.stars);
        term_puts(colbuf, strlen(colbuf));
    }
    else if(orbit.scope == LEVEL_STAR) {
        term_move_cursor(num_columns - 20, orbit.position++);
        sprintf(colbuf, "| Star: %.10s", orbit.star.name);
        term_puts(colbuf, strlen(colbuf));

        /* Only ever shows 0,0 -mfw
         * term_move_cursor(num_columns - 20, orbit.position++);
         * sprintf(colbuf, "| Locn: %.0f,%.0f", orbit.star.x, orbit.star.y);
         * term_puts(colbuf, strlen(colbuf));
         */

        term_move_cursor(num_columns - 20, orbit.position++);
        sprintf(colbuf, "| Grav: %.2lf", orbit.star.gravity);
        term_puts(colbuf, strlen(colbuf));

        if(orbit.star.novastage) {
            term_move_cursor(num_columns - 20, orbit.position++);
            sprintf(colbuf, "| Nova: %d (1-16)", orbit.star.novastage);
            term_puts(colbuf, strlen(colbuf));
        }
        else {
            term_move_cursor(num_columns - 20, orbit.position++);

            if(orbit.star.stability < 0) {
                sprintf(colbuf, "| Stab: ??");
            }
            else {
                sprintf(colbuf, "| Stab: %d%%", 100 - orbit.star.stability);
            }

            term_puts(colbuf, strlen(colbuf));
        }

        term_move_cursor(num_columns - 20, orbit.position++);
        sprintf(colbuf, "| Temp: %d (1-10)", orbit.star.tempclass);
        term_puts(colbuf, strlen(colbuf));

        term_move_cursor(num_columns - 20, orbit.position++);
        sprintf(colbuf, "| %d Planets --", orbit.star_numplan);
        term_puts(colbuf, strlen(colbuf));
    }
    else if(orbit.scope == LEVEL_PLANET) {
        term_move_cursor(num_columns - 20, orbit.position++);
        sprintf(colbuf, "| Planet: %.10s", orbit.planet.name);
        term_puts(colbuf, strlen(colbuf));

        term_move_cursor(num_columns - 20, orbit.position++);
        sprintf(colbuf, "| Compat: %.2f%%", orbit.planet.compat);
        term_puts(colbuf, strlen(colbuf));
    }
}
