#ifndef SAVE_H_
#define SAVE_H_

#include <stdio.h>

int save_game();
void auto_save(int parameter);
int save_file(FILE *savef);
int restore(char *file, char **envp);
int encwrite(char *start, unsigned int size, FILE *outf);
int encread(char *start, unsigned int size, int inf);

#endif
