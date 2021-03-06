/* bit-fields.c
   Copyright (C) 2014, 2016, 2018 Eric Herman <eric@freesa.org>

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

#define BITS_UNSIGNED_LONG (sizeof(unsigned long) * 8)
#define BITS_UNSIGNED_INT (sizeof(unsigned int) * 8)
#define BITS_UI_BUF_LEN (BITS_UNSIGNED_INT + 1)

/* this syntax uses only a few bits per field */
struct teeny_s {
	unsigned int u:3;
	unsigned int v:2;
	unsigned int w:1;
	int x:3;
	int y:2;
	int z:1;
};

#if _POSIX_C_SOURCE < 200809L
size_t strnlen(char *str, size_t buf_size)
{
	size_t len = strlen(str);
	if (len > buf_size) {
		/* exit(EXIT_FAILURE); */
		return buf_size;
	}
	return len;
}
#endif

void revstr(char *str, size_t buf_size)
{
	size_t i, j, len;
	char swap;

	if (buf_size == 0) {
		/* exit(EXIT_FAILURE); */
		return;
	}

	len = strnlen(str, buf_size);
	for (i = 0, j = len - 1; i < j; i++, j--) {
		swap = str[i];
		str[i] = str[j];
		str[j] = swap;
	}
}

/*
  unsigned to binary
  represents a binary value as a string of zeros and ones
*/
char *utob(char *buf, size_t len, unsigned long val, int bits, int lil_endian)
{
	size_t i, shift, str_pos;
	if (len == 0) {
		/* exit(EXIT_FAILURE); */
		/* return NULL; */
		return buf;
	}
	if (bits < 1) {
		bits = BITS_UNSIGNED_LONG;
	}

	str_pos = 0;
	for (i = bits; i > 0; i--) {
		if (str_pos >= (len - 1)) {
			break;
		}
		shift = (i - 1);
		buf[str_pos++] = ((val >> shift) & 1) ? '1' : '0';
	}
	buf[str_pos] = '\0';
	if (lil_endian) {
		revstr(buf, len);
	}
	return buf;
}

int main(int argc, char *argv[])
{
	int a;
	struct teeny_s s;
	char buf[BITS_UI_BUF_LEN];
	int le;

	a = (argc > 1) ? atoi(argv[1]) : 5;
	le = (argc > 2) ? atoi(argv[2]) : 0;

	s.u = a;
	s.v = a;
	s.w = a;
	s.x = a;
	s.y = a;
	s.z = a;

	printf("             int = %33d (decimal)\n", a);
	printf("             int = %33s (binary)\n\n",
	       utob(buf, BITS_UI_BUF_LEN, a, BITS_UNSIGNED_INT, le));

	printf("unsigned int u:3 = %33u (decimal)\n", s.u);
	printf("unsigned int u:3 = %33s (binary)\n\n",
	       utob(buf, BITS_UI_BUF_LEN, s.u, 3, le));

	printf("unsigned int v:2 = %33u (decimal)\n", s.v);
	printf("unsigned int v:2 = %33s (binary)\n\n",
	       utob(buf, BITS_UI_BUF_LEN, s.v, 2, le));

	printf("unsigned int w:1 = %33u (decimal)\n", s.w);
	printf("unsigned int w:1 = %33s (binary)\n\n",
	       utob(buf, BITS_UI_BUF_LEN, s.w, 1, le));

	printf("         int x:3 = %33d (decimal)\n", s.x);
	printf("         int x:3 = %33s (binary)\n\n",
	       utob(buf, BITS_UI_BUF_LEN, s.x, 3, le));

	printf("         int y:2 = %33d (decimal)\n", s.y);
	printf("         int y:2 = %33s (binary)\n\n",
	       utob(buf, BITS_UI_BUF_LEN, s.y, 2, le));

	printf("         int z:1 = %33d (decimal)\n", s.z);
	printf("         int z:1 = %33s (binary)\n\n",
	       utob(buf, BITS_UI_BUF_LEN, s.z, 1, le));

	printf("sizeof struct is %u bytes\n", sizeof(struct teeny_s));
	return 0;
}
