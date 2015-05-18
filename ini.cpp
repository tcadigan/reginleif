/*
 * ini.cpp
 * 2006 Shamus Young
 *
 * This takes various types of data and dumps them into a predefined ini file.
 */

#define FORMAT_VECTOR "%f %f %f"
#define MAX_RESULT 256
#define FORMAT_FLOAT "%1.2f"
#define INIT_FILE ".\\" APP ".ini"
#define SECTION "Settings"

#include <sstream>
#include <stdio.h>
#include "glTypes.hpp"

#include "app.hpp"
#include "ini.hpp"

static char result[MAX_RESULT];

int IniInt(char const *entry)
{
    int result = -1;

    // result = GetPrivateProfileInt(SECTION, entry, -1, INIT_FILE);
    
    return result;
}

void IniIntSet(char const *entry, int val)
{
    char buf[20];

    sprintf(buf, "%d", val);
    // WritePrivateProfileString(SECTION, entry, buf, INI_FILE);
}

float IniFloat(char const *entry)
{
    float f;

    // GetPrivateProfileString(SECTION, entry, "", result, MAX_RESULT, IN_FILE);
    std::stringstream conversion;
    conversion << result;
    conversion >> f;

    return f;
}

void IniFloatSet(char const *entry, float val)
{
    char buf[20];

    sprintf(buf, FORMAT_FLOAT, val);
    // WritePrivateProfileString(SECTION, entry, buf, INI_FILE);
}

char *IniString(char const *entry)
{
    // GetPrivateProfileString(SECTION, entry, "", result, MAX_RESULT, INI_FILE);

    return result;
}

void InitStringSet(char const *entry, char *val)
{
    // WritePrivateProfileString(SECTION, entry, val, INI_FILE);
}

void IniVectorSet(char const *entry, GLvector v)
{
    sprintf(result, FORMAT_VECTOR, v.x, v.y, v.z);
    // WritePrivateProfileString(SECTION, entry, result, INI_FILE);
}

GLvector IniVector(char const *entry)
{
    GLvector v;

    v.z = 0.0f;
    v.y = v.z;
    v.x = v.y;

    // GetPrivateProfileString(SECTION, 
    //                         entry,
    //                         "0 0 0",
    //                         result, 
    //                         MAX_RESULT,
    //                         INI_FILE);

    sscanf(result, FORMAT_VECTOR, &v.x, &v.y, &v.z);

    return v;
}
