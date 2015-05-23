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
#include <SDL_opengl.h>

struct texture {
    struct texture *next;
    GLuint id;
    char name[16];
    char *image_name;
    char *mask_name;
    // AUX_RGBImageRec *image;
};

static struct texture *head_texture;

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

// Texture id
static struct texture *LoadTexture(char const *name)
{
    char filename[128];
    struct texture *t;

    t = new struct texture;
    strcpy(t->name, name);
    sprintf(filename, "textures/%s.bmp", name);
    // t->image = LoadBMP(filename);

    // if(t->image == NULL) {
    //     t->id = 0;
        
    //     return t;
    // }

    // Create the texture
    glGenTextures(1, &t->id);

    // Typical texture generation using data from the bitmap
    glBindTexture(GL_TEXTURE_2D, t->id);

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

    t->next = head_texture;
    head_texture = t;
    
    return t;
}

unsigned int TextureFromName(char const*name)
{
    struct texture *t;
    
    for(t = head_texture; t != NULL; t = t->next) {
        if(strcmp(name, t->name) == 0) {
            return t->id;
        }
    }

    t = LoadTexture(name);

    if(t != NULL) {
        return t->id;
    }

    return 0;
}

void TextureInit()
{
}

void TextureTerm(void)
{
    struct texture *t;

    while(head_texture != NULL) {
        t = head_texture->next;
        free(head_texture);
        head_texture = t;
    }
}
