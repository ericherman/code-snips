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

	for (size_t i = 0; i < octal_len; i += 8) {
		size_t used = 0;
		uint32_t b3 = 0;
		for (size_t j = 0; j < 8; ++j) {
			uint32_t ob = 0;
			size_t k = (i + j);
			if (k < octal_len) {
				char oc = octal[k];
				if (!oc) {
					octal_len = k;
				} else {
					assert(oc >= '0' && oc <= '7');
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
