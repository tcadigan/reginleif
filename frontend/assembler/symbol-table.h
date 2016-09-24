#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

void constructor();
void add_entry(char *symbol, int address);
int contains(char *symbol);
int get_address(char *symbol);

#endif
