#ifndef LOAD_H_
#define LOAD_H_

#include <stdio.h>

void cmd_loadf(char *args);
void shell_out(char *args);
void load_predefined(char *fname);
void load_init_file(FILE *fd);
void cmd_source(char *args);
void cmd_oldshell(char *args);
void cmd_log(char *args);
void log_file(char *args);
void expand_file(char *fname);

#endif // LOAD_H_
