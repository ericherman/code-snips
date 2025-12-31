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

static unsigned char hex_to_nibble(char hex)
{
	if (hex >= '0' && hex <= '9') {
		return (unsigned char)hex - '0';
	}
	if (hex >= 'a' && hex <= 'f') {
		return (unsigned char)10 + hex - 'a';
	}
	if (hex >= 'A' && hex <= 'F') {
		return (unsigned char)10 + hex - 'A';
	}

	return 255;
}

unsigned char hex_chars_to_byte(char high, char low)
{
	unsigned char byte;

	byte = hex_to_nibble(high) << 4;
	byte += hex_to_nibble(low);

	return byte;
}

static int is_space(char c)
{
	switch (c) {
	case ' ':
	case '\n':
	case '\r':
	case '\t':
	case '\v':
	case '\f':
		return 1;
	default:
		return 0;
	}
}

static int is_hex(char c)
{
	return ((c >= '0' && c <= '9')
		|| (c >= 'a' && c <= 'f')
		|| (c >= 'A' && c <= 'F')
	    )? 1 : 0;
}

int hex_to_bin(void)
{
	int comment = 0;
	char c = 0;
	int have_hi = 0;
	unsigned char b = 0;
	char hi = 0;
	char lo = 0;
	while (read_byte(&c)) {
		if (c == '#') {
			comment = 1;
		} else if (c == '\n' || c == '\r') {
			comment = 0;
		}
		if (is_space(c) || comment) {
			if (have_hi) {
				return 1;
			}
			continue;
		}
		if (!is_hex(c)) {
			return 1;
		}
		if (have_hi) {
			have_hi = 0;
			lo = c;
			b = hex_chars_to_byte(hi, lo);
			write_byte(&b);
		} else {
			have_hi = 1;
			hi = c;
		}
	}
	return have_hi ? 1 : 0;
}

int main(void)
{
	return hex_to_bin();
}
