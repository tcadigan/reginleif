#ifndef INI_MANAGER_HPP_
#define INI_MANAGER_HPP_

#include <map>
#include <string>

#include "gl-vector3.hpp"

using namespace std;

class ini_manager {
public:
    ini_manager();
    ini_manager(string const &ini_filename);
    virtual ~ini_manager();

    void init();
    void update();
    void term();

    void set_int(string const &section, string const &entry, GLint value);
    void set_float(string const &section, string const &entry, GLfloat value);
    void set_string(string const &section, 
                    string const &entry,
                    string const &value);

    void set_vector(string const &section, 
                    string const &entry,
                    gl_vector3 const &value);

    GLint get_int(string const &section, string const &entry) const;
    GLfloat get_float(string const &section, string const &entry) const;
    string get_string(string const &section, string const &entry) const;
    gl_vector3 get_vector(string const &section, string const &entry) const;
    
private:
    string inner_get_string(string const &section,
                            string const &entry,
                            string const &default_value) const;

    void parse_contents();

    string ini_filename_;
    map<string, map<string, string> > contents_;
};

#endif
