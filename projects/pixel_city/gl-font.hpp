#ifndef __GL_FONT_HPP__
#define __GL_FONT_HPP__

#include <string>

class gl_font {
public:
  gl_font(std::string const &name, unsigned int base_char);
  unsigned int get_base_char() const;

private:
  std::string name;
  unsigned int base_char;
};

#endif /* __GL_FONT_HPP__ */
