/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* octal-bytes.c : encode bytes in octal (simple, slow) */
/* Copyright (C) 2023 Eric Herman <eric@freesa.org> */

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#ifndef OCTAL_BIT
#define OCTAL_BIT 3
#endif

char *octal_from_bytes(char *octal, size_t octal_size,
		       const uint8_t *bytes, size_t bytes_len)
{
	assert(octal);
	assert(octal_size);
	assert(bytes);

	size_t pos = 0;
	size_t bits_len = (bytes_len * CHAR_BIT);
	for (size_t i = 0; i < bits_len;) {
		uint8_t oct = 0;
		for (size_t j = 0; j < OCTAL_BIT && i < bits_len; ++j, ++i) {
			size_t byte = i / CHAR_BIT;
			size_t offset = i % CHAR_BIT;
			uint8_t b = bytes[byte];
			int set = (b & (1U << offset)) ? 1 : 0;
			oct = oct | (set << j);
		}
		char coct = '0' + oct;

		if (pos >= octal_size) {
			memset(octal, 0x00, octal_size);
			return NULL;
		}
		octal[pos++] = coct;
	}

	octal[pos] = '\0';
	return octal;
}

uint8_t *octal_to_bytes(uint8_t *bytes, size_t bytes_size,
			const char *octal, size_t octal_len)
{
	assert(bytes);
	assert(bytes_size);
	assert(octal);

	size_t pos = 0;
	size_t bits_len = octal_len * OCTAL_BIT;
	for (size_t i = 0; i < bits_len;) {
		uint8_t byte = 0x00;
		for (size_t j = 0; j < CHAR_BIT && i < bits_len; ++j, ++i) {
			size_t cidx = i / OCTAL_BIT;
			size_t offset = i % OCTAL_BIT;
			char c = octal[cidx];
			assert(c >= '0' && c <= '7');
			uint8_t oc = c - '0';
			int set = (oc >> offset) & 0x01;
			byte = byte | (set << j);
		}

		if (pos >= bytes_size) {
			memset(bytes, 0x00, bytes_size);
			return NULL;
		}
		bytes[pos++] = byte;
	}
	return bytes;
}

/* ***************************************************************** */
#include <stdio.h>
#include <stdlib.h>
static size_t octal_space_needed(size_t bytes_len)
{
	return ((bytes_len * CHAR_BIT) / OCTAL_BIT) +
	    (((bytes_len * CHAR_BIT) % OCTAL_BIT) ? 1 : 0) + 1;
}

int main(int argc, char **argv)
{

	const char *str = argc > 1 ? argv[1] : "ILOVEUNIX";
	const uint8_t *bytes = (const uint8_t *)str;
	size_t bytes_len = strlen(str);

	uint8_t out_size = bytes_len + 1;
	uint8_t *out = malloc(out_size);
	memset(out, 0x00, out_size);

	size_t octal_size = octal_space_needed(bytes_len);
	char *octal = malloc(octal_size);
	memset(octal, 0x00, octal_size);

	octal_from_bytes(octal, octal_size, bytes, bytes_len);
	octal_to_bytes(out, out_size, octal, strlen(octal));

	printf(" in hex: ");
	for (size_t i = 0; i < bytes_len; ++i) {
		printf("%x", bytes[i]);
	}
	printf("\n");

	printf("  octal: %s \n", octal);

	printf("out hex: ");
	for (size_t i = 0; i < bytes_len; ++i) {
		printf("%x", out[i]);
	}
	printf("\n");

	free(octal);
	free(out);
	return 0;
}
