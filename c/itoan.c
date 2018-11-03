/* itoan.c
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
#include <string.h>

/*
 * The function "itoa" is not a standard function but shows up a lot,
 * so this can be used instead.
 *
 * If "itoa" source compatibility is needed, use "gcc -DNEED_UNSAFE_ITOA=1".
 *
 * Typically, itoa treats non-base10 calls as unsigned, however this
 * behavior can be altered with "gcc -DNEED_SIGNED_NON_BASE10_ITOA=1".
 */

#if NEED_UNSAFE_ITOA
#define itoa(n, buf, base) itoan(n, buf, ((size_t)-1), base)
#endif

char *itoan(int n, char *buf, size_t buflen, unsigned base)
{
	size_t i, j, maxlen;

#if NEED_SIGNED_NON_BASE10_ITOA
	long l;
#else
	unsigned l;
#endif

	char c;

	if (buf == NULL || buflen == 0 || base == 0 || base > 36) {
		return NULL;
	}
	maxlen = buflen - 1;

	/* base is 10; good grief, just use sprintf */
	if (base == 10) {
		snprintf(buf, buflen, "%d", n);
		return buf;
	}

	/* cast to long to try to preserve sign-ness of INT_MIN */
#if NEED_SIGNED_NON_BASE10_ITOA
	l = (n < 0) ? -((long)n) : n;
#else
	l = n;
#endif

	/* fill the string in little endian order */
	i = 0;
	do {
		if (i >= maxlen) {
			/* buffer too short, bail out */
			buf[0] = '\0';
			return NULL;
		}

		c = l % base;
		if (c < 10) {
			c = '0' + c;
		} else {
			c = 'A' + (c - 10);
		}
		buf[i++] = c;

		l = l / base;
	} while (l);

#if NEED_SIGNED_NON_BASE10_ITOA
	/* tack on the sign if needed */
	if (n < 0) {
		if (i >= maxlen) {
			/* buffer too short, bail out */
			buf[0] = '\0';
			return NULL;
		}
		buf[i++] = '-';
	}
#endif

	buf[i] = '\0';

	/* reverse the string for big endian */
	for (i = 0, j = (strnlen(buf, buflen) - 1); i < j; ++i, --j) {
		c = buf[i];
		buf[i] = buf[j];
		buf[j] = c;
	}

	return buf;
}

#define BUFLEN 255
int main(int argc, char **argv)
{
	char buf[BUFLEN];
	int i;
	unsigned base;

	i = (argc > 1) ? atoi(argv[1]) : 0;
	base = (argc > 2) ? (unsigned)atoi(argv[2]) : 10;

	printf("i:%s\n", itoan(i, buf, BUFLEN, base));

#ifdef NEED_UNSAFE_ITOA
	printf(" :%s\n", itoa(i, buf, base));
#endif

	return 0;
}
