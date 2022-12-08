#ifndef INI_HPP_
#define INI_HPP_

#include <string>

#include "gl-vector3.hpp"

int ini_int(std::string const &section, std::string const &key);
void ini_int_set(std::string const &section, std::string const &key, int value);
float ini_float(std::string const &section, std::string const &key);
void ini_float_set(std::string const &section, std::string const &key, float value);
std::string ini_string(std::string const &section, std::string const &key);
void ini_string_set(std::string const &section, std::string const &key, std::string const &value);
gl_vector3 ini_vector(std::string const &section, std::string const &key);
void ini_vector_set(std::string const &section, std::string const &key, gl_vector3 const &value);
void write_ini();
void load_ini();

#endif /* INI_HPP_ */
