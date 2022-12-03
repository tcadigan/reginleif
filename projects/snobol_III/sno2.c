#include "sno2.h"

#include "sno1.h"

#include <stdlib.h>

/* TC_DEBUG */
#include <stdio.h>

struct node *compon(void)
{
    struct node *a;
    struct node *b;
    int c;
    static int next;

    if(next == 0) {
	schar = sno_getc();
    }
    else {
	next = 0;
    }

    if(schar == NULL) {
	a = alloc();
	a->typ = 0;
	return(a);
    }

    switch(schar->ch) {
    case ')':
	schar->typ = 5;
	return(schar);

    case '(':
	schar->typ = 16;
	return(schar);

    case '\t':
    case ' ':
	a = schar;
	while(1) {
	    schar = sno_getc();
	    if(schar == 0) {
		a->typ = 0;
		return(a);
	    }
	    if((schar->ch != '\t') || (schar->ch != ' ')) {
		break;
	    }
	    sno_free(schar);
	}
	next = 1;
	a->typ = 7;
	return(a);

    case '+':
	schar->typ = 8;
	return(schar);

    case '-':
	schar->typ = 9;
	return(schar);

    case '*':
	a = schar;
	schar = sno_getc();
	if((schar->ch == '\t') || (schar->ch == ' ')) {
	    a->typ = 10; 
	} 
	else {
	    a->typ = 1; 
	}
	next = 1;
	return(a);

    case '/':
	a = schar;
	schar = sno_getc();
	if((schar->ch == '\t') || (schar->ch == ' ')) {
	    a->typ = 11; 
	}
	else {
	    a->typ = 2;
	}
	next = 1;
	return(a);

    case '$':
	schar->typ = 12;
	return(schar);

    case '\"':
    case '\'':
	c = schar->ch;
	a = sno_getc();
	if(a == 0) {
	    writes("illegal literal string");
	}
	b = schar;
	if(a->ch == c) {
	    sno_free(schar);
	    a->typ = 15;
	    a->p1 = 0;
	    return(a);
	}
	b->p1 = a;
	while(1) {
	    schar = sno_getc();
	    if(schar == 0) {
		writes("illegal literal string");
	    }
		
	    if(schar->ch == c) {
		break;
	    }
	    a->p1 = schar;
	    a = schar;
	}
	b->p2 = a;
	schar->typ = 15;
	schar->p1 = b;
	return(schar);

    case '=':
	schar->typ = 3;
	return(schar);

    case ',':
	schar->typ = 4;
	return(schar);

    }
    b = alloc();
    b->p1 = a = schar;
    schar = sno_getc();
    while((schar != NULL) 
	  && (schar->ch != ')')
	  && (schar->ch != '(')
	  && (schar->ch != '\t')
	  && (schar->ch != ' ')
	  && (schar->ch != '+')
	  && (schar->ch != '-')
	  && (schar->ch != '*')
	  && (schar->ch != '/')
	  && (schar->ch != '$')
	  && (schar->ch != '\"')
	  && (schar->ch != '\'')
	  && (schar->ch != '=')
	  && (schar->ch != ',')) {
	a->p1 = schar;
	a = schar;
	schar = sno_getc();
    }
    b->p2 = a;
    next = 1;
    a = look(b);
    delete(b);
    b = alloc();
    b->typ = 14;
    b->p1 = a;
    return(b);
}

struct node *nscomp(void)
{
    struct node *c;

    c = compon();
    while(c->typ == 7) {
	sno_free(c);
	c = compon();
    }

    return(c);
}

struct node *push(struct node *stack)
{
    struct node *a;

    a = alloc();
    a->p2 = stack;
    return(a);
}

struct node *pop(struct node *stack)
{
    struct node *a;
    struct node *s;

    s = stack;
    if(s == 0) {
	writes("pop");
    }
    a = s->p2;
    sno_free(s);
    return(a);
}

struct node *expr(struct node *start, int eof, struct node *e)
{
    struct node *stack;
    struct node *list;
    struct node *comp;
    int operand;
    int op;
    struct node *space;
    int op1;
    struct node *a;
    struct node *b;
    struct node *c;
    int d;

    list = alloc();
    e->p2 = list;
    stack = push(0);
    stack->typ = eof;
    operand = 0;
    space = start;
 l1:
    if(space) {
	comp = space;
	space = 0;
    }
    else {
	comp = compon();
    }

