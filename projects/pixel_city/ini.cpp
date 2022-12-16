/*
 * ini.cpp
 *
 * 2009 Shamus Young
 *
 * This takes various types of data and dumps them into a predefined ini file.
 *
 */

#include "ini.hpp"

#include <cstdio>
#include <cstdlib>

#include <fstream>
#include <map>
#include <string>

#include "win.hpp"

static std::string const FORMAT_VECTOR = "%f %f %f";
static std::string const INI_FILE = "pixelcity.ini";

static std::map<std::string, std::map<std::string, std::string>> contents;

int ini_int(std::string const &section, std::string const &key) {
  if (contents.find(section) != contents.end()) {
    if (contents[section].find(key) != contents[section].end()) {
      return std::stoi(contents[section][key]);
    }
  }

  return -1;
}

void ini_int_set(std::string const &section, std::string const &key,
                 int value) {
  if (contents.find(section) == contents.end()) {
    contents[section] = std::map<std::string, std::string>();
  }

  contents[section][key] = std::to_string(value);
}

float ini_float(std::string const &section, std::string const &key) {
  if (contents.find(section) != contents.end()) {
    if (contents[section].find(key) != contents[section].end()) {
      return std::stof(contents[section][key]);
    }
  }

  return -1.0f;
}

void ini_float_set(std::string const &section, std::string const &key,
                   float value) {
  if (contents.find(section) == contents.end()) {
    contents[section] = std::map<std::string, std::string>();
  }

  contents[section][key] = std::to_string(value);
}

std::string ini_string(std::string const &section, std::string const &key) {
  if (contents.find(section) != contents.end()) {
    if (contents[section].find(key) != contents[section].end()) {
      return contents[section][key];
    }
  }

  return "";
}

void ini_string_set(std::string const &section, std::string const &key,
                    std::string const &value) {
  if (contents.find(section) == contents.end()) {
    contents[section] = std::map<std::string, std::string>();
  }

  contents[section][key] = value;
}

gl_vector3 ini_vector(std::string const &section, std::string const &key) {
  if (contents.find(section) != contents.end()) {
    if (contents[section].find(key) != contents[section].end()) {
      float x;
      float y;
      float z;

      std::sscanf(contents[section][key].c_str(), "%f %f %f", &x, &y, &z);

      return gl_vector3(x, y, z);
    }
  }

  return gl_vector3();
}

void ini_vector_set(std::string const &section, std::string const &key,
                    gl_vector3 const &value) {
  if (contents.find(section) == contents.end()) {
    contents[section] = std::map<std::string, std::string>();
  }

  std::array<float, 3> content(value.get_data());
  int size = std::snprintf(nullptr, 0, "%f %f %f", content[0], content[1],
                           content[2]) +
             1;
  char str[size + 1];
  std::snprintf(str, size, "%f %f %f", content[0], content[1], content[2]);

  contents[section][key] = std::string(str);
}

void write_ini() {
  std::ofstream output(INI_FILE, std::ios::out | std::ios::trunc);

  for (std::pair<std::string, std::map<std::string, std::string>> outer :
       contents) {
    if (output.tellp() != 0) {
      output << std::endl;
    }

    output << "[" << outer.first << "]\n";

    for (std::pair<std::string, std::string> inner : outer.second) {
      output << inner.first << " = " << inner.second << std::endl;
    }
  }
}

void load_ini() {
  std::ifstream input(INI_FILE);

  std::string line;
  std::string section;
  std::string key;
  std::string value;

  while (std::getline(input, line)) {
    if (line.empty()) {
      section.clear();
      continue;
    } else if (line.at(0) == ';') {
      continue;
    } else if (line.at(0) == '[') {
      section = line.substr(1, line.size() - 2);
    } else if (std::isalpha(line.at(0))) {
      if (section.empty()) {
        section = "unmapped";
      }

      std::string::size_type pos = line.find('=');

      key = line.substr(0, pos - 2);
      value = line.substr(pos + 1);

      if (contents.find(section) == contents.end()) {
        contents[section] = std::map<std::string, std::string>();
      }

      contents[section][key] = value;
    }
  }
}
