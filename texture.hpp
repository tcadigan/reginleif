#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <SDL_opengl.h>

class texture {
public:
    texture();
    virtual ~texture();

    void init();
    void term();

    GLuint from_name(char const *name);

private:
    texture *load(char const *name);

    GLuint id_;
    texture *next_;
    char name_[16];
    // AUX_RGBImageRec *image_;
};

#endif
