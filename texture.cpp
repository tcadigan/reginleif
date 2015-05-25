/*
 * texture.cpp
 * 2006 Shamus Young
 *
 * This loads in textures. Nothin' fancy.
 */

#include "texture.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

static texture *head_texture;

texture::texture()
{
}

texture::~texture()
{
}

void texture::init()
{
}

void texture::term(void)
{
    texture *t;

    while(head_texture != NULL) {
        t = head_texture->next_;
        free(head_texture);
        head_texture = t;
    }
}

GLuint texture::from_name(char const *name)
{
    texture *t;
    
    for(t = head_texture; t != NULL; t = t->next_) {
        if(strcmp(name, t->name_) == 0) {
            return t->id_;
        }
    }

    t = load(name);

    if(t != NULL) {
        return t->id_;
    }

    return 0;
}

// Texture id
texture *texture::load(char const *name)
{
    char filename[128];
    texture *t;

    t = new texture;
    strcpy(t->name_, name);
    sprintf(filename, "textures/%s.bmp", name);
    // t->image = LoadBMP(filename);

    // if(t->image == NULL) {
    //     t->id = 0;
        
    //     return t;
    // }

    // Create the texture
    glGenTextures(1, &t->id_);

    // Typical texture generation using data from the bitmap
    glBindTexture(GL_TEXTURE_2D, t->id_);

    // // Generate the texture
    // glTexImage2D(GL_TEXTURE_2D,
    //              0,
    //              3, 
    //              t->image->sizeX,
    //              t->image->sizeY,
    //              0,
    //              GL_RGB,
    //              GL_UNSIGNED_BYTE,
    //              t->image->data);

    // free(t->image);
    
    // Linear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    // Linear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    t->next_ = head_texture;
    head_texture = t;
    
    return t;
}

// static AUX_RGBImageRec *LoadBMP(char *Filename)
// {
//     FILE *File = NULL;
    
//     if(Filename == NULL) {
//         return NULL;
//     }

//     File = fopen(Filename, "r");

//     if(File != 0) {
//         fclose(file);

//         return auxDIBImageLoad(Filename);
//     }

//     return NULL;
// }
