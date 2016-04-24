/*

gcc -std=c89 -Wall -Wextra -Wpedantic -Werror -ggdb -O0 \
	-o choose-k-from-n ./choose-k-from-n.c

This is basically a copy-paste-rework of:
 https://compprog.files.wordpress.com/2007/10/comb1.c
linked from:
 https://compprog.wordpress.com/2007/10/17/generating-combinations-1/

*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int choose_k_from_n(size_t *idxs, size_t k, size_t n)
{
	size_t i;

	i = k - 1;
	++idxs[i];
	while (idxs[i] >= (n - k + 1 + i)) {
		if (i == 0) {
			break;
		}
		--i;
		++idxs[i];
	}

	if (idxs[0] > (n - k)) {
		return 0;
	}

	for (i = (i + 1); i < k; ++i) {
		idxs[i] = idxs[i - 1] + 1;
	}

	return 1;
}

char *ints_idxs_to_string(char *buf, int *ints, size_t *idxs, size_t len)
{
	size_t i;
	int x;

	x = sprintf(buf, "[");
	for (i = 0; i < len; ++i) {
		x += sprintf(buf + x, "%d, ", ints[idxs[i]]);
	}
	/* trim trailing comma */
	if (x > 2) {
		x -= 2;
	}
	sprintf(buf + x, "]");
	return buf;
}

void *alloc_or_die(void *ptr, size_t bytes)
{
	ptr = realloc(ptr, bytes);
	if (!ptr) {
		fprintf(stderr, "failed to allocate %lu bytes (%s)\n",
			(unsigned long)bytes, strerror(errno));
		exit(EXIT_FAILURE);
	}
	return ptr;
}

int *add_val(int val, int *vals, size_t *vals_size, size_t *vals_len)
{
	size_t bytes;

	if (((*vals_len) + 1) >= *vals_size) {
		*vals_size += 100;
		bytes = sizeof(int) * (*vals_size);
		vals = alloc_or_die(vals, bytes);
	}

	vals[(*vals_len)++] = val;

	return vals;
}

#define LINE_LEN 250
int main(int argc, char **argv)
{
	size_t i, k, k2, n, vals_size, *idxs;
	int val, *vals, matched;
	FILE *input;
	char buf[LINE_LEN];

	k = (argc > 1) ? (size_t)atoi(argv[1]) : 3;
	k2 = (argc > 2) ? (size_t)atoi(argv[2]) : k;

	n = 0;
	vals = NULL;
	vals_size = 0;
	if (argc > 3) {
		input = fopen(argv[3], "r");
		if (!input) {
			fprintf(stderr, "could not open %s\n", argv[3]);
			exit(EXIT_FAILURE);
		}
		while (fgets(buf, LINE_LEN, input)) {
			matched = sscanf(buf, "%d", &val);
			if (matched) {
				vals = add_val(val, vals, &vals_size, &n);
			}
		}
	} else {
		for (i = 0; i < (k2 * 2); ++i) {
			val = i + 1;
			vals = add_val(val, vals, &vals_size, &n);
		}
	}

	idxs = alloc_or_die(NULL, sizeof(size_t) * n);
	for (i = 0; i < n; ++i) {
		idxs[i] = i;
	}

	printf("------------------\n");
	printf("%s\n", ints_idxs_to_string(buf, vals, idxs, n));
	printf("------------------\n");

	printf("%s\n", ints_idxs_to_string(buf, vals, idxs, k));
	while (choose_k_from_n(idxs, k, n)) {
		printf("%s\n", ints_idxs_to_string(buf, vals, idxs, k));
	}

	if (k != k2) {
		/* first, need to reset indexes */
		for (i = 0; i < n; ++i) {
			idxs[i] = i;
		}
		printf("\n");
		printf("%s\n", ints_idxs_to_string(buf, vals, idxs, k2));
		while (choose_k_from_n(idxs, k2, n)) {
			printf("%s\n",
			       ints_idxs_to_string(buf, vals, idxs, k2));
		}
	}

	free(idxs);
	free(vals);

	return 0;
}
