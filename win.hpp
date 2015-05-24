#ifndef WIN_HPP_
#define WIN_HPP_

// HWND win_hwnd(void);
void win_popup(char *message, ...);
void win_term(void);
bool win_init(void);
int win_width(void);
int win_height(void);
void win_mouse_position(int *x, int *y);

#endif
