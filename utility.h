#ifndef UTILITY_H_
#define UTILITY_H_

int stlmatch(char *small, char *big);
int filelength(char *f);
void critical();
void uncritical();
void int_exit(void(*exitproc)());
int lock_file(char *lockfil, int maxtime);
void unlock_file(char *lokfil);
char *getname();

#endif
