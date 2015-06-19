#ifndef LEARN_H_
#define LEARN_H_

#include <stdio.h>

int pickgenotype();
void evalknobs(int gid, int score, int level);
void setknobs(int *newid, int *knb, int *best, int *avg);
FILE *openlog(char *genelog);
void closelog();
void writegenes(char *genepool);
int readgenes(char *genepool);
void initpool(int k, int m);

#endif
