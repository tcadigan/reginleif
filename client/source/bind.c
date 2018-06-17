/*
 * bind.c: Handles the key bindings and interpretation
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1992
 *
 * See the COPYRIGHT file.
 */
#include "bind.h"

#include "gb.h"
#include "icomm.h"
#include "imap.h"
#include "key.h"
#include "str.h"
#include "vars.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* Typedefs */
typedef struct bindkey {
    char *cptr;
    void (*func) (char);
    int is_string;
    int has_changed;
} BindKey;

typedef struct bindname {
    char *name;
    void (*func) (char);
} BindName;

char convert_string(char *s, int *mode);
char *display_char(char c, int mode);
void bind_translate_char(signed char c , int mode);
void list_binding(int listkey, int mode);
void more_quit(char ch);
void more_clear(char ch);
void more_cancel(char ch);
void more_nonstop(char ch);
void more_forward(char ch);
void more_oneline(char ch);
void bind_underline(char ch);
void bind_boldface(char ch);
void bind_bell(void);
void bind_inverse(char ch);
void free_bindings(void);

static int bind_changed = false;

/* Normal keys nothing fancy */
static BindKey BindNormKeys[] = {
    { NULL,   input_ch_into_buf, 0, 0 }, /* ^@ (null)     0 */
    { NULL,        cursor_begin, 0, 0 }, /* ctrl-a          */
    { NULL,     cursor_backward, 0, 0 }, /* ctrl-b          */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ctrl-c          */
    { NULL, delete_under_cursor, 0, 0 }, /* ctrl-d          */
    { NULL,          cursor_end, 0, 0 }, /* ctrl-e        5 */
    { NULL,      cursor_forward, 0, 0 }, /* ctrl-f          */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ctrl-g          */
    { NULL,           backspace, 0, 0 }, /* ctrl-h          */
    { NULL,        do_edit_mode, 0, 0 }, /* ctrl-i          */
    { NULL,      handle_key_buf, 0, 0 }, /* ctrl-j       10 */
    { NULL,    kill_to_end_line, 0, 0 }, /* ctrl-k          */
    { NULL,      refresh_screen, 0, 0 }, /* ctrl-l          */
    { NULL,      handle_key_buf, 0, 0 }, /* ctrl-m          */
    { NULL,          do_recallf, 0, 0 }, /* ctrl-n          */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ctrl-o       15 */
    { NULL,          do_recallb, 0, 0 }, /* ctrl-p          */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ctrl-q          */
    { NULL,       refresh_input, 0, 0 }, /* ctrl-r          */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ctrl-s          */
    { NULL,     transpose_chars, 0, 0 }, /* ctrl-t       20 */
    { NULL,        cancel_input, 0, 0 }, /* ctrl-u          */
    { NULL,           quote_key, 0, 0 }, /* ctrl-v          */
    { NULL,    delete_word_left, 0, 0 }, /* ctrl-w          */
    { NULL,   delete_word_right, 0, 0 }, /* ctrl-x          */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ctrl-y       25 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ctrl-z          */
    { NULL,          escape_key, 0, 0 }, /* ^[ (ESC)        */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ^\              */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ^]              */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ^^           30 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ^_              */
    { NULL,   input_ch_into_buf, 0, 0 }, /* space bar       */
    { NULL,   input_ch_into_buf, 0, 0 }, /* !               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* "               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* #            35 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* $               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* %               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* &               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* '               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* (            40 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* )               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* *               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* +               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ,               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* -            45 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* .               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* /               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* 0               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* 1               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* 2            50 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* 3               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* 4               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* 5               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* 6               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* 7            55 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* 8               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* 9               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* :               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ;               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* <            60 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* =               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* >               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ?               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* @               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* A            65 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* B               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* C               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* D               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* E               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* F            70 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* G               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* H               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* I               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* J               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* K            75 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* L               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* M               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* N               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* O               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* P            80 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* Q               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* R               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* S               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* T               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* U            85 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* V               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* W               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* X               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* Y               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* Z            90 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* [               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* \               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ]               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ^               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* _            95 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* `               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* a               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* b               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* c               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* d           100 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* e               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* f               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* g               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* h               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* i           105 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* j               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* k               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* l               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* m               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* n           115 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* o               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* p               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* q               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* r               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* s           120 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* t               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* u               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* v               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* w               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* x           125 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* y               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* z               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* {               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* |               */
    { NULL,   input_ch_into_buf, 0, 0 }, /* }           130 */
    { NULL,   input_ch_into_buf, 0, 0 }, /* ~               */
    { NULL,           backspace, 0, 0 }  /* ^? (delete)     */
};

