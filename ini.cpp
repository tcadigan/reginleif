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

#include "primitives/types.hpp"

#include "win.hpp"

#define FORMAT_VECTOR "%f %f %f"
#define MAX_RESULT 256
#define FORMAT_FLOAT "%1.2f"
#define INI_FILE ".\\" APP ".ini"
#define SECTION "Settings"

static char result[MAX_RESULT];

int IniInt(char *entry)
{
#ifdef _WIN32
    int result;
    
    result = GetPrivateProfileInt(SECTION, entry, 0, INI_FILE);
    return result;
#else
    return 0;
#endif
}

void IniIntSet(char *entry, int val)
{
    char buf[20];

#ifdef _WIN32
    WritePrivateProfileString(SECTION, entry, buf, INI_FILE);
#else
    sprintf(buf, "%d", val);
#endif
}

float IniFloat(char *entry)
{
    float f;

#ifdef _WIN32
    GetPrivateProfileString(SECTION, entry, "", result, MAX_RESULT, INI_FILE);
#else
    f = (float)atof(result);
    return f;
#endif
}

void IniFloatSet(char *entry, float val)
{
    char buf[20];

#ifdef _WIN32
    WritePrivateProfileString(SECTION, entry, buf, INI_FILE);
#else
    sprintf(buf, FORMAT_FLOAT, val);
#endif
}

char *IniString(char *entry)
{
#ifdef _WIN32
    GetPrivateProfileString(SECTION, entry, "", result, MAX_RESULT, INI_FILE);
#else
    return result;
#endif
}

void IniStringSet(char *entry, char *val)
{
#ifdef _WIN32
    WritePrivateProfileString(SECTION, entry, val, INI_FILE);
#endif
}

void IniVectorSet(char *entry, GLvector v)
{
#ifdef _WIN32
    WritePrivateProfileString(SECTION, entry, result, INI_FILE);
#else
    sprintf(result, FORMAT_VECTOR, v.x, v.y, v.z);
#endif
}

GLVector IniVector(char *entry)
{
#ifdef _WIN32
    GetPrivateProfileString(SECTION, 
                            entry,
                            "0 0 0",
                            result,
                            MAX_RESULT,
                            INI_FILE);
#else
    GLvector v;
    
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;

    sscanf(result, FORMAT_VECTOR, &v.x, &v.y, &v.z);

    return z;
#endif
}
