#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

void Constructor();
void addEntry(char *symbol, int address);
int contains(char *symbol);
int GetAddress(char *symbol);

#endif
