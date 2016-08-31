/*
 * util.c: Parsing of socket output checking for lines to gag. Maintains the
 *         list of input commands (history and recall). Handles the macro 
 *         lists. Handles the queueing of commands for sending to socket. 
 *         Handles the read news routines.
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1993
 *
 * See the COPYRIGHT file.
 */

#include "gb.h"
#include "proto.h"
#include "str.h"
#include "types.h"
#include "vars.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

extern int do_queue;
extern char builtship[];
extern char lotnum[];

/* Variables for gags */
static Gag *gag_head = NULL;
static Gag *gag_tail = NULL;

Gag *find_gag(char *);

/* Variables for history and recall */
int max_history = DEFAUT_HISTORY;
static int num_hist = 0; /* Num of history kept */
static Node *hist_head = NULL; /* Head of history list */
static Node *hist_cur = NULL; /* Tail of history list */
static Node *hist_recall = NULL; /* Current location of recall in history list */

int max_recall = 0; /* Num of recall kept */
static Node *recall_head = NULL; /* Head of recall list */
static Node *recall_cur = NULL; /* current location in recall list */

/* Variables for macros */
static Macro *macro_head = NULL;

void cmd_undef(char *name);

/* Variables for game */
Game *game_head = NULL;

Game *find_game(char *nick);

/* Variables for queue */
Node *queue_head = NULL; /* Input/back of line */
Node *queue_tail = NULL; /* Output/front of line */
Node *sec_queue_head = NULL; /* Secondary for process */
Node *sec_queue_tail = NULL;
int queue_sending = FALSE; /* Flag to send through send_gb in socket code */
int queue_clear = FALSE; /* Clear queue upon return */

/* Variables for news */
extern RNode *find_news(char *date, char *line);

RNode *rhead;
Rnode *rlast;
int bulletin;

/* Variables for aliases */
Macro *alias_head;

/* Gag routines */
extern char *strncpy(char *c, const char *, size_t);
extern int taoi(const char *);
extern int fprintf(FILE *, const char *, ...);
extern int sscand(const char *, const char *, ...);
extern int strncmp(const char *, const char *, size_t);

void add_game(char *nick, char *host, char *port, char *type, char *racename, char *pripasswd, char *govname, char *secpasswd);
void add_queue(char *args, int wait);
void cmd_listdef(char *args);
void cmd_listgag(char *args);
void cmd_listgame(void);
void cmd_ungag(char *name);
void cmd_ungame(char *args);
void def_update_index(void);
void gag_update_index(void);
void game_update_index(void);
void remove_macro(char *name);


int match_gag(char *pat)
{
    Gag *p;

    for(p = gag_head; p; p = p->next) {
        if(MATCH(pat, p->name)) {
            return 1;
        }
    }

    return 0;
}

void cmd_gag(char *args)
{
    Gag *p;
    char temp[SMABUF];

    if(!*args) {
        cmd_listgag(NULL);

        return;
    }

    /* -gag_pattern will remove the gag */
    if(*args == '-') {
        cmd_ungag(args + 1);

        return;
    }

    if(find_gag(args)) {
        return;
    }

    p = (Gag *)malloc(sizeof(Gag));

    if(!p) {
        msg("-- Could not allocate memory for gag.");

        return;
    }

    if(!gag_head) {
        gag_head = p;
        gag_tail = p;
        p->next = NULL;
        p->prev = NULL;
    }
    else {
        gag_tail->next = p;
        p->prev = gag_tail;
        p->next (Gag *)NULL;
        gag_tail = p;
    }

    p->name = string(args);
    gag_update_index();
    msg("-- Gag(#%D): added \'%s\'.", p->indx, p->name);
    sprintf(tmp, "#%d", p->indx);
    add_assign("pid", temp);
}

