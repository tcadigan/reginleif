#ifndef INI_HPP_
#define INI_HPP_

#include "gl-vector3.hpp"

// What I would like to convert this to:
// A proper object! It takes a path of the ini file
// It reads all of it into memory, keeps a map<string, <map<string, string> >
// of all data. In general this is no good due to malicious input,
// but for here the files should be small and it doesn't need to be bullet
// proof. It writes on destruction. Again race condition here if external
// program modifies the file, it's all gone. So there will be exactly
// one of these ini objects for everything. The application init will create
// it and pass it through to the objects that will need to read/write to it.
// All of the constants should be able to to the ini file.
//
// Design/parser decisions:
// multiple of the same section allowed
// first value wins
// comment is the ';'
// quotes are not dropped, the value is exactly as written
// blank lines are allowed
// whitespace is significant for sections and keys (should fix this)

// Constants
#define FORMAT_VECTOR "%f %f %f"
#define MAX_RESULT 256
#define FORMAT_FLOAT "%1.2f"
#define INIT_FILE "terrain.ini"
#define SECTION "Settings"

int IniInit(char const *entry);
void IniIntSet(char const *entry, int val);
float IniFloat(char const *entry);
void IniFloatSet(char const *entry, float val);
char *IniString(char const *entry);
void IniStringSet(char const *entry, char *val);
void IniVectorSet(char const *entry, GLvector3 v);
GLvector3 IniVector(char const *entry);

#endif
