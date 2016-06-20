/*
 * args.c: Does argument substitution for strings
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1992-1993
 *
 * See the COPYRIGHT file
 */

#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <sys/types.h>

#include "gb.h"
#include "proto.h"
#include "str.h"
#include "types.h"
#include "vars.h"

#define LOOPNAME "__internal_loop"
#define OPEN_BRACKET '{'
#define END_BRACKET '}'

#define is_valid_assign_char(C) (isalnum(C) || (C) == '_')

static ArrStruct main_arg_list;
static ArrStruct macro_arg_list;
static ArrStruct sec_arg_list;
static ArrStruct *arg_list = &main_arg_list;

Assign *assign_head = NULL;

/* For the parsing of variables */
/* static char assign_buf[BUFSIZ]; */
/* static char *assign_ptr; */

int assign_flag = TRUE;

Assign *find_assign(char *name);
char *get_assign_unit();
char *get_assign_identifier();
char *get_argify(char *fmt);
char *get_args(int lo, int hi);
char *get_assign(char *name);
void argify(char *list);
void add_assign(char *name, char *value);
void remove_assign(char *name);
int valid_assign_name(char *name);
int return_range(char *s, int *low, int *high, int *len);

extern int strncmp(const char *, const char *, size_t);
extern int fprintf(FILE *, const char *, ...);
extern int fputc(int, FILE *);
extern int sscanf(const char *, const char *, ...);
extern int atoi(const char *);

/*
 * parse_given_string: Takes a string (s) and based on the specified
 * mode (PARSE_SLASH, PARSE_VARIABLES, PARSE_ALL) parses thru the
 * string, changing based on mode. It returns a pointer to the changed
 * string, and if no changes were made, then it returns a NULL
 * pointer.
 */
char *parse_given_string(char *s, int mode)
{
    static char parsedstring[MAXSIZ + 1];
    char *p;
    char *q;
    int made_change = FALSE;

    p = s;
    q = parsedstring;

    debug(3, "In parse_given_string (%d)", mode);

    while(*p) {
        if((*p == '\\')
           && ((mode == PARSE_SLASH)
               || (mode == PARSE_SLASH_NOTNL)
               || (mode == PARSE_ALL))) {
            ++p;
            made_change = TRUE;

            switch(*p) {
            case '\\':
                *q++ = '\\';
                ++p;

                break;
            case 'n':
                if(mode == PARSE_SLASH_NOTNL) {
                    *q++ = '\\';
                    *q++ = 'n';
                }
                else {
                    *p++ = '\n';
                }

                ++p;

                break;
            default:
                *q++ = *p++;
            }
        }
        else {
            *q++ = *p++;
        }
    }

    *q = '\0';

    debug(4,
          "parse_given_string: %s (%d)",
          (made_change ? parsedstring : s),
          made_change);

    if(!made_change) {
        return s;
    }

    return parsedstring;
}

void parse_variables(char *s)
{
    char *p;
    char *q;
    char *n;
    char *r;
    char *x;
    char name[MAXSIZ];
    char out[MAXSIZ];
    char temp[MAXSIZ];
    int nest = 0;
    int made_change = TRUE;

    if(!*s) {
        return;
    }

    while(made_change) {
        p = s;
        q = out;
        made_change = FALSE;

        while(*p) {
            if(*p == VAR_CHAR) {
                ++p;

                /* Put 1 $ */
                if(*p == VAR_CHAR) {
                    *q++ = VAR_CHAR;
                    *q++ = *p++;
                }
                else { /* Is it a variable ? */
                    if(*p == OPEN_BRACKET) {
                        n = name;
                        ++nest;
                        x = p;
                        ++p;

                        while(*p && nest) {
                            if(*p == OPEN_BRACKET) {
                                ++nest;
                            }
                            else if(*p == END_BRACKET) {
                                --nest;
                            }

                            *n++ = *p++;
                        }

                        --n;
                        *n = '\0';
                        r = get_assign(name);

                        if(r) {
                            strcpy(q, r);
                            q += strlen(r);
                            made_change = TRUE;
                        }
                        else {
                            p = x;
                            *q++ = '$';
                            *q++ = *p++;
                        }
                    }
                    else {
                        n = name;
                        r = p;

                        while(*r && is_valid_assign_char(*r)) {
                            *n++ = *r++;
                        }

                        *n = '\0';
                        n = get_assign(name);

                        /* Valid assign name */
                        if(n) {
                            strcpy(q, n);
                            q += strlen(n);
                            made_change = TRUE;
                            p = r;
                        }
                        else { /* Invalid assign name */
                            sprintf(temp, "$%s", name);
                            strcpy(q, temp);
                            q += strlen(temp);
                            p = r;
                        }
                    }
                }
            }
            else {
                *q++ = *p++;
            }
        }

        *q = '\0';
        strcpy(s, out);
    }
    
    /* now parse out any double $'s */
    p = s;
    q = out;
    
    while(*p) {
        if(*p == VAR_CHAR) {
            ++p;
            
            /* Put 1 $ */
            if(*p == VAR_CHAR) {
                *q++ = *p++;
            }
            else { /* Is it a variable? */
                *q++ = VAR_CHAR;
                *q++ = *p++;
            }
        }
        else {
            *q++ = *p++;
        }
    }
    
    *q = '\0';
    strcpy(s, out);
}

