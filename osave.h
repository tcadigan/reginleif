#ifndef OSAVE_H_
#define OSAVE_H_

int save_game(int compress, char *savestr);
int restore_game(char *savestr);
void signalsave(int signum);

#endif
