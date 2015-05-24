#ifndef GL_RGBA_HPP_
#define GL_RGBA_HPP_

class gl_rgba {
public:
    gl_rgba();
    gl_rgba(float red, float green, float blue);
    gl_rgba(float luminance);
    gl_rgba(float red, float green, float blue, float alpha);
    gl_rgba(long c);
    virtual ~gl_rgba();

    float red_;
    float green_;
    float blue_;
    float alpha_;
};

gl_rgba gl_rgba_add(gl_rgba c1, gl_rgba c2);
gl_rgba gl_rgba_subtract(gl_rgba c1, gl_rgba c2);
gl_rgba gl_rgba_interpolate(gl_rgba c1, gl_rgba c2, float delta);
gl_rgba gl_rgba_scale(gl_rgba c, float scale);
gl_rgba gl_rgba_multiply(gl_rgba c1, gl_rgba c2);

#endif
