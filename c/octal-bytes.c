/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* octal-bytes.c : encode bytes in octal (simple, slow) */
/* Copyright (C) 2023 Eric Herman <eric@freesa.org> */

#ifdef ARDUINO
#include <Arduino.h>
#endif

#ifdef __cplusplus
#define Octal__begin_C_functions extern "C" {
#define Octal__end_C_functions }
#ifndef _Static_assert
#define _Static_assert static_assert
#endif
#else
#define Octal__begin_C_functions
#define Octal__end_C_functions
#endif

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

Octal__begin_C_functions
#undef Octal__begin_C_functions

/* * * * * * * * *\
 * API FUNCTIONS *
\* * * * * * * * */
char *octal_encode(char *octal, size_t octal_size, size_t *written,
		   const uint8_t *bytes, size_t bytes_len);

uint8_t *octal_decode(uint8_t *bytes, size_t bytes_size, size_t *written,
		      const char *octal, size_t octal_len);

size_t octal_find_first_nonoctal(const char *octal, size_t octal_size);

size_t octal_encode_size_needed_for_string(size_t bytes_len);

/* round_up should be set to 0 unless the original octal string did not come
 * from an 8bit binary source, otherwise the trailing bits should be ignored */
size_t octal_decode_size_needed_for_bytes(size_t octal_str_len, int round_up);

/* * * * * * * * *\
 * Sanity checks *
\* * * * * * * * */
#define STR(x) #x
#define CONCAT_NUM(msg,num) msg STR(num)
#define CHAR_BIT_SIZE_MSG CONCAT_NUM("CHAR_BIT == ", CHAR_BIT)
_Static_assert(CHAR_BIT == 8, "Expect 8, but " CHAR_BIT_SIZE_MSG "?!");

#ifndef OCTAL_BIT
#define OCTAL_BIT 3
#endif
#define OCTAL_BIT_SIZE_MSG CONCAT_NUM("OCTAL_BIT == ", OCTAL_BIT)
_Static_assert(OCTAL_BIT == 3, "Expect 3, but " OCTAL_BIT_SIZE_MSG "?!");

/* * * * * * * * *\
 * implementation *
\* * * * * * * * */
#include <assert.h>

size_t octal_encode_size_needed_for_string(size_t bytes_len)
{
	size_t needed = ((bytes_len * CHAR_BIT) / OCTAL_BIT);
	size_t round_up = (((bytes_len * CHAR_BIT) % OCTAL_BIT) ? 1 : 0);
	size_t null_terminator = 1;
	return needed + round_up + null_terminator;
}

/* if the octal string came from encoding bytes, there will not be a need
 * to round up */
size_t octal_decode_size_needed_for_bytes(size_t octal_str_len, int round_up)
{
	size_t needed = ((octal_str_len * OCTAL_BIT) / CHAR_BIT);
	if (round_up && ((octal_str_len * OCTAL_BIT) % CHAR_BIT)) {
		++needed;
	}
	return needed;
}

char *octal_encode(char *octal, size_t octal_size, size_t *written,
		   const uint8_t *bytes, size_t bytes_len)
{
	assert(octal);
	assert(octal_size);
	assert(written);
	assert(bytes);

	*written = 0;

	size_t needed = octal_encode_size_needed_for_string(bytes_len);
	if (octal_size < needed) {
		if (octal_size) {
			octal[0] = '\0';
		}
		return NULL;
	}

	for (size_t i = 0; i < bytes_len; i += 3) {
		uint32_t b24 = 0;
		size_t used = 0;
		for (size_t j = 0; j < 3; ++j) {
			size_t k = i + j;
			uint32_t bij = 0;
			if (k < bytes_len) {
				bij = bytes[k];
				++used;
			}
			b24 = (b24 << CHAR_BIT) | bij;
		}

		size_t jmax = used == 3 ? 8 : used == 2 ? 6 : used == 1 ? 3 : 0;
		for (size_t j = 0; j < jmax; ++j) {
			size_t offset = (21 - (j * 3));
			uint32_t downshifted = (b24 >> offset);
			uint8_t oj = (0x07 & downshifted);
			char coct = '0' + oj;
			octal[(*written)++] = coct;
		}
	}

	octal[*written] = '\0';
	return octal;
}

size_t octal_find_first_nonoctal(const char *octal, size_t octal_size)
{
	assert(octal);

	for (size_t i = 0; i < octal_size; ++i) {
		if (octal[i] < '0' || octal[i] > '7') {
			return i;
		}
	}

	return octal_size;
}

