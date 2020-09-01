#ifndef ACTION_H_
#define ACTION_H_

#include <stdio.h>

void cmd_action(char *args);
void save_actions(FILE *fd);
int handle_action_matches(char *s);

#endif // ACTION_H_
