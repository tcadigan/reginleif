#ifndef SIGNALS_H_
#define SIGNALS_H_

void controlz();
void no_controlz();
void init_signals();
void ignore_signals();
void restore_signals();
void default_signals();
void signal_save_core(int sig);
void signal_save_no_core(int sig);
void signal_ask_quit(int sig);

#endif
