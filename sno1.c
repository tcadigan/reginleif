/* Snobol III */

#include "sno1.h"
#include "sno2.h"
#include "sno4.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct node *freespace_head = NULL;

int freesize = 0;
struct node *freespace = NULL;
struct node *freelist = NULL;
int *fault = NULL;

int fin;
int fout;

/* Allocates a string to be printed */
void mes(char *s) {
    sysput(sno_strstr(s));
}

struct node *init(char *s, int t)
{
    struct node *a;
    struct node *b;

    a = sno_strstr(s);
    b = look(a);
    delete(a);
    b->typ = t;

    return b;
}

int main(int argc, char *argv[])
{
    struct node *a;
    struct node *b;
    struct node *c;

    if(argc > 1) {
	fin = open(argv[1], 0);
	
	if(fin < 0) {
	    mes("Cannot open input");

	    return 0;
	}
    }

    dup2(STDOUT_FILENO, fout);
    lookf = init("f", 0);
    looks = init("s", 0);
    lookend = init("end", 0);
    lookstart = init("start", 0);
    lookdef = init("define", 0);
    lookret = init("return", 0);
    lookfret = init("freturn", 0);
    init("syspit", 3);
    init("syspot", 4);

    c = compile();
    a = c;

    while(lookend->typ != 2) {
	b = compile();
	a->p1 = b;
	a = b;
    }

    cfail = 1;
    a->p1 = 0;

    if(lookstart->typ == 2) {
	c = lookstart->p2;
    }

    c = execute(c);

    while(c) {
	c = execute(c);
    }

    fflush(stdout);

    return 0;
}

struct node *syspit(void)
{
    struct node *b;
    struct node *c;
    struct node *d;
    int a;

    a = getchar();
    
    if(a == '\n') {
	return 0;
    }

    c = alloc();
    b = c;

    while(a != '\n') {
	d = alloc();
	c->p1 = d;
	c = d;

	while(1) {
	    c->ch = a;
	    
	    if(a == '\0') {
		if(fin) {
		    close(fin);
		    fin = 0;
		    a = getchar();
		    continue;
		}

		rfail = 1;
		b->p2 = c;
		b = 0;

		return b;
	    }
	    else {
		break;
	    }
	}

	a = getchar();
    }

    b->p2 = c;
    
    if(rfail) {
	delete(b);
	b = 0;
    }

    return b;
}

/* Output a string character by character */
void syspot(struct node *string)
{
    struct node *pos;
    struct node *end;

    if(string != NULL) {
	pos = string->p1;
	end = string->p2;

	while(pos != end) {
	    putchar(pos->ch);
	    pos = pos->p1;
	}

	/* 
	 * The final character, 
	 * one exists because non-NULL
	 */
	putchar(pos->ch);
    }

    putchar('\n');
}

/*
 * There is a head node that links backwards
 * to last char, and forwards to first char.
 * Every char backward links as well.
 */
struct node *sno_strstr(char *s)
{
    char *str_pos = s;
    struct node *pos;
    struct node *head;

    pos = alloc();
    head = pos;

    while(*str_pos != '\0') {
	pos->p1 = alloc();
	pos->p1->p2 = pos;
	pos->p1->ch = *str_pos;
	pos = pos->p1;
	++str_pos;
    }

    head->p2 = pos;

    return head;
}

/* Provide quasi-enumeration for the symbols */
int class(int c) {
    switch(c) {
    case ')':
	return 1;
    case '(':
	return 2;
    case '\t':
    case ' ':
	return 3;
    case '+':
	return 4;
    case '-':
	return 5;
    case '*':
	return 6;
    case '/':
	return 7;
    case '$':
	return 8;
    case '"':
    case '\'':
	return 9;
    case '=':
	return 10;
    case ',':
	return 11;
    default:
	return 0;
    }
}

/* Get nodes from a pool of created ones */
struct node *alloc(void)
{
    struct node *result = NULL;
    struct node *freespace_position = NULL;
    int expand_amount = 200;
    int i;

