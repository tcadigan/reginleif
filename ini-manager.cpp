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

ini_manager::ini_manager(string const &ini_filename)
    : ini_filename_(ini_filename)
{
    parse_contents();
}

ini_manager::~ini_manager()
{
    ofstream output;
    output.open(ini_filename_.c_str(), ofstream::out | ofstream::trunc);
    if(!output.good()) {
        return;
    }

    map<string, map<string, string> >::iterator itr;
    map<string, string>::iterator itr2;

    for(itr = contents_.begin(); itr != contents_.end(); ++itr) {
        output << "[" << itr->first << "]" << endl;

        for(itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2) {
            output << itr2->first << "=" << itr2->second << endl;
        }
    }

    output.close();
}

void ini_manager::set_int(string const &section,
                          string const &entry,
                          int value)
{
    string value_str;
    stringstream input;

    input << value;
    input >> value_str;

    set_string(section, entry, value_str);
}

void ini_manager::set_float(string const &section,
                            string const &entry,
                            float value)
{
    string value_str;
    stringstream input;

    input << value;
    input >> value_str;

    set_string(section, entry, value_str);
}

void ini_manager::set_string(string const &section,
                             string const &entry, 
                             string const &value)
{
    string section_str(section);
    for(unsigned int i = 0; i < section_str.size(); ++i) {
        section_str[i] = tolower(section_str[i]);
    }

    map<string, map<string, string> >::iterator itr = 
        contents_.find(section_str);

    if(itr != contents_.end()) {
        if(entry.empty()) {
            contents_.erase(itr);
        }
        else {
            string entry_str(entry);
            for(unsigned int i = 0; i < entry_str.size(); ++i) {
                entry_str[i] = tolower(entry_str[i]);
            }

            map<string, string>::iterator inner_itr = 
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
            string entry_str(entry);
            for(unsigned int i = 0; i < entry_str.size(); ++i) {
                entry_str[i] = tolower(entry_str[i]);
            }

            contents_[section_str][entry_str] = value;
        }
    }
}

void ini_manager::set_vector(string const &section,
                             string const &entry,
                             gl_vector3 const &value)
{
    stringstream input;

    input << value.get_x() << " "
          << value.get_y() << " "
          << value.get_z();

    set_string(section, entry, input.str());
}

int ini_manager::get_int(string const &section, string const &entry) const
{
    int result;
    stringstream output;

    output << inner_get_string(section, entry, "-1");

    output >> result;

    return result;
}

float ini_manager::get_float(string const &section, string const &entry) const
{
    float result;
    stringstream output;

    output << inner_get_string(section, entry, "0");
    output >> result;

    return result;
}

string ini_manager::get_string(string const &section, string const &entry) const
{
    string result;
    stringstream output;

    output << inner_get_string(section, entry, "");
    output >> result;

    return result;
}

gl_vector3 ini_manager::get_vector(string const &section, string const &entry) const
{
    gl_vector3 result(0, 0, 0);

    stringstream output;

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

string ini_manager::inner_get_string(string const &section,
                                     string const &entry, 
                                     string const &default_value) const
{
    stringstream output;

    string section_str(section);
    for(unsigned int i = 0; i < section_str.size(); ++i) {
        section_str[i] = tolower(section_str[i]);
    }

    map<string, map<string, string> >::const_iterator itr;

    if(section_str.empty()) {
        for(itr = contents_.begin(); itr != contents_.end(); ++itr) {
            output << itr->first << " ";
        }
    }
    else {
        itr = contents_.find(section_str);

        if(itr != contents_.end()) {
            map<string, string>::const_iterator itr2;
            map<string, string> const &inner_map = itr->second;

            if(entry.empty()) {
                for(itr2 = inner_map.begin(); itr2 != inner_map.end(); ++itr2) {
                    output << itr2->first << " ";
                }
            }
            else{
                string entry_str(entry);
                for(unsigned int i = 0; i < entry_str.size(); ++i) {
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
    ifstream input;
    input.open(ini_filename_.c_str());
    if(!input.good()) {
        return;
    }

    string line;

    string current_section;
    while(getline(input, line)) {
        if(!line.empty()) {
            if((*line.begin() == '[') && (*line.rbegin() == ']')) {
                for(unsigned int i = 0; i < line.size(); ++i) {
                    line.at(i) = tolower(line.at(i));
                }
                
                current_section = line;
            }
            else if(!current_section.empty()
                    && (line.at(0) != ';')
                    && (line.find_first_of('=') != string::npos)) {
                string key = line.substr(0, line.find_first_of('='));
                for(unsigned int i = 0; i < key.size(); ++i) {
                    key[i] = tolower(key[i]);
                }

                string value = line.substr(line.find_first_of('=') + 1);

                contents_[current_section][key] = value;
            }
        }
    }

    input.close();
}