/* Escape set of the keys */
static BindKey BindEscKeys[] = {
    { NULL,    esc_default, 0, 0 }, /* ^@ (null)     0 */
    { NULL,    esc_default, 0, 0 }, /* ctrl-a          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-b          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-c          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-d          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-e        5 */
    { NULL,    esc_default, 0, 0 }, /* ctrl-f          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-g          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-h          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-i          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-j       10 */
    { NULL,    esc_default, 0, 0 }, /* ctrl-k          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-l          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-m          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-n          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-o       15 */
    { NULL,    esc_default, 0, 0 }, /* ctrl-p          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-q          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-r          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-s          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-t       20 */
    { NULL,    esc_default, 0, 0 }, /* ctrl-u          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-v          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-w          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-x          */
    { NULL,    esc_default, 0, 0 }, /* ctrl-y       25 */
    { NULL,    esc_default, 0, 0 }, /* ctrl-z          */
    { NULL,     esc_escape, 0, 0 }, /* ^[ (ESC)        */
    { NULL,    esc_default, 0, 0 }, /* ^\              */
    { NULL,    esc_default, 0, 0 }, /* ^]              */
    { NULL,    esc_default, 0, 0 }, /* ^^           30 */
    { NULL,    esc_default, 0, 0 }, /* ^_              */
    { NULL,     esc_escape, 0, 0 }, /* space bar       */
    { NULL,    esc_default, 0, 0 }, /* !               */
    { NULL,    esc_default, 0, 0 }, /* "               */
    { NULL,    esc_default, 0, 0 }, /* #            35 */
    { NULL,    esc_default, 0, 0 }, /* $               */
    { NULL,    esc_default, 0, 0 }, /* %               */
    { NULL,    esc_default, 0, 0 }, /* &               */
    { NULL,    esc_default, 0, 0 }, /* '               */
    { NULL,    esc_default, 0, 0 }, /* (            40 */
    { NULL,    esc_default, 0, 0 }, /* )               */
    { NULL,    esc_default, 0, 0 }, /* *               */
    { NULL,    esc_default, 0, 0 }, /* +               */
    { NULL,    esc_default, 0, 0 }, /* ,               */
    { NULL,    esc_default, 0, 0 }, /* -            45 */
    { NULL,    esc_default, 0, 0 }, /* .               */
    { NULL,    esc_default, 0, 0 }, /* /               */
    { NULL,    esc_default, 0, 0 }, /* 0               */
    { NULL,    esc_default, 0, 0 }, /* 1               */
    { NULL,    esc_default, 0, 0 }, /* 2            50 */
    { NULL,    esc_default, 0, 0 }, /* 3               */
    { NULL,    esc_default, 0, 0 }, /* 4               */
    { NULL,    esc_default, 0, 0 }, /* 5               */
    { NULL,    esc_default, 0, 0 }, /* 6               */
    { NULL,    esc_default, 0, 0 }, /* 7            55 */
    { NULL,    esc_default, 0, 0 }, /* 8               */
    { NULL,    esc_default, 0, 0 }, /* 9               */
    { NULL,    esc_default, 0, 0 }, /* :               */
    { NULL,    esc_default, 0, 0 }, /* ;               */
    { NULL,    esc_default, 0, 0 }, /* <            60 */
    { NULL,    esc_default, 0, 0 }, /* =               */
    { NULL,    esc_default, 0, 0 }, /* >               */
    { NULL,    esc_default, 0, 0 }, /* ?               */
    { NULL,    esc_default, 0, 0 }, /* @               */
    { NULL,    esc_default, 0, 0 }, /* A            65 */
    { NULL,    esc_default, 0, 0 }, /* B               */
    { NULL,    esc_default, 0, 0 }, /* C               */
    { NULL,    esc_default, 0, 0 }, /* D               */
    { NULL,    esc_default, 0, 0 }, /* E               */
    { NULL,    esc_default, 0, 0 }, /* F            70 */
    { NULL,    esc_default, 0, 0 }, /* G               */
    { NULL,    esc_default, 0, 0 }, /* H               */
    { NULL,    esc_default, 0, 0 }, /* I               */
    { NULL,    esc_default, 0, 0 }, /* J               */
    { NULL,    esc_default, 0, 0 }, /* K            75 */
    { NULL,    esc_default, 0, 0 }, /* L               */
    { NULL,    esc_default, 0, 0 }, /* M               */
    { NULL,    esc_default, 0, 0 }, /* N               */
    { NULL,    esc_default, 0, 0 }, /* O               */
    { NULL,    esc_default, 0, 0 }, /* P            80 */
    { NULL,    esc_default, 0, 0 }, /* Q               */
    { NULL,    esc_default, 0, 0 }, /* R               */
    { NULL,    esc_default, 0, 0 }, /* S               */
    { NULL,    esc_default, 0, 0 }, /* T               */
    { NULL,    esc_default, 0, 0 }, /* U            85 */
    { NULL,    esc_default, 0, 0 }, /* V               */
    { NULL,    esc_default, 0, 0 }, /* W               */
    { NULL,    esc_default, 0, 0 }, /* X               */
    { NULL,    esc_default, 0, 0 }, /* Y               */
    { NULL,    esc_default, 0, 0 }, /* Z            90 */
    { NULL,    esc_default, 0, 0 }, /* [               */
    { NULL,    esc_default, 0, 0 }, /* \               */
    { NULL,    esc_default, 0, 0 }, /* ]               */
    { NULL,    esc_default, 0, 0 }, /* ^               */
    { NULL,    esc_default, 0, 0 }, /* _            95 */
    { NULL,    esc_default, 0, 0 }, /* `               */
    { NULL,    esc_default, 0, 0 }, /* a               */
    { NULL,  bind_boldface, 0, 0 }, /* b               */
    { NULL,  recall_crypts, 0, 0 }, /* c               */
    { NULL,    esc_default, 0, 0 }, /* d           100 */
    { NULL,    esc_default, 0, 0 }, /* e               */
    { NULL,    esc_default, 0, 0 }, /* f               */
    { NULL,    esc_default, 0, 0 }, /* g               */
    { NULL,    esc_default, 0, 0 }, /* h               */
    { NULL,    esc_default, 0, 0 }, /* i           105 */
    { NULL,    esc_default, 0, 0 }, /* j               */
    { NULL,    esc_default, 0, 0 }, /* k               */
    { NULL,    esc_default, 0, 0 }, /* l               */
#ifdef IMAP
    { NULL,  imap_map_mode, 0, 0 }, /* m               */
#else
    { NULL,    esc_default, 0, 0 }, /* m               */
#endif
    { NULL,    esc_default, 0, 0 }, /* n           110 */
    { NULL,    esc_default, 0, 0 }, /* o               */
    { NULL,    esc_default, 0, 0 }, /* p               */
    { NULL,    esc_default, 0, 0 }, /* q               */
    { NULL,    esc_default, 0, 0 }, /* r               */
#ifdef IMAP
    { NULL,  imap_say_mode, 0, 0 }, /* s           115 */
#else
    { NULL,    esc_default, 0, 0 }, /* s           115 */
#endif
    { NULL,   cmd_talk_off, 0, 0 }, /* t               */
    { NULL, bind_underline, 0, 0 }, /* u               */
    { NULL,    esc_default, 0, 0 }, /* v               */
    { NULL,    esc_default, 0, 0 }, /* w               */
    { NULL,    esc_default, 0, 0 }, /* x           120 */
    { NULL,    esc_default, 0, 0 }, /* y               */
    { NULL,    esc_default, 0, 0 }, /* z               */
    { NULL,    esc_default, 0, 0 }, /* {               */
    { NULL,    esc_default, 0, 0 }, /* |               */
    { NULL,    esc_default, 0, 0 }, /* }           125 */
    { NULL,    esc_default, 0, 0 }, /* ~               */
    { NULL,    esc_default, 0, 0 }  /* ^? (delete)     */
};

