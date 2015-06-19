#ifndef STATS_H_
#define STATS_H_

#include <stdio.h>

#include "types.h"

void addprob(probability *p, int success);
void parseprob(char *buf, probability *p);
void writeprob(FILE *f, probability *p);
void clearprob(probability *p);
void addstat(statistic *s, int datum);
void parsestat(char *buf, statistic *s);
void writestat(FILE *, statistic *s);
void clearstat(statistic *s);
double mean(statistic *s);
double stdev(statistic *s);

#endif
