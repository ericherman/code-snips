/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* octal-bytes.c : encode bytes in octal (simple, slow) */
/* Copyright (C) 2023 Eric Herman <eric@freesa.org> */

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#ifndef OCTAL_BIT
#define OCTAL_BIT 3
#endif

size_t octal_encode_size_needed_for_string(size_t bytes_len)
{
	return ((bytes_len * CHAR_BIT) / OCTAL_BIT) +
	    (((bytes_len * CHAR_BIT) % OCTAL_BIT) ? 1 : 0) + 1;
}

size_t octal_decode_size_needed_for_bytes(size_t octal_str_len)
{
	return ((octal_str_len * OCTAL_BIT) / CHAR_BIT) +
	    (((octal_str_len * OCTAL_BIT) % CHAR_BIT) ? 1 : 0);
}

char *octal_encode(char *octal, size_t octal_size, size_t *written,
		   const uint8_t *bytes, size_t bytes_len)
{
	assert(octal);
	assert(octal_size);
	assert(written);
	assert(bytes);

	*written = 0;

	if (octal_size < octal_encode_size_needed_for_string(bytes_len)) {
		if (octal_size) {
			octal[0] = '\0';
		}
		return NULL;
	}

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
		octal[(*written)++] = coct;
	}

	octal[*written] = '\0';
	return octal;
}

uint8_t *octal_decode(uint8_t *bytes, size_t bytes_size, size_t *written,
		      const char *octal, size_t octal_len)
{
	assert(bytes);
	assert(bytes_size);
	assert(written);
	assert(octal);

	*written = 0;

	if (bytes_size < octal_decode_size_needed_for_bytes(octal_len)) {
		return NULL;
	}

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
		bytes[(*written)++] = byte;
	}
	return bytes;
}

/* ***************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{

	const char *str = argc > 1 ? argv[1] : "ILOVEUNIX";
	const uint8_t *bytes = (const uint8_t *)str;
	size_t bytes_len = strlen(str);

	uint8_t out_size = bytes_len + 1;
	uint8_t *out = malloc(out_size);
	memset(out, 0x00, out_size);

	size_t octal_size = octal_encode_size_needed_for_string(bytes_len);
	char *octal = malloc(octal_size);
	memset(octal, 0x00, octal_size);

	printf(" in hex: ");
	for (size_t i = 0; i < bytes_len; ++i) {
		printf("%x", bytes[i]);
	}
	printf(" (%zu bytes to read)\n", bytes_len);

	size_t written = 0;
	octal_encode(octal, octal_size, &written, bytes, bytes_len);
	printf("  octal: %s (%zu characters written)\n", octal, written);

	octal_decode(out, out_size, &written, octal, strlen(octal));
	printf("out hex: ");
	for (size_t i = 0; i < out_size && i < written; ++i) {
		printf("%x", out[i]);
	}
	printf(" (%zu bytes written)\n", written);

	free(octal);
	free(out);
	return 0;
}
