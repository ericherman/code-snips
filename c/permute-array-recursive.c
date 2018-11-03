/* permute-array-recursive.c
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
#include <stdio.h>		/* printf */
#include <stdlib.h>		/* malloc, free */
#include <string.h>		/* memmove, memcpy */

/*
gcc -std=c89 -Wall -Wextra -Wpedantic -Werror \
    -o permute-array-recursive permute-array-recursive.c
*/

#define MAKE_VALGRIND_HAPPY 0

void swap(void *a, void *b, void *buf, size_t element_size)
{
	memcpy(buf, b, element_size);	/* save b */
	memcpy(b, a, element_size);	/* a -> b */
	memcpy(a, buf, element_size);	/* saved b -> a */
}

typedef int (*pfunc) (void *context);

/* permutes the source array in-place, recursively */
/* allows action for each permutation via the function pointer */
void permute(void *arr, size_t from, size_t last, void *buf, size_t elem_size,
	     pfunc func, void *context)
{
	size_t i;
	unsigned char *p;

	if (from == last) {
		func(context);
		return;
	}

	p = (unsigned char *)arr;	/* allow ptr math */
	for (i = from; i <= last; i++) {
		swap((p + (from * elem_size)), (p + (i * elem_size)), buf, elem_size);
		permute(p, (from + 1), last, buf, elem_size, func, context);
		swap((p + (from * elem_size)), (p + (i * elem_size)), buf, elem_size);
	}
}

struct int_arr_s {
	int *a;
	size_t len;
};

int print_ints(void *context)
{
	size_t i;
	struct int_arr_s *s;

	s = (struct int_arr_s *)context;
	printf("[");
	for (i = 0; i < s->len; ++i) {
		printf("%d,", s->a[i]);
	}
	printf("\b]\n");

	return 0;
}

int main(int argc, char *argv[])
{
	int *ints, x;
	size_t i, len, elem_size;
	struct int_arr_s ctx;

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
	for (i = 0; i < len; ++i) {
		ints[i] = (int)i;
	}
	ctx.a = ints;
	ctx.len = len;

	printf("---------\n");
	print_ints(&ctx);
	printf("---------\n");

	permute(ints, 0, len - 1, &x, elem_size, print_ints, &ctx);

	if (MAKE_VALGRIND_HAPPY) {
		free(ints);
	}

	return 0;
}
