#ifndef WIN_HPP_
#define WIN_HPP_

// HWND WinHwnd(void);
void WinPopup(char *message, ...);
void WinTerm(void);
bool WinInit(void);
int WinWidth(void);
int WinHeight(void);
void WinMousePosition(int *x, int *y);

#endif