/* Map modes */
#ifdef IMAP
static BindKey BindImapKeys[] = {
    { NULL,          imap_default, 0, 0 }, /* ^@ (null)     0 */
    { NULL,          imap_default, 0, 0 }, /* ctrl-a          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-b          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-c          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-d          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-e        5 */
    { NULL,          imap_default, 0, 0 }, /* ctrl-f          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-g          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-h          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-i          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-j       10 */
    { NULL,          imap_default, 0, 0 }, /* ctrl-k          */
    { NULL,         imap_map_mode, 0, 0 }, /* ctrl-m          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-n          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-o       15 */
    { NULL,          imap_default, 0, 0 }, /* ctrl-p          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-q          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-r          */
    { NULL,         imap_say_mode, 0, 0 }, /* ctrl-s          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-t       20 */
    { NULL,          imap_default, 0, 0 }, /* ctrl-u          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-v          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-w          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-x          */
    { NULL,          imap_default, 0, 0 }, /* ctrl-y       25 */
    { NULL,          imap_default, 0, 0 }, /* ctrl-z          */
    { NULL,            escape_key, 0, 0 }, /* ^[ (ESC)        */
    { NULL,          imap_default, 0, 0 }, /* ^\              */
    { NULL,          imap_default, 0, 0 }, /* ^]              */
    { NULL,          imap_default, 0, 0 }, /* ^^           30 */
    { NULL,          imap_default, 0, 0 }, /* ^_              */
    { NULL,      imap_ping_sector, 0, 0 }, /* space bar       */
    { NULL,          imap_default, 0, 0 }, /* !               */
    { NULL,          imap_default, 0, 0 }, /* "               */
    { NULL,          imap_default, 0, 0 }, /* #            35 */
    { NULL,          imap_default, 0, 0 }, /* $               */
    { NULL,          imap_default, 0, 0 }, /* %               */
    { NULL,          imap_default, 0, 0 }, /* &               */
    { NULL,          imap_default, 0, 0 }, /* '               */
    { NULL,          imap_default, 0, 0 }, /* (            40 */
    { NULL,          imap_default, 0, 0 }, /* )               */
    { NULL,          imap_default, 0, 0 }, /* *               */
    { NULL,          imap_default, 0, 0 }, /* +               */
    { NULL,          imap_default, 0, 0 }, /* ,               */
    { NULL,          imap_default, 0, 0 }, /* -            45 */
    { NULL,          imap_default, 0, 0 }, /* .               */
    { NULL,          imap_default, 0, 0 }, /* /               */
    { NULL,          imap_default, 0, 0 }, /* 0               */
    { NULL,          imap_move_sw, 0, 0 }, /* 1               */
    { NULL,           imap_move_s, 0, 0 }, /* 2            50 */
    { NULL,          imap_move_se, 0, 0 }, /* 3               */
    { NULL,           imap_move_w, 0, 0 }, /* 4               */
    { NULL,      imap_ping_sector, 0, 0 }, /* 5               */
    { NULL,           imap_move_e, 0, 0 }, /* 6               */
    { NULL,          imap_move_nw, 0, 0 }, /* 7            55 */
    { NULL,           imap_move_n, 0, 0 }, /* 8               */
    { NULL,          imap_move_ne, 0, 0 }, /* 9               */
    { NULL,          imap_default, 0, 0 }, /* :               */
    { NULL,          imap_default, 0, 0 }, /* ;               */
    { NULL,          imap_default, 0, 0 }, /* <            60 */
    { NULL,          imap_default, 0, 0 }, /* =               */
    { NULL,          imap_default, 0, 0 }, /* >               */
    { NULL,          imap_default, 0, 0 }, /* ?               */
    { NULL,          imap_default, 0, 0 }, /* @               */
    { NULL,      imap_toggle_ansi, 0, 0 }, /* A            65 */
    { NULL,          imap_default, 0, 0 }, /* B               */
    { NULL,     imap_capture_ship, 0, 0 }, /* C               */
    { NULL,          imap_default, 0, 0 }, /* D               */
    { NULL,          imap_default, 0, 0 }, /* E               */
    { NULL,             imap_fire, 0, 0 }, /* F            70 */
    { NULL, imap_toggle_geography, 0, 0 }, /* G               */
    { NULL,          imap_default, 0, 0 }, /* H               */
    { NULL,   imap_toggle_inverse, 0, 0 }, /* I               */
    { NULL,          imap_default, 0, 0 }, /* J               */
    { NULL,          imap_default, 0, 0 }, /* K            75 */
    { NULL,        imap_land_ship, 0, 0 }, /* L               */
    { NULL,          imap_default, 0, 0 }, /* M               */
    { NULL,          imap_default, 0, 0 }, /* N               */
    { NULL,          imap_default, 0, 0 }, /* O               */
    { NULL,          imap_default, 0, 0 }, /* P            80 */
    { NULL,          imap_default, 0, 0 }, /* Q               */
    { NULL,          imap_default, 0, 0 }, /* R               */
    { NULL,          imap_default, 0, 0 }, /* S               */
    { NULL,     imap_complex_move, 0, 0 }, /* T               */
    { NULL,      imap_launch_ship, 0, 0 }, /* U            85 */
    { NULL,          imap_default, 0, 0 }, /* V               */
    { NULL,          imap_default, 0, 0 }, /* W               */
    { NULL,          imap_default, 0, 0 }, /* X               */
    { NULL,          imap_default, 0, 0 }, /* Y               */
    { NULL,          imap_default, 0, 0 }, /* Z            90 */
    { NULL,          imap_default, 0, 0 }, /* [               */
    { NULL,          imap_default, 0, 0 }, /* \               */
    { NULL,          imap_default, 0, 0 }, /* ]               */
    { NULL,          imap_default, 0, 0 }, /* ^               */
    { NULL,          imap_default, 0, 0 }, /* _            95 */
    { NULL,          imap_default, 0, 0 }, /* `               */
    { NULL,      imap_toggle_ansi, 0, 0 }, /* a               */
    { NULL,          imap_move_sw, 0, 0 }, /* b               */
    { NULL,          imap_default, 0, 0 }, /* c               */
    { NULL,           imap_deploy, 0, 0 }, /* d           100 */
    { NULL,          imap_default, 0, 0 }, /* e               */
    { NULL,     imap_force_redraw, 0, 0 }, /* f               */
    { NULL, imap_toggle_geography, 0, 0 }, /* g               */
    { NULL,           imap_move_w, 0, 0 }, /* h               */
    { NULL,   imap_toggle_inverse, 0, 0 }, /* i           105 */
    { NULL,           imap_move_s, 0, 0 }, /* j               */
    { NULL,           imap_move_n, 0, 0 }, /* k               */
    { NULL,           imap_move_e, 0, 0 }, /* l               */
    { NULL,            imap_mover, 0, 0 }, /* m               */
    { NULL,          imap_move_se, 0, 0 }, /* n           110 */
    { NULL,          imap_default, 0, 0 }, /* o               */
    { NULL,          imap_default, 0, 0 }, /* p               */
    { NULL,          imap_default, 0, 0 }, /* q               */
    { NULL,          imap_default, 0, 0 }, /* r               */
    { NULL,         imap_say_mode, 0, 0 }, /* s           115 */
    { NULL,          imap_default, 0, 0 }, /* t               */
    { NULL,          imap_move_ne, 0, 0 }, /* u               */
    { NULL,          imap_default, 0, 0 }, /* v               */
    { NULL,          imap_default, 0, 0 }, /* w               */
    { NULL,          imap_default, 0, 0 }, /* x           120 */
    { NULL,          imap_move_nw, 0, 0 }, /* y               */
    { NULL,      imap_zoom_sector, 0, 0 }, /* z               */
    { NULL,          imap_default, 0, 0 }, /* {               */
    { NULL,          imap_default, 0, 0 }, /* |               */
    { NULL,          imap_default, 0, 0 }, /* }           125 */
    { NULL,          imap_default, 0, 0 }, /* ~               */
    { NULL,          imap_default, 0, 0 }  /* ^? (delete)     */
};
#endif // IMAP

