#ifndef OFILE_H_
#define OFILE_H_

#include <stdio.h>

void abyss_file();
int filecheck();
void user_intro();
void show_license();
void combat_help();
void copyfile(char *srcstr);
void displayfile(char *filestr);
void showfile(FILE *fd);
void filescanstring(FILE *fd, char *fstr);
void inv_help();

#endif