/* Remove gag node from gag list by name. Formerly remove_gag */
void cmd_ungag(char *name)
{
    Gag *p;
    int val = 0;

    if(*name == '#') {
        val = atoi(name + 1);
        p = gag_head;

        while(p && (p->indx != val)) {
            p = p->next;
        }
    }
    else {
        p = find_gag(name);
    }

    if(!p) {
        msg("-- Gag %s was not defined.", name);

        return;
    }

    /* Head of list ? */
    if(!p->prev) {
        /* Not sole node */
        if(p->next) {
            gag_head = p->next;
            gag_head->prev = (Gag *)NULL;
        }
        else {
            gag_head = (Gag *)NULL;
            gag_tail = (Gag *)NULL;
        }
    }
    else if(!p->next) { /* End of list */
        gag_tail = p->prev;
        p->prev->next = (Gag *)NULL;
    }
    else { /* Middle of list */
        p->prev->next = p->next;
        p->next->prev = p->prev;
    }

    msg("-- Gag %s removed.", name);
    strfree(p->name);
    free(p);
    gag_update_index();
}

/* List gags by name */
void cmd_listgag(char *args)
{
    Gag *p;
    int i = 1;

    if(!gag_head) {
        msg("-- No gags defined.");

        return;
    }

    msg("-- Gags:");

    for(p = gag_head; p; p = p->next) {
        p->indx = i;
        msg("%3d) %s", i++, p->name);
    }

    msg("-- End of gag list.");
}

void gag_update_index(void)
{
    Gag *p;
    int i = 1;

    if(!gag_head) {
        return;
    }

    for(p = gag_head; p; p = p->next) {
        p->index = i++;
    }
}

void save_gags(FILE *fd)
{
    Gag *p;

    if(!gag_head) {
        return;
    }

    fprintf(fd, "\n#\n# Gags\n#\n");

    for(p = gag_head; p; p = p->next) {
        fprintf(fd, "gag %s\n", p->name);
    }
}

/* Erase all nodes from the gag list */
void cmd_cleargag(char *args)
{
    Gag *p;

    for(p = gag_head; p; p = p->next) {
        free(p->name);
        free(p);
    }

    gag_head = NULL;
    msg("-- All gags cleared.");
}

/* Find the gag node in the list by name */
Gag *find_gag(char *name)
{
    Gag *p;

    p = gag_head;

    while(p && !streq(name, p->name)) {
        p = p->next;
    }

    return p;
}

/* History routines */

/* Adds one line of len to the history list. Removing a node if necessary */
void add_history(char *line)
{
    if(*line == '\0') {
        return;
    }

    if(!hist_head) {
        hist_head = (Node *)malloc(sizeof(Node));
        hist_cur = hist_head;
        hist_cur->next = NULL;
        hist_cur->prev = NULL;
    }
    else {
        hist_cur->next = (Node *)malloc(sizeof(Node));
        hist_cur->next->prev = hist_cur;
        hist_cur = hist_cur->next;
        hist_cur->next = NULL;
    }

    hist_cur->line = string(line);
    ++num_hist;

    while(num_hist > max_history) {
        Node *temp;
        temp = hist_head;
        hist_head = hist_head->next;

        if(hist_head != NULL) {
            hist_head->prev = NULL;
        }

        free(temp->line);
        free(temp);
        --num_hist;
    }

    hist_recall = NULL;
}

void free_history(void)
{
    while(hist_head) {
        Node *temp;
        temp = hist_head;
        hist_head = hist_head->next;

        if(hist_head != NULL) {
            hist_head_prev = NULL;
        }

        free(temp->line);
        free(temp);
        --num_hist;
    }
}

/* Recall one line forward in the history list. ctrl-n is the usual key */
int recallf(char *line)
{
    if(!hist_head || !hist_recall || !hist_recall->next) {
        return FALSE;
    }

    hist_recall = hist_recall->next;
    strcpy(line, hist_recall->line);

    return TRUE;
}

/* Recall one line back in the history list. ctrl-p is the usual key */
int recallb(char *line)
{
    if(!hist_head) {
        return FALSE;
    }

    if(!hist_recall) {
        hist_recall = hist_cur;
    }
    else {
        if(!hist_recall->prev) {
            return FALSE;
        }

        hist_recall = hist_recall->prev;
    }

    strcpy(line, hist_recall->line);

    return TRUE;
}

/* Recall routines */