/* Arrow key set */
static BindKey BindArrowKeys[] = {
    { NULL, arrow_default, 0, 0 }, /* UP    (A) */
    { NULL, arrow_default, 0, 0 }, /* DOWN  (B) */
    { NULL, arrow_default, 0, 0 }, /* RIGHT (C) */
    { NULL, arrow_default, 0, 0 }  /* LEFT  (D) */
};

/* Function key set */
static BindKey BindFuncKeys[] = {
    { NULL, func_default, 0, 0 }, /* F1 */
    { NULL, func_default, 0, 0 }, /* F2 */
    { NULL, func_default, 0, 0 }, /* F3 */
    { NULL, func_default, 0, 0 }, /* F4 */
    { NULL, func_default, 0, 0 }, /* F5 */
    { NULL, func_default, 0, 0 }, /* F6 */
    { NULL, func_default, 0, 0 }, /* F7 */
    { NULL, func_default, 0, 0 }, /* F8 */
    { NULL, func_default, 0, 0 }, /* F9 */
    { NULL, func_default, 0, 0 }  /* F0 */
};

static BindKey BindMoreKeys[] = {
    { NULL,    more_quit, 0, 0 },
    { NULL,   more_clear, 0, 0 },
    { NULL,  more_cancel, 0, 0 },
    { NULL, more_nonstop, 0, 0 },
    { NULL, more_forward, 0, 0 },
    { NULL, more_oneline, 0, 0 }
};

/* Bind names */
static BindName BindNames[] = {
    {            "backspace",             backspace },
    {             "boldface",         bind_boldface },
    {          "cancel_line",          cancel_input },
    {      "client_test_key",           test_client },
    {         "clear_screen",          clear_screen },
    {         "crypt_recall",         recall_crypts },
    {         "cursor_begin",          cursor_begin },
    {           "cursor_end",            cursor_end },
    {       "cursor_forward",        cursor_forward },
    {              "default",     input_ch_into_buf },
    {    "default_for_arrow",         arrow_default },
    {   "default_for_escape",           esc_default },
    { "default_for_function",          func_default },
#ifdef IMAP
    {     "default_for_imap",          imap_default },
#endif
    {    "default_for_input",     input_ch_into_buf },
    {  "delete_under_cursor",   delete_under_cursor },
    {     "delete_word_left",      delete_word_left },
    {    "delete_word_right",     delete_word_right },
#ifdef IMAP
    {           "enter_imap",         imap_map_mode },
#endif
    {           "escape_key",            escape_key },
    {          "icomm_flush",    icomm_command_done },
    {              "inverse",          bind_inverse },
#ifdef IMAP
    {            "imap_ansi",      imap_toggle_ansi },
    {         "imap_bombard",          imap_bombard },
    {    "imap_capture_ship",     imap_capture_ship },
    {    "imap_complex_move",     imap_complex_move },
    {          "imap_defend",           imap_defend },
    {      "imap_deploy_mil",           imap_deploy },
    {       "imap_file_ship",             imap_fire },
    {    "imap_force_redraw",     imap_force_redraw },
    {       "imap_geography", imap_toggle_geography },
    {         "imap_inverse",   imap_toggle_inverse },
    {       "imap_land_ship",        imap_land_ship },
    {     "imap_launch_ship",      imap_launch_ship },
    {        "imap_move_civ",            imap_mover },
    {          "imap_move_e",           imap_move_e },
    {          "imap_move_n",           imap_move_n },
    {         "imap_move_ne",          imap_move_ne },
    {         "imap_move_nw",          imap_move_nw },
    {          "imap_move_s",           imap_move_s },
    {         "imap_move_se",          imap_move_se },
    {         "imap_move_sw",          imap_move_sw },
    {         "imap_move_w" ,           imap_move_w },
    {     "imap_ping_sector",      imap_ping_sector },
    {        "imap_say_mode",         imap_say_mode },
    {            "imap_test",             imap_test },
    {     "imap_zoom_sector",      imap_zoom_sector },
#endif
    {     "kill_to_end_line",      kill_to_end_line },
    {            "more_quit",             more_quit },
    {           "more_clear",            more_clear },
    {          "more_cancel",           more_cancel },
    {         "more_nonstop",          more_nonstop },
    {         "more_forward",          more_forward },
    {         "more_oneline",          more_oneline },
    {               "normal",     input_ch_into_buf },
    {           "on_newline",        handle_key_buf },
    {  "recall_last_history",            do_recallb },
    {  "recall_next_history",            do_recallf },
    {         "refresh_line",         refresh_input },
    {       "refresh_screen",        refresh_screen },
    {          "stop_things",           stop_things },
    {             "talk_off",          cmd_talk_off },
    {     "toggle_edit_mode",          do_edit_mode },
    {      "transpose_chars",       transpose_chars },
    {            "underline",        bind_underline },
    {      "quote_character",             quote_key }
};

