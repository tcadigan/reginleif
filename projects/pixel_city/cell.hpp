#ifndef __CELL_HPP__
#define __CELL_HPP__

#include "gl-vector3.hpp"

class Cell {
public:
  unsigned int list_textured;
  unsigned int list_flat;
  unsigned int list_flat_wireframe;
  unsigned int list_alpha;
  gl_vector3 pos;
};

#endif /* __CELL_HPP__ */