/* Recalls n lines from the recall buffer */
void recall(int n, int type)
{
    Node *temp;

    --n;

    for(temp = recall_cur; temp && n; temp = temp->prev) {
        if(type) {
            if(temp->type) {
                --n;
            }
        }
        else {
            --n;
        }
    }

    if(!temp) {
        temp = recall_head;
    }

    while(temp) {
        if(type) {
            if(temp->type) {
                msg("%s", temp->line);
            }
        }
        else {
            msg("%s", temp->line);
        }
        
        temp = temp->next;
    }

    msg("-- end of %s", (type ? "convo" : "recall"));
}

/* Recalls lines n to m from recall buffer */
void recall_n_m(int n, int m, int type)
{
    int dist;
    Node *p;

    dist = m - n;

    if(dist <= 0) {
        msg("-- recall must be non-zero.");

        return;
    }
    if((n < 0) || (m < 0)) {
        msg("-- recall arguments must be positive.");

        return;
    }

    for(p = recall_head; p && n; p->next) {
        if(type) {
            if(p->type) {
                --n;
            }
        }
        else {
            --n;
        }
    }

    while(p && dist) {
        if(type) {
            if(p->type) {
                msg("%s", p->line);
            }
        }
        else {
            msg("%s", p->line);
        }
        
        p = p->next;
        --dist;
    }

    msg("-- end of %s", (type ? "convo" : "recall"));
}

/* Adds line to the recall list and maintains num_recall nodes. */
void add_recall(char *line, int type)
{
    /* 0 is normal, 1/2 is broadcast/annouce */
    if((*line == '\n') || (*line == '\0') || !max_recall) {
        return;
    }

    if(!recall_head) {
        recall_head = (Node *)malloc(sizeof(Node));
        recall_cur = recall_head;
        recall_cur->next = NULL;
        recall_cur->prev = NULL;
    }
    else {
        recall_cur->next = (Node *)malloc(sizeof(Node));
        recall_cur->next->prev = recall_cur;
        recall_cur = recall_cur->next;
        recall_cur->next = NULL;
    }

    recall_cur->line = string(line);
    recall_cur->type = type;
    ++num_recall;

    while(num_recall > max_recall) {
        Node *temp;
        temp = recall_head;
        recall_head = recall_head->next;
        recall_head->prev = NULL;

        if(temp->line /* && *temp->line */) {
            free(temp->line);
        }

        free(temp);
        --num_recall;
    }
}

void free_recall(void)
{
    while(recall_head) {
        Node *temp;
        temp = recall_head;
        recall_head = recall_head->next;

        if(recall_head != NULL) {
            recall_head->prev = NULL;
        }

        if(temp->line) {
            free(temp->line);
        }

        free(temp);
        --num_recall;
    }
}

/*
 * Recalls all nodes from recall list matching <args> utilizes the
 * MATCH function.
 */
void recall_match(char *args, int type)
{
    Node *p;

    for(p = recall_head; p; p = p->next) {
        if(type && !p->type) {
            continue;
        }

        if(MATCH(p->line, args)) {
            msg("%s", p->line);
        }
    }

    msg("-- end of %s", (type ? "convo" : "recall"));
}

/*
 * This does the UNIx style ^xx^yy replace search. Only affects last
 * command and display "Modified failed." on a no match.
 */
void history_sub(char *args)
{
    char *p;
    char buf[BUFSIZ];
    char pat[BUFSIZ];

    p = strchr(args, '^');

    if(!p) {
        msg("-- No specified modifier.");

        return;
    }

    *p = '\0';
    sprintf(pat, "*%s*", args);

    if(!MATCH(hist_cur->prev->line, pat)) {
        msg("-- Modifier failed.");

        return;
    }
    else {
        sprintf(buf, "%s%s%s", pattern1, p + 1, pattern2);
        msg("%s", buf);
        procesS_key(buf, FALSE);
        add_history(buf);
    }
}

/* Macro routines */

/*
 * Find the macro node in the list by name or by index #. To request a
 * macro by index # the string must be #number
 */
Macro *find_macro(char *name)
{
    Macro *p;
    int val;

    if(*name == '#') {
        val = atoi(name + 1);
        p = macro_head;

        while(p && (p->indx != val)) {
            p = p->next;
        }
    }
    else {
        for(p = macro_head; p; p = p->next) {
            val = strcmp(name, p->name);

            if(!val) {
                return p;
            }
            else if(val < 0) {
                return NULL;
            }
        }

        return NULL;
    }

    return p;
}

