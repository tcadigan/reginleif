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
int strprefix(char *prefix, char *s);
int hour();
void initdirs();
int sign(int n);
int distance(int x1, int y1, int x2, int y2);
void do_object_los(char pyx, int *x1, int *y1, int x2, int y2);
int los_p(int x1, int y1, int x2, int y2);
int view_los_p(int x1, int y1, int x2, int y2);
void do_los(char pyx, int *x1, int *y1, int x2, int y2);
char *ordinal(int number);
int calc_points();
void findspace(int *x, int *y, int baux);
int nighttime();
int strmem(char c, char *s);
int unblocked(int x, int y);
int m_unblocked(struct monster *m, int x, int y);


#endif
