#ifndef INI_HPP_
#define INI_HPP_

#include "types.hpp"

int IniInt(char *entry);
void IniIntSet(char *entry, int val);
float IniFloat(char *entry);
void IniFloatSet(char *entry, float val);
char *IniString(char *entry);
void IniStringSet(char *entry, char *val);
void IniVectorSet(char *entry, GLvector v);
GLvector IniVector(char *entry);

#endif /* INI_HPP_ */