void init_binding()
{
    char buf[SMABUF];

    sprintf(buf, "%c", MORE_DEFAULT_QUITCH);
    BindMoreKeys[0].cptr = string(buf);
    BindMoreKeys[0].is_string = true;
    more_val.k_quit = MORE_DEFAULT_QUITCH;

    sprintf(buf, "%c", MORE_DEFAULT_CLEARCH);
    BindMoreKeys[1].cptr = string(buf);
    BindMoreKeys[1].is_string = true;
    more_val.k_clear = MORE_DEFAULT_CLEARCH;

    sprintf(buf, "%c", MORE_DEFAULT_CANCELCH);
    BindMoreKeys[2].cptr = string(buf);
    BindMoreKeys[2].is_string = true;
    more_val.k_cancel = MORE_DEFAULT_CANCELCH;

    sprintf(buf, "%c", MORE_DEFAULT_NONSTOPCH);
    BindMoreKeys[3].cptr = string(buf);
    BindMoreKeys[3].is_string = true;
    more_val.k_nonstop = MORE_DEFAULT_NONSTOPCH;

    sprintf(buf, "%c", MORE_DEFAULT_FORWARDCH);
    BindMoreKeys[4].cptr = string(buf);
    BindMoreKeys[4].is_string = true;
    more_val.k_forward = MORE_DEFAULT_FORWARDCH;

    sprintf(buf, "%c", MORE_DEFAULT_ONELINECH);
    BindMoreKeys[5].cptr = string(buf);
    BindMoreKeys[5].is_string = true;
    more_val.k_oneline = MORE_DEFAULT_ONELINECH;
}

void cmd_bind(char *args)
{
    char key[SMABUF];
    int bindkey;
    char rest[MAXSIZ];
    int numnames;
    int i;
    int mode = 0;

    split(args, key, rest);

    if (!*key) {
        list_binding(-1, 0);

        return;
    }

    bindkey = convert_string(key, &mode);

    if (mode == -1) {
        msg("-- Bind: unknown character \'%s\'", key);

        return;
    }

    if (!*rest) {
        list_binding(bindkey, mode);

        return;
    }

#ifdef IMAP
    if (!strcmp(rest, "imap_") && (mode != BIND_IMAP)) {
        msg("-- Bind: \'%s\' can only be bound to Imap keys.", rest);

        return;
    }
#endif

    if (!strcmp(rest, "default")) {
        switch (mode) {
        case BIND_NORM:
            strcpy(rest, "default_for_input");

            break;
        case BIND_ESC:
            strcpy(rest, "default_for_escape");

            break;
        case BIND_IMAP:
            strcpy(rest, "default_for_imap");

            break;
        case BIND_MORE:

            break;
        case BIND_ARROW:
            strcpy(rest, "default_for_arrow");

            break;
        case BIND_FUNC:
            strcpy(rest, "default_for_function");

            break;
        }
    }

    // TC_DEBUG: Start useless block
    numnames = sizeof(BindNames) / sizeof(BindName);

    for (i = 0; i < numnames; ++i) {
        if (!strcmp(rest, BindNames[i].name)) {
            break;
        }
    }

    if (i >= numnames) {
        if (mode == BIND_NORM) {
            if ((BindNormKeys[bindkey].cptr != NULL)
                && BindNormKeys[bindkey].is_string) {
                free(BindNormKeys[bindkey].cptr);
            }

            BindNormKeys[bindkey].cptr = string(rest);
            BindNormKeys[bindkey].is_string = true;
            BindNormKeys[bindkey].has_changed = true;
            bind_changed = true;
        }
    }
    // TC_DEBUG: End useless block

    numnames = sizeof(BindNames) / sizeof(BindName);

    for (i = 0; i < numnames; ++i) {
        if (!strcmp(rest, BindNames[i].name)) {
            break;
        }
    }

    if (i >= numnames) {
        switch (mode) {
        case BIND_NORM:
            if ((BindNormKeys[bindkey].cptr != NULL)
                && (BindNormKeys[bindkey].is_string)) {
                free(BindNormKeys[bindkey].cptr);
            }

            BindNormKeys[bindkey].cptr = string(rest);
            BindNormKeys[bindkey].is_string = true;
            BindNormKeys[bindkey].has_changed = true;
            bind_changed = true;
            msg("-- Bind: \'%s\' bound to key \'%s\'",
                rest,
                display_char(bindkey, mode));

            break;
        case BIND_ESC:
            if ((BindEscKeys[bindkey].cptr != NULL)
                && BindEscKeys[bindkey].is_string) {
                free(BindEscKeys[bindkey].cptr);
            }

            BindEscKeys[bindkey].cptr = string(rest);
            BindEscKeys[bindkey].is_string = true;
            BindEscKeys[bindkey].has_changed = true;
            bind_changed = true;
            msg("-- Bind: \'%s\' bound to key \'%s\'",
                rest,
                display_char(bindkey, mode));

            break;
        case BIND_IMAP:
            msg("-- Bind: \'%s\' is not a valid binding function.", rest);

            break;
        case BIND_MORE:
            if (bindkey == BindMoreKeys[0].cptr[0]) {
                bindkey = 0;
            } else if (bindkey == BindMoreKeys[1].cptr[0]) {
                bindkey = 1;
            } else if (bindkey == BindMoreKeys[2].cptr[0]) {
                bindkey = 2;
            } else if (bindkey == BindMoreKeys[3].cptr[0]) {
                bindkey = 3;
            } else if (bindkey == BindMoreKeys[4].cptr[0]) {
                bindkey = 4;
            } else if (bindkey == BindMoreKeys[5].cptr[0]) {
                bindkey = 5;
            }

            if ((BindMoreKeys[bindkey].cptr != NULL)
                && BindMoreKeys[bindkey].is_string) {
                free(BindMoreKeys[bindkey].cptr);
            }

            BindMoreKeys[bindkey].cptr = string(rest);
            BindMoreKeys[bindkey].is_string = true;
            BindMoreKeys[bindkey].has_changed = true;
            bind_changed = true;

            if (bindkey == 0) {
                more_val.k_quit = BindMoreKeys[bindkey].cptr[0];
            } else if (bindkey == 1) {
                more_val.k_clear = BindMoreKeys[bindkey].cptr[0];
            } else if (bindkey == 2) {
                more_val.k_cancel = BindMoreKeys[bindkey].cptr[0];
            } else if (bindkey == 3) {
                more_val.k_nonstop = BindMoreKeys[bindkey].cptr[0];
            } else if (bindkey == 4) {
                more_val.k_forward = BindMoreKeys[bindkey].cptr[0];
            } else if (bindkey == 5) {
                more_val.k_oneline = BindMoreKeys[bindkey].cptr[0];
            }

            msg("-- Bind: \'%s\' bound to key \'%s\'",
                rest,
                display_char(bindkey, mode));

            break;
        case BIND_ARROW:
            if ((BindArrowKeys[bindkey - 'A'].cptr != NULL)
                && BindArrowKeys[bindkey - 'A'].is_string) {
                free(BindArrowKeys[bindkey - 'A'].cptr);
            }

            BindArrowKeys[bindkey - 'A'].cptr = string(rest);
            BindArrowKeys[bindkey - 'A'].is_string = true;
            BindArrowKeys[bindkey - 'A'].has_changed = true;
            bind_changed = true;
            msg("-- Bind: \'%s\' bound to key \'%s\'",
                rest,
                display_char(bindkey - 'A', mode));

            break;
        case BIND_FUNC:
            if ((BindFuncKeys[bindkey - '1'].cptr != NULL)
                && (BindFuncKeys[bindkey - '1'].is_string)) {
                free(BindFuncKeys[bindkey - '1'].cptr);
            }

            BindFuncKeys[bindkey - '1'].cptr = string(rest);
            BindFuncKeys[bindkey - '1'].is_string = true;
            BindFuncKeys[bindkey - '1'].has_changed = true;
            bind_changed = true;
            msg("-- Bind: \'%s\' bound to key \'%s\'",
                rest,
                display_char(bindkey - '1', mode));

            break;
        }

        return;
    }

    switch(mode) {
    case BIND_NORM:
        if ((BindNormKeys[bindkey].cptr != NULL)
            && BindNormKeys[bindkey].is_string) {
            free(BindNormKeys[bindkey].cptr);
        }

        BindNormKeys[bindkey].func = BindNames[i].func;
        BindNormKeys[bindkey].cptr = BindNames[i].name;
        BindNormKeys[bindkey].is_string = false;
        BindNormKeys[bindkey].has_changed = true;
        bind_changed = true;
        msg("-- Bind: key \'%s\' bound to function '\%s\'",
            display_char(bindkey, mode),
            BindNames[i].name);

        break;
    case BIND_ESC:
        if ((BindEscKeys[bindkey].cptr != NULL)
            && BindEscKeys[bindkey].is_string) {
            free(BindEscKeys[bindkey].cptr);
        }

        BindEscKeys[bindkey].func = BindNames[i].func;
        BindEscKeys[bindkey].cptr = BindNames[i].name;
        BindEscKeys[bindkey].is_string = false;
        BindEscKeys[bindkey].has_changed = true;
        bind_changed = true;
        msg("-- Bind: key \'%s\' bound to function \'%s\'",
            display_char(bindkey, mode),
            BindNames[i].name);

        break;
    case BIND_IMAP:
#ifdef IMAP
        BindImapKeys[bindkey].func = BindNames[i].func;
        BindImapKeys[bindkey].cptr = BindNames[i].name;
        BindImapKeys[bindkey].has_changed = true;
        bind_changed = true;
        msg("-- Bind: key \'%s\' bound to imap function \'%s\'",
            display_char(bindkey, mode),
            BindNames[i].name);
#endif

        break;
    case BIND_MORE:
        msg("-- Bind: \'%s\' is not a valid binding function.", rest);

        break;
    case BIND_ARROW:
        if ((BindArrowKeys[bindkey - 'A'].cptr != NULL)
            && BindArrowKeys[bindkey - 'A'].is_string) {
            free(BindArrowKeys[bindkey - 'A'].cptr);
        }

        BindArrowKeys[bindkey - 'A'].func = BindNames[i].func;
        BindArrowKeys[bindkey - 'A'].cptr = BindNames[i].name;
        BindArrowKeys[bindkey - 'A'].is_string = false;
        BindArrowKeys[bindkey - 'A'].has_changed = true;
        bind_changed = true;
        msg("-- Bind: key \'%s\' bound to function \'%s\'",
            display_char(bindkey - 'A', mode),
            BindNames[i].name);

        break;
    case BIND_FUNC:
        if ((BindFuncKeys[bindkey - '1'].cptr != NULL)
            && BindFuncKeys[bindkey - '1'].is_string) {
            free(BindFuncKeys[bindkey - '1'].cptr);
        }

        BindFuncKeys[bindkey - '1'].func = BindNames[i].func;
        BindFuncKeys[bindkey - '1'].cptr = BindNames[i].name;
        BindFuncKeys[bindkey - '1'].is_string = false;
        BindFuncKeys[bindkey - '1'].has_changed = true;
        bind_changed = true;
        msg("-- Bind: key \'%s\' bound to function \'%s\'",
            display_char(bindkey - '1', mode),
            BindNames[i].name);

        break;
    }
}

