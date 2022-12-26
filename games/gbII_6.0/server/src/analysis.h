#ifndef ANALYSIS_H_
#define ANALYSIS_H_

#include "vars.h"

struct anal_sect {
    int x;
    int y;
    int value;
    int des;
};

void analysis(int, int, int, int, orbitinfo *);

void do_analysis(int, int, int, int, int, int, int);
void Insert(int, struct anal_sect[], int, int, int, int);
void PrintTop(int, int, struct anal_sect[], char const *);

#endif /* ANALYSIS_H_ */
