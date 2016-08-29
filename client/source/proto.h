/*
 * proto.h: Header file of the prototypes
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1992
 *
 * See the COPYRIGHT file.
 */

#ifdef XAMP
#include "xmap.h"
#endif

#include <stdarg.h>

#if defined(__STDC__) || defined(__cplusplus)
#define P_(s) s

#else

#define P_(s) ()
#endif

/* action.c */
extern void cmd_action P_((char *));
extern void cmd_unaction P_((char *));
extern void cmd_listaction P_((char *));
extern void action_update_index P_((void));
extern void save_actions P_((FILE *));
extern void cmd_clearaction P_((char *));
extern Action *find_action P_((char *));
extern int handle_action_matches P_((char *));

/* args.c */
extern char *parse_given_string P_((char *, int));
extern void parse_variables P_((char *));
extern void init_assign P_((int));
extern void add_assign P_((char *, char *));
extern void free_assign P_((void));
extern Assign *find_assign P_((char *));
extern void cmd_listassign P_((char *));
extern char *get_assign P_((char *));
extern void cmd_assign P_((char *));
extern int valid_assign_name P_((char *));
extern void remove_assign P_((char *));
extern int test_assign P_((char *));
extern void save_assigns P_((FILE *));
extern char *parse_macro_args P_((char *, char *));
extern char *parse_sec_args P_((char *, char *));
extern void argify P_((char *));
extern char *get_argify P_((char *));
extern int return_range P_((char *, int *, int *, int *));
extern char *get_args P_((int, int));
extern void strcap P_((char *, char ));
extern int parse_for_loops P_((char *));

/* bind.c */
extern void cmd_bind P_((char *));
extern char convert_string P_((char *, int *));
extern char *display_char P_((char, int));
extern void bind_translate_char P_((signed char, int));
extern void list_binding P_((int, int));
extern void save_binds P_((FILE *));
extern void more_quit P_((void));
extern void more_clear P_((void));
extern void more_cancel P_((void));
extern void more_nonstop P_((void));
extern void more_forward P_((void));
extern void move_oneline P_((void));
extern void bind_unserline P_((void));
extern void bind_boldface P_((void));
extern void bind_bell P_((void));
extern void bind_inverse P_((void));
extern void free_bindings P_((void));

/* buffer.c */
extern void add_buffer P_((BufferInfo *, char *, int));
extern char *remove_buffer P_((BufferInfo *));
extern int have_buffer P_((BufferInfo *));

/* command.c */
extern int client_command P_((char *, int));
extern Command *binary_search P_((char *));
extern void cmd_info P_((char *));
extern void cmd_wait P_((char *));
extern void cmd_help P_((char *));
extern void cmd_read P_((char *));
extern void cmd_cd P_((char *));
extern void cmd_hide P_((char *));
extern void cmd_internal P_((char *));
extern void cmd_detach P_((char *));
extern void cmd_post P_((char *));
extern void cmd_echo P_((char *));
extern void cmd_msg P_((void));
extern void cmd_queue P_((char *));
extern void cmd_helpc P_((char *));
extern void cmd_helps P_((char *));
extern void cmd_watch4 P_((char *));
extern void done_rwho P_((void));
extern void start_rwho P_((void));
extern void cmd_version P_((char *));
extern void cmd_clear P_((char *));
extern void cmd_quote P_((char *));
extern void cmd_savef P_((char *));
extern void cmd_sleep P_((char *));
extern void cmd_repeat P_((char *));
extern void cmd_quit P_((char *));
extern void cmd_recall P_((char *));
extern void cmd_convo P_((char *));
extern int can_log P_((char*));
#ifdef RWHO
extern void icomm_rwho P_((char *));
#endif
extern void init_rwho P_((void));
extern void process_spoken P_((char *, char *, int, int, char *));
extern void robo_say P_((char *, int));
extern void cancel_post P_((void));
extern void add_to_post P_((char *));
extern int handle_pipes_and_redirects P_((char *));