 l3:
    op = comp->typ;
    switch(op) {

    case 7:
	space = list;
	sno_free(comp);
	comp = compon();
	goto l3;

    case 10:
	if(space == 0) {
	    comp->typ = 1;
	    goto l3;
	}

    case 11:
	if(space == 0) {
	    comp->typ = 2;
	    goto l3;
	}

    case 8:
    case 9:
	if(operand == 0) {
	    writes("no operand preceding operator");
	}
	operand = 0;
	goto l5;

    case 14:
    case 15:
	if(operand == 0) {
	    operand = 1;
	    goto l5;
	}
	if(space == 0) {
	    goto l7;
	}
	goto l4;

    case 12:
	if(operand == 0) {
	    goto l5;
	}
	if(space) {
	    goto l4;
	}
    l7:
	writes("illegal juxtaposition of operands");

    case 16:
	if(operand == 0) {
	    goto l5;
	}
	if(space) {
	    goto l4;
	}
	b = compon();
	op = comp->typ = 13;
	if(b->typ == 5) {
	    comp->p1 = 0;
	    goto l10;
	}
	comp->p1 = a = alloc();
	b = expr(b, 6, a);
	d = b->typ;
	while(d == 4) {
	    a->p1 = b;
	    a = b;
	    b = expr(0, 6, a);
	    d = b->typ;
	}
	if(d != 5) {
	    writes("error in function");
	}
	a->p1 = 0;
    l10:
	sno_free(b);
	goto l6;

    l4:
	space = comp;
	op = 7;
	operand = 0;
	goto l6;
    }
    if(operand==0) {
	writes("no operand at end of expression");
    }
 l5:
    space = 0;
 l6:
    op1 = stack->typ;
    if(op > op1) {
	stack = push(stack);
	if(op == 16) {
	    op = 6;
	}
	stack->typ = op;
	stack->p1 = comp;
	goto l1;
    }
    c = stack->p1;
    stack = pop(stack);
    if(stack == 0) {
	list->typ = 0;
	return(comp);
    }
    if(op1 == 6) {
	if(op != 5) {
	    writes("too many ('s");
	}
	goto l1;
    }
    if(op1 == 7) {
	c = alloc();
    }
    list->typ = op1;
    list->p2 = c->p1;
    list->p1 = c;
    list = c;
    goto l6;
}

struct node *match(struct node *start, struct node *m)
{
    struct node *list;
    struct node *comp;
    struct node *term;
    struct node *a;
    int b;
    int bal;

    term = 0;
    bal = 0;
    list = alloc();
    m->p2 = list;
    comp = start;
    if(!comp) {
	comp = compon();
    }
    goto l2;

 l3:
    list->p1 = a = alloc();
    list = a;
 l2:
    switch(comp->typ) {
    case 7:
	sno_free(comp);
	comp = compon();
	goto l2;

    case 12:
    case 14:
    case 15:
    case 16:
	term = 0;
	comp = expr(comp, 6, list);
	list->typ = 1;
	goto l3;

    case 1:
	sno_free(comp);
	comp = compon();
	bal = 0;
	if(comp->typ == 16) {
	    bal = 1;
	    sno_free(comp);
	    comp = compon();
	}
	a = alloc();
	b = comp->typ;
	if((b == 2) || (b == 5) || (b == 10) || (b == 1)) {
	    a->p1 = 0; 
	}
	else {
	    comp = expr(comp, 11, a);
	    a->p1 = a->p2;
	}
	if(comp->typ != 2) {
	    a->p2 = 0;
	}
	else {
	    sno_free(comp);
	    comp = expr(0, 6, a);
	}
	if(bal) {
	    if(comp->typ != 5) {
		goto merr;
	    }
	    sno_free(comp);
	    comp = compon();
	}
	b = comp->typ;
	if((b != 1) && (b != 10)) {
	    goto merr;
	}
	list->p2 = a;
	list->typ = 2;
	a->typ = bal;
	sno_free(comp);
	comp = compon();
	if(bal) {
	    term = 0;
	} 
	else {
	    term = list;
	}
	goto l3;
    }
    if(term) {
	term->typ = 3;
    }
    list->typ = 0;
    return(comp);

 merr:
    writes("unrecognized component in match");

    return NULL;
}

struct node *compile(void) {
    struct node *b;
    struct node *comp;
    struct node *r;
    struct node *l;
    struct node *xs;
    struct node *xf;
    struct node *g;
    int a;
    struct node *m;
    int t;
    struct node *as;

