#ifndef STATUS_H_
#define STATUS_H_

void update_status(void);
void force_update_status(void);
void put_status(void);
int check_mail(void);
char *print_time(long ptime);
char *build_status(void);
char *build_scope_prompt(void);

#endif // STATUS_H_
