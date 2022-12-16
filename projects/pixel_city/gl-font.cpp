#include "gl-font.hpp"

gl_font::gl_font(std::string const &name, unsigned int base_char)
    : name(name), base_char(base_char) {}

unsigned int gl_font::get_base_char() const { return base_char; }