    /* There is no freespace at all */
    if(freespace_head == NULL) {
	freespace_head = (struct node *)malloc(sizeof(struct node));
	
	if(freespace_head == NULL) {
	    fflush(stdout);
	    write(fout, "Out of free space\n", strlen("Out of free space\n"));

	    exit(1);
	}
    }

    /* Expand by freespace nodes */
    if(freespace_head->p1 == NULL) {
	freespace_position = freespace_head;
	for(i = 0; i < expand_amount; ++i) {
	    freespace_position->p1 = (struct node *)malloc(sizeof(struct node));
	    
	    if(freespace_position->p1 == NULL) {
		fflush(stdout);
		write(fout, "Out of free space\n", strlen("Out of free space\n"));

		exit(1);
	    }
	    
	    freespace_position->p1->p2 = freespace_position;
	    freespace_position = freespace_position->p1;
	}
    }

    /* Break off the head */
    result = freespace_head;
    freespace_head = freespace_head->p1;
    freespace_head->p2 = NULL;

    result->p1 = NULL;
    
    return result;
}

/* Destroy a single node */
void sno_free(struct node *pointer)
{
    pointer->p1 = freespace_head;
    pointer->p2 = NULL;
    pointer->typ = '\0';
    pointer->ch = '\0';

    freespace_head->p2 = pointer;
    freespace_head = pointer;
}

/* Simple count of the freespace nodes */
int nfree(void)
{
    struct node *pos = freespace_head;
    int result = 0;

    while(pos != NULL) {
	++result;
	pos = pos->p1;
    }

    return result;
}

struct node *look(struct node *string)
{
    struct node *i;
    struct node *j;
    struct node *k;

    k = 0;
    i = namelist;

    while(i) {
	j = i->p1;
	
	if(equal(j->p1, string) == 0) {
	    return j;
	}

	k = i;
	i = k->p2;
    }

    i = alloc();
    i->p2 = 0;

    if(k) {
	k->p2 = i;
    }
    else {
	namelist = i;
    }

    j = alloc();
    i->p1 = j;
    j->p1 = copy(string);
    j->p2 = 0;
    j->typ = 0;

    return j;
}

/* 
 * Copy a string into a new one,
 * character by character
 */
struct node *copy(struct node *string)
{
    struct node *new_pos = NULL;
    struct node *new_head = NULL;
    struct node *old_pos = NULL;
    struct node *old_tail = NULL;

    if(string != NULL) {
	new_pos = alloc();
	new_head = new_pos;

	old_pos = string->p1;
	old_tail = string->p2;

	while(old_pos != old_tail) {
	    new_pos->p1 = alloc();
	    new_pos->p1->p2 = new_pos;
	    new_pos->p1->ch = old_pos->ch;
	    new_pos = new_pos->p1;

	    /* Move forward in the old string */
	    old_pos = old_pos->p1;
	}

	/* Copy last character */
	new_pos->p1 = alloc();
	new_pos->p1->p2 = new_pos;
	new_pos->ch = old_pos->ch;

	/* Move to last character */
	new_pos = new_pos->p1;
    }

    new_head->p2 = new_pos;

    return new_head;
}

int equal(struct node *string1, struct node *string2)
{
    struct node *i;
    struct node *j;
    struct node *k;
    struct node *l;
    int n;
    int m;

    if(string1 == 0) {
	if(string2 == 0) {
	    return 0;
	}

	return -1;
    }

    if(string2 == 0) {
	return 1;
    }

    i = string1;
    j = string1->p2;
    k = string2;
    l = string2->p2;

    while(1) {
	i = i->p1;
	m = i->ch;
	k = k->p1;
	n = k->ch;

	if(m > n) {
	    return 1;
	}
	
	if(m < n) {
	    return -1;
	}

	if(i == j) {
	    if(k == l) {
		return 0;
	    }

	    return -1;
	}

	if(k == l) {
	    return 1;
	}
    }
}

