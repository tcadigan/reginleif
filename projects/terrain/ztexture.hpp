#ifndef ZTEXTURE_HPP_
#define ZTEXTURE_HPP_

#include <SDL2/SDL_opengl.h>

class ztexture {
public:
    ztexture();
    virtual ~ztexture();

    void set_ready(GLboolean ready);
    void set_size(GLint size);
    void set_texture(GLuint texture);

    GLboolean get_ready() const;
    GLint get_size() const;
    GLuint *get_texture_address();
    GLuint get_texture() const;
    
private:
    GLboolean ready_;
    GLint size_;
    GLuint texture_;
};

#endif
