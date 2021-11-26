/* SPDX-License-Identifier: GPL-3.0-or-later */
/* Copyright (C) 2021 Eric Herman <eric@freesa.org> */

#include <stdalign.h>

struct unaligned_data {
	char c;
	char buf1[41];
	char buf2[41];
};

struct aligned_data {
	char c;
	alignas(void *) char buf1[41];
	alignas(void *) char buf2[41];
};

#include <stdio.h>
int main(void)
{
	printf(" sizeof(struct unaligned_data): %4zu\n",
	       sizeof(struct unaligned_data));
	printf("alignof(struct unaligned_data): %4zu\n",
	       alignof(struct unaligned_data));

	printf("\n");

	printf(" sizeof(struct aligned_data):   %4zu\n",
	       sizeof(struct aligned_data));
	printf("alignof(struct aligned_data):   %4zu\n",
	       alignof(struct aligned_data));

	return 0;
}
