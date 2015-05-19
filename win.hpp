#ifndef WIN_HPP_
#define WIN_HPP_

// Constants
#define CONSOLE_SIZE 30000
#define CONSOLE_PAGE 1000
#define MOUSE_MOVEMENT 0.4f

// HWND WinHwnd(void);
void WinPopup(char *message, ...);
void WinTerm(void);
bool WinInit(void);
int WinWidth(void);
int WinHeight(void);
void WinMousePosition(int *x, int *y);

#endif
