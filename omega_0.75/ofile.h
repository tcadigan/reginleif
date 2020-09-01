#ifndef OFILE_H_
#define OFILE_H_

#include <stdio.h>

void abyss_file();
int filecheck();
void user_intro();
void show_license();
void showscores();
void showmotd();
void combat_help();
void copyfile(char *srcstr);
void displayfile(char *filestr);
void showfile(FILE *fd);
void adeptfile();
void wishfile();
void cityguidefile();
void theologyfile();
void filescanstring(FILE *fd, char *fstr);
void inv_help();
void extendlog(char *descrip, int lifestatus);

#endif
