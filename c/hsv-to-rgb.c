/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* hsv-to-rgb.c: playing with color conversion */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */
/*
   cc -o hsv-to-rgb \
     hsv-to-rgb.c -lm && \
   ./hsv-to-rgb
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct rgb24_s {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

// values are from 0.0 to 1.0
struct rgb_s {
	double red;
	double green;
	double blue;
};

// hue is 0.0 to 360.0
// saturation and value are 0.0 to 0.1
struct hsv_s {
	double hue;
	double sat;
	double val;
};

int invalid_hsv_s(struct hsv_s hsv)
{
	if (!(hsv.hue >= 0.0 && hsv.hue <= 360.0)) {
		return 1;
	}
	if (!(hsv.sat >= 0 && hsv.sat <= 1.0)) {
		return 1;
	}
	if (!(hsv.val >= 0 && hsv.val <= 1.0)) {
		return 1;
	}
	return 0;
}

void rgb24_from_rgb(struct rgb24_s *out, struct rgb_s in)
{
	out->red = 255 * in.red;
	out->green = 255 * in.green;
	out->blue = 255 * in.blue;
}

// https://dystopiancode.blogspot.com/2012/06/hsv-rgb-conversion-algorithms-in-c.html
// https://en.wikipedia.org/wiki/HSL_and_HSV
int rgb_from_hsv(struct rgb_s *rgb, struct hsv_s hsv)
{
	if (!rgb || invalid_hsv_s(hsv)) {
		return 1;
	}

	double hue = hsv.hue == 360.0 ? 0.0 : hsv.hue;
	double chroma = hsv.val * hsv.sat;
	double offset = chroma * (1.0 - fabs(fmod(hue / 60.0, 2) - 1.0));
	double smallm = hsv.val - chroma;

	if (hue >= 0.0 && hue < 60.0) {
		rgb->red = chroma + smallm;
		rgb->green = offset + smallm;
		rgb->blue = smallm;
	} else if (hue >= 60.0 && hue < 120.0) {
		rgb->red = offset + smallm;
		rgb->green = chroma + smallm;
		rgb->blue = smallm;
	} else if (hue >= 120.0 && hue < 180.0) {
		rgb->red = smallm;
		rgb->green = chroma + smallm;
		rgb->blue = offset + smallm;
	} else if (hue >= 180.0 && hue < 240.0) {
		rgb->red = smallm;
		rgb->green = offset + smallm;
		rgb->blue = chroma + smallm;
	} else if (hue >= 240.0 && hue < 300.0) {
		rgb->red = offset + smallm;
		rgb->green = smallm;
		rgb->blue = chroma + smallm;
	} else if (hue >= 300.0 && hue < 360.0) {
		rgb->red = chroma + smallm;
		rgb->green = smallm;
		rgb->blue = offset + smallm;
	} else {
		rgb->red = smallm;
		rgb->green = smallm;
		rgb->blue = smallm;
	}

	return 0;
}

void bright_palette(struct rgb24_s *palette, size_t len)
{
	struct rgb_s tmp;
	for (size_t i = 0; i < len; ++i) {
		double hue = 360.0 - ((360.0 / len) * i);
		struct hsv_s hsv;
		hsv.hue = hue;
		hsv.sat = 1.0;
		hsv.val = 1.0;
		rgb_from_hsv(&tmp, hsv);
		rgb24_from_rgb(palette + i, tmp);
	}
}

int main(int argc, const char **argv)
{
	int steps = argc > 1 ? atoi(argv[1]) : 0;
	size_t len = (steps < 1) ? 15 : (unsigned)steps;
	struct rgb24_s palette[len];

	bright_palette(palette, len);

	for (size_t i = 0; i < len; ++i) {
		struct rgb24_s *c = palette + i;
		printf("{ 0x%02x, 0x%02x, 0x%02x }\n",	//
		       c->red, c->green, c->blue);
	}

	return 0;
}
