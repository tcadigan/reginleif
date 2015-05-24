#ifndef GL_BBOX_HPP_
#define GL_BBOX_HPP_

#include "gl-vector-3d.hpp"

class gl_bbox {
public:
    gl_bbox();
    virtual ~gl_bbox();

    gl_vector_3d min_;
    gl_vector_3d max_;
};

bool gl_bbox_test_point(gl_bbox box, gl_vector_3d point);
gl_bbox gl_bbox_contain_point(gl_bbox box, gl_vector_3d point);
gl_bbox gl_bbox_clear(void);

#endif
