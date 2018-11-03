/* permute-array-swap.c
   Copyright (C) 2017, 2018 Eric Herman <eric@freesa.org>

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
#include <stdio.h>		/* printf */
#include <stdlib.h>		/* malloc, free */
#include <string.h>		/* memmove, memcpy */

/*
gcc -std=c89 -Wall -Wextra -Wpedantic -Werror \
    -o permute-array-swap permute-array-swap.c
*/

#define MAKE_VALGRIND_HAPPY 0

void swap(void *a, void *b, void *buf, size_t element_size)
{
	memcpy(buf, b, element_size);	/* save b */
	memcpy(b, a, element_size);	/* a -> b */
	memcpy(a, buf, element_size);	/* saved b -> a */
}

size_t factorial(size_t n)
{
	size_t result = n;
	if (n < 2) {
		return 1;
	}
	do {
		result *= --n;
	} while (n > 1);
	return result;
}

/* allows caller to fetch a specifically indexed permutation */
void permute(size_t permutation, const void *src, void *dest, size_t len,
	     void *buf, size_t elem_size)
{
	size_t i, sub_perm, n_sub_perms, next_swap;
	unsigned char *d;
	void *a, *b;

	memcpy(dest, src, len * elem_size);
	sub_perm = permutation;
	d = (unsigned char *)dest;	/* allow ptr math */

	for (i = 0; i < len - 1; ++i) {
		n_sub_perms = factorial(len - 1 - i);
		next_swap = sub_perm / n_sub_perms;
		sub_perm = sub_perm % n_sub_perms;
		if (next_swap != 0) {
			a = d + i * elem_size;
			b = d + (i + next_swap) * elem_size;
			swap(a, b, buf, elem_size);
		}
	}
}

void print_ints(int *a, size_t len)
{
	size_t i;

	printf("[");
	for (i = 0; i < len; ++i) {
		printf("%d,", a[i]);
	}
	printf("\b]\n");
}

int main(int argc, char *argv[])
{
	int *ints, *permuted_ints, x;
	size_t i, len, elem_size;

	len = (argc > 1) ? (size_t) atoi(argv[1]) : 4;
	if (len == 0) {
		printf("[]\n");
		return 0;
	}
	elem_size = sizeof(int);
	ints = malloc(elem_size * len);
	if (!ints) {
		fprintf(stderr, "malloc(%lu) == NULL\n",
			(unsigned long)elem_size * len);
		return 1;
	}
	permuted_ints = malloc(elem_size * len);
	if (!permuted_ints) {
		fprintf(stderr, "2nd malloc(%lu) == NULL\n",
			(unsigned long)elem_size * len);
		return 1;
	}

	for (i = 0; i < len; ++i) {
		ints[i] = (int)i;
	}

	printf("---------\n");
	print_ints(ints, len);
	printf("---------\n");

	for (i = 0; i < factorial(len); ++i) {
		permute(i, ints, permuted_ints, len, &x, elem_size);
		print_ints(permuted_ints, len);
	}

	if (MAKE_VALGRIND_HAPPY) {
		free(ints);
		free(permuted_ints);
	}

	return 0;
}
