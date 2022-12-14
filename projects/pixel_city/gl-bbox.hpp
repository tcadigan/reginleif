#ifndef __GL_BBOX_HPP__
#define __GL_BBOX_HPP__

#include "gl-vector3.hpp"

class gl_bbox {
public:
  bool test_point(gl_vector3 const &point);
  void contain_point(gl_vector3 const &point);
  void clear();

  void set_min(gl_vector3 const &min);
  void set_max(gl_vector3 const &max);

  gl_vector3 get_min() const;
  gl_vector3 get_max() const;

private:
  gl_vector3 min_;
  gl_vector3 max_;
};

#endif /* __GL_BBOX_HPP__ */