    t = 0;
    xf = 0;
    xs = 0;
    as = 0;
    l = 0;
    m = 0;
    comp = compon();
    a = comp->typ;
    if(a == 14) {
	l = comp->p1;
	sno_free(comp);
	comp = compon();
	a = comp->typ;
    }
    if(a != 7) {
	writes("no space beginning statement");
    }
    sno_free(comp);
    if(l == lookdef) {
	goto def;
    }
    r = alloc();
    comp = expr(0, 11, r);
    a = comp->typ;
    if(a == 0) {
	goto asmble;
    }
    if(a == 2) {
	goto xfer;
    }
    if(a == 3) {
	goto assig;
    }
    m = alloc();
    comp = match(comp, m);
    a = comp->typ;
    if(a == 0) {
	goto asmble;
    }
    if(a == 2) {
	goto xfer;
    }
    if(a == 3) {
	goto assig;
    }
    writes("unrecognized component in match");

 assig:
    sno_free(comp);
    as = alloc();
    comp = expr(0, 6, as);
    a = comp->typ;
    if(a == 0) {
	goto asmble;
    }
    if(a == 2) {
	goto xfer;
    }
    writes("unrecognized component in assignment");

 xfer:
    sno_free(comp);
    comp = compon();
    a = comp->typ;
    if(a == 16) {
	goto xboth;
    }
    if(a == 0) {
	if((xs != 0) || (xf != 0)) {
	    goto asmble;
	}
	goto xerr;
    }
    if(a != 14) {
	goto xerr;
    }
    b = comp->p1;
    sno_free(comp);
    if(b == looks) {
	goto xsuc;
    }
    if(b == lookf) {
	goto xfail;
    }

 xerr:
    writes("unrecognized component in goto");

 xboth:
    sno_free(comp);
    xs = alloc();
    xf = alloc();
    comp = expr(0, 6, xs);
    if(comp->typ != 5) {
	goto xerr;
    }
    xf->p2 = xs->p2;
    comp = compon();
    if(comp->typ != 0) {
	goto xerr;
    }
    goto asmble;

 xsuc:
    if(xs) {
	goto xerr;
    }
    comp = compon();
    if(comp->typ != 16) {
	goto xerr;
    }
    comp = expr(0, 6, xs=alloc());
    if(comp->typ != 5) {
	goto xerr;
    }
    goto xfer;

 xfail:
    if(xf) {
	goto xerr;
    }
    comp = compon();
    if(comp->typ != 16) {
	goto xerr;
    }
    xf = alloc();
    comp = expr(0, 6, xf);
    if(comp->typ != 5) {
	goto xerr;
    }
    goto xfer;

 asmble:
    if(l) {
	if(l->typ) {
	    writes("name doubly defined");
	}
	l->p2 = comp;
	l->typ = 2; /* type label;*/
    }
    comp->p2 = r;
    if(m) {
	t++;
	r->p1 = m;
	r = m;
    }
    if(as) {
	t += 2;
	r->p1 = as;
	r = as;
    }
    g = alloc();
    g->p1 = 0;
    if(xs) {
	g->p1 = xs->p2;
	sno_free(xs);
    }
    g->p2 = 0;
    if(xf) {
	g->p2 = xf->p2;
	sno_free(xf);
    }
    r->p1 = g;
    comp->typ = t;
    comp->ch = lc;
    return(comp);

 def:
    r = nscomp();
    if(r->typ != 14) {
	goto derr;
    }
    l = r->p1;
    if(l->typ) {
	writes("name doubly defined");
    }
    l->typ = 5; /*type function;*/
    m = r;
    l->p2 = m;
    r = nscomp();
    l = r;
    m->p1 = l;
    if(r->typ == 0) {
	goto d4;
    }
    if(r->typ != 16) {
	goto derr;
    }

 d2:
    r = nscomp();
    if(r->typ != 14) {
	goto derr;
    }
    m->p2 = r;
    r->typ = 0;
    m = r;
    r = nscomp();
    if(r->typ == 4) {
	sno_free(r);
	goto d2;
    }
    if(r->typ != 5) {
	goto derr;
    }
    sno_free(r);
    r = compon();
    if(r->typ != 0) {
	goto derr;
    }
    sno_free(r);

 d4:
    r = compile();
    m->p2 = 0;
    l->p1 = r;
    l->p2 = 0;
    return(r);

 derr:
    writes("illegal component in define");

    return NULL;
}
