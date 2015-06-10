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

#include "types.hpp"

#include "win.hpp"

#define FORMAT_VECTOR "%f %f %f"
#define MAX_RESULT 256
#define FORMAT_FLOAT "%1.2f"
#define INI_FILE ".\\" APP ".ini"
#define SECTION "Settings"

static char result[MAX_RESULT];

int IniInt(char *entry)
{
    return 0;
}

void IniIntSet(char *entry, int val)
{
    char buf[20];

    sprintf(buf, "%d", val);
}

float IniFloat(char *entry)
{
    float f;

    f = (float)atof(result);
    return f;
}

void IniFloatSet(char *entry, float val)
{
    char buf[20];

    sprintf(buf, FORMAT_FLOAT, val);
}

char *IniString(char *entry)
{
    return result;
}

void IniStringSet(char *entry, char *val)
{
}

void IniVectorSet(char *entry, GLvector v)
{
    sprintf(result, FORMAT_VECTOR, v.x, v.y, v.z);
}

GLVector IniVector(char *entry)
{
    GLvector v;
    
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;

    sscanf(result, FORMAT_VECTOR, &v.x, &v.y, &v.z);

    return z;
}
