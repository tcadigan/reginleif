#ifndef OUTIL_H_
#define OUTIL_H_

int ok_to_free(plv level);
void setgamestatus(long flag);
void resetgamestatus(long flag);
char *getarticle(char *str);
int max(int a, int b);
int min(int a, int b);
void calc_weight();
int random_range(int k);
int confirmation();
int gamestatusp(long flag);
int inbounds(int x, int y);
int hitp(int hit, int ac);
char *salloc(char *str);

#endif
