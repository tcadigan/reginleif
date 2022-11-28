#ifndef INI_HPP_
#define INI_HPP_

#include "gl-vector3.hpp"

int IniInt(char const *entry);
void IniIntSet(char const *entry, int val);
float IniFloat(char const *entry);
void IniFloatSet(char const *entry, float val);
char *IniString(char const *entry);
void IniStringSet(char const *entry, char *val);
void IniVectorSet(char const *entry, gl_vector3 v);
gl_vector3 IniVector(char const *entry);

#endif /* INI_HPP_ */
