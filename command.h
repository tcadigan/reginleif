#ifndef COMMAND_H_
#define COMMAND_H_

void command(int tmode, char *f, ...);
int commandcount(char *cmd);
void showcommand(char *cmd);
void clearcommand();
void adjustpack(char *cmd);
void bumpsearchcount();
void usemsg(char *str, int obj);

#endif
