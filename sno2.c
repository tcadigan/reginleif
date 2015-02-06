#include "sno2.h"

#include "sno1.h"

#include <stdlib.h>

/*
 * Checks the class of the node and
 * sets the appropriate type.
 * Notes:
 *   "Literal" strings get sent back as handle to the string
 *
 *   "Normal" strings get put in the namelist and sent back as
 *   handle to the string
 *
 *   Unsure of impact of next
 */
struct node *compon(void)
{
    struct node *result;
    struct node *sentinel;
    int quote_ch;
    static int next;

    if(next == 0) {
	schar = sno_getc();
    }
    else {
	next = 0;
    }

    if(schar == NULL) {
	result = alloc();
	result->typ = 0;

	return result;
    }

    switch(class(schar->ch)) {
    case 1:
	/* ')' */
	schar->typ = 5;

	return schar;
    case 2:
	/* '(' */
	schar->typ = 16;

	return schar;
    case 3:
	/* '\t' or ' ' */
	result = schar;
	
	/* Keep skipping whitespace */
	while(1) {
	    schar = sno_getc();
	    
	    if(schar == NULL) {
		result->typ = 0;

		return result;
	    }
	    
	    if(class(schar->ch) != 3) {
		break;
	    }

	    sno_free(schar);
	}

	next = 1;
	result->typ = 7;

	return result;
    case 4:
	/* '+' */
	schar->typ = 8;
	
	return schar;
    case 5:
	/* '-' */
	schar->typ = 9;
	
	return schar;
    case 6:
	/* '*' */
	result = schar;
	schar = sno_getc();
	
	/* Checking for following whitespace */
	if(class(schar->ch) == 3) {
	    result->typ = 10;
	}
	else {
	    result->typ = 1;
	}

	next = 1;

	return result;
    case 7:
	/* '/' */
	result = schar;
	schar = sno_getc();

	/* Checking for following '(' */
	if(class(schar->ch) == 2) {
	    result->typ = 11;
	}
	else {
	    result->typ = 2;
	}

	next = 1;

	return result;
    case 8:
	/* '$' */
	schar->typ = 12;

	return schar;
    case 9:
	/* '"' or '\'' */ 
	quote_ch = schar->ch;
	result = sno_getc();
	
	/* There must be more to the literal string */
	if(result == NULL) {
	    writes("Illegal literal result");
	}

	sentinel = schar;

	/* 
	 * Checking for the closing quote,
	 * this is the empty result case
	 */
	if(result->ch == quote_ch) {
	    sno_free(schar);
	    result->typ = 15;
	    result->p1 = NULL;

	    return result;
	}

	sentinel->p1 = result;

	/*
	 * Get the rest of the literal string,
	 * must end in a closing quote 
	 */
	while(1) {
	    schar = sno_getc();

	    if(schar == NULL) {
		writes("Illegal literal result");
	    }

	    if(schar->ch == quote_ch) {
		break;
	    }

	    result->p1 = schar;
	    schar->p2 = result;
	    result = schar;
	}

	sentinel->p2 = result;
	schar->typ = 15;
	schar->p1 = sentinel;
	
	return schar;
    case 10:
	/* '=' */
	schar->typ = 3;
	
	return schar;
    case 11:
	/* ',' */
	schar->typ = 4;

	return schar;
    }

    /* This is for handling everything else */
    sentinel = alloc();
    result = schar;
    sentinel->p1 = result;
    schar = sno_getc();

    while((schar != NULL) && !class(schar->ch)) {
	result->p1 = schar;
	schar->p2 = result;
	result = schar;
	schar = sno_getc();
    }

    sentinel->p2 = result;
    next = 1;

    /* 
     * Find/Put it in the namelist and get the
     * namelist node
     */
    result = look(sentinel);
    delete(sentinel);

    /* Create a handle to the namelist node */
    sentinel = alloc();
    sentinel->typ = 14;
    sentinel->p1 = result;

    return sentinel;
}

/* Skip everything that is pure white space */
struct node *nscomp(void)
{
    struct node *result;
    
    result = compon();

    /* '\t' or ' ' */
    while(result->typ == 7) {
	sno_free(result);
	result = compon();
    }

