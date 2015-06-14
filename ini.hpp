#ifndef INI_HPP_
#define INI_HPP_

#include "gl-vector3.hpp"

int IniInt(char *entry);
void IniIntSet(char *entry, int val);
float IniFloat(char *entry);
void IniFloatSet(char *entry, float val);
char *IniString(char *entry);
void IniStringSet(char *entry, char *val);
void IniVectorSet(char *entry, gl_vector3 v);
gl_vector3 IniVector(char *entry);

#endif /* INI_HPP_ */
