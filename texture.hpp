#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <SDL_opengl.h>

class texture {
public:
    texture();
    virtual ~texture();

    texture *next_;
    GLuint id_;
    char name_[16];
    char *image_name_;
    char *mask_name_;
    // AUX_RGBImageRec *image_;
};

unsigned int texture_from_name(char const *name);
void texture_init(void);
void texture_term(void);

#endif
