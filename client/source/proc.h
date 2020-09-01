#ifndef PROC_H_
#define PROC_H_

#include <sys/select.h>

void cmd_proc(char *args);
void read_process(fd_set rd);
void set_process(fd_set *rd);
void check_process(void);
void send_process(char *s);
void process_eof(void);
int process_running(void);
void kill_process(void);
void proc_test(void);
char *print_process_string(char *header, char *s);
void flush_process_string(char *header, char *s);
void close_up_process(void);
void signal_pipe(int a);
void procmsg(char *header, char *args);
void internal_pipe_off(void);

extern int pipe_running;

#endif // PROC_H_
