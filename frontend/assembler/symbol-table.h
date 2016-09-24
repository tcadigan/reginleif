#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

void construct_symbol_table();
void destroy_symbol_table();
void add_entry(char *symbol, int address);
int contains(char *symbol);
int get_address(char *symbol);

#endif