/*
 * Place a macro on the list with corresponding action. A NULL action
 * will erase the name macro from the list
 */
void cmd_def(char *args)
{
    /* char *name; */
    /* char *action; */
    Macro *new;
    Macro *p;
    char *q;
    char *r;
    char *name;
    char *action;
    char fmtact[MAXSIZ];
    int carg = 1;
    int edit = FALSE;

    name = get_args(carg, 0);

    if(!*name) {
        cmd_listdef(NULL);

        return;
    }

    if(streq(name, "edit")) {
        msg("-- alias: %s is an invalid macro name since it is an option.", name);
        strfree(name);

        return;
    }
    else if(*name == '#') {
        msg("-- alias: macros may not start with %c due to their use in indexes.", *name);
        strfree(name);

        return;
    }
    else if(*name == '-') { /* -macro will delete the macro from the list */
        if(streq(name + 1, "edit")) {
            ++carg;
            edit = TRUE;
            strfree(name);
            name = get_args(carg, 0);
        }
        else {
            cmd_undef(name + 1);
            strfree(name);

            return;
        }
    }

    action = get_args(carg + 1, 100);
    p = find_macro(name);

    if(edit) {
        if(p) {
            sprintf(fmtact, "def %s %s", p->name, p->action);
            set_edit_buffer(fmtact);
        }
        else {
            msg("-- Alias: \'%s\' not found.", name);
        }

        strfree(name);
        strfree(action);

        return;
    }

    if(!*action) {
        if(p == NULL) {
            msg("-- Macros need a corresponding action.");
        }
        else {
            msg("-- def: %sd) %s = %s", p->indx, p->name, p->action);
        }

        strfree(name);
        strfree(action);

        return;
    }

    if(p) {
        remove_macro(name);
        p = NULL;
    }

    new = (Macro *)malloc(sizeof(Macro));

    if(!new) {
        msg("-- Could not allocat memory for macro.");

        return;
    }

    if(!macro_head) {
        macro_head = new;
        new->next = NULL;
        new->prev = NULL;
    }
    else {
        /* Add in, in strcmp order */
        p = macro_head;

        while(p->next && (strcmp(name, p->name) > 0)) {
            p = p->next;
        }

        /* Goes on end */
        if(!p->next && (strcmp(name, p->name) > 0)) {
            p->next = new;
            new->prev = p;
            new->next = (Macro *)NULL;
        }
        else { /* Goes in front of p */
            new->next = p;
            new->prev = p->prev;

            if(p->prev && p->prev->next) {
                p->prev->next = new;
            }

            p->prev = new;

            if(macro_head == p) {
                macro_head = new;
            }
        }
    }

    /* Put spaces around semi-colons to prevent arg problems later */
    for(r = fmtact, q = action; *q; ++q) {
        if(*q == ';') {
            if(*(q - 1) != ' ') {
                *r++ = ' ';
            }

            *r++ = ';';

            if(*(q + 1) != ' ') {
                *r++ = ' ';
            }
        }
        else {
            *r++ = *q;
        }
    }

    *r = '\0';

    strcpy(action, fmtact);
    new->name = name;
    new->action = action;
    new->flag = FALSE;
    def_update_index();
    msg("-- Alias(#%d): \'%s\' \'%s\'", new->indx, new->name, new->action);
    sprintf(fmtact, "#%d", new->indx);
    add_assign("pid", fmtact);
}

void cmd_undef(char *name)
{
    Macro *p;

    p = find_macro(name);

    if(p == NULL) {
        msg("-- No such macro (%s) defined.", name);
    }
    else {
        msg("-- Removed macro: %s", name);
        remove_macro(p->name);
    }
}

/* Remove macro node from macro list by name */
void remove_macro(char *name)
{
    Macro *p;

    p = find_macro(name);

    if(!p) {
        msg("-- Macro %s was not defined.", name);

        return;
    }

    if(!p->prev) {
        if(p->next) {
            macro_head = p->next;
            macro_head->prev = NULL;
        }
        else {
            macro_head = NULL;
        }
    }
    else if(!p->next) {
        p->prev->next = NULL;
    }
    else {
        p->prev->next = p->next;
        p->next->prev = p->prev;
    }

    free(p->action);
    free(p->name);
    free(p);
    def_update_index();
}