int strbin(struct node *string)
{
    int n;
    int m;
    int sign;
    struct node *p;
    struct node *q;
    struct node *s;

    s = string;
    n = 0;
    
    if(s == 0) {
	return 0;
    }

    p = s->p1;
    q = s->p2;
    sign = 1;

    if(class(p->ch) == 5) {
	/* minus */
	sign = -1;

	if(p == q) {
	    return 0;
	}

	p = p->p1;
    }

    while(1) {
	m = p->ch - '0';
	if((m > 9) || (m <0)) {
	    writes("Bad integer string");
	}

	n = (n * 10) + m;

	if(p == q) {
	    return (n * sign);
	}

	p = p->p1;
    }
}

struct node *binstr(int binary) {
    int n;
    int sign;
    struct node *m;
    struct node *p;
    struct node *q;

    n = binary;
    p = alloc();
    q = alloc();
    sign = 1;

    if(binary < 0) {
	sign = -1;
	n = -binary;
    }

    p->p2 = q;

    while(1) {
	q->ch = (n % 10) + '0';
	n = n / 10;

	if(n == 0) {
	    if(sign < 0) {
		m = alloc();
		m->p1 = q;
		q = m;
		q->ch = '-';
	    }

	    p->p1 = q;

	    return p;
	}

	m = alloc();
	m->p1 = q;
	q = m;
    }
}

/* Add operation */
struct node *add(struct node *string1, struct node *string2)
{
    return binstr(strbin(string1) + strbin(string2));
}

/* Subtract operation */
struct node *sub(struct node *string1, struct node *string2)
{
    return binstr(strbin(string1) - strbin(string2));
}

/* Multiply operation */
struct node *mult(struct node *string1, struct node *string2)
{
    return binstr(strbin(string1) - strbin(string2));
}

/* Division operation */
struct node *sno_div(struct node *string1, struct node *string2)
{
    return binstr(strbin(string1) / strbin(string2));
}

struct node *cat(struct node *string1, struct node *string2)
{
    struct node *a;
    struct node *b;

    if(string1 == 0) {
	return copy(string2);
    }
    
    if(string2 == 0) {
	return copy(string1);
    }

    a = copy(string1);
    b = copy(string2);
    a->p2->p1 = b->p1;
    a->p2 = b->p2;
    sno_free(b);

    return a;
}

struct node *dcat(struct node *a, struct node *b)
{
    struct node *c;

    c = cat(a, b);
    delete(a);
    delete(b);

    return c;
}

/* Delete a string, one node at a time */
void delete(struct node *string)
{
    struct node *pos;
    struct node *end;

    if(string != NULL) {
	pos = string->p1;
	end = string->p2;

	while(pos != end) {
	    pos = pos->p1;
	    sno_free(pos->p2);
	}

	/*
	 * The final character,
	 * one exists because non-NULL
	 */
	sno_free(pos);
    }
}

/* Print string then delete it */
void sysput(struct node *string)
{
    syspot(string);
    delete(string);
}

void dump(void)
{
    dump1(namelist);
}

int dump1(struct node *base)
{
    struct node *b;
    struct node *c;
    struct node *e;
    struct node *d;
    
    while(base) {
	b = base->p1;
	c = binstr(b->typ);
	d = sno_strstr("  ");
	e = dcat(c, d);
	sysput(cat(e, b->p1));
	delete(e);
	
	if(b->typ == 1) {
	    c = sno_strstr("   ");
	    sysput(cat(c, b->p2));
	    delete(c);
	}

	base = base->p2;
    }

    return 0;
}

int writes(char *s)
{
    sysput(dcat(binstr(lc), dcat(sno_strstr("\t"), sno_strstr(s))));
    fflush(stdout);

    if(cfail) {
	dump();
	fflush(stdout);

	exit(1);
    }

    while(sno_getc());

    while(compile());

    fflush(stdout);

    exit(1);
}

struct node *sno_getc(void)
{
    struct node *a;
    static struct node *line;
    static int linflg;

    while(line == 0) {
	line = syspit();
	
	if(rfail) {
	    ++cfail;
	    writes("EOF on input");
	}

	++lc;
    }

    if(linflg) {
	line = 0;
	linflg = 0;

	return NULL;
    }

    a = line->p1;

    if(a == line->p2) {
	sno_free(line);
	++linflg;
    }
    else {
	line->p1 = a->p1;
    }

    return a;
}
