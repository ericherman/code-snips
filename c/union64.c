/* union64.c
   Copyright (C) 2018 Eric Herman <eric@freesa.org>

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
#include <stdint.h>
#include <stdlib.h>

union union64 {
	uint64_t bits;

	uint8_t bytes[8];
	uint16_t ushorts[4];
	uint32_t uints[2];
	uint64_t ulong;

	int8_t tinys[8];
	int16_t shorts[4];
	int32_t ints[2];
	int64_t slong;

	/* Float32 floats[2]; */
	/* Float64 doubles[1]; */
};

uint8_t union64_bit_at(union union64 u64, uint8_t bit)
{
	if (bit > 63) {
		return 0;
	}
	return (u64.bits & (1ULL << bit)) ? 1 : 0;
}

uint8_t union64_bit_set(union union64 u64, uint8_t bit, uint8_t val)
{
	uint8_t previous;

	if (bit > 63) {
		return 0;
	}
	previous = (u64.bits & (1ULL << bit)) ? 1 : 0;
	if (val && !previous) {
		u64.bits += (1ULL << bit);
	} else if (!val && previous) {
		u64.bits -= (1ULL << bit);
	}
	return previous;
}

int main(int argc, char *argv[])
{
	uint64_t val;
	size_t i;
	union union64 u64;

	if (argc > 1) {
		val = (uint64_t)strtoll(argv[1], NULL, 10);
	} else {
		val = (1ULL << 2) + (1ULL << 60);
	}

	u64.ulong = val;

	printf("sizeof u64 is %u bytes\n", sizeof(u64));

	printf("u64.ulong = %llu\n", (unsigned long long)u64.ulong);

	for (i = 0; i < 2; ++i) {
		printf("u64.uints[%u] = %lu\n", (unsigned)i,
		       (unsigned long)u64.uints[i]);
	}

	for (i = 0; i < 4; ++i) {
		printf("u64.ushorts[%u] = %u\n", (unsigned)i,
		       (unsigned)u64.ushorts[i]);
	}
	for (i = 0; i < 8; ++i) {
		printf("u64.bytes[%u] = %u\n", (unsigned)i,
		       (unsigned)u64.bytes[i]);
	}

	for (i = 0; i < 64; ++i) {
		printf("union64_bit_at(u64, %u) = %u\n", (unsigned)i,
		       (unsigned)union64_bit_at(u64, i));
	}

	return 0;
}
