/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#ifndef DEF_FUNC_TAB_H_
#define DEF_FUNC_TAB_H_

struct func_tab {
    char f_char;
    int (*f_funct)();
};

extern struct func_tab list[];

#endif
