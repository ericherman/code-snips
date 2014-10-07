#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "qsort_ints.h"
#include "silly_sorts.h"

#define NUM_FUNCS 4
typedef struct sort_func_t_ {
	const char *name;
	void (*sort_func) (int *elements, size_t num_elements);
} sort_func_t;

int main(void)
{
	int num_elements = 4;
	int *array;
	int *vals;
	int element_max_value = 15;
	sort_func_t sort_funcs[NUM_FUNCS];
	int i, j, last, negate;
	int sorted = 1;
	clock_t start, end;
	double elapsed;

	array = (int *)malloc(num_elements * sizeof(int));
	vals = (int *)malloc(num_elements * sizeof(int));

	sort_funcs[0].name = "qsort";
	sort_funcs[0].sort_func = qsort_ints;

	sort_funcs[1].name = "random_sort";
	sort_funcs[1].sort_func = random_sort;

	sort_funcs[2].name = "sleep_sort";
	sort_funcs[2].sort_func = sleep_sort;

	sort_funcs[3].name = "random_sleep_sort";
	sort_funcs[3].sort_func = random_sleep_sort;

	printf("%25s: { ", "un-sorted");
	for (i = 0; i < num_elements; i++) {
		negate = rand() % 3;
		vals[i] = rand() % element_max_value;
		if (negate == 1) {
			vals[i] *= -1;
		}
		printf("%d, ", vals[i]);
	}
	printf("}\n");

	for (j = 0; j < NUM_FUNCS; j++) {
		memcpy(array, vals, num_elements * sizeof(int));

		printf("%18s sorted: { ", sort_funcs[j].name);
		fflush(stdout);

		start = clock();
		sort_funcs[j].sort_func(array, num_elements);
		end = clock();
		elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;

		last = array[0];
		for (i = 0; i < num_elements; i++) {
			if (array[i] < last) {
				sorted = 0;
			}
			last = array[i];
			printf("%d, ", array[i]);
		}
		printf("} elapsed: %9.2f\n", elapsed * 10000);
	}

	free(array);
	free(vals);

	exit(sorted ? EXIT_SUCCESS : EXIT_FAILURE);
}
