#ifndef GL_VECTOR3_HPP_
#define GL_VECTOR3_HPP_

#include <SDL2/SDL_opengl.h>

class gl_vector3 {
public:
    gl_vector3();
    gl_vector3(float x, float y, float z);
    virtual ~gl_vector3();

    float length();
    void normalize();
    void reflect(gl_vector3 const &normal);

    gl_vector3 &operator+=(gl_vector3 const &rhs);
    gl_vector3 &operator-=(gl_vector3 const &rhs);
    gl_vector3 &operator*=(float const &rhs);
    gl_vector3 &operator/=(float const &rhs);

    float dot_product(gl_vector3 const &rhs) const;
    gl_vector3 cross_product(gl_vector3 const &rhs) const;
    gl_vector3 interpolate(gl_vector3 const &rhs, float scalar) const;

    void set_data(float x, float y, float z);
    void set_x(float x);
    void set_y(float y);
    void set_z(float z);

    float *get_data();
    float get_x() const;
    float get_y() const;
    float get_z() const;

private:
    float data_[3];
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

inline float dot_product(gl_vector3 const &lhs, gl_vector3 const &rhs)
{
    return lhs.dot_product(rhs);
}

inline gl_vector3 cross_product(gl_vector3 const &lhs, gl_vector3 const &rhs)
{
    return lhs.cross_product(lhs);
}

inline bool operator==(gl_vector3 const &lhs, gl_vector3 const &rhs)
{
    return ((lhs.get_x() == rhs.get_x())
            && (lhs.get_y() == rhs.get_y())
            && (lhs.get_z() == rhs.get_z()));
}

inline bool operator!=(gl_vector3 const &lhs, gl_vector3 const &rhs)
{
    return !operator==(lhs, rhs);
}

#endif
