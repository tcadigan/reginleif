#ifndef WIN_HPP_
#define WIN_HPP_


#include <SDL.h>
#include <SDL_opengl.h>

#include "ini-manager.hpp"
#include "point.hpp"

class win {
public:
    win(ini_manager const &ini_mgr);
    virtual ~win();

    GLboolean init(void);
    void term(void);

    SDL_Surface *handle();
    GLint get_width(void);
    GLint get_height(void);
    void mouse_position(GLint *x, GLint *y);

private:
    void center_cursor();
    void move_cursor(GLint x, GLint y);

    ini_manager const &ini_mgr_;

    // static HWND hwnd;
    SDL_Surface *surface_;
    // static HINSTANCE module;
    GLint width_;
    GLint height_;
    // bool lmb;
    // bool rmb;
    GLboolean mouse_forced_;
    // point mouse_pos;
    point select_pos_;
    GLfloat mouse_movement_;
};

#endif
