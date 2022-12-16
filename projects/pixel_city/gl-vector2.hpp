#ifndef __GL_VECTOR2_HPP__
#define __GL_VECTOR2_HPP__

#include <SDL2/SDL_opengl.h>

#include <array>

class gl_vector2 {
public:
  gl_vector2() = default;
  gl_vector2(float x, float y);

  float length();

  gl_vector2 &operator+=(gl_vector2 const &rhs);
  gl_vector2 &operator-=(gl_vector2 const &rhs);
  gl_vector2 &operator/=(float const &rhs);

  void set_x(float x);
  void set_y(float y);

  std::array<float, 2> get_data();
  float get_x() const;
  float get_y() const;

private:
  std::array<float, 2> data_;
};

inline gl_vector2 operator+(gl_vector2 lhs, gl_vector2 const &rhs) {
  lhs += rhs;

  return lhs;
}

inline gl_vector2 operator-(gl_vector2 lhs, gl_vector2 const &rhs) {
  lhs -= rhs;

  return lhs;
}

inline gl_vector2 operator/(gl_vector2 lhs, float const &rhs) {
  lhs /= rhs;

  return lhs;
}

#endif /* GL_VECTOR2_HPP__ */