/*
 * Assign the pre-defined variables so that their names are set by the
 * client and not the user, hence reserving the name for our use
 * new_conn: if TRUE it reset all assign variables, otherwise we are
 * reconnecting to our previous game, so retain information. (like
 * builtship, etc.).
 */
void init_assign(int new_conn)
{
    if(new_conn) {
        add_assign("aps", "0");
        add_assign("builtship", "0");
        add_assign("govid", "0");
        add_assign("govname", "none");
        add_assign("lotnum", "0");
        add_assign("mothership", "");
        add_assign("planet", "");
        add_assign("pripassword", "none");
        add_assign("raceid", "0");
        add_assign("racename", "none");
        add_assign("secpassword", "none");
        add_assign("ship", "");
        add_assign("star", "");
    }

    add_assign("connected", "FALSE");
    add_assign("game_type", "UNKNOWN");
    add_assign("game_nick", "none");
    add_assign("host", "none");
    add_assign("num_args", "0");
    add_assign("pid", "0");
    add_assign("port", "0");
    add_assign("scope", "Not Connected");
    add_assign("scope_level", "0");
}

void add_assign(char *name, char *value)
{
    Assign *p;

    p = find_assign(name);

    /* New assignment */
    if(!p) {
        if(!assign_head) {
            assign_head = (Assign *)malloc(sizeof(Assign));
            assign_head->prev = (Assign *)NULL;
            assign_head->next = (Assign *)NULL;
            p = assign_head;
        }
        else {
            p = assign_head;
            
            while(p->next) {
                p = p->next;
            }
            
            p->next = (Assign *)malloc(sizeof(Assign));
            p->next->next = (Assign *)NULL;
            p->next->prev = p;
            p = p->next;
        }
    }
    else { /* Old assignment */
        if(p->mode && !assign_flag) {
            msg("-- Assign: you can not redefine that variable.");

            return;
        }

        p->name = strfree(p->name);
        p->str = strfree(p->str);
    }

    p->name = string(name);
    p->str = string(value);
    p->mode = assign_flag;
    debug(2, "Assign %s (%s)", p->name, p->str);
}

void free_assign(void)
{
    while(assign_head) {
        Assign *temp;
        temp = assign_head;
        assign_head = assign_head->next;

        if(assign_head != NULL) {
            assign_head->prev = NULL;
        }

        if(temp->name) {
            free(temp->name);
        }

        if(temp->str) {
            free(temp->str);
        }

        free(temp);
    }
}

Assign *find_assign(char *name)
{
    Assign *p = assign_head;

    while(p && !streq(name, p->name)) {
        p = p->next;
    }

    return p;
}

void cmd_listassign(char *args)
{
    Assign *p = assign_head;
    int show_all = FALSE;

    if(args && streq(args, "-")) {
        show_all = TRUE;
    }

    msg("-- Assign list:");

    while(p) {
        if(p->mode) {
            if(show_all) {
                msg(" * %s = %s", p->name, p->str);
            }
        }
        else {
            msg("   %s = %s", p->name, p->str);
        }
        
        p = p->next;
    }

    msg("-- assign list done.");
}

/*
 * get_assign: Finds the given name in the assign list.
 *
 * args:
 *     name - Character string of the variable name to look for if the variable
 *            begins with a $, assumes is another variable and calls recursively
 *            until a string is found.
 *
 * return:
 *     Pointer to the value associated with the name otherwise it returns a null
 *     pointer
 */