/* crypt.c */
extern Crypt *find_crypt P_((char *));
extern void cmd_crypt P_((char *));
extern void cmd_uncrypt P_((void));
extern void cmd_listcrypt P_((void));
extern void crypt_update_index P_((void));
extern char *is_crypted P_((char *));
extern char *check_crypt P_((char *, int));
extern void cmd_cr P_((char *));
extern void init_crypt P_((void));
extern void encode P_((char *, char *));
extern void save_crypts P_((FILE *));
extern int do_crypt_recall P_((char *));
extern void add_crypt_recall P_((char *));
extern void crypt_test P_((void));

/* csp.c */
extern void process_special P_((char *));
extern int cspr_qsort_cmp P_((const void *, const void *));
extern int csps_qsort_cmp P_((const void *, const void *));
extern void init_csp P_((void));
extern void cspr_backup_end P_((int, char *));
extern void cspr_update_end P_((int, char *));
extern void cspr_segment_end P_((int, char *));
extern void cspr_reset_end P_((int char *));
extern void cspr_map P_((int, char *));
extern char *get_map_info_buf P_((int, Pmap *));
extern void cspr_ping P_((int, char *));
extern void cspr_pause P_((int, char *));
extern void cspr_survey P_((int, char *));
extern void cspr_updates_suspended P_((int, char *));
extern void cspr_updates_resumed P_((int, char *));
extern void cspr_knowledge P_((int, char *));
extern void cspr_err P_((int, char *));
extern CSPSendVal *csps_binary_search P_((int));
extern CSPReceiveVal *cspr_binary_search P_((int));
extern void waitfor P_((char *, int, int));
extern int csp_send_request P_((int, char *));
extern void cspr_profile P_((int, char *));
extern void csp_profile_output P_((Profile *));
extern void cspr_relation P_((int, char *));
extern void cspr_print P_((int, char *));
extern void cspr_client_on P_((int, char *));
extern void cspr_client_off P_((int, char *));
extern void cspr_event P_((int, char *));
extern void cspr_scope_prompt P_((int, char *));
extern void cspr_explore P_((int, char *));
extern void cspr_who P_((int, char *));
extern char *csps_relation P_((char *));
extern char *csps_knowledge P_((char *));
extern char *csps_login P_((char *));
extern char *csps_map P_((char *));
extern char *csps_ping P_((char *));
extern char *csps_survey P_((char *));
extern char *csps_null P_((char *));
extern void csp_msg P_((char *, ...));
extern void cspr_orbit P_((int, char *));
extern void plot_orbit_object P_((void));
extern void orbit_info_box P_((void));

/* gb.c */
extern int main P_((int, char **));
extern void gbs P_((void));
extern void test_client P_((void));
extern void init_race_colors P_((void));

/* help.c */
extern void help P_((char *args, FILE *fdhelp));

/* icomm.c */
extern void init_icommand P_((void));
extern void init_start_commands P_((int));
extern int start_command P_((int, int));
extern void icomm_command_done P_((void));
extern int icomm_valid_csp P_((int));
extern int icomm_valid_csp_end P_((int));
extern void icomm_handling_command P_((char *));
extern void icomm_issue_command P_((char *, int));
#ifdef SMART_CLIENT
extern void icomm_profile P_((char *));
extern void icomm_relation P_((char *));
extern void icomm_done_relation P_((void));
extern int type_relation P_((char *));
extern void icomm_status P_((char *));
extern void icomm_done_status P_((void));
#endif
#ifdef OPTTECH
extern double find_derivative P_((long int, long int));
extern void switch_planets P_((long int, long int));
extern void cmd_opttech P_((char *));
#endif

