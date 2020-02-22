/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* coord-plane-iteration-1.c: playing with mandlebrot and such */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

/*
   cc coord-plane-iteration-1.c \
	-o ./coord-plane-iteration-1 \
	&& ./coord-plane-iteration-1
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct iterxy_s {
	/* pixel location */
	unsigned px;
	unsigned py;

	/* coordinate location */
	double cx;
	double cy;

	/* calculated next location */
	double zx;
	double zy;

	unsigned iterations;

	unsigned escaped;
};

typedef void (*pfunc_f)(struct iterxy_s * p);

void ordinary_square(struct iterxy_s *p)
{
	if (p->escaped) {
		return;
	}
	double escape_radius = 2;
	if ((fabs(p->zy) + fabs(p->zx)) > escape_radius) {
		p->escaped = p->iterations;
	} else {
		p->zy = (p->zy == 0) ? p->cy : (p->zy * p->zy);
		p->zx = (p->zx == 0) ? p->cx : (p->zx * p->zx);
		++(p->iterations);
	}
}

/* Z[n+1] = (Z[n])^2 + Orig */
void mandlebrot(struct iterxy_s *p)
{
	if (p->escaped) {
		return;
	}
	double escape_radius = 2;
	if ((fabs(p->zy) + fabs(p->zx)) > escape_radius) {
		p->escaped = p->iterations;
	} else {
		/* first, square the complex */
		/* the y is understood to contain an i, the sqrt(-1) */
		/* generate and combine together the four combos */
		double xx = p->zx * p->zx;	/* no i */
		double yx = p->zy * p->zx;	/* has i */
		double xy = p->zx * p->zy;	/* has i */
		double yy = p->zy * p->zy * -1;	/* loses the i */

		double result_x = xx + yy;	/* terms do not contain i */
		double result_y = yx + xy;	/* terms contain an i */

		/* then add the original C to the Z[n]^2 result */
		p->zx = result_x + p->cx;
		p->zy = result_y + p->cy;

		++(p->iterations);
	}
}

/* Z[n+1] = collapse_to_y2_to_y((Z[n])^2) + Orig */
void square_binomial_collapse_y2_and_add_orig(struct iterxy_s *p)
{
	if (p->escaped) {
		return;
	}
	double escape_radius = 2;
	if ((fabs(p->zy) + fabs(p->zx)) > escape_radius) {
		p->escaped = p->iterations;
	} else {
		/* z[n+1] = z[n]^2 + B */

		/* squaring a binomial == adding together four combos */
		double xx = p->zx * p->zx;
		double yx = p->zy * p->zx;
		double xy = p->zx * p->zy;
		double yy = p->zy * p->zy;
		double binomial_x = xx;	/* no y terms */
		double collapse_y_and_y2_terms = yx + xy + yy;

		/* now add the original C */
		p->zx = binomial_x + p->cx;
		p->zy = collapse_y_and_y2_terms + p->cy;
		++(p->iterations);
	}
}

/* Z[n+1] = ignore_y2((Z[n])^2) + Orig */
void square_binomial_ignore_y2_and_add_orig(struct iterxy_s *p)
{
	if (p->escaped) {
		return;
	}
	double escape_radius = 2;
	if ((fabs(p->zy) + fabs(p->zx)) > escape_radius) {
		p->escaped = p->iterations;
	} else {
		/* z[n+1] = z[n]^2 + B */

		/* squaring a binomial == adding together four combos */
		double xx = p->zx * p->zx;
		double yx = p->zy * p->zx;
		double xy = p->zx * p->zy;
		double yy = p->zy * p->zy;

		/* now add the original C */
		p->zx = xx + p->cx;
		p->zy = xy + yx + p->cy;
		++(p->iterations);
	}
}

void not_a_circle(struct iterxy_s *p)
{
	if (p->escaped) {
		return;
	}
	double escape_radius = 2;
	if ((fabs(p->zy) + fabs(p->zx)) > escape_radius) {
		p->escaped = p->iterations;
	} else {
		if (p->iterations == 0) {
			p->zy = p->cy;
			p->zx = p->cx;
		} else {
			double xx = p->zx * p->zx;
			double yy = p->zy * p->zy;
			p->zy = yy + (0.5 * p->zx);
			p->zx = xx + (0.5 * p->zy);
		}
		++(p->iterations);
	}
}

struct coordinate_plane_s {
	unsigned screen_width;
	unsigned screen_height;
	double cx_min;
	double cx_max;
	double coord_width;
	double cy_min;
	double cy_max;
	double coord_height;
	double point_width;
	double point_height;

	struct iterxy_s *points;
	size_t len;
};

static inline double _dmax(double a, double b)
{
	return a > b ? a : b;
}

