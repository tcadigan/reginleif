#ifndef GL_VECTOR_2D_HPP_
#define GL_VECTOR_2D_HPP_

class gl_vector_2d {
public:
    gl_vector_2d();
    gl_vector_2d(float x, float y);
    virtual ~gl_vector_2d();

    float x_;
    float y_;
};

gl_vector_2d gl_vector_add(gl_vector_2d val1, gl_vector_2d val2);
gl_vector_2d gl_vector_subtract(gl_vector_2d val1, gl_vector_2d val2);

#endif
