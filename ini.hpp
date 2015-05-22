#ifndef INI_HPP_
#define INI_HPP_

#include "gl-vector3.hpp"

// Constants
#define INI_FILE "terrain.ini"
#define SECTION "Settings"

#include <string>
#include <map>

using namespace std;

class IniManager {
public:
    IniManager();
    IniManager(string const &ini_filename);
    virtual ~IniManager();

    void set_int(string const &entry, int value);
    void set_float(string const &entry, float value);
    void set_string(string const &entry, string const &value);
    void set_vector(string const &entry, GLvector3 const &value);

    int get_int(string const &entry);
    float get_float(string const &entry);
    string get_string(string const &entry);
    GLvector3 get_vector(string const &entry);
    
private:
    string inner_get_string(string const &entry,
                            string const &default_value);

    void parse_contents();

    string ini_filename_;
    map<string, map<string, string> > contents_;
};

#endif
