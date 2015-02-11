/* Snobol III */

#include "sno1.h"
#include "sno2.h"
#include "sno4.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct node *freelist = NULL;
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

    /* TC_DEBUG: */
    struct node *reverse_namelist;

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

    /* TC_DEBUG: Start */
    reverse_namelist = namelist;
    
    while(reverse_namelist != NULL) {
	print_node_list(reverse_namelist, NULL);
	reverse_namelist = reverse_namelist->p2;
    }
    /* TC_DEBUG: Finish */

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
 * */
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
	new_pos->p1->ch = old_pos->ch;

	/* Move to last character */
	new_pos = new_pos->p1;

	new_head->p2 = new_pos;
    }

    return new_head;
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
    if(class(current->ch) == 5) {
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

/* Concatenate two strings together */
struct node *cat(struct node *string1, struct node *string2)
{
    struct node *a;
    struct node *b;

    if(string1 == NULL) {
	return copy(string2);
    }
    
    if(string2 == NULL) {
	return copy(string1);
    }

    a = copy(string1);
    b = copy(string2);

    /* 
     * Make the last node of the first string
     * point to the first node of the second
     * and vice versa
     */
    a->p2->p1 = b->p1;
    b->p1->p2 = a->p2;

    /*
     * Update the end of the first string to
     * the end of the second string
     */
    a->p2 = b->p2;

    /* Remove the sentinel of the second string */
    sno_free(b);

    return a;
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

/* Shortcut to print out the namelist */
void dump(void)
{
    dump1(namelist);
}

/* Print and move through a chain of nodes */
int dump1(struct node *base)
{
    struct node *current_string;
    struct node *type;
    struct node *temp;
    struct node *space;
    
    while(base) {
	current_string = base->p1;
	type = binstr(current_string->typ);
	space = sno_strstr("  ");
	temp = dcat(type, space);
	sysput(cat(temp, current_string->p1));
	delete(temp);

	/* Prints out backwards from current point */
	if(current_string->typ == 1) {
	    space = sno_strstr("   ");
	    sysput(cat(space, current_string->p2));
	    delete(space);
	}

	/* Move to the end of the current string */
	base = base->p2;
    }

    return 0;
}

/* Prints the line number and a message */
int writes(char *s)
{
    sysput(dcat(binstr(lc), dcat(sno_strstr("\t"), sno_strstr(s))));
    fflush(stdout);

    /* If compilation failure occurred dump the namelist */
    if(cfail) {
	dump();
	fflush(stdout);

	exit(1);
    }

    /* Read the rest of the line */
    while(sno_getc());

    /* Go back to compiling */
    while(compile());

    fflush(stdout);

    exit(1);
}

/*
 * Process a line of input and give it
 * back character by character
 */
struct node *sno_getc(void)
{
    struct node *current_char;
    static struct node *line;
    static int line_finished;

    /* Process the next line of input */
    while(line == NULL) {
	line = syspit();
	
	if(rfail) {
	    ++cfail;
	    writes("EOF on input");
	}

	++lc;
    }

    /* Finished processing line */
    if(line_finished) {
	line = NULL;
	line_finished = 0;

	return NULL;
    }

    current_char = line->p1;

    /* Reached the end of the line */
    if(current_char == line->p2) {
	sno_free(line);
	++line_finished;
    }
    else {
	/* Move sentinel of line to next character */
	line->p1 = current_char->p1;
    }

    return current_char;
}

/* TC_DEBUG: Print a particular node */
void print_node(struct node *n)
{
    printf("%p", n);
    
    if(n != NULL) {
	printf(": (%9p, %2d,", n->p2, n->typ);

	/* Print as a character if it makes sense */
	if((n->ch >= 32) && (n->ch <= 126)) {
	    printf(" '%c',", n->ch);
	}
	else {
	    printf(" %3d,", n->ch);
	}

	printf(" %9p)", n->p1);
    }

    printf("\n");
}

/* TC_DEBUG: Print a list of nodes */
void print_node_list(struct node *n, char const *name)
{
    if(name == NULL) {
	printf("(unnamed):\n");
    }
    else {
	printf("\"%s\":\n", name);
    }

    if(n == NULL) {
	printf("\t %p\n", n);
    }
    else {
	while(n != NULL) {
	    printf("\t");
	    print_node(n);

	    if((n->p1 > n) || (n->p1 == NULL)) {
		n = n->p1;
	    }
	    else {
		break;
	    }
	}
    }
}
