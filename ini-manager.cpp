/*
 * ini-manager.cpp
 * 2006 Shamus Young
 *
 * This takes various types of data and dumps them into a predefined ini file.
 */

#include "ini-manager.hpp"

#include <fstream>
#include <sstream>

ini_manager::ini_manager()
    : ini_filename_("terrain.ini")
{
    parse_contents();
}

ini_manager::ini_manager(std::string const &ini_filename)
    : ini_filename_(ini_filename)
{
}

ini_manager::~ini_manager()
{
}

void ini_manager::init()
{
    parse_contents();
}

void ini_manager::update()
{
    // nothing
}

void ini_manager::term()
{
    std::ofstream output;
    output.open(ini_filename_.c_str(), 
                std::ofstream::out | std::ofstream::trunc);

    if(!output.good()) {
        return;
    }

    std::map<std::string, std::map<std::string, std::string> >::iterator itr;
    std::map<std::string, std::string>::iterator itr2;

    for(itr = contents_.begin(); itr != contents_.end(); ++itr) {
        output << "[" << itr->first << "]" << std::endl;

        for(itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2) {
            output << itr2->first << "=" << itr2->second << std::endl;
        }
    }

    output.close();
}

void ini_manager::set_int(std::string const &section,
                          std::string const &entry,
                          GLint value)
{
    std::string value_str;
    std::stringstream input;

    input << value;
    input >> value_str;

    set_string(section, entry, value_str);
}

void ini_manager::set_float(std::string const &section,
                            std::string const &entry,
                            GLfloat value)
{
    std::string value_str;
    std::stringstream input;

    input << value;
    input >> value_str;

    set_string(section, entry, value_str);
}

void ini_manager::set_string(std::string const &section,
                             std::string const &entry, 
                             std::string const &value)
{
    std::string section_str(section);
    for(GLuint i = 0; i < section_str.size(); ++i) {
        section_str[i] = tolower(section_str[i]);
    }

    std::map<std::string, std::map<std::string, std::string> >::iterator itr = 
        contents_.find(section_str);

    if(itr != contents_.end()) {
        if(entry.empty()) {
            contents_.erase(itr);
        }
        else {
            std::string entry_str(entry);
            for(GLuint i = 0; i < entry_str.size(); ++i) {
                entry_str[i] = tolower(entry_str[i]);
            }

            std::map<std::string, std::string>::iterator inner_itr = 
                itr->second.find(entry_str);
            
            if(inner_itr != itr->second.end()) {
                if(value.empty()) {
                    itr->second.erase(inner_itr);
                }
                else {
                    inner_itr->second = value;
                }
            }
        }
    }
    else {
        if(!entry.empty() && !value.empty()) {
            std::string entry_str(entry);
            for(GLuint i = 0; i < entry_str.size(); ++i) {
                entry_str[i] = tolower(entry_str[i]);
            }

            contents_[section_str][entry_str] = value;
        }
    }
}

void ini_manager::set_vector(std::string const &section,
                             std::string const &entry,
                             gl_vector3 const &value)
{
    std::stringstream input;

    input << value.get_x() << " "
          << value.get_y() << " "
          << value.get_z();

    set_string(section, entry, input.str());
}

GLint ini_manager::get_int(std::string const &section,
                           std::string const &entry) const
{
    GLint result;
    std::stringstream output;

    output << inner_get_string(section, entry, "-1");

    output >> result;

    return result;
}

GLfloat ini_manager::get_float(std::string const &section, 
                               std::string const &entry) const
{
    GLfloat result;
    std::stringstream output;

    output << inner_get_string(section, entry, "0");
    output >> result;

    return result;
}

std::string ini_manager::get_string(std::string const &section,
                                    std::string const &entry) const
{
    std::string result;
    std::stringstream output;

    output << inner_get_string(section, entry, "");
    output >> result;

    return result;
}

gl_vector3 ini_manager::get_vector(std::string const &section,
                                   std::string const &entry) const
{
    gl_vector3 result(0, 0, 0);

    std::stringstream output;

    output << inner_get_string(section, entry, "0 0 0");
    
    GLfloat value = 0.0f;
    output >> value;
    result.set_x(value);
    
    value = 0.0f;
    output >> value;
    result.set_y(value);

    value = 0.0f;
    output >> value;
    result.set_z(value);

    return result;
}

std::string ini_manager::inner_get_string(std::string const &section,
                                          std::string const &entry, 
                                          std::string const &default_value) const
{
    std::stringstream output;

    std::string section_str(section);
    for(GLuint i = 0; i < section_str.size(); ++i) {
        section_str[i] = tolower(section_str[i]);
    }

    std::map<std::string, std::map<std::string, std::string> >::const_iterator itr;

    if(section_str.empty()) {
        for(itr = contents_.begin(); itr != contents_.end(); ++itr) {
            output << itr->first << " ";
        }
    }
    else {
        itr = contents_.find(section_str);

        if(itr != contents_.end()) {
            std::map<std::string, std::string>::const_iterator itr2;
            std::map<std::string, std::string> const &inner_map = itr->second;

            if(entry.empty()) {
                for(itr2 = inner_map.begin(); itr2 != inner_map.end(); ++itr2) {
                    output << itr2->first << " ";
                }
            }
            else{
                std::string entry_str(entry);
                for(GLuint i = 0; i < entry_str.size(); ++i) {
                    entry_str[i] = tolower(entry_str[i]);
                }

                itr2 = itr->second.find(entry);
                
                if(itr2 != itr->second.end()) {
                    output << itr2->second;
                }
                else {
                    output << default_value;
                }
            }
        }
        else {
            output << default_value;
        }
    }

    return output.str();
}

void ini_manager::parse_contents()
{
    std::ifstream input;
    input.open(ini_filename_.c_str());
    if(!input.good()) {
        return;
    }

    std::string line;

    std::string current_section;
    while(getline(input, line)) {
        if(!line.empty()) {
            if((*line.begin() == '[') && (*line.rbegin() == ']')) {
                for(GLuint i = 0; i < line.size(); ++i) {
                    line.at(i) = tolower(line.at(i));
                }
                
                current_section = line;
            }
            else if(!current_section.empty()
                    && (line.at(0) != ';')
                    && (line.find_first_of('=') != std::string::npos)) {
                std::string key = line.substr(0, line.find_first_of('='));
                for(GLuint i = 0; i < key.size(); ++i) {
                    key[i] = tolower(key[i]);
                }

                std::string value = line.substr(line.find_first_of('=') + 1);

                contents_[current_section][key] = value;
            }
        }
    }

    input.close();
}