    return result;
}

/* Push empty node onto the provided stack */
struct node *push(struct node *stack)
{
    struct node *empty_node;

    empty_node = alloc();
    empty_node->p2 = stack;
    
    /* Keep the strongly linked crusade alive */
    if(stack != NULL) {
	stack->p1 = empty_node;
    }

    return empty_node;
}

/* 
 * Remove from the top of the stack,
 * return the top top element
 */
struct node *pop(struct node *stack)
{
    struct node *result;
    struct node *top;

    top = stack;

    if(top == NULL) {
	writes("Pop");
    }
    
    result = top->p2;
    sno_free(top);

    /* Clean the forward link */
    top->p1 = NULL;

    return result;
}


/* DEBUG: */
struct node *Unknown1(int *op,
		      struct node **stack,
		      struct node **comp,
		      struct node **space,
		      struct node **list);

/* 
 * strings go on in normal order?
 * i.e. cat is 'c', 'a', 't'
 *
 * higher typ get pushed onto stack, meaning
 * lower is higher precendence
 */
struct node *Unknown1(int *op,
		      struct node **stack,
		      struct node **comp,
		      struct node **space,
		      struct node **list)
{
    int op1;
    struct node *c;
    
    while(1) {
	op1 = (*stack)->typ;
	
	/* if lower precendence push to stack */
	if(*op > op1) {
	    *stack = push(*stack);
	    
	    /* '(' */
	    if(*op == 16) {
		*op = 6;
	    }
	    
	    (*stack)->typ = *op;
	    (*stack)->p1 = *comp;
	    
	    if(*space) {
		*comp = *space;
		*space = NULL;
	    }
	    else {
		*comp = compon();
	    }
	    
	    return NULL;
	}
	
	/* Look ahead one char */
	c = (*stack)->p1;
	*stack = pop(*stack);
	
	if(*stack == NULL) {
	    (*list)->typ = 0;
	    
	    return *comp;
	}
	
	if(op1 == 6) {
	    if(*op != 5) {
		writes("Too many ('s");
	    }
	    
	    if(*space) {
		*comp = *space;
		*space = NULL;
	    }
	    else {
		*comp = compon();
	    }
	    
	    return NULL;
	}
	
	/* It is not whitespace */
	if(op1 == 7) {
	    c = alloc();
	}
	
	/* Add the lookahead to the current list */
	(*list)->typ = op1;
	(*list)->p2 = c->p1;
	(*list)->p1 = c;
	*list = c;
    }
}

struct node *expr(struct node *start, int eof, struct node *e)
{
    struct node *stack;
    struct node *list;
    struct node *comp;
    int operand;
    int op;
    struct node *space;
    struct node *a;
    struct node *b;
    struct node *result;
    int d;

    list = alloc();
    e->p2 = list;
    stack = push(NULL);
    stack->typ = eof;
    operand = 0;
    space = start;

    if(space) {
	comp = space;
	space = NULL;
    }
    else {
	comp = compon();
    }

