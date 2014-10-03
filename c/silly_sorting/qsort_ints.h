#ifndef _QSORT_INTS_H_
#define _QSORT_INTS_H_

#define _GNU_SOURCE
#include <stdlib.h>

void qsort_ints(int *elements, size_t num_elements);
int compare_ints(int a, int b, int descending);
int int_comparator(const void *vs_a, const void *vs_b, void *context);

#endif /* _QSORT_INTS_H_ */