char convert_string(char *s, int *mode)
{
    char *p;
    char *q;

    p = strtou(s);
    *mode = 0;

    if (!strncmp(p, "E-", strlen("E-"))
        || !strncmp(p, "ESC-", strlen("ESC-"))) {
        *mode = BIND_ESC;
        q = strchr(s, '-');
        p = q;
        ++p;

        if (*p == '\0') {
            *mode = -1;

            return 0;
        }
    } else if (!strncmp(p, "I-", strlen("I-"))
               || !strncmp(p, "IMAP-", strlen("IMAP-"))) {
        *mode = BIND_IMAP;
        q = strchr(s, '-');
        p = q;
        ++p;

        if (*p == '\0') {
            *mode = -1;

            return 0;
        }
    } else if (!strncmp(p, "M-", strlen("M-"))
               || !strncmp(p, "MORE-", strlen("MORE-"))) {
        *mode = BIND_MORE;
        q = strchr(s, '-');
        p = q;
        ++p;

        if (!strcmp(p, BindMoreKeys[0].cptr)
            && !strcmp(p, BindMoreKeys[1].cptr)
            && !strcmp(p, BindMoreKeys[2].cptr)
            && !strcmp(p, BindMoreKeys[3].cptr)
            && !strcmp(p, BindMoreKeys[4].cptr)
            && !strcmp(p, BindMoreKeys[5].cptr)) {
            *mode = -1;

            return 0;
        }
    } else if (!strncmp(p, "A-", strlen("A-"))
               || !strncmp(p, "ARROW-", strlen("ARROW-"))) {
        *mode = BIND_ARROW;
        q = strchr(s, '-');
        p = q;
        ++p;

        if (*p == '\0') {
            *mode = -1;

            return 0;
        }
    } else if (!strncmp(p, "F-", strlen("F-"))
               || !strncmp(p, "FUNC-", strlen("FUNC-"))) {
        *mode = BIND_FUNC;
        q = strchr(s, '-');
        p = q;
        ++p;

        if (*p == '\0') {
            *mode = -1;

            return 0;
        }
    }
    else {
        p = s;
    }

    if ((*p == '\\') && (*(p + 1) == 'n')) {
        return '\n';
    }

    if ((*p == '^') && p) {
        ++p;
        q = strtou(p);
        p = q;

        /* Highest ctrl char char value */
        if (*p <= '_') {
            return (*p - 64);
        }
        else {
            /* -mfw */
            *mode = BIND_ERR;

            return -1;
        }
    } else if (!strncmp(p, "C-", strlen("C-")) && p) {
        p += 2;
        q = strtou(p);
        p = q;

        /* Highest ctrl char char value */
        if(*p <= '_') {
            return (*p - 64);
        }
        else {
            /* -mfw */
            *mode = BIND_ERR;

            return -1;
        }
    } else if (!strncmp(p, "CTRL-", strlen("CTRL-")) && p) {
        p += 5;
        q = strtou(p);
        p = q;

        /* Highest ctrl char char value */
        if (*p <= '_') {
            return (*p - 64);
        }
        else {
            /* -mfw */
            *mode = BIND_ERR;

            return -1;
        }
    }

    if (*mode == BIND_ARROW) {
        if ((*p == 'U') || (*p == 'u')) {
            return 'A';
        }
        else if ((*p == 'D') || (*p == 'd')) {
            return 'B';
        }
        else if ((*p == 'R') || (*p == 'r')) {
            return 'C';
        }
        else if ((*p == 'L') || (*p == 'l')) {
            return 'D';
        }
        else {
            *mode = BIND_ERR;

            return -1;
        }
    }
    else if(*mode == BIND_FUNC) {
        if ((*p < '1') || (*p > '9')) {
            *mode = BIND_ERR;

            return -1;
        }
        else {
            return *p;
        }
    }
    else {
        return *p;
    }
}

