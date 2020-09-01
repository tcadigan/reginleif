#ifndef IMAP_H_
#define IMAP_H_

#include "types.h"

void imap_input(int comm_num, char *procbuf);
int doing_imap_command(void);
void process_client_survey(int cnum, char *s, Map *buildmap);

#ifdef IMAP
void set_up_map_window(void);
void set_up_interactive_map(void);
void redraw_map_window(void);
void clear_map_window(void);
void refresh_map(void);
void redraw_map(void);
void redraw_sector(void);
void update_sector(Sector *ptr);
void handle_map_mode(int nomsgs);
void cursor_to_map(void);
void imap_map_mode(char ch);
void map_move_down(void);
void map_move_up(void);
void map_move_right(void);
void map_move_left(void);
Sector *get_direction(char *c);
void map_bombard_or_defend(int mode);
void ping_current_sector(void);
void ping_sector(int x, int y);
Sector *find_ship(int ship, int *pos);
int invalid_map_screen_sizes(void);
void map_prompt_force_redraw(void);
void imap_bombard(char ch);
void imap_capture_ship(char ch);
void imap_complex_move(char ch);
void imap_default(char ch);
void imap_defend(char ch);
void imap_deploy(char ch);
void imap_fire(char ch);
void imap_force_redraw(char ch);
void imap_land_ship(char ch);
void imap_launch_ship(char ch);
void imap_move_e(char ch);
void imap_move_n(char ch);
void imap_move_ne(char ch);
void imap_move_nw(char ch);
void imap_move_s(char ch);
void imap_move_se(char ch);
void imap_move_sw(char ch);
void imap_move_w(char ch);
void imap_mover(char ch);
void imap_ping_sector(char ch);
void imap_say_mode(char ch);
void imap_test(char ch);
void imap_toggle_geography(char ch);
void imap_toggle_inverse(char ch);
void imap_toggle_ansi(char ch);
void imap_zoom_sector(char ch);

#endif // IMAP

#endif // IMAP_H_
