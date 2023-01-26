/* SPDX-License-Identifier: GPL-3.0-or-later */
/* cga.c : RGBI bit fiddling to CGA 16-color palette */
/* Copyright (C) 2023 Eric Herman <eric@freesa.org> */
/* https://en.wikipedia.org/wiki/Color_Graphics_Adapter */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Returns an 8-bit value, but only lower 4 bits are used.
   Parameters are the Red, Green, Blue, and "Intensity" bits,
   corresponding to ~5v or ~0v TTL levels on CGA pins 3, 4, 5, and 6 */
uint8_t cga_rgbi_bits_to_rgbi4(bool r, bool g, bool b, bool i)
{
	uint8_t rbgi4 = 0x00	/* only 4 bits are actually used */
	    | (i ? (1 << 3) : 0)
	    | (r ? (1 << 2) : 0)
	    | (g ? (1 << 1) : 0)
	    | (b ? (1 << 0) : 0);

	return rbgi4;
}

bool cga_rgbi4_to_intensity(uint8_t rbgi4)
{
	assert((0x0F & rbgi4) == rbgi4);
	return (rbgi4 & 8);
}

bool cga_rgbi4_to_red(uint8_t rbgi4)
{
	assert((0x0F & rbgi4) == rbgi4);
	return (rbgi4 & 4);
}

bool cga_rgbi4_to_green(uint8_t rbgi4)
{
	assert((0x0F & rbgi4) == rbgi4);
	return (rbgi4 & 2);
}

bool cga_rgbi4_to_blue(uint8_t rbgi4)
{
	assert((0x0F & rbgi4) == rbgi4);
	return (rbgi4 & 1);
}

/* Returns a 32-bit value, but only the lower 24 bits are used.
   Some CGA systems adjust "gross" dark yellow to be "more pleasing" brown.
   CGA 0x6	dark yellow	0xAAAA00	this may be replaced by brown
   CGA 0x6	brown		0xAA5500	alternative for dark yellow */
uint32_t cga_rgbi4_to_rgb24(uint8_t rbgi4, bool dark_yellow_to_brown)
{
	assert((0x0F & rbgi4) == rbgi4);

	bool i = cga_rgbi4_to_intensity(rbgi4);
	bool r = cga_rgbi4_to_red(rbgi4);
	bool g = cga_rgbi4_to_green(rbgi4);
	bool b = cga_rgbi4_to_blue(rbgi4);

	uint32_t set = i ? 0xFF : 0xAA;
	uint32_t off = i ? 0x55 : 0x00;

	uint32_t rgb24 = 0x00000000	/* only bottom 24 bits are used */
	    | (r ? set : off) << (2 * 8)
	    | (g ? set : off) << (1 * 8)
	    | (b ? set : off) << (0 * 8);

	if (dark_yellow_to_brown && rgb24 == 0xAAAA00) {
		rgb24 = 0xAA5500;
	}

	return rgb24;
}

/* Contents of r, g, and b will be set to a value from 0.0 to 1.0;
   possible values are 0.0, 1/3, 2/3, 1.0. */
void cga_rgb24_to_strength(uint32_t rgb24, double *r, double *g, double *b)
{
	assert((0x00FFFFFF & rgb24) == rgb24);

	uint8_t r8 = (0x000000FF & (rgb24 >> (2 * 8)));
	uint8_t g8 = (0x000000FF & (rgb24 >> (1 * 8)));
	uint8_t b8 = (0x000000FF & (rgb24 >> (0 * 8)));

	assert((r8 & 0x0F) == (r8 >> 4));
	assert((g8 & 0x0F) == (g8 >> 4));
	assert((b8 & 0x0F) == (b8 >> 4));

	uint8_t r4 = (0x000000F & r8);
	uint8_t g4 = (0x000000F & g8);
	uint8_t b4 = (0x000000F & b8);

	*r = r4 / 15.0;
	*g = g4 / 15.0;
	*b = b4 / 15.0;
}

/* Common English names associated with CGA color palette (not canonical).
   Values other than CGA values will return an empty string. */
