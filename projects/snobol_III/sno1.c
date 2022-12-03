/* Snobol III */

#include "sno1.h"
#include "sno2.h"
#include "sno4.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int freesize = 0;
struct node *lookf = NULL;
struct node *looks = NULL;
struct node *lookend = NULL;
struct node *lookstart = NULL;
struct node *lookdef = NULL;
struct node *lookret = NULL;
struct node *lookfret = NULL;
int cfail = 0;
int rfail = 0;
struct node *freelist = NULL;
struct node *namelist = NULL;
int lc = 0;
struct node *schar = NULL;

int fin = 0;
int fout = 0;

/* Allocates a string to be printed */
void mes(char *s)
{
    sysput(sno_strstr(s));
}

/*
 * Initialize a string into the namelist
 * with a given type
 */
struct node *init(char *s, int t)
{
    struct node *new_node;
    struct node *namelist_node;

    /* Create the new node */
    new_node = sno_strstr(s);

    /* Get the namelist node */
    namelist_node = look(new_node);
    delete(new_node);
    namelist_node->typ = t;

    return namelist_node;
}

int main(int argc, char *argv[])
{
    struct node *a;
    struct node *b;
    struct node *c;

    if(argc > 1) {
	fin = open(argv[1], O_RDONLY);

	if(fin < 0) {
	    mes("Cannot open input");

	    return 1;
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
    a->p1 = NULL;

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

/*
 * Read a line from the input. Caution last line
 * should be an empty line it will be deleted
 */
struct node *syspit(void)
{
    struct node *sentinel;
    struct node *current;
    struct node *temp;
    int character;

    character = getchar();

    if(character == '\n') {
	return NULL;
    }

    current = alloc();
    sentinel = current;

    /* Read the line character by character */
    while(character != '\n') {
	temp = alloc();
	current->p1 = temp;
	temp->p2 = current;
	current = temp;

	/* Special case end of input, drop last line */
	while(1) {
	    current->ch = character;

	    if(character == EOF) {
		if(fin) {
		    close(fin);
		    fin = 0;
		    character = getchar();
		    continue;
		}

		rfail = 1;
		sentinel->p2 = current;
		delete(sentinel);
		sentinel = NULL;

		return sentinel;
	    }
	    else {
		break;
	    }
	}

	character = getchar();
    }

    sentinel->p2 = current;

    if(rfail) {
	delete(sentinel);
	sentinel = NULL;
    }

    return sentinel;
}

/* Output a string character by character */
void syspot(struct node *string)
{
    struct node *pos;
    struct node *end;
    struct node *s;

    s = string;
    if(string != NULL) {
	pos = s;
	end = s->p2;

	while(pos != end) {
	    pos = pos->p1;
	    putchar(pos->ch);
	}
    }

    putchar('\n');
}

/*
 * There is a head node that links backwards
 * to last char, and forwards to first char.
 * Every char backward links as well.
 */

struct node *sno_strstr(char s[])
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
    return head;}

/* Get nodes from a pool of created ones */
struct node *alloc(void)
{
    struct node *result = NULL;
    struct node *freespace_position = NULL;
    int expand_amount = 200;
    int i;

    /* There is no freespace at all */
    if(freelist == NULL) {
	freelist = (struct node *)malloc(sizeof(struct node));

	if(freelist == NULL) {
	    fflush(stdout);
	    write(fout, "Out of free space\n", strlen("Out of free space\n"));

	    exit(1);
	}
    }

    /* Expand by expand_amount nodes */
    if(freelist->p1 == NULL) {
	freespace_position = freelist;
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
    result = freelist;
    freelist = freelist->p1;
    freelist->p2 = NULL;

    result->p1 = NULL;

    return result;
}

/*
 * Destroy a single node, return to freelist
 * does not update links from connecting nodes
 */
void sno_free(struct node *pointer)
{
    pointer->p1 = freelist;
    pointer->p2 = NULL;
    pointer->typ = '\0';
    pointer->ch = '\0';

    freelist->p2 = pointer;
    freelist = pointer;
}

/* Simple count of the freespace nodes */
int nfree(void)
{
    struct node *pos = freelist;
    int result = 0;

    while(pos != NULL) {
	++result;
	pos = pos->p1;
    }

    return result;
}

/*
 * Looks for strings in the namelist.
 * Things go meta here, there is a sentinel
 * that points to a sentinel that points to
 * a string (which starts with a sentinel itself)
 *
 * Unless it is a match then it is only a sentinel
 * pointing to a string (which starts with a
 * sentinel itself)
 */
struct node *look(struct node *string)
{
    struct node *current_name;
    struct node *new_name;
    struct node *previous_name;

    previous_name = NULL;
    current_name = namelist;

    while(current_name) {
	new_name = current_name->p1;

	if(equal(new_name->p1, string) == 0) {
	    return new_name;
	}

	previous_name = current_name;
	current_name = previous_name->p2;
    }

    current_name = alloc();
    current_name->p2 = NULL;

    if(previous_name != NULL) {
	previous_name->p2 = current_name;
    }
    else {
	namelist = current_name;
    }

    new_name = alloc();
    current_name->p1 = new_name;
    new_name->p1 = copy(string);
    new_name->p2 = NULL;
    new_name->typ = 0;

    return new_name;
}

/*
 * Copy a string into a new one,
 * character by character
 */
struct node *copy(struct node *string)
{
    struct node *j;
    struct node *l;
    struct node *m;
    struct node *i;
    struct node *k;

    if(string == 0) {
	return(0);
    }
    l = alloc();
    i = l;
    j = string;
    k = string->p2;
    while(j != k) {
	m = alloc();
	j = j->p1;
	m->ch = j->ch;
	l->p1 = m;
	l = m;
    }
    i->p2 = l;
    return(i);
}

/* Determine if two strings are equal */
int equal(struct node *string1, struct node *string2)
{
    struct node *first_pos;
    struct node *first_end;
    struct node *second_pos;
    struct node *second_end;
    int first_value;
    int second_value;

    /* Handle the empty cases */
    if(string1 == NULL) {
	if(string2 == NULL) {
	    return 0;
	}

	return -1;
    }

    if(string2 == NULL) {
	return 1;
    }

    first_pos = string1;
    first_end = string1->p2;
    second_pos = string2;
    second_end = string2->p2;

    while(1) {
	first_pos = first_pos->p1;
	first_value = first_pos->ch;
	second_pos = second_pos->p1;
	second_value = second_pos->ch;

	/* Compare character values */
	if(first_value > second_value) {
	    return 1;
	}
	else if(first_value < second_value) {
	    return -1;
	}
	else {
	    /* The end of the first string*/
	    if(first_pos == first_end) {
		if(second_pos == second_end) {
		    return 0;
		}

		return -1;
	    }

	    /* First is longer than the second */
	    if(second_pos == second_end) {
		return 1;
	    }
	}
    }
}

/*
 * A very standard string to integer function
 * operating on their special string structure
 */
int strbin(struct node *string)
{
    int result;
    int digit;
    int sign;
    struct node *current;
    struct node *end;
    struct node *sentinel;

    sentinel = string;
    result = 0;

    if(sentinel == NULL) {
	return 0;
    }

    current = sentinel->p1;
    end = sentinel->p2;
    sign = 1;

    /* '-' sign */
    if(current->ch == '-') {
	/* minus */
	sign = -1;

	if(current == end) {
	    return 0;
	}

	current = current->p1;
    }

    /* Process digit by digit */
    while(1) {
	digit = current->ch - '0';
	if((digit > 9) || (digit < 0)) {
	    writes("Bad integer string");
	}

	result = (result * 10) + digit;

	if(current == end) {
	    return(result * sign);
	}

	current = current->p1;
    }
}

/* Take an number and convert to string */
struct node *binstr(int binary) {
    int number;
    int sign;
    struct node *next;
    struct node *sentinel;
    struct node *current;

    number = binary;
    sentinel = alloc();
    current = alloc();
    sign = 1;

    /* Handle negative numbers */
    if(binary < 0) {
	sign = -1;
	number = -binary;
    }

    /* Start from the back and move forward */
    sentinel->p2 = current;

    /*
     * Determine the string value of the digit,
     * when complete handle negative sign if needed
     */
    while(1) {
	current->ch = (number % 10) + '0';
	number = number / 10;

	if(number == 0) {
	    if(sign < 0) {
		next = alloc();
		next->p1 = current;
		current = next;
		current->ch = '-';
	    }

	    sentinel->p1 = current;

	    return sentinel;
	}

	next = alloc();
	next->p1 = current;
	current = next;
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
	return(copy(string2));
    }
    if(string2 == 0) {
	return(copy(string1));
    }
    a = copy(string1);
    b = copy(string2);
    a->p2->p1 = b->p1;
    a->p2 = b->p2;
    free(b);
    return(a);
}

/* Destructive concatenation */
struct node *dcat(struct node *a, struct node *b)
{
    struct node *result;

    result = cat(a, b);
    delete(a);
    delete(b);

    return result;
}

void delete(struct node *string)
{
    struct node *a;
    struct node *b;
    struct node *c;

    if(string == 0) {
	return;
    }
    a = string;
    b = string->p2;
    while(a != b) {
	c = a->p1;
	sno_free(a);
	a = c;
    }
    sno_free(a);
}

/* Print string then delete it */
void sysput(struct node *string)
{
    syspot(string);
    delete(string);
}

/* Shortcut to print out the namelist */
void dump(void)
{
    dump1(namelist);
}


void dump1(struct node *base)
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
}

void writes(char *s)
{
    sysput(dcat(binstr(lc),dcat(sno_strstr("\t"),sno_strstr(s))));
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

    while(line == NULL) {
	line = syspit();
	if(rfail) {
	    cfail++;
	    writes("eof on input");
	}
	lc++;
    }
    if(linflg) {
	line = NULL;
	linflg = 0;
	return(NULL);
    }
    a = line->p1;
    if(a == line->p2) {
	sno_free(line);
	linflg++;
    }
    else {
	line->p1 = a->p1;
    }
    return(a);
}