char *display_char(char c, int mode)
{
    static char ret[10];
    int start = 0;

    if (mode == BIND_ESC) {
        strcpy(ret, "ESC-");
        start += 4;
    }
    else if (mode == BIND_IMAP) {
        strcpy(ret, "IMAP-");
        start += 5;
    }
    else if (mode == BIND_MORE) {
        strcpy(ret, "MORE-");
        start += 5;

        if (c == 0) {
            c = BindMoreKeys[0].cptr[0];
        } else if (c == 1) {
            c = BindMoreKeys[1].cptr[0];
        } else if (c == 2) {
            c = BindMoreKeys[2].cptr[0];
        } else if (c == 3) {
            c = BindMoreKeys[3].cptr[0];
        } else if (c == 4) {
            c = BindMoreKeys[4].cptr[0];
        } else if (c == 5) {
            c = BindMoreKeys[5].cptr[0];
        }
    }
    else if (mode == BIND_ARROW) {
        strcpy(ret, "ARROW-");
        start += 6;

        if (c == 0) {
            c = 'U';
        }
        else if (c == 1) {
            c = 'D';
        }
        else if (c == 2) {
            c = 'R';
        }
        else if (c == 3) {
            c = 'L';
        }
    }
    else if (mode == BIND_FUNC) {
        strcpy(ret, "FUNC-");
        start += 5;
        c += '1';
    }

    if (c == '\n') {
        ret[start] = '\\';
        ++start;
        ret[start] = 'n';
        ++start;
        ret[start] = '\0';
    }
    else if (c <= 32) { /* ctrl char */
        ret[start] = '^';
        ++start;
        ret[start] = c + 64;
        ++start;
        ret[start] = '\0';
    }
    else if (c == 127) {
        strcpy(ret + start, "^?");
    }
    else {
        ret[start] = c;
        ++start;
        ret[start] = '\0';
    }

    return ret;
}

void bind_translate_char(signed char c, int mode)
{
    char buf[MAXSIZ];
    int x;

    switch(mode) {
    case BIND_NORM:
        if ((BindNormKeys[c].cptr != NULL) && BindNormKeys[c].is_string) {
            msg("--> %s", BindNormKeys[c].cptr);
            strcpy(buf, BindNormKeys[c].cptr);
            process_key(buf, false);
        }
        else {
            BindNormKeys[c].func(c);
        }

        break;
    case BIND_ESC:
        if ((BindEscKeys[c].cptr != NULL) && BindNormKeys[c].is_string) {
            msg("--> %s", BindEscKeys[c].cptr);
            strcpy(buf, BindEscKeys[c].cptr);
            process_key(buf, false);
        }
        else {
            BindEscKeys[c].func(c);
        }

        break;
#ifdef IMAP
    case BIND_IMAP:
        BindImapKeys[c].func(c);

        break;
#endif
    case BIND_MORE:
        if (c == BindMoreKeys[0].cptr[0]) {
            BindMoreKeys[0].func(c);
        } else if (c == BindMoreKeys[1].cptr[0]) {
            BindMoreKeys[1].func(c);
        } else if (c == BindMoreKeys[2].cptr[0]) {
            BindMoreKeys[2].func(c);
        } else if (c == BindMoreKeys[3].cptr[0]) {
            BindMoreKeys[3].func(c);
        } else if (c == BindMoreKeys[4].cptr[0]) {
            BindMoreKeys[4].func(c);
        } else if (c == BindMoreKeys[5].cptr[0]) {
            BindMoreKeys[5].func(c);
        }

        break;
    case BIND_ARROW:
        if ((BindArrowKeys[c - 'A'].cptr != NULL)
            && BindArrowKeys[c - 'A'].is_string) {
            msg("--> %s", BindArrowKeys[c - 'A'].cptr);
            strcpy(buf, BindArrowKeys[c - 'A'].cptr);
            process_key(buf, false);
        }
        else {
            BindArrowKeys[c - 'A'].func(c);
        }

        break;
    case BIND_FUNC:
        x = trans_func_key_to_num(c);

        if ((BindFuncKeys[x].cptr != NULL) && BindFuncKeys[x].is_string) {
            msg("--> %s", BindFuncKeys[x].cptr);
            strcpy(buf, BindFuncKeys[x].cptr);
            process_key(buf, false);
        }
        else {
            BindFuncKeys[x].func(x);
        }

        break;
    default:
        msg("-- Funny mode(%d) in bind_translate...report this please.", mode);

        break;
    }
}

