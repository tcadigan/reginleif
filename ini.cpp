/*
 * ini.cpp
 * 2006 Shamus Young
 *
 * This takes various types of data and dumps them into a predefined ini file.
 */


#include "ini.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace std;

static char result[MAX_RESULT];

int IniInt(char const *entry)
{
    
    int result_value = -1;

    if(entry == NULL) {
        return result_value;
    }

    string local_entry(entry);

    for(unsigned int i = 0; i < local_entry.size(); ++i) {
        local_entry.at(i) = tolower(local_entry.at(i));
    }

    string local_section(SECTION);
    for(unsigned int i = 0; i < local_section.size(); ++i) {
        local_section.at(i) = tolower(local_section.at(i));
    }

    ifstream input;
    input.open(INIT_FILE);
    if(!input.good()) {
        return result_value;
    }

    string line;
    getline(input, line);

    bool found_section = false;
    while(input.good()) {
        if(!line.empty()) {
            if((*line.begin() == '[') && (*line.rbegin() == ']')) {
                for(unsigned int i = 0; i < line.size(); ++i) {
                    line.at(i) = tolower(line.at(i));
                }
                
                if(line.substr(1, line.size() - 2) == local_section) {
                    found_section = true;
                }
                else {
                    found_section = false;
                }
            }
            else if(found_section 
                    && (line.at(0) != ';')
                    && (line.find_first_of('=') != string::npos)) {
                string key = line.substr(0, line.find_first_of('='));
                
                for(unsigned int i = 0; i < key.size(); ++i) {
                    key.at(i) = tolower(key.at(i));
                }
                
                if(key == local_entry) {
                    stringstream conversion;
                    conversion << line.substr(line.find_first_of('=') + 1);
                    conversion >> result_value;
                    
                    break;
                }
            }
        }

        getline(input, line);
    }

    input.close();
    
    return result_value;
}

void IniIntSet(char const *entry, int val)
{
    char buf[20];

    sprintf(buf, "%d", val);
    // WritePrivateProfileString(SECTION, entry, buf, INI_FILE);
}

float IniFloat(char const *entry)
{
    float result_value = 0;

    if(entry == NULL) {
        return result_value;
    }

    string local_entry(entry);

    for(unsigned int i = 0; i < local_entry.size(); ++i) {
        local_entry.at(i) = tolower(local_entry.at(i));
    }

    string local_section(SECTION);
    for(unsigned int i = 0; i < local_section.size(); ++i) {
        local_section.at(i) = tolower(local_section.at(i));
    }
    
    ifstream input;
    input.open(INIT_FILE);
    if(!input.good()) {
        return result_value;
    }

    string line;
    getline(input, line);

    bool found_section = false;
    while(input.good()) {
        if(!line.empty()) {
            if((*line.begin() == '[') && (*line.rbegin() == ']')) {
                for(unsigned int i = 0; i < line.size(); ++i) {
                    line.at(i) = tolower(line.at(i));
                }
                
                if(line.substr(1, line.size() - 2) == local_section) {
                    found_section = true;
                }
                else {
                    found_section = false;
                }
            }
            else if(found_section 
                    && (line.at(0) != ';')
                    && (line.find_first_of('=') != string::npos)) {
                string key = line.substr(0, line.find_first_of('='));
                
                for(unsigned int i = 0; i < key.size(); ++i) {
                    key.at(i) = tolower(key.at(i));
                }
                
                if(key == local_entry) {
                    stringstream conversion;
                    conversion << line.substr(line.find_first_of('=') + 1);
                    conversion >> result;
                    
                    break;
                }
            }
        }

        getline(input, line);
    }

    input.close();
    
    return result_value;
}

void IniFloatSet(char const *entry, float val)
{
    char buf[20];

    sprintf(buf, FORMAT_FLOAT, val);
    // WritePrivateProfileString(SECTION, entry, buf, INI_FILE);
}