char *get_assign(char *name)
{
    Assign *p;

    p = find_assign(name);

    if(!p) {
        debug(3, "get_assign (%s) found_nothing.", name);

        return (char *)NULL;
    }
    else {
        debug(3, "get_assign (%s) found: '%s'", name, p->str);

        return (char *)p->str;
    }
}

void cmd_assign(char *args)
{
    char *aptr;
    char *rptr;

    aptr = get_args(1, 1);

    if(!*aptr || streq(aptr, "-")) {
        cmd_listassign(aptr);
        strfree(aptr);

        return;
    }

    if(*aptr == '-') {
        assign_flag = 0;
        remove_assign(aptr + 1);
        assign_flag = 1;
        strfree(aptr);

        return;
    }

    rptr = get_args(2, MAX_NUM_ARGS);

    /*
     * An override, just in case also the ^ does not generate a msg which
     * cmd_for takes advantage of
     */
    if((*aptr == '^') && valid_assign_name(aptr + 1)) {
        assign_flag = 1;
        add_assign(aptr + 1, rptr);
    }
    else if(streqrn(aptr, "__")) {
        msg("-- Assign: Invalid name (%s). __ is reserved for internal usage.", aptr);
        strfree(aptr);
        strfree(rptr);

        return;
    }
    else if(valid_assign_name(aptr)) {
        assign_flag = 0;
        add_assign(aptr, rptr);
        assign_flag = 1;
        msg("-- Assign: '%s' was assigned '%s'", aptr, rptr);
    }
    else {
        msg("-- Assign: Invalid name (%s). Valid names can contain only characters, numbers, or underscore. And must not be a number", aptr);
        strfree(aptr);
        strfree(rptr);

        return;
    }

    strfree(aptr);
    strfree(rptr);
}

int valid_assign_name(char *name)
{
    char *p;

    /* Assume true until proven otherwise */
    int is_num = TRUE;

    for(p = name; *p; ++p) {
        if(!is_valid_assign_char(*p)) {
            return FALSE;
        }

        if(!isdigit(*p) && is_num) {
            is_num = FALSE;
        }
    }

    if(is_num) {
        return FALSE;
    }

    return TRUE;
}

void remove_assign(char *name)
{
    Assign *p;
    int override = FALSE;

    if(*name == '^') {
        override = TRUE;
        ++name;
    }

    p = find_assign(name);

    if(!p) {
        return;
    }

    if(p->mode && !assign_flag && !override) {
        msg("-- Assign: '%s' can not be removed by you.", name);

        return;
    }

    if(!p->prev) {
        if(p->next) {
            assign_head = p->next;
            assign_head->prev = NULL;
        }
        else {
            assign_head = NULL;
        }
    }
    else if(!p->next) {
        p->prev->next = NULL;
    }
    else {
        p->prev->next = p->next;
        p->next->prev = p->prev;
    }

    free(p->name);
    free(p->str);
    free(p);
}

