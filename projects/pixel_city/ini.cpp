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

#include "win.hpp"

#define FORMAT_VECTOR "%f %f %f"
#define MAX_RESULT 256
#define FORMAT_FLOAT "%1.2f"
#define INI_FILE ".\\" APP ".ini"
#define SECTION "Settings"

static char result[MAX_RESULT];

int ini_int(char const *entry)
{
    return 0;
}

void ini_int_set(char const *entry, int val)
{
    char buf[20];

    sprintf(buf, "%d", val);
}

float ini_float(char const *entry)
{
    float f;

    f = (float)atof(result);
    return f;
}

void ini_float_set(char const *entry, float val)
{
    char buf[20];

    sprintf(buf, FORMAT_FLOAT, val);
}

char *ini_string(char const *entry)
{
    return result;
}

void ini_string_set(char const *entry, char *val)
{
}

gl_vector3 ini_vector(char const *entry)
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    sscanf(result, FORMAT_VECTOR, &x, &y, &z);

    return gl_vector3(x, y, z);
}

void ini_vector_set(char const *entry, gl_vector3 v)
{
    sprintf(result, FORMAT_VECTOR, v.get_x(), v.get_y(), v.get_z());
}
