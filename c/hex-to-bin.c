/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2016-2026 Eric Herman <eric@freesa.org> */

#ifndef read_byte_stdin
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <unistd.h>
static int read_byte_stdin_posix(void *b)
{
	return (read(STDIN_FILENO, b, 1) == 1) ? 1 : 0;
}

#define read_byte_stdin read_byte_stdin_posix
#endif
#endif
#ifndef read_byte_stdin
#error "read_byte_stdin not defined"
#endif

#ifndef write_byte_stdout
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <unistd.h>
static int write_byte_stdout_posix(void *b)
{
	return (write(STDOUT_FILENO, b, 1) == 0) ? 1 : 0;
}

#define write_byte_stdout write_byte_stdout_posix
#endif
#endif
#ifndef write_byte_stdout
#error "write_byte_stdout not defined"
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

int hex_to_bin(int (*read_byte)(void *b), int (*write_byte)(void *b))
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

#ifndef HEX_TO_BIN_LIB
int main(void)
{
	return hex_to_bin(read_byte_stdin, write_byte_stdout);
}
#endif
