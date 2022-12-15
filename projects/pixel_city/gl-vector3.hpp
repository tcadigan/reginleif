#ifndef __GL_VECTOR3_HPP__
#define __GL_VECTOR3_HPP__

#include <SDL2/SDL_opengl.h>

#include <array>

class gl_vector3 {
public:
    gl_vector3() = default;
    gl_vector3(float x, float y, float z);

    float length();
    void normalize();

    gl_vector3 &operator+=(gl_vector3 const &rhs);
    gl_vector3 &operator-=(gl_vector3 const &rhs);
    gl_vector3 &operator*=(float const &rhs);
    gl_vector3 &operator/=(float const &rhs);

    gl_vector3 cross_product(gl_vector3 const &rhs) const;
    gl_vector3 interpolate(gl_vector3 const &rhs, float scalar) const;

    void set_x(float x);
    void set_y(float y);
    void set_z(float z);

    std::array<float, 3> get_data() const;
    float get_x() const;
    float get_y() const;
    float get_z() const;

private:
    std::array<float, 3> data_;
};

inline gl_vector3 operator+(gl_vector3 lhs, gl_vector3 const &rhs)
{
    lhs += rhs;

    return lhs;
}

inline gl_vector3 operator-(gl_vector3 lhs, gl_vector3 const &rhs)
{
    lhs -= rhs;

    return lhs;
}

inline gl_vector3 operator*(gl_vector3 lhs, float const &rhs)
{
    lhs *= rhs;

    return lhs;
}

inline gl_vector3 operator/(gl_vector3 lhs, float const &rhs)
{
    lhs /= rhs;

    return lhs;
}

inline gl_vector3 cross_product(gl_vector3 const &lhs, gl_vector3 const &rhs)
{
    return lhs.cross_product(rhs);
}

#endif /* _GL_VECTOR3_HPP__ */
