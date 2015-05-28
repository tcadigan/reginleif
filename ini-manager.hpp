#ifndef INI_MANAGER_HPP_
#define INI_MANAGER_HPP_

#include <map>
#include <string>

#include "gl-vector3.hpp"

class ini_manager {
public:
    ini_manager();
    ini_manager(std::string const &ini_filename);
    virtual ~ini_manager();

    void init();
    void term();

    void set_int(std::string const &section,
                 std::string const &entry, 
                 GLint value);

    void set_float(std::string const &section, 
                   std::string const &entry,
                   GLfloat value);

    void set_string(std::string const &section, 
                    std::string const &entry,
                    std::string const &value);

    void set_vector(std::string const &section, 
                    std::string const &entry,
                    gl_vector3 const &value);

    GLint get_int(std::string const &section, std::string const &entry) const;
    GLfloat get_float(std::string const &section, 
                      std::string const &entry) const;

    std::string get_string(std::string const &section, 
                           std::string const &entry) const;

    gl_vector3 get_vector(std::string const &section, 
                          std::string const &entry) const;
    
private:
    std::string inner_get_string(std::string const &section,
                                 std::string const &entry,
                                 std::string const &default_value) const;

    void parse_contents();

    std::string ini_filename_;
    std::map<std::string, std::map<std::string, std::string> > contents_;
};

#endif