/* Upon matching name, do the corresponding action */
int do_macro(char *str)
{
    char named[BUFSIZ];
    char args[MAXSIZ];
    Macro *macro_ptr;
    char *r;
    char *d;
    char *p;
    int silent = 0;
    char *name = named;

    split(str, named, args);

    /* Silent macro */
    if(*name == '-') {
        silent = TRUE;
        ++name;
    }

    macro_ptr = find_macro(name);

    if(!macro) {
        return FALSE;
    }

    r = parse_macro_args(macro_ptr->action, args);
    d = r;

    if(silent) {
        add_queue("set display off", 1);
    }

    p = strchr(d, ';');

    while(p) {
        *p = '\0';
        add_queue(d, 1);
        d = p + 1;
        p = strchr(d, ';');
    }

    add_queue(d, 1);

    if(silent) {
        add_queue("set display on", 1);
    }

    strfree(r);

    return TRUE;
}

/* List macros by name */
void cmd_listdef(char *args)
{
    Macro *p;
    int i = 1;

    if(!macro_head) {
        msg("-- No aliases defined.");

        return;
    }

    msg("-- Aliases:");

    for(p = macro_head; p; p = p->next) {
        p->indx = i;
        msg("%3d) %s = %s", i++, p->name, p->action);
    }

    msg("-- End of aliases listing.");
}

void def_update_index(void)
{
    Macro *p;
    int i = 1;

    if(!macro_head) {
        return;
    }

    for(p = macro_head; p; p = p->next) {
        p->indx = i++;
    }
}

void save_defs(FILE *fd)
{
    Macro *p;

    if(!macro_head) {
        return;
    }

    fprintf(fd, "\n#\n# Macros\n#\n");

    for(p = macro_head; p; p = p->next) {
        fprintf(fd, "def %s %s\n", p->name, p->action);
    }
}

/* Erase all nodes from the macro list */
void cmd_cleardef(void)
{
    Macro *p;

    for(p = macro_head; p; p = p->next) {
        free(p->name);
        free(p->action);
        free(p);
    }

    macro_head = NULL;
}

/* Game routines */

/* Preps a line for possible adding to game list */
void cmd_game(char *args)
{
    char nick[BUFSIZ];
    char host[BUFSIZ];
    char port[BUFSIZ];
    char type[BUFSIZ];
    char sub1[BUFSIZ];
    char sub2[BUFSIZ];
    char sub3[BUFSIZ];
    char sub4[BUFSIZ];
    int cnt;

    memset(nick, '\0', sizeof(nick));
    memset(host, '\0', sizeof(host));
    memset(port, '\0', sizeof(port));
    memset(type, '\0', sizeof(type));
    memset(sub1, '\0', sizeof(sub1));
    memset(sub2, '\0', sizeof(sub2));
    memset(sub3, '\0', sizeof(sub3));
    memset(sub4, '\0', sizeof(sub4));

    if(!*args) {
        cmd_listgame();

        return;
    }

    cnd = sscanf(args,
                 "%s %s %s %s %s %s %s %s",
                 nick,
                 host,
                 port,
                 type,
                 sub1,
                 sub2,
                 sub3,
                 sub4);

    if(cnt > 3) {
        if(streq(type, "plain")) {
            switch(cnt) {
            case 5:
                *sub2 = '\0';
            case 6:
                *sub3 = '\0';
                *sub4 = '\0';
                /* args: nick , host, prot, type, racename, passwd, govname, passwd */
                add_game(nick, host, prot, type, sub3, sub1, sub4, sub2);

                break;
            default:
                msg("-- Error in add_game, plain auth.");

                break;
            }

            return;
        }
        else if(streq(type, "chap")) {
            if(cnt == 8) {
                add_game(nick, host, port, type, sub1, sub2, sub3, sub4);
            }
            else {
                msg("-- Error in add_gam, chap auth.");
            }

            break;
        }
        else {
            msg("-- Error in game line %s: login type should be \'plain\' or \'chap\'.", nick);
        }

        return;
    }
    else if(cnt == 3) {
        strcpy(type, "none"); /* No authentication -mfw */
        add_game(nick, host, port, type, sub1, sub2, sub3, sub4);

        return;
    }

    msg("-- Error in game line: incorrect number of args.");
}

