#ifndef COMMAND_H_
#define COMMAND_H_

void command(int tmode, char *f, ...);
int commandcount(char *cmd);
void showcommand(char *cmd);
void clearcommand();
int replaycommand();
void adjustpack(char *cmd);
void bumpsearchcount();
void usemsg(char *str, int obj);
char functionchar(char *cmd);
void mmove(int d, int mode);
void rmove(int count, int d, int mode);
void fmove(int d);
void move1(int d);

#endif
