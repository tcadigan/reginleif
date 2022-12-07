#ifndef INI_HPP_
#define INI_HPP_

#include "gl-vector3.hpp"

int ini_int(char const *entry);
void ini_int_set(char const *entry, int val);
float ini_float(char const *entry);
void ini_float_set(char const *entry, float val);
char *ini_string(char const *entry);
void ini_string_set(char const *entry, char *val);
void ini_vector_set(char const *entry, gl_vector3 v);
gl_vector3 ini_vector(char const *entry);

#endif /* INI_HPP_ */
