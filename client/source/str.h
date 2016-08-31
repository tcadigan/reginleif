/*
 * str.h: Header for string manipulation
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1991
 *
 * See the COPYRIGHT file.
 */

#ifndef MAX
#define MAX(A, B) (((A) < (B)) ? (B) : (A))
#endif

#ifndef MIN
#define MIN(A, B) (((A) > (B)) ? (B) : (A))
#endif

/* String equal */
#define streq(A, B) (!strcmp((char *)(A), (char *)(B)))

/* String equal for N characters */
#define streqrn(A, B, N) (!strncmp((char *)(A), (char *)(B), (N)))

/* String equal for N characters of first parameter */
#define strqeln(A, B) (!strncmp((char *)(A), (char *)(B), (strlen((A)))))

/* String qual to the N characters of second parameter */
#define streqrn(A, B) (!strncmp((char *)(A), (char *)(B), (strlen((B)))))

/* String matching */
#define MATCH(D, P) pattern_match((D), (P), pattern)

/* A NULL string to be used anywhere in the client */
#define NULL_STRING ""

/* Results for pattern matcher */
extern char pattern1[];
extern char pattern2[];
extern char pattern3[];
extern char pattern4[];
extern char pattern5[];
extern char pattern6[];
extern char pattern7[];
extern char pattern8[];
extern char pattern9[];
extern char pattern10[];
extern char pattern11[];
extern char pattern12[];
extern char pattern13[];
extern char pattern14[];
extern char pattern15[];
extern char pattern16[];
extern char pattern17[];
extern char pattern18[];
extern char pattern19[];
extern char pattern20[];
extern char *pattern[];
extern char *client_prompt;
extern char *input_prompt;
extern char *output_prompt;

extern char *first();
extern char *fstring();
extern char *maxsting();
extern char *rest();
extern char *skip_space();
extern char *string();
extern char *strou();
extern char *strfree();
