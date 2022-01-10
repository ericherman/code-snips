/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* rev_u8.c: comparing different ways of inverting bits in a byte */
/* Copyright (C) 2022 Eric Herman <eric@freesa.org> */
/*  gcc -Wall -Wextra -g -O3 -DIS_TEST=1 rev_u8.c -o rev_u8 &&
    { for i in `seq 1000`; do echo $i; ./rev_u8; done } > rev.out &&
    { for func in \
      rev_u8_loop rev_u8_ugly rev_u8_8shifts rev_u8_3swaps rev_u8_table; do
      grep $func rev.out | sort -rn | tail -n10; done } | sort -n
    # -O3, -Os, -O2 may provide different orderings
*/

#include <stdint.h>

/* rev_u8_loop returns b with the bit-order reversed. */
uint8_t rev_u8_loop(uint8_t b)
{
	/* this is written to be very easy to think about, but likely slow */
	uint8_t r = 0;
	for (uint8_t i = 0; i < 8; ++i) {
		uint8_t position = (1 << i);
		if (b & position) {
			uint8_t set_bit = (1 << (7 - i));
			r = (r | set_bit);
		}
	}
	return r;
}

/* rev_u8_ugly returns b with the bit-order reversed. */
uint8_t rev_u8_ugly(uint8_t b)
{
	/* (this is rev_u8_loop unrolled) */
	/* this is written to be fairly easy to think about, but very ugly */
	return ((b & (1 << 0)) ? (1 << 7) : 0)
	    | ((b & (1 << 1)) ? (1 << 6) : 0)
	    | ((b & (1 << 2)) ? (1 << 5) : 0)
	    | ((b & (1 << 3)) ? (1 << 4) : 0)
	    | ((b & (1 << 4)) ? (1 << 3) : 0)
	    | ((b & (1 << 5)) ? (1 << 2) : 0)
	    | ((b & (1 << 6)) ? (1 << 1) : 0)
	    | ((b & (1 << 7)) ? (1 << 0) : 0);
}

/* rev_u8_8shifts returns b with the bit-order reversed. */
uint8_t rev_u8_8shifts(uint8_t b)
{
	/* this could be a fine choice for many applications */
	/* this is written to be somewhat concise, and not too tricky */
	/* (probably the most obvious non-loop way to think about it) */
	/* depending on the platform, this may be fastest */

	/* For each bit, */
	/* shift byte so the bit is in the new position */
	/* the lower four bits will have to shift some amount up and */
	/* the upper four bits will have to shift some amount down, */
	/* then AND the shifted byte with a byte having a 1 only in the */
	/* new position */

	/* whether we AND first or SHIFT first, it compiles the same, thus */
	/* it's more a question of which is easier to read or think about */
#ifndef REV_U8_SHIFT_FIRST
#define REV_U8_SHIFT_FIRST 0
#endif
#if (REV_U8_SHIFT_FIRST)
	return ((b << 7) & (1 << 7))
	    | ((b << 5) & (1 << 6))
	    | ((b << 3) & (1 << 5))
	    | ((b << 1) & (1 << 4))
	    | ((b >> 1) & (1 << 3))
	    | ((b >> 3) & (1 << 2))
	    | ((b >> 5) & (1 << 1))
	    | ((b >> 7) & (1 << 0));
#else
	return ((b & (1 << 0)) << 7)
	    | ((b & (1 << 1)) << 5)
	    | ((b & (1 << 2)) << 3)
	    | ((b & (1 << 3)) << 1)
	    | ((b & (1 << 4)) >> 1)
	    | ((b & (1 << 5)) >> 3)
	    | ((b & (1 << 6)) >> 5)
	    | ((b & (1 << 7)) >> 7);
#endif
}

/* rev_u8_3swaps returns b with the bit-order reversed. */
uint8_t rev_u8_3swaps(uint8_t b)
{
	/* while "too clever", this produces the least code and likley fast */
	/* thus, is probably the one to choose for firmware, as it is small */
	/* and this may be fastest (especially with -Os) */
	/* (would seem less tricky using non-standard "0b" style constants) */

	/* b = 87654321 */
	/* first, swap 2 halves */
	b = (((b & 0xF0) >> 4) | ((b & 0x0F) << 4));
	/* 0xF0:11110000,    0x0F:00001111 */
	/* b = (87650000 >> 4) | (00004321 << 4) */
	/* b =  00008765       |  43210000 */
	/* b =  43218765 */

	/* next, swap 4 pairs */
	b = (((b & 0xCC) >> 2) | ((b & 0x33) << 2));
	/* 0xCC:11001100,    0x33:00110011 */
	/* b = (43008700 >> 2) | (00210065 << 2) */
	/* b =  00430087       |  21006500 */
	/* b =  21436587 */

	/* last, swap 8 bits */
	b = (((b & 0xAA) >> 1) | ((b & 0x55) << 1));
	/* 0xAA:10101010,    0x55:01010101 */
	/* b = (20406080 >> 1) | (01030507 << 1) */
	/* b =  02040608       |  10305070 */
	/* b =  12345678 */

	return b;
}

