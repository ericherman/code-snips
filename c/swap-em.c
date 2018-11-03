/* swap-em.c
   Copyright (C) 2016, 2018 Eric Herman <eric@freesa.org>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

	https://www.gnu.org/licenses/lgpl-3.0.txt
	https://www.gnu.org/licenses/gpl-3.0.txt
 */
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
