#include <stdio.h>
#include <stdint.h>

/* this is the way to swap pointers */
void standard_swap(char **left, char **right)
{
	char *tmp;

	tmp = *left;
	*left = *right;
	*right = tmp;
}

/*
 * The xor swapping of pointers is doable, but dumb.
 * Typically bitwise operations do not work on pointers in C.
 * Casting to intptr_t is a work-around, but ugly.
 * Even so, the result does not look a lot more opimized to me.
 * $ gcc -S -O3 swap-em.c
 * $ cat swap-em.s
 */
void xor_swap(char **left, char **right)
{
	*left = (char*) (((intptr_t)*left) ^ ((intptr_t)*right));
	*right = (char*) (((intptr_t)*left) ^ ((intptr_t)*right));
	*left = (char*) (((intptr_t)*left) ^ ((intptr_t)*right));
}

int main(int argc, char **argv)
{
	char *a, *b;
	char *o, *t;

	o = "one";
	t = "two";

	a = o;
	b = t;

	printf("initial state: %s, %s\n", a, b);
	standard_swap(&a, &b);
	printf("standard swap: %s, %s\n", a, b);
	xor_swap(&a, &b);
	printf("xor swap back: %s, %s\n", a, b);

	return 0;
}
