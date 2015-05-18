/*
 * win.cpp
 * 2006 Shamus Young
 *
 * Create the main window and make it go.
 */

#define CONSOLE_SIZE 30000
#define CONSOLE_PAGE 1000
#define MOUSE_MOVEMENT 0.4f

#include "win.hpp"

// static HWND hwnd;
// static HINSTANCE module;
// static int width;
// static int height;
// static bool lmb;
// static bool rmb;
static bool mouse_forced;
// static point mouse_pos;
// static point select_pos;

void CenterCursor()
{
    // int center_x;
    // int center_y;
    // RECT rect;

    // SetCursor(NULL);
    mouse_forced = true;
    // GetWindowRect(hwnd, &rect);
    // center_x = rect.left + ((rect.right - rect.left) / 2);
    // center_y = rect.top + ((rect.bottom - rect.top) / 2);
    // SetCursorPos(center_x, center_y);
}

void MoveCursor(int x, int y)
{
    // int center_x;
    // int center_y;
    // RECT rect;

    // SetCursor(NULL);
    mouse_forced = true;
    // GetWindowRect(hwnd, &rect);
    // center_x = rect.left + x;
    // center_y = rect.top + y;
    // SetCursorPos(center_x, center_y);
}

// LRESULT CALLBACKWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
//             MoveCursor(selecte_pos.x, select_pos.y);
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
//             delta_x = (float)(mouse_pos.x - p.x) / MOUSE_MOVEMENT;
//             delta_y = (float)(mouse_pos.y - p.y) / MOUSE_MOVEMENT;

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
