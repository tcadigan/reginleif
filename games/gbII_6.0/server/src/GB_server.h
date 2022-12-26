#ifndef GB_SERVER_H_
#define GB_SERVER_H_

#include "vars.h"

int main(int, char **);
void print_usage(char *);
void print_version(void);
void set_signals(void);
void notify_race(int, char const *);
int notify(int, int, char const *);
int send_special_string(int, int);
void d_think(int, int, char *, char *);
void d_broadcast(int, int, char *, char *, int);
void d_shout(int, int, char *, char *);
void d_announce(int, int, int, char *, char *);
int command_loop(void);
void scheduled(void);
int whack_args(int);
void chomp_opts(void);
int process_fd(int);
int Login_Process(int, int, int);
int checkfds(void);
int readdes(int);
int shutdown_socket(int);
int readfd(int, char *, unsigned int);
int writefd(int, char *, unsigned int);
int connection(void);
void outstr(int, char const *);
int init_network(unsigned int);
char *addrout(unsigned int);
void goodbye_user(int);
void do_update(int, int, int, int, orbitinfo *);
void do_segment(int, int, int, int, orbitinfo *);
void update_times(int);

#ifdef CHAP_AUTH
int Login_Parse(char *, char *, char *, char *, char *);

#else

void Login_Parse(char *, char *, char *);
#endif

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
void _repair(int, int);
void close_game(int, int, int, int, orbitinfo *);
void read_schedule_file(int, int);
void _freeship(int, int, int, int, orbitinfo *);
void show_uptime(int, int, int, int, orbitinfo *);

#endif /* GB_SERVER_H_ */
