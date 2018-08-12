#ifndef MISC_H_
#define MISC_H_

double logscale(int);
void adjust_morale(racetype *, racetype *, int);
void load_star_data(void);
void load_race_data(void);
void welcome_user(int);
void check_for_telegrams(int, int);
void setdebug(int, int);
void backup(void);
void suspend(int, int);
int getfdtablesize(void);
void malloc_warning(char *);
void print_motd(int);
void *malloc_spec(size_t);
double calc_fuse(shiptype *, double);

#endif /* MISC_H_ */