/* imap.c */
extern void imap_input P_((int, char *));
extern int doing_imap_command P_((void));
/* #ifdef MAP */
extern void procesS_client_survey P_((int, char *, Map *));
/* #endif */
#ifdef IMAP
extern void set_up_map_window P_((void));
extern void set_up_interactive_map P_((void));
extern void redraw_map_window P_((void));
extern void clear_map_window P_((void));
extern void refresh_map P_((void));
extern void redraw_map P_((void));
extern void redraw_sector P_((void));
extern void update_sector P_((Sector *));
extern void imap_test P_((void));
extern void handle_map_mode P_((int));
extern void cursor_to_map P_((void));
extern void imap_map_mode P_((void));
extern void imap_say_mode P_((void));
extern void map_move_down P_((void));
extern void map_move_up P_((void));
extern void map_move_right P_((void));
extern void map_move_left P_((void));
extern void imap_mover P_((void));
extern void imap_deploy P_((void));
extern Sector *get_direction P_((char *));
extern void imap_zoom_sector P_((void));
extern void imap_capture_ship P_((void));
extern void imap_launch_ship P_((void));
extern void imap_fire P_((void));
extern void map_bombard_or_defend P_((int));
extern void imap_land_ship P_((void));
extern void ping_current_sector P_((void));
extern void ping_sector P_((int, int));
extern void map_complex_move P_((int));
extern Sector *find_ship P_((int, int *));
extern int invalid_map_screen_sizes P_((void));
extern void imap_complex_move P_((void));
extern void imap_default P_((void));
extern void imap_bombard P_((void));
extern void imap_defend P_((void));
extern void imap_move_sw P_((void));
extern void imap_move_s P_((void));
extern void imap_move_se P_((void));
extern void imap_move_e P_((void));
extern void imap_move_ne P_((void));
extern void imap_move_n P_((void));
extern void imap_move_nw P_((void));
extern void imap_move_w P_((void));
extern void imap_toggle_inverse P_((void));
extern void imap_toggle_geography P_((void));
extern void imap_toggle_ansi P_((void));
extern void imap_force_redraw P_((void));
extern void map_prompt_force_redraw P_((void));
extern void imap_ping_sector P_((void));
#endif

/* key.c */
extern void init_key P_((void));
extern void quit_gb P_((int, char *, char *, char *));
extern void signal_int P_((int));
extern void signal_tstp P_((int));
extern void signal_usr1 P_((int));
extern void signal_winch P_((int));
extern void reset_key P_((void));
extern void cursor_to_window P_((void));
extern void cursor_to_input P_((void));
extern void clear_buf P_((void));
extern void input_ch_into_buf P_((char *));
extern void input_string_into_buf P_((char *));
extern void add_key_buf P_((char));
extern void transpose_chars P_((void));
extern void erase_space_left P_((void));
extern void erase_space_right P_((void));
extern void erase_input P_((int));
extern void delete_under_cursor P_((void));
extern void delete_word_left P_((void));
extern void delete_word_right P_((void));
extern void backspace P_((void));
extern void set_marks P_((void));
extern void refresh_input P_((void));
extern void update_key P_((int));
extern void cursor_forward P_((void));
extern void cursor_backward P_((void));
extern void cancel_input P_((void));
extern void set_edit_buffer P_((char *));
extern void do_recallf P_((void));
extern void do_recallb P_((void));
extern void recall_crypts P_((void));
extern void refresh_screen P_((void));
extern void clear_screen P_((void));
extern void do_edit_mode P_((void));
extern void cursor_begin P_((void));
extern void cursor_end P_((void));
extern void kill_to_end_line P_((void));
extern void escape_key P_((void));
extern void esc_escape P_((void));
extern void esc_default P_((char));
extern void arrow_default P_((char));
extern void fund_default P_((char));
extern void print_key_string P_((int));
extern void handle_key_buf P_((void));
extern void process_key P_((char *, int));
extern void get_key P_((void));
extern void quote_key P_((void));
extern void stop_things P_((void));
extern void promptfor P_((char *, char *, int));
extern void update_input_prompt P_((char *));
extern void put_input_prompt P_((void));
extern void cmd_talk P_((char *));
extern void cmd_talk_off P_((void));
extern int in_talk_mode P_((void));
extern int trans_func_key_to_num P_((char));

/* load.c */
extern void cmd_loadf P_((char *));
extern void shell_out P_((char *));
extern void load_predefined P_((char *));
extern void load_init_file P_((FILE *));
extern void cmd_source P_((char *));
extern void cmd_oldshell P_((char *));
extern void cmd_log P_((char *));
extern void log_file P_((char *));
extern void expand_file P_((char *));

/* map.c */
extern void plot_surface P_((char *));
extern void print_X P_((int));
extern void plot_orbit P_((char *));
#ifdef ARRAY
extern void DispArray P_((int, int, int, int, char **, float));
#endif

