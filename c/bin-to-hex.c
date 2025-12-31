/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2016-2025 Eric Herman <eric@freesa.org> */

#ifndef read_byte
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <unistd.h>
static int read_byte_posix(void *b)
{
	return (read(STDIN_FILENO, b, 1) == 1) ? 1 : 0;
}

#define read_byte(b) read_byte_posix(b)
#endif
#endif
#ifndef read_byte
#error "read_byte() not defined"
#endif

#ifndef write_byte
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <unistd.h>
static int write_byte_posix(void *b)
{
	return (write(STDOUT_FILENO, b, 1) == 0) ? 1 : 0;
}

#define write_byte(b) write_byte_posix(b)
#endif
#endif
#ifndef write_byte
#error "write_byte() not defined"
#endif

static char nibble_to_hex(unsigned char nibble)
{
	if (nibble < 10) {
		return '0' + nibble;
	}
	if (nibble < 16) {
		return 'A' + nibble - 10;
	}

	return '?';
}

void byte_to_hex_chars(unsigned char byte, char *high, char *low)
{
	*high = nibble_to_hex((byte & 0xF0) >> 4);
	*low = nibble_to_hex((byte & 0x0F));
}

int bin_to_hex(void)
{
	size_t pos = 0;
	unsigned char b = 0;
	char hi = 0;
	char lo = 0;
	char space = ' ';
	char newln = '\n';
	while (read_byte(&b)) {
		if (pos) {
			write_byte(&space);
		}
		byte_to_hex_chars(b, &hi, &lo);
		write_byte(&hi);
		write_byte(&lo);
		++pos;
		if (pos == 15) {
			write_byte(&newln);
			pos = 0;
		}
	}
	if (pos) {
		write_byte(&newln);
	}
	return 0;
}

int main(void)
{
	return bin_to_hex();
}