const char *cga_name24(uint32_t rgb24)
{
	switch (rgb24) {
	case 0x000000:
		return "black";
	case 0x0000AA:
		return "blue";
	case 0x00AA00:
		return "green";
	case 0x00AAAA:
		return "cyan";
	case 0xAA0000:
		return "red";
	case 0xAA00AA:
		return "magenta";
	case 0xAAAA00:
		return "dark yellow";
	case 0xAA5500:
		return "brown";
	case 0xAAAAAA:
		return "light gray";
	case 0x555555:
		return "dark gray";
	case 0x5555FF:
		return "light blue";
	case 0x55FF55:
		return "light green";
	case 0x55FFFF:
		return "light cyan";
	case 0xFF5555:
		return "light red";
	case 0xFF55FF:
		return "light magenta";
	case 0xFFFF55:
		return "yellow";
	case 0xFFFFFF:
		return "white";
	default:
		return "";
	}
}

/***********************************************************\
|* Demonstration                                           *|
\***********************************************************/
#include <stdio.h>
static void cga_check_round_trip(size_t rgbi4)
{
	assert((0x0F & rgbi4) == rgbi4);
	bool i = cga_rgbi4_to_intensity(rgbi4);
	bool r = cga_rgbi4_to_red(rgbi4);
	bool g = cga_rgbi4_to_green(rgbi4);
	bool b = cga_rgbi4_to_blue(rgbi4);

	uint8_t rgbi4_back = cga_rgbi_bits_to_rgbi4(r, g, b, i);
	assert(rgbi4 == rgbi4_back);

	(void)i;
	(void)r;
	(void)g;
	(void)b;
	(void)rgbi4_back;
}

int main(void)
{
	printf("num irgb   24-bit    red    green  blue\n");
	const char *fmt =
	    "%2zu%c %zu%zu%zu%zu  #%06X    %.2f   %.2f   %.2f    %s\n";

	const size_t rgbi_palette_len = 16;
	for (size_t rgbi = 0; rgbi < rgbi_palette_len; ++rgbi) {
		cga_check_round_trip(rgbi);

		bool dark_yellow_to_brown = 0;
		uint32_t rgb24 = cga_rgbi4_to_rgb24(rgbi, dark_yellow_to_brown);

		double sr = 0, sg = 0, sb = 0;
		cga_rgb24_to_strength(rgb24, &sr, &sg, &sb);

		const char *name = cga_name24(rgb24);
		printf(fmt, rgbi, ' ', (rgbi >> 3) & 1, (rgbi >> 2) & 1,
		       (rgbi >> 1) & 1, (rgbi >> 0) & 1, rgb24, sr, sg, sb,
		       name);

		if (rgbi == 6) {
			dark_yellow_to_brown = 1;
			rgb24 = cga_rgbi4_to_rgb24(rgbi, dark_yellow_to_brown);
			cga_rgb24_to_strength(rgb24, &sr, &sg, &sb);
			name = cga_name24(rgb24);
			printf(fmt, rgbi, '\'', (rgbi >> 3) & 1,
			       (rgbi >> 2) & 1, (rgbi >> 1) & 1,
			       (rgbi >> 0) & 1, rgb24, sr, sg, sb, name);
		}
	}
	return 0;
}

/*
num irgb   24-bit    red    green  blue
 0  0000  #000000    0.00   0.00   0.00    black
 1  0001  #0000AA    0.00   0.00   0.67    blue
 2  0010  #00AA00    0.00   0.67   0.00    green
 3  0011  #00AAAA    0.00   0.67   0.67    cyan
 4  0100  #AA0000    0.67   0.00   0.00    red
 5  0101  #AA00AA    0.67   0.00   0.67    magenta
 6  0110  #AAAA00    0.67   0.67   0.00    dark yellow
 6' 0110  #AA5500    0.67   0.33   0.00    brown
 7  0111  #AAAAAA    0.67   0.67   0.67    light gray
 8  1000  #555555    0.33   0.33   0.33    dark gray
 9  1001  #5555FF    0.33   0.33   1.00    light blue
10  1010  #55FF55    0.33   1.00   0.33    light green
11  1011  #55FFFF    0.33   1.00   1.00    light cyan
12  1100  #FF5555    1.00   0.33   0.33    light red
13  1101  #FF55FF    1.00   0.33   1.00    light magenta
14  1110  #FFFF55    1.00   1.00   0.33    yellow
15  1111  #FFFFFF    1.00   1.00   1.00    white
*/
