#ifndef CELL_HPP_
#define CELL_HPP_

#include "gl-vector-3d.hpp"
#include "gl-rgba.hpp"
#include "map.hpp"

class cell {
public:
    cell();
    virtual ~cell();

    bool shadow_;
    // Move this to low-res sub-map?
    float distance_;
    unsigned char layer_[LAYER_COUNT - 1];
    gl_vector_3d position_;
    gl_vector_3d normal_;
    gl_rgba light_;
};

#endif
