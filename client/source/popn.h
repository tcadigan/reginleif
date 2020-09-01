#ifndef POPN_H_
#define POPN_H_

void cmd_popn(char *args);

#ifdef POPN
void popn_input(int comm_num, char *procbuf);
int doing_popn_command(void);
void handle_popn(void);
#endif // POPN

#endif // POPN_H_
