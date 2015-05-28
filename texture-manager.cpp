/*
 * texture-manager.cpp
 * 2006 Shamus Young
 *
 * This loads in textures. Nothin' fancy.
 */

#include "texture-manager.hpp"

#include <sstream>

texture_manager::texture_manager()
{
}

texture_manager::~texture_manager()
{
}

void texture_manager::init()
{
}

void texture_manager::term(void)
{
    std::vector<texture_item *>::iterator itr;

    for(itr = textures_.begin(); itr != textures_.end(); ++itr) {
        delete *itr;
    }
}

GLuint texture_manager::from_name(std::string const &basename)
{
    GLuint result = 0;
    std::vector<texture_item *>::iterator itr;

    for(itr = textures_.begin(); itr != textures_.end(); ++itr) {
        if((*itr)->get_name() == basename) {
            result = (*itr)->get_id();

            break;
        }
    }

    if(itr == textures_.end()) {
        texture_item *t = load(basename);

        if(t != NULL) {
            result = t->get_id();
        }
        else {
            result = 0;
        }
    }

    return result;
}

// Texture id
texture_item *texture_manager::load(std::string const &basename)
{
    texture_item *t = new texture_item;
    t->set_name(basename);

    std::stringstream filename_stream;
    filename_stream << "textures" << "/" << basename;

    std::string filename;
    filename_stream >> filename;

    SDL_Surface *image(load_bmp(filename));

    if(image == NULL) {
        t->set_id(0);
        
        return NULL;
    }

    GLint num_colors = image->format->BytesPerPixel;
    GLenum texture_format;

    if(num_colors == 4) {
        if(image->format->Rmask == 0x000000FF) {
            texture_format = GL_RGBA;
        }
        else {
            texture_format = GL_BGRA;
        }
    }
    else if(num_colors == 3) {
        if(image->format->Rmask == 0x000000FF) {
            texture_format = GL_RGB;
        }
        else {
            texture_format = GL_BGR;
        }
    }
    else {
        // Some other color, likely a problem
    }

    // Create the texture
    glGenTextures(1, t->get_id_data());

    // Typical texture generation using data from the bitmap
    glBindTexture(GL_TEXTURE_2D, t->get_id());
    
    // Linear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Generate the texture
    glTexImage2D(GL_TEXTURE_2D,
                 /* level */ 0,
                 num_colors,
                 image->w,
                 image->h,
                 /* border */ 0,
                 texture_format,
                 GL_UNSIGNED_BYTE,
                 image->pixels);

    // Free the surface
    SDL_FreeSurface(image);

    textures_.push_back(t);

    return textures_.back();
}

SDL_Surface *texture_manager::load_bmp(std::string const &filename)
{
    if(filename.empty()) {
        return NULL;
    }

    SDL_Surface *surface(SDL_LoadBMP(filename.c_str()));

    if(surface == NULL) {
        // unable to load
        return NULL;
    }
    else if((surface->w & (surface->w - 1)) != 0) {
        // width not power of 2
        return NULL;
    }
    else if((surface->h & (surface->h - 1)) != 0) {
        // height not power of 2
        return NULL;
    }

    return surface;
}
