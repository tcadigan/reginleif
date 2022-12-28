#ifndef GB_SERVER_H_
#define GB_SERVER_H_

#include <stdio.h>

#include "vars.h"

void notify_race(int, char const *);
int notify(int, int, char const *);
int send_special_string(int, int);
void d_think(int, int, char *, char *);
void d_broadcast(int, int, char *, char *, int);
void d_shout(int, int, char *, char *);
void d_announce(int, int, int, char *, char *);
void outstr(int, char const *);
void do_update(int, int, int, int, orbitinfo *);
void do_segment(int, int, int, int, orbitinfo *);
void update_times(int);
void dump_users(int);
void dump_users_priv(int, int, int, int, orbitinfo *);
void boot_user(int, int, int, int, orbitinfo *);
void GB_time(int, int, int, int, orbitinfo *);
void compute_power_blocks(void);
void warn_race(int, char *);
void warn(int, int, char *);
void warn_star(int, int, int, char *);
void notify_star(int, int, int, int, char *);
void shut_game(int, int, int, int, orbitinfo *);
void voidpoint(int, int, int, int, orbitinfo *);
int clear_all_fds(void);
void _reset(int, int, int, int, orbitinfo *);
void _emulate(int, int, int, int, orbitinfo *);
void _schedule(int, int, int, int, orbitinfo *);
void last_logip(int, int, int, int, orbitinfo *);
void close_game(int, int, int, int, orbitinfo *);
void read_schedule_file(int, int);
void _freeship(int, int, int, int, orbitinfo *);
void show_uptime(int, int, int, int, orbitinfo *);

extern char const *Desnames[];
extern char const Dessymbols[];
extern int chat_static;
extern int chat_flag;
extern long size_of_words;
extern FILE *garble_file;

#endif /* GB_SERVER_H_ */