/* option.c */
extern void display_set P_((void));
extern void toggle P_((int *, int, char *));
extern void doubletoggle P_((int *, int, char *));
extern void cmd_set P_((char *));
extern CommandSet *binar_set_search P_((char *));
extern void set_client_prompt P_((char *));
extern void set_connect_delay P_((char *));
#ifdef IMAP
extern set_cursor_sector P_((char *, int));
#endif
extern void set_debug P_((char *));
extern void set_encrypt P_((char *));
extern void set_entry_quote P_((char *));
extern void set_exit_quote P_((char *));
extern void set_full_screen P_((char *));
extern void set_help P_((char *));
extern void set_history P_((char *));
extern void set_input_prompt P_((char *));
extern void set_insert_edit_mode P_((char *));
extern void set_macro_char P_((char *));
extern void set_map_opts P_((char *));
extern void set_more P_((char *));
extern void set_more_delay P_((char *));
extern void set_more_rows P_((char *));
extern void set_notify P_((char *));
extern void set_notify_beeps P_((char *));
extern void set_output_prompt P_((char *));
extern void set_overwrite_edit_mode P_((char *));
extern void set_primary_password P_((char *));
extern void set_recall P_((char *));
extern void set_rwho P_((char *));
extern void set_secondary_password P_((char *));
extern void set_show_clock P_((char *, int, char *));
extern void set_show_mail P_((char *, int, char *));
extern void set_status_bar P_((char *));
extern void set_status_bar_char P_((char *));
extern void set_robo P_((char *));
#ifdef CLIENT_DEVEL
extern void set_devel P_((char *));
#endif
extern void set_ansi P_((char *, int, char *));
extern void save_settings P_((FILE *));

/* popn.c */
extern cmd_popn P_((char *));
#ifdef POPN
extern void popn_input P_((int, char *));
extern int doing_popn_command P_((void));
extern void handle_popn P_((void));
#endif

/* proc.c */
extern void cmd_proc P_((char *));
extern void read_process P_((fd_set));
extern void set_process P_((fd_set *));
extern void check_process P_((void));
extern void send_process P_((char *));
extern void process_eof P_((void));
extern int process_running P_((void));
extern void kill_process P_((void));
extern void proc_test P_((void));
extern char *print_process_string P_((char *, char *));
extern void flush_process_string P_((char *, char *));
extern void close_up_process P_((void));
extern void signal_pipe P_((int));
extern void procmsg P_((char *, char *));
extern void internal_pipe_off P_((void));

/* socket.c */
extern int connectgb P_((char *, char *, char *));
extern int is_connected P_((void));
extern void get_socket P_((void));
extern void process_gb P_((char *));
extern void process_socket P_((char *));
extern void socket_final_process P_((char *, int));
extern void oldcheck4_endprompt P_((char *));
extern void connect_prompts P_((char *));
extern void init_endprompt_connect P_((void));
extern void send_gb P_((char *, int));
extern int sendgb P_((char *, int));
extern void cursor_output_window P_((void));
extern void scroll_output_window P_((void));
extern void cmd_connect P_((char *));
extern void set_values_on_end_prompt P_((void));
extern void set_no_logout P_((void));
extern void check_no_logout P_((void));
extern char *build_scope P_((void));
extern void check_for_time_codes P_((char *));
extern void check_for_special_formatting P_((char *, int));
extern void close_gb P_((void));
extern int read_socket P_((void));
extern int have_socket_output P_((void));
extern void loggedin P_((void));
extern void cmd_ping P_((char *));
extern void null_func P_((void));
extern int on_endprompt P_((int));
extern void get_pass_info P_((void));
extern void chap_response P_((char *));
extern void chap_abort P_((void));

/* status.c */
extern void update_status P_((void));
extern void force_update_status P_((void));
extern void put_status P_((void));
extern int check_mail P_((void));
extern char *print_time P_((long ptime));
extern char *build_status P_((void));
extern char *build_scope_prompt P_((void));

/* stmt.c */
extern void cmd_loop P_((char *));
extern int add_loop P_((char *, long int, int));
extern void cmd_removeloop P_((char *));
extern int remove_loop P_((char *));
extern void cmd_listloop P_((char *));
extern void loop_update_index P_((void));
extern Loop *find_loop P_((char *));
extern void handle_loop P_((void));
extern void loop_update_time P_((void));
extern void cmd_for P_((char *));
extern void cmd_if P_((char *));
extern int analyze_cndtl P_((char *));