/* Adds a game entry to the list. */
void add_game(char *nick,
              char *host,
              char *port,
              char *type,
              char *racename,
              char *pripasswd,
              char *govname,
              char *secpasswd)
{
    Game *p;

    p = find_game(nick);

    if(!p) {
        p = game_head;

        if(!game_head) {
            game_head = (Game *)malloc(sizeof(Game));
            game_head->next = NULL;
            game_head->prev = NULL;
            p = game_head;
        }
        else {
            while(p->next) {
                p = p->next;
            }

            p->next = (Game *)malloc(sizeof(Game));
            p->next->prev = p;
            p = p->next;
            p->next = NULL;
        }
    }
    else {
        p->nick = strfree(p->nick);
        p->host = strfree(p->host);
        p->port = strfree(p->port);
        p->type = strfree(p->type);
        p->racename = strfree(p->racename);
        p->pripasswod = strfree(p->prepassword);
        p->govname = strfree(p->govname);
        p->secpassword = strfree(p->secpassword);
    }

    p->nick = string(nick);
    p->host = string(host);
    p->port = string(port);
    p->type = string(type);
    p->racename = string(racename);
    p->pripassword = string(pripasswd);
    p->govname = string(govname);
    p->secpassword = string(secpasswd);

    if(streq(type, "chap")) {
        msg("-- Game: %s added. %s %s with CHAP authentication.",
            p->nick,
            p->host,
            p->port);
    }
    else if(streq(type, "plain")) {
        /*
         * msg("-- Game: %s added. %s %s with password: %s %s",
         *     p->nick,
         *     p->host,
         *     p->port,
         *     p->pripasswod,
         *     p->secpassword);
         */
        msg("-- Game: %s added. %s %s with plain-text authentication.",
            p->nick,
            p->host,
            p->port);
    }
    else if(streq(type, "none")) {
        msg("-- Game: %s added. %s %s with no authentication method.",
            p->nick,
            p->host,
            p->port);
    }

    game_update_index();
}

void free_game(void)
{
    while(game_head) {
        Game *temp;
        temp = game_head;
        game_head = game_head->next;

        if(game_head != NULL) {
            game_head->prev = NULL;
        }

        free(temp->nick);
        free(temp->host);
        free(temp->port);
        free(temp->type);
        free(temp->racename);
        free(temp->pripassword);
        free(temp->govname);
        free(temp->secpassword);
        free(temp);
    }
}

void cmd_ungame(char *args)
{
    Game *p;

    p = find_game(args);

    if(!p) {
        msg("-- No such game nick %s found.", args);

        return;
    }

    if(!p->prev) {
        if(p->next) {
            game_head = p->next;
            game_head->prev = NULL;
        }
        else {
            game_head = NULL;
        }
    }
    else if(!p->next) {
        p->prev->next = NULL;
    }
    else {
        p->prev->next = p->next;
        p->next->prev = p->prev;
    }

    msg("-- Game %s removed.", args);
    free(p->nick);
    free(p->host);
    free(p->port);
    free(p->type);
    free(p->racename);
    free(p->pripassword);
    free(p->govname);
    free(p->secpassword);
    free(p);
    game_update_index();
}

/* Attempts to find the specified game. Returns NULL if not found */
Game *find_game(char *nick)
{
    Game *gp = game_head;

    while(gp) {
        if(streq(gp->nick, nick)) {
            return gp;
        }
        
        gp = gp->next;
    }

    return NULL;
}

/* Lists the games in the list */
void cmd_listgame(void)
{
    Game *p = game_head;
    int i = 1;

    if(!game_head) {
        msg("-- No games defined.");

        return;
    }

    msg("-- Games:");

    while(p) {
        p->indx = i;

        msg("%2d) %s %s %s %s %s",
            i++,
            p->nick,
            p->host,
            p->port,
            p->pripassword,
            p->secpassword);
        
        p = p->next;
    }

    msg("-- End games list.");
}

void game_update_index(void)
{
    Game *p;
    int i = 1;

    if(!game_head) {
        return;
    }

    for(p = game_head; p; p = p->next) {
        p->indx = i++;
    }
}

