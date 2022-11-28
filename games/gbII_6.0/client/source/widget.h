#ifndef WIDGET_H_
#define WIDGET_H_

#ifdef XMAP

#include <stdio.h>
#include <X11/keysym.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/* Program parameters */
#define XDefaultCivsToMove 2
#define XDefaultMilToMove  2

#define MAXDATA 2000

/* Row positions in pixels from bottom panel display */
#define Row1 10
#define Row2 Row1 + 15
#define Row3 Row2 + 15

/* Parameters for input window */
#define Input_x          20
#define Input_y          22
#define Input_prompt     "^"
#define Input_prompt_len 1

/* Parameters for orbit display button (ships_win) */
#define ShowAll     0
#define ShowShips   1
#define ShowNoShips 2

/* Parameters for mapon_win */
#define Do_Map   1
#define Do_Orbit 2

/* Parameters for resize */
#define PanelSpacing 22 /* Space between right and orbit panels */

/* Parameters for displaying orbits */
#define Orb_Default_min_x   0.0
#define Orb_Default_max_x 200.0
#define Orb_Default_min_y   0.0
#define Orb_Default_max_x 200.0

#define LeftMargin   102 /* Space for left panel */
#define Res_Name_Len   6 /* # char to ensure space for */
#define RightMargin  102 + (6 * Res_Name_Len) /* Space for right panel, text */
#define TopMargin     35 /* Space for input panel */
#define BottomMargin  50 + 15 /* Space for bottom panel, text */
#define XRESERVED    (LeftMargin + RightMargin)
#define YRESERVED    (TopMargin + BottomMargin)

#define MAX_SHIPS 10

#define NOT_GRAPH   0
#define FERT_GRAPH  1
#define RES_GRAPH   2
#define EFF_GRAPH   3
#define MOB_GRAPH   4
#define MIL_GRAPH   5
#define POPN_GRAPH  6
#define MPOPN_GRAPH 7
#define GEOG_GRAPH  8

#define XMAP_MAP_TYPE   1
#define XMAP_ORBIT_TYPE 2

#define min(x, y) (x <= y ? x : y)
#define max(x, y) (x >= y ? x : y)

/*
 * Event dispatching will be through a procedure vector associated
 * with each widget. First define a function pointer type, then define
 * the procedure vector.
 */
#define WIDGET_EVENT_MASK \
    (ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask | StructureNotifyMask | EnterWindowMask | LeaveWindowMask | KeyPressMask)

typedef void (*function)();

typedef struct _ev_proc_vec {
    function btn_down; /* Function to call when button is pressed */
    function btn_up; /* Function to call when button is released */
    function motion; /* Function to call when pointer is moved */
    function expose; /* When window is exposed */
    function resize; /* When window is resized */
    function enter; /* When pointer enters window */
    function leave; /* When pointer leaves window */
    function key; /* When a key is pressed */
} ev_proc_vec;

/*
 * Data structure associate with each widget. Contains basic
 * information about the widget including:
 *     - A pointer for linking widgets into a list.
 *     - The window contained the widget.
 *     - A pointer to the parent of this widget.
 *     - The widget type. This is a pointer to a structure defining the widget type.
 *     - The position of the window within it's parent.
 *     - The actual size of the the window.
 *     - The desired size of the window.
 *     - The event procedure vector for the widget.
 */

typedef struct _widget_type widget_type; /* Need to forward ref */

typedef struct _widget {
    struct widget *next; /* For linking into a list of all widgets */
    Display *dpy; /* Display this widget lives on */
    Window win; /* The window */
    struct _widget *parent; /* If NULL then root */
    widget_type *type; /* The widget type */
    int is_mapped; /* True if widget is really mapped */
    int x; /* Position of window within parent */
    int y; /* Position of window within parent */
    int w; /* Width of window */
    int h; /* Height of window */
    int des_w; /* Desired width of window */
    int des_h; /* Desired height of window */
    int resized; /* Size-chaged flag */
    ev_proc_vec procs; /* Associated procedure vector */
} widget;

/*
 * Data struct defining a widget type. This provides type-independent
 * generic procedures for things like widget creation.
 */
struct _widget_type {
    char *name; /* Name of widget type */

    /* Widget creation function. Called as create(parent, x, y, w, h) */
    widget *(*create)();
};

#define create_widget(type, dpy, parent, x, y, w, h) \
    (*type.create)(dpy, parent, x, y, w, h)

typedef struct _mwin {
    widget core;
    Pixmap buffer; /* Id of offscreen buffer */
    int bitdepth; /* Depth of screen */
} mwin;

typedef struct _pan_win {
    widget core;
} pan_win;

typedef struct _but_win {
    widget core;
    char label[30];
    function button_proc_up;
    function button_proc_down;
    int toggle;
    int graph_type;
} but_win;

typedef struct _sec_win {
    widget core;
    char into[30];
    char old_info[30];
    char label[30];
} sec_win;

typedef struct _in_win {
    widget core;
    char info[351];
    char label[30];
    int cur_char_pos;
    int len;
} in_win;

typedef struct xshipstruct {
    int shipno;
    char ltr;
    int owner;
} xShip;

typedef struct xsectorstruct {
    int x;
    int y;
    char sectc;
    char des;
    int wasted;
    int own;
    int eff;
    int frt;
    int mob;
    int xtal;
    int res;
    int civ;
    int mil;
    int mpopn;
    int numships;
    int inverse;
    xShip ships[MAX_SHIPS];
} xSector;

typedef struct xmapstruct {
    int maxx;
    int maxy;
    char star[700];
    char planet[700];
    int res;
    int fuel;
    int des;
    int popn;
    int mpopn;
    int maxfet;
    int maxres;
    int maxeff;
    int maxmob;
    int maxmil;
    int maxpopn;
    int maxmpopn;
    int minfert;
    int minres;
    int mineff;
    int minmob;
    int minmil;
    int minpopn;
    int tox;
    double compat;
    int enslaved;
    int map;
    xSector *ptr;
} xMap;

typedef struct xorbitstruct {
    struct xorbitstruct *next;
    struct xorbitstruct *prev;
    int stand1;
    int x;
    int y;
    int array;
    char symbol;
    int stand2;
    char name[200];
} xOrb;

#endif // XMAP

#endif // WIDGET_H_