/* str.c */
extern char *first P_((char *));
extern char *rest P((char *));
extern void split P((char *, char *, char *));
extern char *skip_space P_((char *));
extern char *fstring P_((char *));
extern int pattern_match P_((char *, char *, char **));
extern char *strtou P_((char *));
extern char *string P_((char *));
extern char *maxstring P_(char *);
extern int wrap P_((char *));
extern int more P_((void));
extern void msg P_((char *, ...));
extern void display_msg P_((char *));
extern void displa_bold_communication P_((char *));
extern void msg_error P_((char *, ...));
extern void debug P_((int, char *, ...));
extern void do_column_maker P_((char *));
extern void set_column_maker P_((int));
extern void flush_column_maker P_((void));
extern char *time_dur P_((long int));
extern void remove_space_at_end P_((char *));
extern char *strfree P_((char *));
extern void place_string_on_output_window P_((char *, int));
extern void write_string P_((char *, int));
extern void init_refresh_lines P_((void));
extern void free_refresh_lines P_((void));
extern void add_refresh_line P_((char *, int));
extern int start_refresh_line_index P_((int *));
extern void clear_refresh_line P_((void));
extern void clear_refresh_line_node P_((void));
extern int has_esc_codes P_((char *));

/* term.c */
extern void term_test P_((void));
extern void term_termcap P_((void));
extern int term_null P_((void *));
extern int term_CE_clear_to_eol P_((void));
extern int term_SPACE_clear_to_eol P_((int, int));
extern int term_CS_scroll P_((int, int, int));
extern int term_ALDL_scroll P_((int, int, int));
extern int term_param_ALDL_scroll P_((int, int, int));
extern int term_DC_delete P_((void));
extern int term_ND_cursor_right P_((void));
extern int term_LE_cursor_left P_((void));
extern int term_BS_cursor_left P_((void));
extern int term_putchar P_((char));
extern void term_puts P_((char *, int));
extern void term_normal_mode P_((void));
extern void term_toggle_standout P_((void));
extern void term_standout_on P_((void));
extern void term_standout_off P_((void));
extern int term_standout_status P_((void));
extern void term_toggle_underline P_((void));
extern void term_underline_on P_((void));
extern void term_underline_off P_((void));
extern void term_toggle_boldface P_((void));
extern void term_boldface_on P_((void));
extern void term_boldface_off P_((void));
extern void term_put_termstring P_((char *));
extern void term_move_cursor P_((int, int));
extern void term_clear_screen P_((void));
extern void term_clear P_((int, int));
extern void term_beep P_((int));
extern void get_screen_size P_((void));
extern void term_mode_on P_((void));
extern void term_mode_off P_((void));

/* util.c */
extern int match_gag P_((char *));
extern void cmd_gag P_((char *));
extern void cmd_ungag P_((char *));
extern void cmd_listgag P_((char *));
extern void gag_update_index P_((void));
extern void save_gags P_((FILE *));
extern void cmd_cleargag P_((char *));
extern Gag *find_gag P_((char *));
extern void add_history P_((char *));
extern void free_history P_((void));
extern int recallf P_((char *));
extern int recallb P_((char *));
extern void recall P_((int, int));
extern void recall_n_m P_((int, int, int));
extern void add_recall P_((char *, int));
extern void free_recall P_((void));
extern void recall_match P_((char *, int));
extern void history_sub P_((char *));
extern Macro *find_macro P_((char *));
extern void cmd_def P_((char *));
extern void cmd_undef P_((char *));
extern void remove_macro P_((char *));
extern int do_macro P_((char *));
extern void cmd_listdef P_((char *));
extern void def_update_index P_((void));
extern void save_defs P_((FILE *));
extern void cmd_cleardef P_((void));
extern void cmd_game P_((char *));
extern void add_game P_((char *, char *, char *, char *, char *, char *, char *, char *));
extern void free_game P_((void));
extern void cmd_ungame P_((char *));
extern Game *find_game P_((char *));
extern void cmd_listgame P_((void));
extern void game_update_index P_((void));
extern void save_games P_((FILE *));
extern void send_password P_((void));
extern void add_queue P_((char *, int));
extern void remove_queue P_((char *));
extern void process_queue P_((char *));
extern int check_queue P_((void));
extern int have_queue P_((void));
extern int do_clear_queue P((void));
extern void clear_queue P_((void));
extern void check_news P_((char *));
extern int add_news P_((char *));
extern RNode *find_news P_((char *, char *));
extern void print_news P_((void));

