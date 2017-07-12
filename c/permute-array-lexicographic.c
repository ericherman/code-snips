#include <stdio.h>		/* printf */
#include <stdlib.h>		/* malloc, free */
#include <string.h>		/* memmove, memcpy */

/*
gcc -std=c89 -Wall -Wextra -Wpedantic -Werror \
    -o permute-array-lexicographic permute-array-lexicographic.c
# to get the 1,000,000th lexicographic ordered permutation of 10 digits:
./permute-array-lexicographic 10 999999
*/

#define MAKE_VALGRIND_HAPPY 0

size_t factorial(size_t n);

/* allows caller to fetch a specific lexicographic zero-indexed permutation */
void *permute(size_t perm_idx, const void *src, void *dest, size_t len,
	      size_t elem_size, void *elem_swap_buf)
{
	size_t i, idx, remainder, num_perms, choice_idx;
	unsigned char *d;
	void *from, *to;

	if (!src || !dest || !len) {
		fprintf(stderr, "src: %p, dest: %p, len:%lu\n", src, dest,
			(unsigned long)len);
		return NULL;
	}

	memcpy(dest, src, len * elem_size);
	if (perm_idx == 0) {
		return dest;
	}

	idx = perm_idx;
	d = (unsigned char *)dest;	/* allow ptr math */

	for (i = 0; idx && i < len - 1; ++i) {
		num_perms = factorial(len - i - 1);
		choice_idx = idx / num_perms;
		remainder = idx % num_perms;
		to = d + (i * elem_size);
		from = d + ((i + choice_idx) * elem_size);
		if (to != from) {
			memcpy(elem_swap_buf, to, elem_size);
			memcpy(to, from, elem_size);
			if ((i < (len - 2))) {
				to = d + ((i + 2) * elem_size);
				from = d + ((i + 1) * elem_size);
				memmove(to, from,
					(elem_size * (choice_idx - 1)));
			}
			to = d + ((i + 1) * elem_size);
			memcpy(to, elem_swap_buf, elem_size);
		}
		idx = remainder;
	}

	return dest;
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

static void print_ints(int *a, size_t len)
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
	int permutation_number;

	len = (argc > 1) ? (size_t)atoi(argv[1]) : 4;
	permutation_number = (argc > 2) ? atoi(argv[2]) : -1;

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

	if (permutation_number < 0) {
		for (i = 0; i < factorial(len); ++i) {
			permute(i, ints, permuted_ints, len, elem_size, &x);
			print_ints(permuted_ints, len);
		}
	} else {
		permute((size_t)permutation_number, ints, permuted_ints, len,
			elem_size, &x);
		print_ints(permuted_ints, len);
	}

	if (MAKE_VALGRIND_HAPPY) {
		free(ints);
		free(permuted_ints);
	}

	return 0;
}
