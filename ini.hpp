#ifndef INI_HPP_
#define INI_HPP_

int IniInit(char const *entry);
void IniIntSet(char const *entry, int val);
float IniFloat(char const *entry);
void IniFloatSet(char const *entry, float val);
char *IniString(char const *entry);
void IniStringSet(char const *entry, char *val);
void IniVectorSet(char const *entry, GLvector v);
GLvector IniVector(char const *entry);

#endif
