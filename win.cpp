/*
 * win.cpp
 * 2006 Shamus Young
 *
 * Create the main window and make it go.
 */

#include "win.hpp"

#include <cstdarg>
#include <cstdio>

#include "ini.hpp"
#include "mouse-pointer.hpp"

// static HWND hwnd;
// static HINSTANCE module;
static int width;
static int height;
// static bool lmb;
// static bool rmb;
static bool mouse_forced;
// static point mouse_pos;
static point select_pos;
static float mouse_movement;

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

void WinPopup(char *message, ...)
{
    va_list marker;
    char buf[1024];

    va_start(marker, message);
    vsprintf(buf, message, marker);
    va_end(marker);

    // MessageBox(NULL, buf, "Terrain Viewer", MB_ICONSTOP | MB_OK | MB_TASKMODAL);
}

int WinWidth(void)
{
    return width;
}

void WinMousePosition(int *x, int *y)
{
    *x = select_pos.x_;
    *y = select_pos.y_;
}

int WinHeight(void)
{
    return height;
}

// void WinTerm(void)
// {
//     DetroyWindow(hwnd);
// }

// HWND WinHwnd(void)
// {
//     return hwnd;
// }

bool WinInit(void)
{
    ini_manager ini_mgr;

    mouse_movement = ini_mgr.get_float("Settings", "mouse movement");

    // WNDLASSEX wcex;

    // wcex.cbSize = sizeof(WNDCLASSEX);
    // wcex.style = CS_HREDRAW | CS_VREDRAW;
    // wcex.lpfnWndProc = (WNDPROC)WndProc;
    // wcex.cbClsExtra = 0;
    // wcex.cbWndExtra = 0;
    // wcex.hInstance = AppInstace();
    // wcex.hIcon = NULL;
    // wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    // wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    // wcex.lpszMenuName = NULL;
    // wcex.lpszClassName = "Terrain Viewer";
    // wcex.hIconSm = NULL;

    // if(!RegisterClassEx(&wcex)) {
    //     WinPopup("Cannot create window class");

    //     return false;
    // }

    // hwnd = CreateWindowEx(0,
    //                       "Terrain Viewer",
    //                       "Terrain Viewer",
    //                       WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    //                       CW_USEDEFAULT,
    //                       0,
    //                       544,
    //                       640,
    //                       NULL,
    //                       NULL,
    //                       AppInstance(),
    //                       NULL);

    // if(!hwnd) {
    //     WinPopup("Cannot create window");
        
    //     return false;
    // }

    // ShowWindow(hwnd, SW_SHOW);
    // UpdateWindow(hwnd);

    return true;
}