uint8_t *octal_decode(uint8_t *bytes, size_t bytes_size, size_t *written,
		      const char *octal, size_t octal_len)
{
	assert(bytes);
	assert(bytes_size);
	assert(written);
	assert(octal);

	*written = 0;

	size_t needed = octal_decode_size_needed_for_bytes(octal_len, 0);
	if (bytes_size < needed) {
		/* maybe there is enough space if we trim trailing non-octal
		 * characters, like newlines ? */
		octal_len = octal_find_first_nonoctal(octal, octal_len);
		needed = octal_decode_size_needed_for_bytes(octal_len, 0);
		if (bytes_size < needed) {
			return NULL;
		}
	}

	for (size_t i = 0; i < octal_len; i += 8) {
		size_t used = 0;
		uint32_t b3 = 0;
		for (size_t j = 0; j < 8; ++j) {
			uint32_t ob = 0;
			size_t k = (i + j);
			if (k < octal_len) {
				char oc = octal[k];
				if (oc < '0' || oc > '7') {
					octal_len = k;
				} else {
					ob = oc - '0';
					++used;
				}
			}
			b3 = (b3 << OCTAL_BIT) | ob;
		}

		size_t jmax = used == 8 ? 3 : used == 6 ? 2 : used == 3 ? 1 : 0;
		for (size_t j = 0; j < jmax; ++j) {
			size_t offset = j == 0 ? 16 : j == 1 ? 8 : 0;
			uint8_t byte = 0xFF & (b3 >> offset);
			bytes[(*written)++] = byte;
		}
	}
	return bytes;
}

/* ***************************************************************** */

/* * * * * * * * *\
 * Demonstration *
\* * * * * * * * */
#ifdef ARDUINO
#define print_s(s) Serial.print(s)
#define print_z(z) Serial.print(z)
#define print_u8_hex(b) Serial.print(b, HEX)
#define print_eol(void) Serial.println()
#else
#include <stdio.h>
#define print_s(s) printf("%s", s)
#define print_z(z) printf("%zu", z)
#define print_u8_hex(b) printf("%02x", (unsigned)b)
#define print_eol(void) printf("\n")
#endif

size_t demo_round_trip(const uint8_t *bytes, size_t bytes_len,
		       char *octal, size_t octal_size,
		       uint8_t *out, size_t out_size)
{

	print_s(" in hex: ");
	for (size_t i = 0; i < bytes_len; ++i) {
		print_u8_hex(bytes[i]);
	}
	print_s(" (");
	print_z(bytes_len);
	print_s(" bytes to read)");
	print_eol();

	size_t written = 0;
	octal_encode(octal, octal_size, &written, bytes, bytes_len);

	print_s("  octal: ");
	print_s(octal);
	print_s(" (");
	print_z(written);
	print_s(" chars written)");
	print_eol();

	octal_decode(out, out_size, &written, octal, octal_size);

	size_t differ = written != bytes_len;
	print_s("out hex: ");
	for (size_t i = 0; i < out_size && i < written; ++i) {
		differ += (out[i] == bytes[i] ? 0 : 1);
		print_u8_hex(out[i]);
	}
	print_s(" (");
	print_z(written);
	print_s(" bytes written)");
	print_eol();

	print_z(differ);
	print_s(differ ? " BYTES DIFFERENT" : " bytes different");
	print_eol();

	return differ;
}

#define Bytes_size	25
#define Out_size	Bytes_size
#define Octal_size	(1 + (size_t)(Bytes_size * (8.0)/(3.0)) + 1)
size_t loop_demo(size_t loop_count)
{
	print_eol();
	print_s("Begin loop #");
	print_z(loop_count);
	print_eol();

	uint8_t bytes[Bytes_size];
	uint8_t out[Out_size];
	char octal[Octal_size];

	size_t bytes_len = 1 + (loop_count % Bytes_size);
	for (size_t i = 0; i < bytes_len; ++i) {
		bytes[i] = loop_count + i;
	}

	return demo_round_trip(bytes, bytes_len,
			       octal, Octal_size, out, Out_size);
}

#ifdef ARDUINO

#include <HardwareSerial.h>

void setup(void)
{
	Serial.begin(9600);
	delay(50);

	Serial.println();
	Serial.println();

	Serial.println("Begin");

	Serial.println();
}

unsigned loop_count = 0;
void loop(void)
{
	size_t error = loop_demo(++loop_count);
	uint16_t delay_ms = error ? (30 * 1000) : (2 * 1000);
	delay(delay_ms);
}

#else

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

size_t round_trip_string(const char *str)
{
	const uint8_t *bytes = (const uint8_t *)str;
	size_t bytes_len = strlen(str);

	uint8_t out_size = bytes_len + 1;
	uint8_t *out = malloc(out_size);
	memset(out, 0x00, out_size);

	size_t octal_size = octal_encode_size_needed_for_string(bytes_len);
	char *octal = malloc(octal_size);
	memset(octal, 0x00, octal_size);

	size_t differ =
	    demo_round_trip(bytes, bytes_len, octal, octal_size, out, out_size);

	free(octal);
	free(out);

	return differ;
}

int main(int argc, char **argv)
{
	if (argc > 1) {
		return round_trip_string(argv[1]) ? EXIT_FAILURE : EXIT_SUCCESS;
	}

	/* if no input, do an infinite loop_demo */
	size_t loop_count = 0;
	while (1) {
		if (loop_demo(++loop_count)) {
			exit(EXIT_FAILURE);
		}
		sleep(1);
	}
}
#endif
Octal__end_C_functions
#undef Octal__end_C_functions
