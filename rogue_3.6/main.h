#ifndef MAIN_H_
#define MAIN_H_

int main(int argc, char **argv, char **envp);
void endit(int parameter);
int fatal(char *s);
int rnd(int range);
int roll(int number, int sides);
void tstp(int parameter);
int setup();
int playit();
int too_much();
int author();
void checkout(int parameter);
int chmsg(char *fmt, void *args);
int loadav(double *avg);
int ucount();
int roguenotes();

#endif
