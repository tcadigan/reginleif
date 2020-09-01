/***************************************************************************
 * XMAP header file...                                                     *
 * Written by Scott Anderson (sander@engin.umich.edu)                      *
 * Modified by Rich Fitzgerald (fitz@rsj.harvard.edu)                      *
 *                                                                         *
 * Bugs: See the README_XMAP file                                          *
 *                                                                         *
 * History:                                                                *
 * 3/91 or so : First version of xmap. Map drawn in window, and that's it. *
 * 9/91 or so : Small bug fix. Not very interesting yet...                 *
 * 9/18/91    : Saves map string for redraw. Fixed resize.                 *
 * 9/30/91    : Click on ector for sector survey. Click and drag for       *
 *              sector range survey.                                       *
 * 10/11/91   : CD buttons                                                 *
 * 10/12/91   : Fixed string pointer problem and added REMAP               *
 * 10/13/91   : Fixed scope problem for name                               *
 * 10/14/91   : Added input window. Fixed resize (previously the bottom CD *
 *              panel did not float with the window size). Input window    *
 *              send commands to the client not. Added cursor in input     *
 *              window. Added SURVEY ANALYSIS and REPORT buttons. Sector   *
 *              window now only redraws when the sector changes.           *
 * 10/16/91   : Fixed erroneous CS with report button                      *
 * 03/09/92   : Fixed (temporary) seg fault with resize. Added support for *
 *              future graph function. I kind of fixed that annoying       *
 *              multi-flicker with resizes. :)                             *
 * 03/10/92   : Added right button panel. Starting changeover to using     *
 *              client_survey for all mapping.                             *
 * 03/16/92   : Uses client_survey now. Adding plaentary data at bottom of *
 *              XMAP screen. Moved CD buttons to left side.                *
 * 08/01/92   : Added parsing of map command to store desired map scope in *
 *              xmap_current_map_scope. Added cs to xmap_current_map_scope *
 *              for new map. Modified do_mapnow_up for consistency.        *
 *              Rich Fitzgerald (fitz@rsj.harvard.edu)                     *
 * 11/07/92   : Incorporated several changes listed in CHANGES.XMAP        *
 *              Rich Fitzgerald (fitz@rsj.harvard.edu)                     *
 * 11/09/92   : Rewrote map_move and changed button_release to allow for   *
 *              wrapping in the x directions for move and deploy.          *
 *              Rich Fitzgerals (fitz@rsj.harvard.edu)                     *
 * 11/10/92   : Added click and drag zoom, zoom out button to xorbit. RF   *
 * 11/12/92   : Removed most of the flicker upon resize by touching up     *
 *              mwin_event_loop(), resize(), and xmap_plot_redraw(). RF    *
 * 11/16/92   : Switched hlite procs to XDrawRectangle, cleaned up remap   *
 *              code a bit, fixed but in widget allocation. RF             *
 * 11/24/92   : Changed keyboard handling from keycodes to keysyms. RF     *
 * 12/02/92   : Added cursor postioning in input window using mouse. RF    *
 * 01/26/93   : Modified process_xmap_survey to put ship letter in p.des.  *
 *              Modified sector_update_stats to show destination stats.    *
 * 02/05/93   : Made CSP compliant.                                        *
 * 02/08/93   : Changed to link lists for orbits, improved scope id, added *
 *              numerical zoom.                                            *
 * 11/30/93   : Made ANSI-compatible, cleaned up.                          *
 ***************************************************************************/

#ifndef XMAP_H_
#define XMAP_H_

#ifdef XMAP

#include "widget.h"

#define XMAP_VERSION "XMAP v. 3.2.4 11/30/93"

extern int xmap_active;

void send_client_survey(char *s, int changescope);
void send_xmap_command(char *s, int changescope);
void button_redraw(but_win *win);
void do_button_down(but_win *win, XButtonEvent *ev);
void do_button_up(but_win *win, XButtonEvent *ev);
void do_remap_up(but_win *win);
void do_mapnow_up(but_win *win);
void do_anal_up(but_win *win);
void do_sur_up(but_win *win);
void do_rep_up(but_win *win);
void do_graph_up(but_win *win);
void do_gbut_up(but_win *win);
void do_shipbut_up(but_win *win);
void do_zoom_up(but_win *win);
void toggle_xmap_on(but_win *win);
void do_cd_up(but_win *win);
void do_cd_uni(but_win *win);
void do_cd_here(but_win *win);
void do_cd_home(but_win *win);
void draw_panel(pan_win *win);
void clear_sector_data(pan_win *win);
void sector_redraw_info(sec_win *win, int force_redraw);
void sector_redraw(sec_win *win);
void clear_window(mwin *win);
void redraw(mwin *win);
void resize(mwin *win);
void draw_name(mwin *win, char *u);
void draw_orb(mwin *win, int x, int y, char *u, int len, int inv);
void draw_sec(mwin *win, int Maxx, int Maxy, int x, int y, char u, int inv);
void draw_line(int x1, int y1, int x2, int y2);
void draw_box(int x1, int y1, int x2, int y2);
void map_move(mwin *win, XMotionEvent *ev);
void button_press(mwin *win, XButtonEvent *ev);
void button_release(mwin *win, XButtonEvent *ev);
void xmap_mover(int x, int y, int x2, int y2, char *u);
void xmap_deploy(int x, int y, int x2, int y2, char *u);
void xmap_ping(xSector *q);
void set_cur_char_pos(in_win *win, XButtonEvent *ev);
void key_event(mwin *win, XKeyEvent *ev);
void add_input(char the_char);
void remove_input(void);
void do_input(void);
void clear_input(void);
void input_redraw(in_win *win);
void input_update(in_win *win, int pos);
void key_command(mwin *mwin, char cmd);
void mwin_event_loop(void);
void xmap_plot_orbit(char *t);
void x_DispArray(int x, int y, int maxx, int maxy, char *array[], float mag);
int set_zoom(int x1, int y1, int x2, int y2);
void xmap_plot_surface(char *t);
void xmap_plot_continue(void);
void xmap_plot_redraw(void);
int doing_xmap_command(void);
void xmap_input(int comm_num, char *procbuf);
void process_xmap_survey(int num, char *s);
char xmap_get_graph(int x, int y);
void hilite_command_in(but_win *win);
void hilite_command_out(but_win *win);
char xmap_get_graph(int x, int y);
widget *create_mwin(Display *dpy, widget *parent, int x, int y, int w, int h);
widget *create_panel(Display *dpy, widget *parent, int x, int y, int w, int h);
widget *create_button(Display *dpy, widget *parent, int x, int y, int w, int h);
widget *create_input(Display *dpy, widget *parent, int x, int y, int w, int h);
widget *create_sector(Display *dpy, widget *parent, int x, int y, int w, int h);
void create_gcs(Display *dpy);
void setup_button(but_win *win, char *name, int tog, function proc, int graph);
void make_mwin(void);
void empty_proc(void);
void add_new_widget(widget *w);
void MapWidget(widget w);
void cmd_map(char *args);
void cmd_xmap(void);

#endif // XMAP

#endif // XMAP_H_