/*
 * Saves the current listing of games in a readable format by loadf to
 * the file descriptor provided to the function.
 */
void save_games(FILE *fd)
{
    Game *g = game_head;

    if(!game_head) {
        return;
    }

    fprintf(fd, "\n#\n# List of Current Games\n#\n");

    while(p) {
        fprintf(fd,
                "game %s %s %s %s %s\n",
                p->nick,
                p->host,
                p->port,
                p->pripassword,
                p->secpassword);
        
        p = p->next;
    }
}

/* Send the primary and secondary passwords as needed */
void send_password(void)
{
    char pass[BUFSIZ];

    debug(1, "send_password() type: %s", cur_game.game.type);

    if(streq(cur_game.game.type, "plain")) {
        sprintf(pass,
                "%s %s\n",
                cur_game.game.pripassword,
                cur_game.game.secpassword);

        send_gb(pass, strlen(pass));
    }
    else if(streq(cur_game.game.type, "chap")) {
        /* Do nothing, handled in connect_promps() -mfw */
    }
    else {
        msg("-- Error in send_password");
    }
}

/* Queue routines */

/* Adds a line to the queue list for later processing */
void add_queue(char *args, int wait)
{
    Node *p;

    if((*args == '\n') || streq(args, "")) {
        return;
    }

    p = (Node *)malloc(sizeof(Node));

    if(!p) {
        quit_gb(-2, "-- Fatal. Could not allocate memory for queue.", NULL, NULL);
    }

    if(!queue_head) {
        queue_head = p;
        queue_tail = p;
        p->next = NULL;
        p->prev = NULL;
    }
    else {
        p->next = queue_head;
        queue_head->prev = p;
        p->prev = NULL;
        queue_head = p;
    }

    p->line = string(args);
    p->type = wait;
    debug(3, "add_queue: \'%s\' added.", p->line);
}

/* Removes the first item in the queue and returns it in args */
void remove_queue(char *args)
{
    Node *p = queue_tail;

    if(!queue_head) {
        *args = '\0';

        return;
    }

    if(p->prev) {
        p->prev->next = NULL;
    }

    /* No more on queue, lock queue for ctrl-c. It gets reset in process_queue() */
    if(queue_tail == queue_head) {
        debug(2, "remove_queue: secondary queue is empty");
        queue_head = NULL;
        queue_tail = queue_head;
    }
    else {
        queue_tail = p->prev;
    }

    debug(2, "remove_queue: returning %s", p->line);
    strcpy(args, p->line);
    free(p->line);
    free(p);
}

void process_queue(char *s)
{
    sec_queue_tail = queue_tail;
    sec_queue_head = queue_head;
    queue_head = NULL;

    debug(3, "process_queue: %s", s);
    queue_sending = TRUE;
    process_key(s, FALSE);
    queue_sending = FALSE;

    /* Old queue exists, place back in line, q is front of old */
    if(sec_queue_tail) {
        sec_queue_tail->next = queue_head;
    }

    /* New queue exists, link back to fron of old queue */
    if(queue_head) {
        queue_head->prev = sec_queue_tail;
    }

    /* p is back of old. It exists and therefore is end of total queue */
    if(sec_queue_head) {
        queue_head = sec_queue_head;
    }
}

/*
 * Returns TRUE if the queue exists, otherwise FALSE. Returns TRUE
 * also if we are not connected to a gb server so that the queue
 * doesn't block output on other connections (or lack of connections).
 */
int check_queue(void)
{
    extern int is_connected();

    /* No queue */
    if(!queue_head) {
        return FALSE;
    }

    /*
     * Is socket connected? If no, TRUE.
     * Is it gb? If no, then TRUE.
     * Are we doing queue? If no, then TRUE.
     */
    if(is_connected() && !NOTGB() && !do_queue) {
        return FALSE;
    }

    /* Else do the queue */
    return TRUE;
}

int have_queue(void)
{
    if(queue_head || sec_queue_head) {
        return TRUE;
    }

    return FALSE;
}

int do_clear_queue(void)
{
    if(queue_head && queue_clear) {
        return TRUE;
    }

    return FALSE;
}

