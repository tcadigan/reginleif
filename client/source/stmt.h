#ifndef STMT_H_
#define STMT_H_

#include "types.h"

void cmd_loop(char *args);
int add_loops(char *cmd, long int timel, int user_defn);
void cmd_removeloop(char *args);
int remove_loop(char *cmd_name);
void cmd_listloop(char *args);
void loop_update_index(void);
Loop *find_loop(char *name);
void handle_loop(void);
void loop_update_time(void);
void cmd_for(char *args);
void cmd_if(char *args);
int analyze_cndtl(char *s);

#endif // STMT_H_
