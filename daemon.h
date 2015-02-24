#ifndef DEAMON_H_
#define DAEMON_H_

struct delayed_action {
    int d_type;
    int (*d_func)();
    int d_arg;
    int d_time;
};

struct delayed_action *d_slot();
struct delayed_action *find_slot(int (*func)());
void start_daemon(int (*func)(), int arg, int type);
void kill_daemon(int (*func)());
void do_daemons(int flag);
void fuse(int (*func)(), int arg, int time, int type);
void lengthen(int (*func)(), int xtime);
void extinguish(int (*func)());
void do_fuses(int flag);
#endif