/* Removes all nodes from the queue */
void clear_queue(void)
{
    char buf[MAXSIZ];

    while(queue_head) {
        remove_queue(buf);
    }

    queue_clear = FALSE;
}

/* News routines */

/* Check to see if news command has started, and build first node */
void check_news(char *s)
{
    bulletin = 0;

    if(streq(s, "The Galactic News")) {
        ICOMM_STATE = S_PROC;
        ICOMM_IGNORE = TRUE;
    }
    else if(*s == '-') {
        ICOMM_STATE = S_PROC;
        ICOMM_IGNORE = TRUE;
    }
    else if(streq(s, "<Output Flushed>")) {
        bulletin = 1;
        ICOMM_STATE = S_PROC;
        ICOMM_IGNORE = TRUE;
    }

    /*
     * This is here in case the News name changes like is has on some
     * games, then at least hopefully news will be parsed correctly
     */
    rhead = (RNode *)malloc(sizeof(RNode));
    rlast = rhead;
    rlast->next = NULL;
    rlast->prev = NULL;
    rlast->line = string(s);
    rlast->date = string("-");
    rlast->count = 1;
}

int add_news(char *s)
{
    RNode *p;
    char date[10];
    char time[9];
    char line[MAXSIZ];
    int idummy;

    *date = '\0';
    *time = '\0';

    if(*s == '\0') {
        return 1;
    }

    if(ICOMM_STATE != S_PROC) {
        return 0;
    }

    if(*s == '-') {
        strcpy(date, "-");
        strncpy(line, s, MAXSIZ);
        ++bulletin;
    }
    else {
        if(sscanf(s, "%2d/ %d %2d:%2d:%2d %*s", &idummy, &idummy, &idummy, &idummy, &idummy) < 5) {
            *date = 'c';
            strncpy(line, s, MAXSIZ);
        }
        else {
            strncpy(date, s, 5); /* 5 because "MM/DD") */
            date[5] = '\0'; /* Just in case */
            strncpy(time, s + 6, 5);
            time[5] = '\0';
            strncpy(line, s + 15, MAXSIZ);
        }
    }

    line[MAXSIZ - 1] = '\0';

    p = find_news(date, line);

    if(((bulletin != 4) || streqrn(line, "Server") || streqrn(line, "Shutdown")) && p) {
        ++(p->count);
        strcpy(p->ltime, time);

        return 1;
    }

    rlast->next = (RNode *)malloc(sizeof(RNode));

    if(!rlast->next) {
        msg("-- malloc error in read news. Aborting.");

        return 0;
    }

    rlast->next->prev = rlast;
    rlast = rlast->next;
    rlast->next = NULL;
    rlast->line = string(line);
    rlast->date = string(date);
    strcpy(rlast->ftime, time);
    strcpy(rlast->ltime, time);
    rlast->count = 1;

    return 1;
}

RNode *find_news(char *date, char *line)
{
    RNode *p;

    p = rhead;

    while(p && (!streq(date, p->date) || !streq(line, p->line))) {
        p = p->next;
    }

    return p;
}

void print_news(void)
{
    RNode *p;
    char buf[BUFSIZ];

    for(p = rhead; p; p = p->next) {
        if(*p->date == '-') {
            sprintf(buf, "%s", p->line);
            msg_type = MSG_NEWS;
            process_socket(buf);
            msg_type = MSG_NEWS;
            process_socket("");
        }
        else if(*p->date == 'c') {
            check_for_special_formatting(buf, FORMAT_NORMAL);
            msg_type = MSG_NEWS;
            process_socket(p->line);
        }
        else {
            if(p->count > 1) {
                sprintf(buf,
                        "%s (%s-%s) %s [%d times]",
                        p->date,
                        p->ftime,
                        p->ltime,
                        p->line,
                        p->count);
            }
            else {
                sprintf(buf, "%s (%s      ) %s", p->date, p->ftime, p->line);
            }

            check_for_special_formatting(buf, FORMAT_NORMAL);
            msg_type = MSG_NEWS:
            process_socket(buf);
        }

        free(p->date);
        free(p->line);
    }

    p = rhead;

    while(p) {
        p = p->next;
        free(rhead);
        rhead = p;
    }
}
