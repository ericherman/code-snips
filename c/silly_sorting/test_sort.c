#include <stdio.h>
#include <stdlib.h>

#include "qsort_ints.h"
#include "silly_sorts.h"

typedef struct sort_func_t_ {
	const char *name;
	void (*sort_func) (int *elements, size_t num_elements);
} sort_func_t;

int main(void)
{
	int num_elements = 7;
	int array[num_elements];
	int element_max_value = 10;
	sort_func_t sort_funcs[2];
	int i, j, last;
	int sorted = 1;

	sort_funcs[0].name = "qsort";
	sort_funcs[0].sort_func = qsort_ints;

	sort_funcs[1].name = "sleep_sort";
	sort_funcs[1].sort_func = sleep_sort;

	for (j = 0; j < 2; j++) {
		printf("%20s: { ", "un-sorted");
		for (i = 0; i < num_elements; i++) {
			array[i] = rand() % element_max_value;
			printf("%d, ", array[i]);
		}
		printf("}\n");

		sort_funcs[j].sort_func(array, num_elements);

		last = array[0];
		printf("%13s sorted: { ", sort_funcs[j].name);
		for (i = 0; i < num_elements; i++) {
			if (array[i] < last) {
				sorted = 0;
			}
			printf("%d, ", array[i]);
		}
		printf("}\n");
	}

	exit(sorted ? EXIT_SUCCESS : EXIT_FAILURE);
}
