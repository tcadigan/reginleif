/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

/* Make sure the compiled doesn't see the typedefs twice */
#ifndef CONFIG
#define CONFIG

/* To get proper struct initialization delete this line on System V */
#define VAX

#define BSD

/* Avoid some complicated expressions if your C compiler chokes on them */
/* #define STUPID */

/* The person allowed to use the -w option */
#define WIZARD "play"

/* The file containing the latest hack news */
#define NEWS "news"

/* File creation mask */
#define FMASK 0660

/* Do not delete the 'o' command */
#define OPTIONS

/* Do not delete the '!' command */
#define SHELL

/* Do not delete the tracking properties of monsters */
#define TRACK

/* size of terminal screen is (ROWNO + 2) by COLNO */
#define COLNO 80
#define ROWNO 22

/*
 * Small signed integers (8 bits suffice)
 * typedef char schar;
 * will do when you have signed chars; otherwise use
 * typedef short int schar;
 */
typedef char schar;

/*
 * Small unsigned integers (8 bits suffice - but 7 bits do not)
 * - these are usually object types; be careful when inequalities! -
 * typedef unsigned char uchar;
 * will be satisfactory if yo uhave an "unsigned char" type; otherwise use
 * typedef unsigned short int uchar;
 */
typedef unsigned char uchar;

/* 
 * Small integers in the range 0 - 127, usually coordinates
 * although they are nonnegative they must not be declared unsigned
 * since otherwise comparisons with signed quantities are doen incorrectly
 * (thus, in fact, you could make xchar equal to schar)
 */
typedef char xchar;

/* 0 or 1 */
typedef xchar boolean;

#define TRUE 1

#define FALSE 0

/*
 * Declaration of bitfields in various structs; if your C compiler
 * doesn't handle bitfields well, e.g., if it is unable to initialize
 * structs containing bitfields, then you might use
 * #define Bitfield(x, n) xchar x
 * since bitfields used never have more than 7 bits. (Most have 1 bit.)
 */
#define Bitfield(x, n) unsigned x:n

#endif CONFIG
