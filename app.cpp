/*
 * app.cpp
 * 2006 Shamus Young
 *
 * This module contains the core of the program.
 */

#include "app.hpp"

// #include "camera.hpp"
// #include "console.hpp"
// #include "entity.hpp"
// #include "map.hpp"
// #include "mapTexture.hpp"
// #include "render.hpp"
// #include "texture.hpp"
// #include "win.hpp"
// #include "world.hpp"

// #pragma comment (lib, "opengl32.lib")
// #pragma comment (lib, "glu32.lib")
// #pramga comment (lib, "GLaux.lib")
 
static bool quit;
// static HINSTANCE instance;

// HINSTANCE AppInstance()
// {
//     return instance;
// }

void AppQuit()
{
    quit = true;
}

int main(int argc, char *argv[])
{

    return 0;
}

// int PASCAL WinMain(HINSTANCE instance_in, 
//                    HINSTANCE previous_instance, 
//                    LPSTR command_line,
//                    int show_style)
// {
//     MSG msg;
//     instance = instance_in;
    
//     WinInit();
//     ConsoleInit();
//     CameraInit();
//     RenderInit();
//     TextureInit();
//     MapInit();
//     MapTextureInit();
//     EntityInit();

//     while(!quit) {
//         if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
//             if(msg.message == WM_QUIT) {
//                 quit = true;
//             }
//             else {
//                 // Translate the message
//                 TranslateMessage(&msg);
                
//                 // Dispatch the message
//                 DispatchMessage(&msg);
//             }
//         }
//         else {
//             // Update
//             cameraUpdate();
//             EntityUpdate();
//             MapUpdate();
//             MapTextureUpdate();
//             WorldUpdate();
//             RenderUpdate();
//             Sleep(1);
//         }
//     }

//     EntityTerm();
//     TextureTerm();
//     MapTerm();
//     MapTextureTerm();
//     WorldTerm();
//     RenderTerm();
//     CameraTerm();
//     ConsoleTerm();
//     WinTerm();

//     return 0;
// }