int test_assign(char *name)
{
    Assign *p;

    p = find_assign(name);

    if(p) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void save_assigns(FILE *fd)
{
    Assign *p = assign_head;
    int changed = FALSE;
    char *q;

    while(p) {
        if(!changed && !p->mode) {
            fprintf(fd, "\n#\n# Assign variables\n#\n");
            changed = TRUE;
        }

        if(!p->mode) {
            fprintf(fd, "assign %s ", p->name);

            for(q = p->str; *q; ++q) {
                if(*q == '$') {
                    fputc(*q, fd);
                }

                fputc(*q, fd);
            }

            fputc('\n', fd);
        }
        
        p = p->next;
    }

    if(changed) {
        fprintf(fd, "\n");
    }
}

/*
 * parse_args_from_string: 
 * 
 * Takes a formatting string which contains the $ marked numerical arguments to
 * substitute from the string list. $ marked arguments can be in one of four
 * forms as follows: 
 * $X or ${X} which means substitute with argument X from the string list 
 * $-X or ${-X} which means substitute from argument 0 through X
 * $X-$Y or ${X-Y} which means substitute from argument X through Y 
 * $X- or ${X-} which means substitute from argument X through the end of the list
 *
 * Passing a NULL list will cause the function to use the contents of the static
 * arg_list. this is so repeated uses of the same list can be done efficiently,
 * without the list being divided into arguments excessively.
 *
 * The function returns a malloc'ed string with the changed line.
 */
char *parse_macro_args(char *fmt, char *list)
{
    char *p;
    char temp[SMABUF];

    arg_list = &marco_arg_list;
    argify(list);
    p = get_argify(fmt);
    sprintf(temp, "%d", macro_arg_list.arg_cnt);
    add_assign("macro_num_args", temp);
    arg_list = &main_arg_list;

    return p;
}

char *parse_sec_args(char *fmt, char *list)
{
    char *p;
    char tmp[SMABUF];

    args_list = &sec_arg_list;
    argify(list);
    p = get_argify(fmt);
    sprintf(temp, "%d", sec_arg_list.arg_cnt);
    add_assign("sec_num_args", temp);
    arg_list = &main_arg_list;

    return p;
}

void argify(char *list)
{
    char temp[SMABUF];
    char *p;
    char *q;
    int i;
    char quotes;

    if(list) {
        p = list;
        q = arg_list->arr[0];
        arg_list->arg_cnt = 0;

        while(*p && (arg_list->arg_cnt < (MAX_NUM_ARGS - 1))) {
            while(isspace(*p)) {
                ++p;
            }

            if(((*p == '\"') || (*p == '\'')) && (*(p - 1) != '\\')) {
                quotes = *p;
                ++p;

                while(*p && !((*p == quotes) && (*(p - 1) != '\\'))) {
                    *q++ = *p++;
                }

                ++p;
            }
            else {
                while(!isspace(*p) && *p) {
                    *q++ = *p++;
                }
            }

            *q = '\0';
            debug(4, "arg (%d): %s", arg_list->arg_cnt, arg_list->arr[arg_list->arg_cnt]);

            /* Finished one line, set up for next */
            ++arg_list->arg_cnt;
            q = arg_list->arr[arg_list->arg_cnt];
        }

        if(*p && (arg_list->arg_cnt == (MAX_NUM_ARGS - 1))) {
            while(isspace(*p)) {
                ++p;
            }

            strcpy(arg_list->arr[arg_list->arg_cnt], p);
            debug(4, "arg final (%d): %s", arg_list->arg_cnt, arg_list->arr[arg_list->arg_cnt]);

            /* Increment to reflect cnt */
            ++arg_list->arg_cnt;
        }

        for(i = arg_list->arg_cnt + 1; i < MAX_NUM_ARGS; ++i) {
            *arg_list->arr[i] = '\0';
        }
    }

    sprintf(temp, "%d", arg_list->arg_cnt);
    add_assign("num_args", temp);
    debug(3, "parse_args_from_string, num_args: %d", arg_list->arg_cnt);
}

char *get_argify(char *fmt)
{
    char *parsedstring;
    char *p;
    char *q;
    int skiplen;
    int low;
    int high;
    int i;

    parsedstring = (char *)malloc(MAXSIZ + 1);

    if(!parsedstring) {
        msg("-- Malloc: failed in get_argify. Aborting.");

        return NULL_STRING;
    }

    p = fmt;
    q = parsedstring;

    debug(4, "entering get_argify: %s", p);

    while(*p) {
        if(*p == '$') {
            ++p;

            switch(*p) {
            case '$':
                *q++ = '$';
                ++p;
                made_change = TRUE;

                break;
            default:
                switch(return_range(p, &low, *high, &skiplen)) {
                case TRUE:
                    p += skiplen;
                    debug(4, "remaining: '%s'", p);

                    if(high > (arg_list->arg_cnt - 1)) {
                        high = arg_list->arg_cnt - 1;
                    }

                    if(low > (arg_list->arg_cnt - 1)) {
                        break;
                    }

                    debug(4, "range (%d,%d) %d", low, high, skiplen);

                    for(i = low; i <= high; ++i) {
                        strcpy(q, arg_list->arr[i]);
                        q += strlen(arg_list->arr[i]);
                        *q++ = ' ';
                    }

                    --q;
                    made_change = TRUE;

                    break;
                case FALSE:
                    *q++ = '$';
                    *q++ = *p++;

                    break;
                case ERROR:
                    p += skiplen;

                    break;
                }

                break;
            }
        }
        else {
            *q++ = *p++;
        }
    }

    *q = '\0';
    debug(3, "get_argify returning: %s", parsedstring);

    return parsedstring;
}

int return_range(char *s, int *low, int *high, int *len)
{
    char *p;
    char *ptr;
    char name[BUFSIZ];
    int end_bracket = FALSE;
    int l = 0;
    int h = MAX_NUM_ARGS;

    if(*s == '{') {
        p = strchr(s, '}');

        if(p) {
            *p = '\0';
            end_bracket = TRUE;
            ptr = first(s + 1);
            *len = strlen(ptr) + 2;
        }
        else {
            return FALSE;
        }
    }
    else {
        ptr = first(s);
        *len = strlen(ptr);
    }

    debug(4, "skiplen for return_range is: %d", *len);

    /* Isolate the word and put it in name */
    if(ptr) {
        strcpy(name, ptr);
    }
    else {
        *name = '\0';
    }

    if(streq(name, "*")) {
        *low = 0;
        *high = MAX_NUM_ARGS;
    }
    else if(sscanf(name, "%d-%d", &l, &h) == 2) {
        *low = l;
        *high = h;
    }
    else if(sscanf(name, "-%d", &h) == 1) {
        *low = 0;
        *high = h;
    }
    else if(sscanf(name, "%d", &l) == 1) {
        *low = 1;

        if(strchr(name, '-')) {
            *high = MAX_NUM_ARGS;
        }
        else {
            *high = l;
        }
    }
    else if(sscanf(name, "%d-", &l) == 1) {
        *low = l;
        *high = MAX_NUM_ARGS;
    }
    else {
        if(end_bracket) {
            *p = '}';
        }

        return FALSE;
    }

    /* Do the calculation here */
    if(end_bracket) {
        *p = '}';
    }

    /* Consistency checks for large args request by used, just abort */
    if((*low < 0) || (*high < 0)) {
        return ERROR;
    }

    return TRUE;
}

char *get_args(int lo, int hi)
{
    char temp[SMABUF];
    char *p;

    if(hi <= lo) {
        sprintf(temp, "${%d}", lo);
    }
    else {
        sprintf(temp, "%{%d-%d}", lo, hi);
    }

    p = get_argify(temp);
    debug(4, "get_args returning(%s): '%s'", temp, p);

    return p;
}

void strcap(char *s, char c)
{
    int len;

    len = strlen(s);
    *(s + len) = c;
    *(s + len + 1) = '\0';
}

int parse_for_loops(char *s)
{
    char *p;
    char *q;
    char *r;
    char buf[MAXSIZ];
    int made_change = FALSE;
    int lo;
    int hi;
    int loocnt = 0;

    /* Format is: $[lo-hi] or $[hi-lo] */
    p = s;
    r = strchr(p, '$');

    while(r) {
        p = r + 1;

        /* Multiple $, skip over this */
        if(*p == '$') {
            while(*p == '$') {
                ++p;
            }

            ++p;
        }

        /* Not $[ so continue */
        if(*p != '[') {
            continue;
        }

        ++p;
        q = p;

        if(*q == '-') {
            if(isdigit(*(q + 1))) {
                ++q;
            }
            else {
                continue;
            }
        }

        /* Skip digits */
        while(isdigit(*q) && (*q != '-')) {
            ++q;
        }

        /* Format error */
        if(*q != '-') {
            continue;
        }

        lo = atoi(p);
        ++q;
        p = q;

        if(*q == '-') {
            if(isdigit(*(q + 1))) {
                ++q;
            }
            else {
                continue;
            }
        }

        /* Skip 2nd set of digits */
        while(isdigit(*q)) {
            ++q;
        }

        /* Format error */
        if(*q != ']') {
            continue;
        }

        hi = atoi(p);
        p = q + 1;

        /* Made it this far, then lo and hi are set up */
        debug(4, "for_loop: %d,%d", lo, hi);
        *r = '\0';

        /* Get a unique loop name */
        ++loopcnt;
        sprintf(buf, "%s%d", LOOPNAME, loopcnt);

        while(test_assign(buf)) {
            ++loopcnt;
            sprintf(buf, "%s%d", LOOPNAME, loopcnt);
        }

        debug(4, "cnt: %d, s = '%s', p = '%s'", loopcnt, s, p);

        sprintf(buf,
                "for -nooutput %s%d %d,%d %s{%s%d}%s",
                LOOPNAME,
                loopcnt,
                lo,
                hi,
                s,
                LOOPNAME,
                loopcnt,
                p);

        ++loopcnt;
        debug(4, "new loop: %s", buf);
        strcpy(s, buf);
        made_change = TRUE;
        p = s;
        
        r = strchr(p, '$');
    }

    return made_change;
}