uint8_t rev_u8_table(uint8_t b)
{
	/* for comparison, what about just a big look-up table? */
	/* bad for the cache and for code-size, but maybe fast? */
	/* (spoilers: not actually fast either) */
	uint8_t table[256] = {
		0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
		0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
		0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
		0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
		0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
		0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
		0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
		0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
		0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
		0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
		0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
		0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
		0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
		0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
		0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
		0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
		0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
		0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
		0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
		0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
		0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
		0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
		0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
		0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
		0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
		0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
		0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
		0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
		0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
		0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
		0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
		0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
	};
	return table[b];
}

#ifndef TEST_EXHAUSTIVE
#define TEST_EXHAUSTIVE 0
#endif

#ifndef IS_TEST
#define IS_TEST (TEST_EXHAUSTIVE)
#endif

#if (IS_TEST)

#ifdef ARDUINO
#include <Arduino.h>
#include <HardwareSerial.h>
void *null_context(void)
{
	return NULL;
}

void *(*log_context)(void) = null_context;

void serial_print_u(void *ctx, uint64_t u)
{
	(void *)ctx;
	Serial.print(u);
}

void (*log_u)(void *context, uint64_t u) = serial_print_u;

void serial_print_s(void *ctx, const char *s)
{
	(void *)ctx;
	Serial.print(s);
}

void (*log_s)(void *context, const char *s) = serial_print_s;

void serial_println(void *ctx)
{
	(void *)ctx;
	Serial.println();
}

void (*log_eol)(void *context) = serial_println;
#else
#if __STDC_HOSTED__
#include <stdio.h>
#include <inttypes.h>
void *stdout_context(void)
{
	return stdout;
}

void *(*log_context)(void) = stdout_context;
void printf_u(void *ctx, uint64_t u)
{
	FILE *stream = (FILE *)ctx;
	fprintf(stream, "%" PRIu64, u);
}

void (*log_u)(void *context, uint64_t u) = printf_u;

void printf_s(void *ctx, const char *s)
{
	FILE *stream = (FILE *)ctx;
	fprintf(stream, "%s", s);
}

void (*log_s)(void *context, const char *s) = printf_s;

void printf_eol(void *ctx)
{
	FILE *stream = (FILE *)ctx;
	fprintf(stream, "\n");
}

void (*log_eol)(void *context) = printf_eol;
#else
/* Okay, on some other platform, you supply the pointers */
void *null_context(void)
{
	return NULL;
}

void *(*log_context)(void) = null_context;
void (*log_u)(void *context, uint64_t u) = NULL;
void (*log_s)(void *context, const char *s) = NULL;
void (*log_eol)(void *context) = NULL;
#endif
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>

char *byte_to_bitstr(char *buf, uint8_t b)
{
	for (size_t i = 0; i < 8; ++i) {
		buf[i] = (b & (1U << (7 - i))) ? '1' : '0';
	}
	buf[8] = '\0';
	return buf;
}

void println_byte_bits(char *label, uint8_t b)
{
	char buf[9];
	memset(buf, 0x00, 9);
	log_s(log_context(), "\t");
	log_s(log_context(), byte_to_bitstr(buf, b));
	log_s(log_context(), "\t(");
	log_u(log_context(), b);
	log_s(log_context(), ")\t");
	log_s(log_context(), label);
	log_eol(log_context());
}

unsigned check_equals_u8(int line, uint8_t a, uint8_t b)
{
	if (a == b) {
		return 0;
	}
	log_s(log_context(), "FAIL ");
	log_u(log_context(), line);
	log_s(log_context(), ": ");
	log_u(log_context(), a);
	log_s(log_context(), " != ");
	log_u(log_context(), b);
	log_eol(log_context());
	return 1;
}

#define Check_equals_u8(a, b) check_equals_u8(__LINE__, a, b)

