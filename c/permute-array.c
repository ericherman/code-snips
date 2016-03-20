#include <stdio.h>		/* printf */
#include <stdlib.h>		/* malloc, free */
#include <string.h>		/* memmove, memcpy */

/*
gcc -std=c89 -Wall -Wextra -Wpedantic -Werror \
   -o permute-array permute-array.c
*/

#define MAKE_VALGRIND_HAPPY 0

void swap(void *a, void *b, void *buf, size_t element_size)
{
	memcpy(buf, b, element_size);	/* save b */
	memcpy(b, a, element_size);	/* a -> b */
	memcpy(a, buf, element_size);	/* saved b -> a */
}

typedef int (*pfunc) (void *context);

void permute(void *arr, size_t from, size_t last, void *buf, size_t size,
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
		swap((p + (from * size)), (p + (i * size)), buf, size);
		permute(p, (from + 1), last, buf, size, func, context);
		swap((p + (from * size)), (p + (i * size)), buf, size);
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
	size_t i, len, size;
	struct int_arr_s ctx;
	void *arr, *buf;

	len = (argc > 1) ? (size_t) atoi(argv[1]) : 4;
	if (len == 0) {
		printf("[]\n");
		return 0;
	}
	size = sizeof(int);
	ints = malloc(size * len);
	if (!ints) {
		fprintf(stderr, "malloc(%lu) == NULL\n",
			(unsigned long)size * len);
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

	arr = (void *)ints;
	buf = &x;
	permute(arr, 0, len - 1, buf, size, print_ints, &ctx);

	if (MAKE_VALGRIND_HAPPY) {
		free(arr);
	}

	return 0;
}