    while(1) {
	op = comp->typ;
	
	switch(op) {
	case 7:
	    space = list;
	    sno_free(comp);
	    comp = compon();
	    
	    continue;
	case 10:
	    if(space == NULL) {
		comp->typ = 1;

		continue;
	    }
	case 11:
	    if(space == NULL) {
		comp->typ = 2;
	
		continue;
	    }
	case 8:
	case 9:
	    if(operand == 0) {
		writes("No operand preceding operator");
	    }

	    operand = 0;
	    space = 0;

	    result = Unknown1(&op, &stack, &comp, &space, &list);
	    
	    if(result == NULL) {
		continue;
	    }
	    else {
		return result;
	    }
	case 14:
	case 15:
	    if(operand == 0) {
		operand = 1;
		space = 0;

		result = Unknown1(&op, &stack, &comp, &space, &list);
		
		if(result == NULL) {
		    continue;
		}
		else {
		    return result;
		}
	    }

	    if(space == 0) {
		writes("Illegal juxtaposition of operands");
	    }

	    space = comp;
	    op = 7;
	    operand = 0;
	    
	    result = Unknown1(&op, &stack, &comp, &space, &list);
	    
	    if(result == NULL) {
		continue;
	    }
	    else {
		return result;
	    }
	case 12:
	    if(operand == 0) {
		space = 0;

		result = Unknown1(&op, &stack, &comp, &space, &list);
		
		if(result == NULL) {
		    continue;
		}
		else {
		    return result;
		}
	    }

	    if(space) {
		space = comp;
		op = 7;
		operand = 0;

		result = Unknown1(&op, &stack, &comp, &space, &list);
		
		if(result == NULL) {
		    continue;
		}
		else {
		    return result;
		}
	    }

	    writes("Illegal juxtaposition of operands");
	case 16:
	    if(operand == 0) {
		space = 0;

		result = Unknown1(&op, &stack, &comp, &space, &list);
		
		if(result == NULL) {
		    continue;
		}
		else {
		    return result;
		}
	    }

	    if(space) {
		space = comp;
		op = 7;
		operand = 0;

		result = Unknown1(&op, &stack, &comp, &space, &list);
		
		if(result == NULL) {
		    continue;
		}
		else {
		    return result;
		}
	    }

	    b = compon();
	    comp->typ = 13;
	    op = comp->typ;

	    if(b->typ == 5) {
		comp->p1 = 0;

		sno_free(b);

		result = Unknown1(&op, &stack, &comp, &space, &list);
		
		if(result == NULL) {
		    continue;
		}
		else {
		    return result;
		}
	    }

	    a = alloc();
	    comp->p1 = a;
	    b = expr(b, 6, a);
	    d = b->typ;

	    while(d == 4) {
		a->p1 = b;
		a = b;
		b = expr(0, 6, a);
		d = b->typ;
	    }

	    if(d != 5) {
		writes("Error in function");
	    }

	    a->p1 = 0;
	    sno_free(b);

	    result = Unknown1(&op, &stack, &comp, &space, &list);
	    
	    if(result == NULL) {
		continue;
	    }
	    else {
		return result;
	    }
	}

	if(operand == 0) {
	    writes("No operand at end of expression");
	}

	space = 0;

	result = Unknown1(&op, &stack, &comp, &space, &list);
	
	if(result == NULL) {
	    continue;
	}
	else {
	    return result;
	}
    }

    return NULL;
}

struct node *match(struct node *start, struct node *m)
{
    struct node *list;
    struct node *comp;
    struct node *term;
    struct node *a;
    int b;
    int bal;
    int i;

    bal = 0;
    i = 0;
    term = NULL;
    list = alloc();
    m->p2 = list;
    comp = start;
    
    if(!comp) {
	comp = compon();
    }

    while(1) {
	if(i) {
	    a = alloc();
	    list->p1 = NULL;
	    list = a;
	}

	switch(comp->typ) {
	case 7:
	    sno_free(comp);
	    comp = compon();

	    i = 0;
	
	    continue;
	case 12:
	case 14:
	case 15:
	case 16:
	    term = 0;
	    comp = expr(comp, 6, list);
	    list->typ = 1;

	    continue;
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
		free(comp);
		comp = expr(0, 6, a);
	    }

	    if(bal) {
		if(comp->typ != 5) {
		    writes("Unrecognized component in match");

		    return 0;
		}

		free(comp);
		comp = compon();
	    }

	    b = comp->typ;
	
	    if((b != 1) && (b != 10)) {
		writes("Unrecognized component in match");

		return 0;
	    }

	    list->p2 = a;
	    list->typ = 2;
	    a->typ = bal;
	    free(comp);
	    comp = compon();

	    if(bal) {
		term = 0;
	    }
	    else {
		term = list;
	    }

	    continue;
	}

	break;
    }

    if(term) {
	term->typ = 3;
    }

    list->typ = 0;

    return comp;
}