struct coordinate_plane_s *new_coordinate_plane(unsigned screen_width,
						unsigned screen_height,
						double cx_min, double cx_max)
{
	size_t size = sizeof(struct coordinate_plane_s);
	struct coordinate_plane_s *plane = malloc(size);
	if (!plane) {
		fprintf(stderr, "could not allocate %zu bytes?\n", size);
		exit(EXIT_FAILURE);
	}

	plane->cx_min = cx_min;
	plane->cx_max = cx_max;
	plane->screen_width = screen_width;
	plane->screen_height = screen_height;
	plane->coord_width = (plane->cx_max - plane->cx_min);
	plane->cy_min =
	    _dmax(cx_max,
		  ((plane->coord_width) *
		   ((1.0 * screen_height) / screen_width)) / 2);
	plane->cy_max = -(plane->cy_min);
	plane->coord_height = (plane->cy_max - plane->cy_min);
	plane->point_width = plane->coord_width / screen_width;
	plane->point_height = plane->coord_height / screen_height;
	plane->points = NULL;
	plane->len = plane->screen_width * plane->screen_height;

	size = plane->len * sizeof(struct iterxy_s);
	plane->points = malloc(size);
	if (!plane->points) {
		fprintf(stderr, "could not allocate %zu bytes?\n", size);
		exit(EXIT_FAILURE);
	}
	for (size_t py = 0; py < plane->screen_height; ++py) {
		for (size_t px = 0; px < plane->screen_width; ++px) {
			size_t i = (py * plane->screen_width) + px;
			struct iterxy_s *p = plane->points + i;

			/* screen location: */
			p->py = py;
			p->px = px;

			/* location on the co-ordinate plane */
			p->cy = plane->cy_min + p->py * plane->point_height;
			if (fabs(p->cy) < (plane->point_height / 2)) {
				/* near enought to zero to call it zero */
				p->cy = 0.0;
			}

			p->cx = plane->cx_min + p->px * plane->point_width;
			if (fabs(p->cx) < (plane->point_width / 2)) {
				/* near enought to zero to call it zero */
				p->cx = 0.0;
			}

			p->zy = 0;
			p->zx = 0;
			p->iterations = 0;
			p->escaped = 0;
		}
	}
	return plane;
}

void delete_coordinate_plane(struct coordinate_plane_s *plane)
{
	if (plane) {
		free(plane->points);
	}
	free(plane);
}

void iterate_plane(struct coordinate_plane_s *plane, pfunc_f pfunc)
{
	for (size_t j = 0; j < plane->len; ++j) {
		struct iterxy_s *p = plane->points + j;
		pfunc(p);
	}
}

void print_coordinate_plane_ascii(struct coordinate_plane_s *plane)
{
	printf("\n");
	/* clear */
	printf("\033[H\033[J");

	for (size_t py = 0; py < plane->screen_height; ++py) {
		for (size_t px = 0; px < plane->screen_width; ++px) {
			size_t i = (py * plane->screen_width) + px;
			struct iterxy_s *p = plane->points + i;
			char c;
			if (p->escaped == 0) {
				c = ' ';
			} else if (p->escaped < 10) {
				c = '0' + p->escaped;
			} else if (p->escaped >= 10 && p->escaped < 36) {
				c = 'A' + p->escaped - 10;
			} else if (p->escaped >= 36 && p->escaped < (36 + 26)) {
				c = 'a' + p->escaped - 36;
			} else {
				c = '*';
			}
			printf("%c", c);
		}
		printf("\n");
	}
}

struct named_pfunc_s {
	pfunc_f pfunc;
	const char *name;
};

int main(int argc, const char **argv)
{
	struct named_pfunc_s nf[] = {
		{ mandlebrot, "mandlebrot" },
		{ ordinary_square, "ordinary_square" },
		{ not_a_circle, "not_a_circle" },
		{ square_binomial_collapse_y2_and_add_orig,
		 "square_binomial_collapse_y2_and_add_orig," },
		{ square_binomial_ignore_y2_and_add_orig,
		 "square_binomial_ignore_y2_and_add_orig," }
	};

	// int screen_x = argc > 1 ? atoi(argv[1]) : 800;
	// int screen_y = argc > 2 ? atoi(argv[2]) : (screen_x * 3) / 4;
	int screen_x = argc > 1 ? atoi(argv[1]) : 80;
	int screen_y = argc > 2 ? atoi(argv[2]) : 24;
	int func_idx = argc > 3 ? atoi(argv[3]) : 0;

	if (screen_x <= 0 || screen_y <= 0) {
		fprintf(stderr, "screen_x = %d\nscreen_y = %d\n", screen_x,
			screen_y);
		return 1;
	}

	/* define the range of the X dimension */
	double cx_min = -2.5;
	double cx_max = 1.5;
	struct coordinate_plane_s *plane =
	    new_coordinate_plane(screen_x, screen_y, cx_min, cx_max);

	if (func_idx < 0 || func_idx > 4) {
		func_idx = 0;
	}

	for (unsigned long c = 0, i = 0; c != 'q'; ++i) {
		iterate_plane(plane, nf[func_idx].pfunc);
		print_coordinate_plane_ascii(plane);
		printf("%s %lu. <enter> to continue or 'q<enter>' to quit: ",
		       nf[func_idx].name, i);
		c = getchar();
	}

	delete_coordinate_plane(plane);

	return 0;
}