void list_binding(int listkey, int mode)
{
    int i;
    BindKey *bind;

    if (listkey != -1) {
        switch(mode) {
        case BIND_NORM:
            bind = BindNormKeys;

            break;
        case BIND_ESC:
            bind = BindEscKeys;

            break;
#ifdef IMAP
        case BIND_IMAP:
            bind = BindImapKeys;

            break;
#endif
        case BIND_ARROW:
            bind = BindArrowKeys;

            break;
        case BIND_FUNC:
            bind = BindFuncKeys;

            break;
        case BIND_MORE:
            bind = BindMoreKeys;

            break;
        default:

            return;
        }

        msg("-- Bind: \'%s\' = \'%s\' (%s)",
            display_char(listkey, mode),
            (bind[listkey].cptr ? bind[listkey].cptr : "default"),
            (bind[listkey].is_string ? "string" : "function"));

        return;
    }

    for (i = 0; i < 127; ++i) {
        if (BindNormKeys[i].has_changed) {
            msg("-- Bind: \'%s\' = \'%s\' (%s)",
                display_char(i, BIND_NORM),
                (BindNormKeys[i].cptr ? BindNormKeys[i].cptr : "default"),
                (BindNormKeys[i].is_string ? "string" : "function"));
        }
    }

    for (i = 0; i < 127; ++i) {
        if (BindEscKeys[i].has_changed) {
            msg("-- Bind: \'%s\' = \'%s\' (%s)",
                display_char(i, BIND_ESC),
                (BindEscKeys[i].cptr ? BindEscKeys[i].cptr : "default"),
                (BindEscKeys[i].is_string ? "string" : "function"));
        }
    }

#ifdef IMAP
    for (i = 0; i < 127; ++i) {
        if (BindImapKeys[i].has_changed) {
            msg("-- Bind: \'%s\' = \'%s\' (%s)",
                display_char(i, BIND_IMAP),
                (BindImapKeys[i].cptr ? BindImapKeys[i].cptr : "default"),
                (BindImapKeys[i].is_string ? "string" : "imap function"));
        }
    }
#endif

    for (i = 0; i < 4; ++i) {
        if (BindArrowKeys[i].has_changed) {
            msg("-- Bind: \'%s\' = \'%s\' (%s)",
                display_char(i, BIND_ARROW),
                (BindArrowKeys[i].cptr ? BindArrowKeys[i].cptr : "default"),
                (BindArrowKeys[i].is_string ? "string" : "function"));
        }
    }

    for (i = 0; i < 9; ++i) {
        if (BindFuncKeys[i].has_changed) {
            msg("-- Bind: \'%s\' = \'%s\' (%s)",
                display_char(i, BIND_FUNC),
                (BindFuncKeys[i].cptr ? BindFuncKeys[i].cptr : "default"),
                (BindFuncKeys[i].is_string ? "string" : "function"));
        }
    }

    for (i = 0; i < 6; ++i) {
        if (BindMoreKeys[i].has_changed) {
            msg("-- Bind: \'%s\' = \'%s\' (string)",
                display_char(i, BIND_MORE),
                BindMoreKeys[i].cptr);
        }
    }
}

void save_binds(FILE *fd)
{
    int i;

    if (!bind_changed) {
        return;
    }

    fprintf(fd, "#\n# Bindings which are different than client's default\n#\n");

    for (i = 0; i < 127; ++i) {
        if (BindNormKeys[i].has_changed) {
            fprintf(fd,
                    "bind %s %s\n",
                    display_char(i, BIND_NORM),
                    fstring(BindNormKeys[i].cptr));
        }
    }

    for (i = 0; i < 127; ++i) {
        if (BindEscKeys[i].has_changed) {
            fprintf(fd,
                    "bind %s %s\n",
                    display_char(i, BIND_ESC),
                    fstring(BindEscKeys[i].cptr));
        }
    }

#ifdef IMAP
    for (i = 0; i < 127; ++i) {
        if (BindImapKeys[i].has_changed) {
            fprintf(fd,
                    "bind %s %s\n",
                    display_char(i, BIND_IMAP),
                    BindImapKeys[i].cptr);
        }
    }
#endif

    if (BindMoreKeys[0].has_changed) {
        fprintf(fd, "bind %s more_quit\n", display_char(0, BIND_MORE));
    }

    if (BindMoreKeys[1].has_changed) {
        fprintf(fd, "bind %s more_clear\n", display_char(1, BIND_MORE));
    }

    if (BindMoreKeys[2].has_changed) {
        fprintf(fd, "bind %s more_cancel\n", display_char(2, BIND_MORE));
    }

    if (BindMoreKeys[3].has_changed) {
        fprintf(fd, "bind %s more_nonstop\n", display_char(3, BIND_MORE));
    }

    if (BindMoreKeys[4].has_changed) {
        fprintf(fd, "bind %s more_forward\n", display_char(4, BIND_MORE));
    }

    if (BindMoreKeys[5].has_changed) {
        fprintf(fd, "bind %s more_oneline\n", display_char(5, BIND_MORE));
    }
}

void more_quit(char ch)
{
    // Handle separately
}

void more_clear(char ch)
{
    clear_screen(ch);
    more_val.num_lines_scrolled = 0;
}

void more_cancel(char ch)
{
    more_val.on = false;
}

void more_nonstop(char ch)
{
    more_val.non_stop = true;
}

void more_forward(char ch)
{
    // Handle separately
}

void more_oneline(char ch)
{
    --more_val.num_lines_scrolled;
}

void bind_underline(char ch)
{
    input_ch_into_buf(UNDERLINE_CHAR);
}

void bind_boldface(char ch)
{
    input_ch_into_buf(BOLD_CHAR);
}

void bind_bell(void)
{
    input_ch_into_buf(BELL_CHAR);
}

void bind_inverse(char ch)
{
    input_ch_into_buf(INVERSE_CHAR);
}

void free_bindings(void)
{
    int i;
    int siz;

    siz = sizeof(BindNormKeys) / sizeof(BindKey);

    for (i = 0; i < siz; ++i) {
        if ((BindNormKeys[i].cptr != NULL) && BindNormKeys[i].is_string) {
            free(BindNormKeys[i].cptr);
        }
    }

    siz = sizeof(BindEscKeys) / sizeof(BindKey);

    for (i = 0; i < siz; ++i) {
        if ((BindEscKeys[i].cptr != NULL) && BindEscKeys[i].is_string) {
            free(BindEscKeys[i].cptr);
        }
    }

    siz = sizeof(BindArrowKeys) / sizeof(BindKey);

    for (i = 0; i < siz; ++i) {
        if ((BindArrowKeys[i].cptr != NULL) && BindArrowKeys[i].is_string) {
            free(BindArrowKeys[i].cptr);
        }
    }

    siz = sizeof(BindFuncKeys) / sizeof(BindKey);

    for (i = 0; i < siz; ++i) {
        if ((BindFuncKeys[i].cptr != NULL) && BindFuncKeys[i].is_string) {
            free(BindFuncKeys[i].cptr);
        }
    }

    siz = sizeof(BindMoreKeys) / sizeof(BindKey);

    for (i = 0; i < siz; ++i) {
        if ((BindMoreKeys[i].cptr != NULL) && BindMoreKeys[i].is_string) {
            free(BindMoreKeys[i].cptr);
        }
    }
}