char *IniString(char const *entry)
{
    string local_entry;

    if(entry != NULL) {
        local_entry = entry;

        for(unsigned int i = 0; i < local_entry.size(); ++i) {
            local_entry.at(i) = tolower(local_entry.at(i));
        }
    }

    string local_section(SECTION);
    for(unsigned int i = 0; i < local_section.size(); ++i) {
        local_section.at(i) = tolower(local_section.at(i));
    }

    result[0] = '\0';
    
    ifstream input;
    input.open(INIT_FILE);
    if(!input.good()) {
        return result;
    }

    string line;
    getline(input, line);

    string result_string;
    bool found_section = false;
    while(input.good()) {
        if(!line.empty()) {
            if((*line.begin() == '[') && (*line.rbegin() == ']')) {
                for(unsigned int i = 0; i < line.size(); ++i) {
                    line.at(i) = tolower(line.at(i));
                }
                
                if(line.substr(1, line.size() - 2) == local_section) {
                    found_section = true;
                }
                else {
                    found_section = false;
                }
            }
            else if(found_section 
                    && (line.at(0) != ';')
                    && (line.find_first_of('=') != string::npos)) {
                string key = line.substr(0, line.find_first_of('='));
                
                for(unsigned int i = 0; i < key.size(); ++i) {
                    key.at(i) = tolower(key.at(i));
                }
                
                result_string.append(line.substr(line.find_first_of('=') + 1));
                result_string.append('\0');
                
                if(key == local_entry) {
                    break;
                }
            }
        }

        getline(input, line);
    }

    for(unsigned int i = 0; i < MAX_RESULT; ++i) {
        if(i < result_string.size()) {
            result[i] = result_string.at(i);
        }
        else {
            result[i] = '\0';
        }
    }

    if(result_string.size() > MAX_RESULT) {
        result[MAX_RESULT - 2] = '\0';
    }
    
    return result;
}

void InitStringSet(char const *entry, char *val)
{
    // WritePrivateProfileString(SECTION, entry, val, INI_FILE);
}

void IniVectorSet(char const *entry, GLvector3 v)
{
    sprintf(result, FORMAT_VECTOR, v.x, v.y, v.z);
    // WritePrivateProfileString(SECTION, entry, result, INI_FILE);
}

GLvector3 IniVector(char const *entry)
{
    GLvector3 v;

    v.z = 0.0f;
    v.y = v.z;
    v.x = v.y;

    if(entry == NULL) {
        return v;
    }

    string local_entry(entry);

    for(unsigned int i = 0; i < local_entry.size(); ++i) {
        local_entry.at(i) = tolower(local_entry.at(i));
    }

    string local_section(SECTION);
    for(unsigned int i = 0; i < local_section.size(); ++i) {
        local_section.at(i) = tolower(local_section.at(i));
    }
    
    ifstream input;
    input.open(INIT_FILE);
    if(!input.good()) {
        return v;
    }

    string line;
    getline(input, line);

    bool found_section = false;
    while(input.good()) {
        if(!line.empty()) {
            if((*line.begin() == '[') && (*line.rbegin() == ']')) {
                for(unsigned int i = 0; i < line.size(); ++i) {
                    line.at(i) = tolower(line.at(i));
                }
                
                if(line.substr(1, line.size() - 2) == local_section) {
                    found_section = true;
                }
                else {
                    found_section = false;
                }
            }
            else if(found_section 
                    && (line.at(0) != ';')
                    && (line.find_first_of('=') != string::npos)) {
                string key = line.substr(0, line.find_first_of('='));
                
                for(unsigned int i = 0; i < key.size(); ++i) {
                    key.at(i) = tolower(key.at(i));
                }
                
                if(key == local_entry) {
                    stringstream conversion;
                    conversion << line.substr(line.find_first_of('=') + 1);
                    conversion >> v.x;
                    conversion >> v.y;
                    conversion >> v.z;
                    
                    break;
                }
            }
        }
            
        getline(input, line);
    }

    input.close();

    return v;
}
