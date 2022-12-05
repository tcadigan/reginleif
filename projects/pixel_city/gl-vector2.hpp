#ifndef GL_VECTOR2_HPP_
#define GL_VECTOR2_HPP_

#include <SDL2/SDL_opengl.h>

class gl_vector2 {
public:
    gl_vector2();
    gl_vector2(float x, float y);
    virtual ~gl_vector2();

    float length();
    void normalize();
    void reflect(gl_vector2 const &normal);

    gl_vector2 &operator+=(gl_vector2 const &rhs);
    gl_vector2 &operator-=(gl_vector2 const &rhs);
    gl_vector2 &operator*=(float const &rhs);
    gl_vector2 &operator/=(float const &rhs);

    float dot_product(gl_vector2 const &rhs) const;
    gl_vector2 interpolate(gl_vector2 const &rhs, float scalar) const;

    void set_data(float x, float y);
    void set_x(float x);
    void set_y(float y);

    float *get_data();
    float get_x() const;
    float get_y() const;

private:
    float data_[2];
};

inline gl_vector2 operator+(gl_vector2 lhs, gl_vector2 const &rhs)
{
    lhs += rhs;

    return lhs;
}

inline gl_vector2 operator-(gl_vector2 lhs, gl_vector2 const &rhs)
{
    lhs -= rhs;

    return lhs;
}

inline gl_vector2 operator*(gl_vector2 lhs, float const &rhs)
{
    lhs *= rhs;

    return lhs;
}

inline gl_vector2 operator/(gl_vector2 lhs, float const &rhs)
{
    lhs /= rhs;

    return lhs;
}

inline float dot_product(gl_vector2 const &lhs, gl_vector2 const &rhs)
{
    return lhs.dot_product(rhs);
}

inline bool operator==(gl_vector2 const &lhs, gl_vector2 const &rhs)
{
    return ((lhs.get_x() == rhs.get_x())
            && (lhs.get_y() == rhs.get_y()));
}

inline bool operator!=(gl_vector2 const &lhs, gl_vector2 const &rhs)
{
    return !operator==(lhs, rhs);
}

#endif