/* xmap.c */
#ifdef XMAP
extern void send_client_survey P_((char *, int));
extern void send_xmap_command P_((char *, int));
extern void button_redraw P_((but_win *));
extern void hilite_command_in P_((but_win *));
extern void hilite_command_out P_((but_win *));
extern void do_button_down P_((but_win *, XButtonEvent *));
extern void do_button_up P_((but_win *, XButtonEvent *));
extern void do_remap_up P_((but_win *));
extern void do_mapnow_up P_((but_win *));
extern void do_anal_up P_((but_win *));
extern void do_sur_up P_((but_win *));
extern void do_rep_up P_((but_win *));
extern void do_graph_up P_((but_win *));
extern void do_gbut_up P_((but_win *));
extern void do_revbut_up P_((but_win *));
extern void do_shipbut_up P_((but_win *));
extern void do_zoom_up P_((but_win *));
extern void toggle_xmap_on P_((but_win *));
extern void do_cd_up P_((but_win *));
extern void do_cd_uni P_((but_win *));
extern void do_cd_here P_((but_win *));
extern void do_cd_home P_((but_win *));
extern void draw_panel P_((pan_win *));
extern void clear_sector_data P_((pan_win *));
extern void sector_redraw_info P_((sec_win *, int));
extern void sector_redraw P_((sec_win *));
extern void clear_window P_((mwin *));
extern void redraw P_((mwin *));
extern void resize P_((mwin *));
extern void draw_name P_((mwin *, char *));
extern void draw_orb P_((mwin *, int, int, char *, int, int));
extern void draw_sec P_((mwin *, int, int, int, int, char, int));
extern void draw_line P_((int, int, int, int));
extern void draw_box P_((int, int, int, int));
extern void map_move P_((mwin *, XMotionEvent *));
extern void button_press P_((mwin *, XButtonEvent *));
extern void button_release P_((mvin *, XButtonEvent *));
extern void xmap_mover P_((int, int, int, int, char *));
extern void xmap_deploy P_((int, int, int, int, char *));
extern void xmap_ping P_((xSector *));
extern void set_cur_char_pos P_((in_win *, XButtonEvent *));
extern void key_event P_((mwin *, XKeyEvent *));
extern void add_input P_((char));
extern void remove_input P_((void));
extern void do_input P_((void));
extern void clear_input P_((void));
extern void input_redraw P_((in_win *));
extern void input_update P_((in_win *, int));
extern void key_command P_((mwin *, char));
extern void mwin_event_loop P_((void));
extern void xmap_plot_orbit P_((char *));
extern void xmap_replot_orbit P_((xOrb *));
extern void x_DispArray P_((int, int, int, int, char **, float));
extern int set_zoom P_((int, int, int, int));
extern void xmap_plot_surface P_((char *));
extern void xmap_plot_continue P_((void));
extern void xamp_plot_redraw P_((void));
extern int doing_xmap_command P_((void));
extern void xmap_input P_((int, char *));
extern void process_xmap_survey P_((int, char *));
extern char xmap_get_graph P_((int, int));
extern widget *create_mwin P_((Display *, widget *, int, int, int, int));
extern widget *create_panel P_((Display *, widget *, int, int, int, int));
extern widget *create_button P_((Display *, widget *, int, int, int, int));
extern widget *create_input P_((Display *, widget *, int, int, int, int));
extern widget *create_sector P_((Display *, widget *, int, int, int, int));
extern void create_gcs P_((Display *));
extern void setup_button P_((but_win *, char *, int, function, int));
extern void make_mwin P_((void));
extern void empty_proc P_((void));
extern void add_new_widget P_((widget *));
extern void MapWidget P_((widget));
extern void cmd_map P_((char *));
extern void cmd_xmap P_((void));
#endif
