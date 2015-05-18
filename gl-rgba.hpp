#ifndef GL_RGBA_HPP_
#define GL_RGBA_HPP_

class GLrgba {
public:
    float red;
    float green;
    float blue;
    float alpha;
};

GLrgba glRgba(float red, float green, float blue);
GLrgba glRgba(float luminance);
GLrgba glRgba(float red, float green, float blue, float alpha);
GLrgba glRgba(long c);
GLrgba glRgbaAdd(GLrgba c1, GLrgba c2);
GLrgba glRgbaSubtract(GLrgba c1, GLrgba c2);
GLrgba glRgbaInterpolate(GLrgba c1, GLrgba c2, float delta);
GLrgba glRgbaScale(GLrgba c, float scale);
GLrgba glRgbaMultiply(GLrgba c1, GLrgba c2);

#endif
