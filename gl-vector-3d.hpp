#ifndef GL_VECTOR_3D_HPP_
#define GL_VECTOR_3D_HPP_

class gl_vector_3d {
public:
    gl_vector_3d();
    gl_vector_3d(float x, float y, float z);
    virtual ~gl_vector_3d();

    float x_;
    float y_;
    float z_;
};

gl_vector_3d gl_vector_add(gl_vector_3d v1, gl_vector_3d v2);
gl_vector_3d gl_vector_cross_product(gl_vector_3d v1, gl_vector_3d v2);
float gl_vector_dot_product(gl_vector_3d v1, gl_vector_3d v2);
void gl_vector_gl(gl_vector_3d v);
gl_vector_3d gl_vector_interpolate(gl_vector_3d v1, 
                                   gl_vector_3d v2,
                                   float scalar);

gl_vector_3d gl_vector_invert(gl_vector_3d v);
float gl_vector_length(gl_vector_3d v);
gl_vector_3d gl_vector_normalize(gl_vector_3d v);
gl_vector_3d gl_vector_scale(gl_vector_3d v, float scale);
gl_vector_3d gl_vector_subtract(gl_vector_3d v1, gl_vector_3d v2);
gl_vector_3d gl_vector_reflect(gl_vector_3d ray, gl_vector_3d normal);

#endif