unsigned check_exhaustive(void)
{
	unsigned failures = 0;
	for (uint16_t i = 0; i <= 0xFF; ++i) {
		log_eol(log_context());
		uint8_t b = i & 0xFF;
		println_byte_bits("start", b);

		uint8_t r0 = rev_u8_loop(b);
		println_byte_bits("rev1", r0);
		uint8_t b0 = rev_u8_ugly(r0);
		println_byte_bits("back", b0);
		failures += Check_equals_u8(b0, b);

		uint8_t r1 = rev_u8_ugly(b);
		println_byte_bits("rev1", r1);
		uint8_t b1 = rev_u8_ugly(r1);
		println_byte_bits("back", b1);
		failures += Check_equals_u8(b1, b);

		uint8_t r2 = rev_u8_3swaps(b);
		println_byte_bits("rev2", r2);
		uint8_t b2 = rev_u8_3swaps(r2);
		println_byte_bits("back", b2);
		failures += Check_equals_u8(b2, b);

		uint8_t r3 = rev_u8_8shifts(b);
		println_byte_bits("rev3", r3);
		uint8_t b3 = rev_u8_8shifts(r3);
		println_byte_bits("back", b3);
		failures += Check_equals_u8(b3, b);

		uint8_t r4 = rev_u8_table(b);
		println_byte_bits("rev4", r4);
		uint8_t b4 = rev_u8_table(r4);
		println_byte_bits("back", b4);
		failures += Check_equals_u8(b4, b);

		failures += Check_equals_u8(r0, r1);
		failures += Check_equals_u8(r1, r2);
		failures += Check_equals_u8(r2, r3);
		failures += Check_equals_u8(r3, r4);
	}
	return failures;
}

clock_t system_start = 0;

clock_t count_clocks(uint8_t (*rev_func)(uint8_t), size_t cycles)
{
	size_t a = 0;
	clock_t start = (clock() - system_start);
	for (size_t i = 0; i < cycles; ++i) {
		uint8_t b = i & 0xFF;
		a += rev_func(b);
	}
	clock_t end = (clock() - system_start);

	if (Check_equals_u8(a > 0 ? 1 : 0, 1)) {
		log_s(log_context(), "cycles = ");
		log_u(log_context(), cycles);
		log_s(log_context(), ", unexpected 'a' of 0");
		log_eol(log_context());
	}
	return (end - start);
}

void print_clocks(clock_t clocks, const char *label)
{
	log_u(log_context(), clocks);
	log_s(log_context(), "\t: ");
	log_s(log_context(), label);
	log_eol(log_context());
}

int main_loop(size_t outer_cycles, size_t inner_cycles)
{
	unsigned failures = 0;

	system_start = clock();

	failures += Check_equals_u8(0xF0, rev_u8_loop(0x0F));
	failures += Check_equals_u8(0xF0, rev_u8_ugly(0x0F));
	failures += Check_equals_u8(0xF0, rev_u8_3swaps(0x0F));
	failures += Check_equals_u8(0xF0, rev_u8_8shifts(0x0F));
	failures += Check_equals_u8(0xF0, rev_u8_table(0x0F));

	failures += Check_equals_u8(0xCC, rev_u8_loop(0x33));
	failures += Check_equals_u8(0xCC, rev_u8_ugly(0x33));
	failures += Check_equals_u8(0xCC, rev_u8_3swaps(0x33));
	failures += Check_equals_u8(0xCC, rev_u8_8shifts(0x33));
	failures += Check_equals_u8(0xCC, rev_u8_table(0x33));

	failures += Check_equals_u8(0xAA, rev_u8_loop(0x55));
	failures += Check_equals_u8(0xAA, rev_u8_ugly(0x55));
	failures += Check_equals_u8(0xAA, rev_u8_3swaps(0x55));
	failures += Check_equals_u8(0xAA, rev_u8_8shifts(0x55));
	failures += Check_equals_u8(0xAA, rev_u8_table(0x55));

	if (TEST_EXHAUSTIVE) {
		failures += check_exhaustive();
	}

	clock_t clocks_loop = 0;
	clock_t clocks_ugly = 0;
	clock_t clocks_straight = 0;
	clock_t clocks_3swaps = 0;
	clock_t clocks_table = 0;

	for (size_t i = 0; i < outer_cycles; ++i) {
		clocks_loop += count_clocks(rev_u8_loop, inner_cycles);
		clocks_ugly += count_clocks(rev_u8_ugly, inner_cycles);
		clocks_straight += count_clocks(rev_u8_8shifts, inner_cycles);
		clocks_3swaps += count_clocks(rev_u8_3swaps, inner_cycles);
		clocks_table += count_clocks(rev_u8_table, inner_cycles);
	}

	print_clocks(clocks_loop, "rev_u8_loop");
	print_clocks(clocks_ugly, "rev_u8_ugly");
	print_clocks(clocks_straight, "rev_u8_8shifts");
	print_clocks(clocks_3swaps, "rev_u8_3swaps");
	print_clocks(clocks_table, "rev_u8_table");

	return failures ? 1 : 0;
}

#ifdef ARDUINO
void setup(void)
{
	Serial.begin(9600);
	delay(50);
}

void loop(void)
{
	size_t outer_cycles = 100;
	size_t inner_cycles = 100;
	main_loop(outer_cycles, inner_cycles);
	delay(1000);
	Serial.println();
}
#else
int main(void)
{
	size_t outer_cycles = 1000;
	size_t inner_cycles = 1000;
	return main_loop(outer_cycles, inner_cycles);
}
#endif
#endif