struct node *compile(void)
{
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
	free(comp);
	comp = compon();
	a = comp->typ;
    }

    if(a != 7) {
	writes("No space beginning statement");
    }

    free(comp);
    
    if(l == lookfret) {
	r = nscomp();
    
	if(r->typ != 14) {
	    writes("Illegal component in define");
	
	    return 0;
	}

	l = r->p1;
    
	if(l->typ) {
	    writes("Name doubly defined");
	}

	/* type function */
	l->typ = 5;
	m = r;
	l->p2 = m;
	r = nscomp();
	l = r;
	m->p1 = l;

	if(r->typ == 0) {
	    r = compile();
	    m->p2 = 0;
	    l->p1 = r;
	    l->p2 = 0;
	
	    return r;
	}

	if(r->typ != 16) {
	    writes("Illegal component in define");
	
	    return 0;
	}

	while(1) {
	    r = nscomp();
    
	    if(r->typ != 14) {
		writes("Illegal component in define");
	
		return 0;
	    }

	    m->p2 = r;
	    r->typ = 0;
	    m = r;
	    r = nscomp();

	    if(r->typ == 4) {
		sno_free(r);

		continue;
	    }

	    break;
	}
    
	if(r->typ != 5) {
	    writes("Illegal component in define");
	
	    return 0;
	}

	sno_free(r);
	r = compon();
    
	if(r->typ != 0) {
	    writes("Illegal component in define");
	
	    return 0;
	}

	sno_free(r);

	r = compile();
	m->p2 = 0;
	l->p1 = r;
	l->p2 = 0;

	return r;
    }
    
    r = alloc();
    comp = expr(0, 11, r);
    a = comp->typ;

    if(a == 0) {
	if(l) {
	    if(l->typ) {
		writes("Name doubly defined");
	    }

	    l->p2 = comp;

	    /* type label */
	    l->typ = 2;
	}

	comp->p2 = r;

	if(m) {
	    ++t;
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
	    free(xs);
	}

	g->p2 = 0;

	if(xf) {
	    g->p2 = xf->p2;
	    free(xf);
	}

	r->p1 = g;
	comp->typ = t;
	comp->ch = lc;

	return comp;
    }

    if(a == 2) {
	while(1) {
	    free(comp);
	    comp = compon();
	    a = comp->typ;
    
	    if(a == 16) {
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;
	    }

	    if(a == 0) {
		if((xs != 0) || (xf != 0)) {
		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;
		}
	
		writes("Unrecognized component in goto");
    
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;
	    }

	    if(a != 14) {
		writes("Unrecognized component in goto");
    
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;       
	    }

	    b = comp->p1;
	    free(comp);

	    if(b == looks) {
		if(xs) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;

	  
		}

		comp = compon();

		if(comp->typ != 16) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;

	    
		}

		xs = alloc();
		comp = expr(0, 6, xs);

		if(comp->typ != 5) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;

	    
		}

		continue;
	    }

	    if(b == lookf) {
		if(xf) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;

	    
		}

		comp = compon();
		if(comp->typ != 16) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;

	   
		}

		xf = alloc();
		comp = expr(0, 6, xf);

		if(comp->typ != 5) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;
		}

		continue;
	    }

	    break;
	}

	writes("Unrecognized component in goto");
    
	while(1) {
	    free(comp);
	    xs = alloc();
	    xf = alloc();
	    comp = expr(0, 6, xs);
	
	    if(comp->typ != 5) {
		writes("Unrecognized component in goto");
	    
		continue;
	    }
	
	    xf->p2 = xs->p2;
	    comp = compon();
	
	    if(comp->typ != 0) {
		writes("Unrecognized component in goto");
	    
		continue;
	    }
	
	    break;
	}

	if(l) {
	    if(l->typ) {
		writes("Name doubly defined");
	    }

	    l->p2 = comp;

	    /* type label */
	    l->typ = 2;
	}

	comp->p2 = r;

	if(m) {
	    ++t;
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
	    free(xs);
	}

	g->p2 = 0;

	if(xf) {
	    g->p2 = xf->p2;
	    free(xf);
	}

	r->p1 = g;
	comp->typ = t;
	comp->ch = lc;

	return comp;

    }

    if(a == 3) {
	free(comp);
	as = alloc();
	comp = expr(0, 6, as);
	a = comp->typ;
    
	if(a == 0) {
	    if(l) {
		if(l->typ) {
		    writes("Name doubly defined");
		}

		l->p2 = comp;

		/* type label */
		l->typ = 2;
	    }

	    comp->p2 = r;

	    if(m) {
		++t;
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
		free(xs);
	    }

	    g->p2 = 0;

	    if(xf) {
		g->p2 = xf->p2;
		free(xf);
	    }

	    r->p1 = g;
	    comp->typ = t;
	    comp->ch = lc;

	    return comp;
	}

	if(a != 2) {
	    writes("Unrecognized component in assignment");
	}

	while(1) {
	    free(comp);
	    comp = compon();
	    a = comp->typ;
    
	    if(a == 16) {
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;
	    }

	    if(a == 0) {
		if((xs != 0) || (xf != 0)) {
		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;
		}
	
		writes("Unrecognized component in goto");
    
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;
	    }

	    if(a != 14) {
		writes("Unrecognized component in goto");
    
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;       
	    }

	    b = comp->p1;
	    free(comp);

	    if(b == looks) {
		if(xs) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;

	  
		}

		comp = compon();

		if(comp->typ != 16) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;

	    
		}

		xs = alloc();
		comp = expr(0, 6, xs);

		if(comp->typ != 5) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;

	    
		}

		continue;
	    }

	    if(b == lookf) {
		if(xf) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;

	    
		}

		comp = compon();

		if(comp->typ != 16) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;
		}

		xf = alloc();
		comp = expr(0, 6, xf);

		if(comp->typ != 5) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;
		}

		continue;
	    }

	    break;
	}

	writes("Unrecognized component in goto");
    
	while(1) {
	    free(comp);
	    xs = alloc();
	    xf = alloc();
	    comp = expr(0, 6, xs);
	
	    if(comp->typ != 5) {
		writes("Unrecognized component in goto");
	    
		continue;
	    }
	
	    xf->p2 = xs->p2;
	    comp = compon();
	
	    if(comp->typ != 0) {
		writes("Unrecognized component in goto");
	    
		continue;
	    }
	
	    break;
	}

	if(l) {
	    if(l->typ) {
		writes("Name doubly defined");
	    }

	    l->p2 = comp;

	    /* type label */
	    l->typ = 2;
	}

	comp->p2 = r;

	if(m) {
	    ++t;
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
	    free(xs);
	}

	g->p2 = 0;

	if(xf) {
	    g->p2 = xf->p2;
	    free(xf);
	}

	r->p1 = g;
	comp->typ = t;
	comp->ch = lc;

	return comp;

    }

    m = alloc();
    comp = match(comp, m);
    a = comp->typ;

    if(a == 0) {
	if(l) {
	    if(l->typ) {
		writes("Name doubly defined");
	    }

	    l->p2 = comp;

	    /* type label */
	    l->typ = 2;
	}

	comp->p2 = r;

	if(m) {
	    ++t;
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
	    free(xs);
	}

	g->p2 = 0;

	if(xf) {
	    g->p2 = xf->p2;
	    free(xf);
	}

	r->p1 = g;
	comp->typ = t;
	comp->ch = lc;

	return comp;
    }

    if(a == 2) {
	while(1) {
	    free(comp);
	    comp = compon();
	    a = comp->typ;
    
	    if(a == 16) {
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;
	    }

	    if(a == 0) {
		if((xs != 0) || (xf != 0)) {
		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;
		}
	
		writes("Unrecognized component in goto");
    
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;
	    }

	    if(a != 14) {
		writes("Unrecognized component in goto");
    
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;       
	    }

	    b = comp->p1;
	    free(comp);

	    if(b == looks) {
		if(xs) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;
		}

		comp = compon();

		if(comp->typ != 16) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;
		}

		xs = alloc();
		comp = expr(0, 6, xs);

		if(comp->typ != 5) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;
		}

		continue;
	    }

	    if(b == lookf) {
		if(xf) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;
		}

		comp = compon();
		if(comp->typ != 16) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;
		}

		xf = alloc();
		comp = expr(0, 6, xf);

		if(comp->typ != 5) {
		    writes("Unrecognized component in goto");
    
		    while(1) {
			free(comp);
			xs = alloc();
			xf = alloc();
			comp = expr(0, 6, xs);
	
			if(comp->typ != 5) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			xf->p2 = xs->p2;
			comp = compon();
	
			if(comp->typ != 0) {
			    writes("Unrecognized component in goto");
	    
			    continue;
			}
	
			break;
		    }

		    if(l) {
			if(l->typ) {
			    writes("Name doubly defined");
			}

			l->p2 = comp;

			/* type label */
			l->typ = 2;
		    }

		    comp->p2 = r;

		    if(m) {
			++t;
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
			free(xs);
		    }

		    g->p2 = 0;

		    if(xf) {
			g->p2 = xf->p2;
			free(xf);
		    }

		    r->p1 = g;
		    comp->typ = t;
		    comp->ch = lc;

		    return comp;
		}

		continue;
	    }

	    break;
	}

	writes("Unrecognized component in goto");
    
	while(1) {
	    free(comp);
	    xs = alloc();
	    xf = alloc();
	    comp = expr(0, 6, xs);
	
	    if(comp->typ != 5) {
		writes("Unrecognized component in goto");
	    
		continue;
	    }
	
	    xf->p2 = xs->p2;
	    comp = compon();
	
	    if(comp->typ != 0) {
		writes("Unrecognized component in goto");
	    
		continue;
	    }
	
	    break;
	}

	if(l) {
	    if(l->typ) {
		writes("Name doubly defined");
	    }

	    l->p2 = comp;

	    /* type label */
	    l->typ = 2;
	}

	comp->p2 = r;

	if(m) {
	    ++t;
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
	    free(xs);
	}

	g->p2 = 0;

	if(xf) {
	    g->p2 = xf->p2;
	    free(xf);
	}

	r->p1 = g;
	comp->typ = t;
	comp->ch = lc;

	return comp;

 
    }

    if(a != 3) {
	writes("Unrecognized component in match");
    }

    free(comp);
    as = alloc();
    comp = expr(0, 6, as);
    a = comp->typ;
    
    if(a == 0) {
	if(l) {
	    if(l->typ) {
		writes("Name doubly defined");
	    }

	    l->p2 = comp;

	    /* type label */
	    l->typ = 2;
	}

	comp->p2 = r;

	if(m) {
	    ++t;
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
	    free(xs);
	}

	g->p2 = 0;

	if(xf) {
	    g->p2 = xf->p2;
	    free(xf);
	}

	r->p1 = g;
	comp->typ = t;
	comp->ch = lc;

	return comp;
    }

    if(a != 2) {
	writes("Unrecognized component in assignment");
    }

    while(1) {
	free(comp);
	comp = compon();
	a = comp->typ;
    
	if(a == 16) {
	    while(1) {
		free(comp);
		xs = alloc();
		xf = alloc();
		comp = expr(0, 6, xs);
	
		if(comp->typ != 5) {
		    writes("Unrecognized component in goto");
	    
		    continue;
		}
	
		xf->p2 = xs->p2;
		comp = compon();
	
		if(comp->typ != 0) {
		    writes("Unrecognized component in goto");
	    
		    continue;
		}
	
		break;
	    }

	    if(l) {
		if(l->typ) {
		    writes("Name doubly defined");
		}

		l->p2 = comp;

		/* type label */
		l->typ = 2;
	    }

	    comp->p2 = r;

	    if(m) {
		++t;
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
		free(xs);
	    }

	    g->p2 = 0;

	    if(xf) {
		g->p2 = xf->p2;
		free(xf);
	    }

	    r->p1 = g;
	    comp->typ = t;
	    comp->ch = lc;

	    return comp;
	}

	if(a == 0) {
	    if((xs != 0) || (xf != 0)) {
		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;
	    }
	
	    writes("Unrecognized component in goto");
    
	    while(1) {
		free(comp);
		xs = alloc();
		xf = alloc();
		comp = expr(0, 6, xs);
	
		if(comp->typ != 5) {
		    writes("Unrecognized component in goto");
	    
		    continue;
		}
	
		xf->p2 = xs->p2;
		comp = compon();
	
		if(comp->typ != 0) {
		    writes("Unrecognized component in goto");
	    
		    continue;
		}
	
		break;
	    }

	    if(l) {
		if(l->typ) {
		    writes("Name doubly defined");
		}

		l->p2 = comp;

		/* type label */
		l->typ = 2;
	    }

	    comp->p2 = r;

	    if(m) {
		++t;
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
		free(xs);
	    }

	    g->p2 = 0;

	    if(xf) {
		g->p2 = xf->p2;
		free(xf);
	    }

	    r->p1 = g;
	    comp->typ = t;
	    comp->ch = lc;

	    return comp;
	}

	if(a != 14) {
	    writes("Unrecognized component in goto");
    
	    while(1) {
		free(comp);
		xs = alloc();
		xf = alloc();
		comp = expr(0, 6, xs);
	
		if(comp->typ != 5) {
		    writes("Unrecognized component in goto");
	    
		    continue;
		}
	
		xf->p2 = xs->p2;
		comp = compon();
	
		if(comp->typ != 0) {
		    writes("Unrecognized component in goto");
	    
		    continue;
		}
	
		break;
	    }

	    if(l) {
		if(l->typ) {
		    writes("Name doubly defined");
		}

		l->p2 = comp;

		/* type label */
		l->typ = 2;
	    }

	    comp->p2 = r;

	    if(m) {
		++t;
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
		free(xs);
	    }

	    g->p2 = 0;

	    if(xf) {
		g->p2 = xf->p2;
		free(xf);
	    }

	    r->p1 = g;
	    comp->typ = t;
	    comp->ch = lc;

	    return comp;       
	}

	b = comp->p1;
	free(comp);

	if(b == looks) {
	    if(xs) {
		writes("Unrecognized component in goto");
    
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;
	    }

	    comp = compon();

	    if(comp->typ != 16) {
		writes("Unrecognized component in goto");
    
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;
	    }

	    xs = alloc();
	    comp = expr(0, 6, xs);

	    if(comp->typ != 5) {
		writes("Unrecognized component in goto");
    
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;
	    }

	    continue;
	}

	if(b == lookf) {
	    if(xf) {
		writes("Unrecognized component in goto");
    
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;
	    }

	    comp = compon();

	    if(comp->typ != 16) {
		writes("Unrecognized component in goto");
    
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;
	    }

	    xf = alloc();
	    comp = expr(0, 6, xf);

	    if(comp->typ != 5) {
		writes("Unrecognized component in goto");
    
		while(1) {
		    free(comp);
		    xs = alloc();
		    xf = alloc();
		    comp = expr(0, 6, xs);
	
		    if(comp->typ != 5) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    xf->p2 = xs->p2;
		    comp = compon();
	
		    if(comp->typ != 0) {
			writes("Unrecognized component in goto");
	    
			continue;
		    }
	
		    break;
		}

		if(l) {
		    if(l->typ) {
			writes("Name doubly defined");
		    }

		    l->p2 = comp;

		    /* type label */
		    l->typ = 2;
		}

		comp->p2 = r;

		if(m) {
		    ++t;
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
		    free(xs);
		}

		g->p2 = 0;

		if(xf) {
		    g->p2 = xf->p2;
		    free(xf);
		}

		r->p1 = g;
		comp->typ = t;
		comp->ch = lc;

		return comp;
	    }

	    continue;
	}

	break;
    }

    writes("Unrecognized component in goto");
    
    while(1) {
	free(comp);
	xs = alloc();
	xf = alloc();
	comp = expr(0, 6, xs);
	
	if(comp->typ != 5) {
	    writes("Unrecognized component in goto");
	    
	    continue;
	}
	
	xf->p2 = xs->p2;
	comp = compon();
	
	if(comp->typ != 0) {
	    writes("Unrecognized component in goto");
	    
	    continue;
	}
	
	break;
    }

    if(l) {
	if(l->typ) {
	    writes("Name doubly defined");
	}

	l->p2 = comp;

	/* type label */
	l->typ = 2;
    }

    comp->p2 = r;

    if(m) {
	++t;
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
	free(xs);
    }

    g->p2 = 0;

    if(xf) {
	g->p2 = xf->p2;
	free(xf);
    }

    r->p1 = g;
    comp->typ = t;
    comp->ch = lc;

    return comp;
}
