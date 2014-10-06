#define _GNU_SOURCE
#include "qsort_ints.h"

#include <stdlib.h>

int compare_ints(int a, int b, int descending)
{
	if (descending == 0) {
		if (a < b)
			return -1;
		else if (a > b)
			return 1;
		return 0;
	} else {
		if (a > b)
			return -1;
		else if (a < b)
			return 1;
		return 0;
	}
}

int int_comparator(const void *vs_a, const void *vs_b, void *context)
{
	int a = *((int *)vs_a);
	int b = *((int *)vs_b);
	int descending = *((int *)context);
	return compare_ints(a, b, descending);
}

void qsort_ints(int *elements, size_t num_elements)
{
	int descending = 0;
	size_t element_size = sizeof(int);
	/*
	   Sort NUM_ELEMENTS elements of ELEMENTS,
	   of ELEMENT_SIZE bytes each,
	   using COMPAR to perform the comparisons,
	   allowing a context pointer CONTEXT
	 */
	qsort_r(elements, num_elements, element_size, int_comparator,
		&descending);
}
