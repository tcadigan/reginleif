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
int start_daemon(int (*func)(), int arg, int type);
int kill_daemon(int (*func)());
int do_daemons(int flag);
int fuse(int (*func)(), int arg, int time, int type);
int lengthen(int (*func)(), int xtime);
int extinguish(int (*func)());
int do_fuses(int flag);
#endif
