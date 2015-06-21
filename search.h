#ifndef SEARCH_H_
#define SEARCH_H_

int makemove(int movetype, int (*evalinit)(), int (*evaluate)(), int reevaluate);
int findmove(int movetype, int (*evalinit)(), int (*evaluate)(), int reevaluate);
void cancelmove(int movetype);
void setnewgoal();
int searchfrom(int row, int col, int (*evaluate)(), char dir[24][80], int *trow, int *tcol);
int searchto(int row, int col, int (*evalaute)(), char dir[24][80], int *trow, int *tcol);
int followmap(int movetype);
int validatemap(int movetype, int (*evalinit)(), int (*evaluate)());
int makemove(int movetype, int (*evalinit)(), int (*evaluate)(), int reevaluate);
int findroom();

#endif
