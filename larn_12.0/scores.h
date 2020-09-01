#ifndef SCORES_H_
#define SCORES_H_

void died(int x);
int getplid(char *name);
void showallscores();
void diedlog();
void showscores();
int makeboard();
void diedsub(int x);
int shou(int x);
long paytaxes(long x);
int hashewon();
void new1sub(long score, int i, char *whoo, long taxes);
void new2sub(long score, int i, char *whoo, int whyded);

#endif
