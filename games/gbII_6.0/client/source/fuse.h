#ifndef FUSE_H_
#define FUSE_H_

void cmd_fuse(char *args);
void cspr_fuse(int cnum, char *line);
void proc_fuel(void);
void plot_fuse(void);
double calc_fuse(double dist);
double max_range(double fuel);

#endif // FUSE_H_
