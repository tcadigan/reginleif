#ifndef TEXTURE_ITEM_HPP_
#define TEXTURE_ITEM_HPP_

#include <SDL_opengl.h>
#include <string>

class texture_item {
public:
    texture_item();
    virtual ~texture_item();

    GLuint *get_id_data();
    GLuint get_id() const;
    std::string get_name() const;

    void set_id(GLuint id);
    void set_name(std::string const &name);

private:
    GLuint id_;
    std::string name_;
};

#endif
