/*
 * win.cpp
 * 2006 Shamus Young
 *
 * Create the main window and make it go.
 */

#include "win.hpp"

#include <SDL.h>

#include "mouse-pointer.hpp"

win::win(ini_manager const &ini_mgr)
    : ini_mgr_(ini_mgr)
{
}

win::~win()
{
}

GLboolean win::init()
{
    mouse_movement_ = ini_mgr_.get_float("Settings", "mouse movement");

    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    surface_ = SDL_SetVideoMode(/* width */ 544,
                               /* height */ 640,
                               /* bpp */ 32,
                               SDL_OPENGL | SDL_RESIZABLE);

    if(surface_ == NULL) {
        return false;
    }

    return true;
}

void win::term()
{
    SDL_FreeSurface(surface_);
}

SDL_Surface *win::handle()
{
    return surface_;
}

GLint win::get_width()
{
    return surface_->w;
}

int win::get_height()
{
    return surface_->h;
}

void win::mouse_position(GLint *x, GLint *y)
{
    *x = select_pos_.get_x();
    *y = select_pos_.get_y();
}

void win::center_cursor()
{
    // int center_x;
    // int center_y;
    // RECT rect;

    // SetCursor(NULL);
    mouse_forced_ = true;
    // GetWindowRect(hwnd, &rect);
    // center_x = rect.left + ((rect.right - rect.left) / 2);
    // center_y = rect.top + ((rect.bottom - rect.top) / 2);
    // SetCursorPos(center_x, center_y);
}

void win::move_cursor(GLint x, GLint y)
{
    // int center_x;
    // int center_y;
    // RECT rect;

    // SetCursor(NULL);
    mouse_forced_ = true;
    // GetWindowRect(hwnd, &rect);
    // center_x = rect.left + x;
    // center_y = rect.top + y;
    // SetCursorPos(center_x, center_y);
}

// LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// {
//     RECT r;
//     float delta_x;
//     float delta_y;
//     point p;

//     switch(message) {
//     case WM_MOVE:
//         GetClientRect(hwnd, &r);
//         height = r.bottom - r.top;
//         width = r.right - r.left;

//         break;
//     case WM_LBUTTONDOWN:
//         lmb = true;
//         SetCapture(hWnd);

//         break;
//     case WM_RBUTTONDOWN:
//         rmb = true;
//         SetCapture(hWnd);

//         break;
//     case WM_LBUTTONUP:
//         lmb = false;
        
//         if(!rmb) {
//             ReleaseCapture();
//             MoveCursor(select_pos.x, select_pos.y);
//         }

//         break;
//     case WM_RBUTTONUP:
//         rmb = false;
        
//         if(!lmb) {
//             ReleaseCapture();
//             MoveCursor(select_pos.x, select_pos.y);
//         }

//         break;
//     case WM_MOUSEMOVE:
//         // Horizontal position of cursor
//         p.x = LOWORD(lParam);

//         // Vertical position of cursor
//         p.y = HIWORD(lParam);

//         if(!mouse_forced && !lmb && !rmb) {
//             select_pos = p;
//         }

//         if(mouse_forced) {
//             mouse_forced = false;
//         }
//         else if(rmb || lmb) {
//             CenterCursor();
//             delta_x = (float)(mouse_pos.x - p.x) / mouse_movement;
//             delta_y = (float)(mouse_pos.y - p.y) / mouse_movement;

//             if(rmb && lmb) {
//                 CameraSelectionYaw(delta_x);
//                 CameraSelectionZoom(-delta_y);
//             }
//             else if(rmb) {
//                 CameraPan(delta_x);
//                 CameraForward(delta_y);
//             }
//             else if(lmb) {
//                 CameraSelectionYaw(delta_x);
//                 CameraSelectionPitch(delta_y);
//             }
//         }

//         mouse_pos = 0;
        
//         break;
//     case WM_CLOSE:
//         AppQuit();

//         return 0;
//     }

//     return DefWindowProc(hWnd, message, wParam, lParam);
// }
