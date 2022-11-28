#ifndef TEXTURE_MANAGER_HPP_
#define TEXTURE_MANAGER_HPP_

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <string>

#include "texture-item.hpp"
#include <vector>

class texture_manager {
public:
    texture_manager();
    virtual ~texture_manager();

    void init();
    void term();

    GLuint from_name(std::string const &basename);

private:
    texture_item *load(std::string const &basename);
    SDL_Surface *load_bmp(std::string const &filename);

    std::vector<texture_item *> textures_;
};

#endif
