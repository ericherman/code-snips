/* realloc.c
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
#include <stdlib.h>
#include <assert.h>

#define CHUNK_SIZE 1000

int main(void)
{
	int i;
	int *one;
	int *one_saved;
	int *two;
	int *two_saved;
	int *one_bigger;
	int *two_bigger;

	one = malloc(sizeof(int) * CHUNK_SIZE);
	if (!one) {
		exit(EXIT_FAILURE);
	}
	one_saved = one;
	two = malloc(sizeof(int) * CHUNK_SIZE);
	if (!two) {
		exit(EXIT_FAILURE);
	}
	two_saved = two;
	for (i = 0; i < CHUNK_SIZE; ++i) {
		one[i] = i;
		two[i] = 1 + (2 * i);
	}

	for (i = 0; i < CHUNK_SIZE; ++i) {
		assert(one[i] == i);
		assert(two[i] == (1 + (2 * i)));
	}

	one_bigger = realloc(one, sizeof(int) * 2 * CHUNK_SIZE);
	two_bigger = realloc(two, sizeof(int) * 2 * CHUNK_SIZE);

	printf("one_saved:  %p\none_bigger: %p\n", one_saved, one_bigger);
	printf("two_saved:  %p\ntwo_bigger: %p\n", two_saved, two_bigger);

	for (i = 0; i < CHUNK_SIZE; ++i) {
		assert(one_bigger[i] == i);
		assert(two_bigger[i] == (1 + (2 * i)));
	}
	printf("storage larger, initial contents the same\n");
	return 0;
}
