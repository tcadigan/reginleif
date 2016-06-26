/*
 * bind.h: Contains the data structs for bind.c
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1992-1993
 *
 * See the COPYRIGHT file.
 */

extern void backspace();
extern void cancel_input();
extern void clear_screen();
extern void cmd_talk_off();
extern void icomm_command_done();
extern void cursor_backward();
extern void cursor_begin();
extern void cursor_end();
extern void cursor_forward();
extern void delete_under_cursor();
extern void delete_word_left();
extern void delete_word_right();
extern void do_edit_mode();
extern void do_recallb();
extern void do_recallf();
extern void esc_default();
extern void esc_escape();
extern void escape_key();
extern void handle_key_buf();
extern void input_ch_into_buf();
extern void kill_to_end_line();
extern void recall_crypts();
extern void refresh_input();
extern void refresh_screen();
extern void stop_things();
extern void test_client();
extern void transpose_chars();
extern void quote_key();
extern void arrow_default();
extern void func_default();

/* Imap functions */
#ifdef IMAP
extern void imap_bombard(void);
extern void imap_capture_ship(void);
extern void imap_complex_move(void);
extern void imap_default(void);
extern void imap_defend(void);
extern void imap_deploy(void);
extern void imap_fire(void);
extern void imap_force_redraw(void);
extern void imap_land_ship(void);
extern void imap_launch_ship(void);
extern void imap_map_mode(void);
extern void imap_move_e(void);
extern void imap_move_n(void);
extern void imap_move_ne(void);
extern void imap_move_nw(void);
extern void imap_move_s(void);
extern void imap_move_se(void);
extern void imap_move_sw(void);
extern void imap_move_w(void);
extern void imap_mover(void);
extern void imap_ping_sector(void);
extern void imap_say_mode(void);
extern void imap_test(void);
extern void imap_toggle_geography(void);
extern void imap_toggle_inverse(void);
extern void imap_toggle_ansi(void);
extern void imap_zoom_sector(void);
#endif

/* Declarations for the more */
void bind_bell(void);
void bind_boldface(void);
void bind_inverse(void);
void bind_underline(void);
void more_cancel(void);
void more_clear(void);
void more_forward(void);
void more_nonstop(void);
void more_oneline(void);
void more_quit(void);

/* Typedefs */
typedef struct bindkey {
    char *cptr;
    void (*func) ();
    int is_string;
    int has_changed;
} BindKey;

typedef struct bindname {
    char *name;
    void (*func) (void);
} BindName;

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
    ( NULL,           imap_deploy, 0, 0 }, /* d           100 */
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
    {          "move_cancel",           more_cancel },
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
