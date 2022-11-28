#ifndef OSPELL_H_
#define OSPELL_H_

void initspells();
void cast_spell(int spell);
int getspell();
char *spellid(int id);
void truesight(int blessing);
void expandspellabbrevs(char prefix[80]);
int expandspell(char prefix[80]);
int spellparse();

#endif
