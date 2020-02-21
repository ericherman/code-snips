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
	double escape_radius = 2;
	if ((fabs(p->zy) + fabs(p->zx)) > escape_radius) {
		p->escaped = p->iterations;
	} else {
		p->zy = (p->zy == 0) ? p->cy : (p->zy * p->zy);
		p->zx = (p->zx == 0) ? p->cx : (p->zx * p->zx);
		++(p->iterations);
	}
}

void mandlebrot(struct iterxy_s *p)
{
	double escape_radius = 2;
	if ((fabs(p->zy) + fabs(p->zx)) > escape_radius) {
		p->escaped = p->iterations;
	} else {
		double previous_zy = p->zy;
		p->zy = 2 * p->zx * p->zy + p->cy;
		p->zx = (p->zx * p->zx) - (previous_zy * previous_zy) + p->cx;
		++(p->iterations);
	}
}

void not_a_circle(struct iterxy_s *p)
{
	double escape_radius = 2;
	if ((fabs(p->zy) + fabs(p->zx)) > escape_radius) {
		p->escaped = p->iterations;
	} else {
		if ( p->iterations == 0) {
			p->zy = p->cy;
			p->zx = p->cx;
		} else {
			double previous_zy = p->zy;
			p->zy = (p->zy * p->zy) + (0.5 * p->zx);
			p->zx = (p->zx * p->zx) + (0.5 * previous_zy);
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
						unsigned screen_height)
{
	size_t size = sizeof(struct coordinate_plane_s);
	struct coordinate_plane_s *plane = malloc(size);
	if (!plane) {
		fprintf(stderr, "could not allocate %zu bytes?\n", size);
		exit(EXIT_FAILURE);
	}

	plane->cx_min = -2.5;
	plane->cx_max = 1.5;
	plane->screen_width = screen_width;
	plane->screen_height = screen_height;
	plane->coord_width = (plane->cx_max - plane->cx_min);
	plane->cy_min =
	    _dmax(1.5,
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

int main(int argc, const char **argv)
{
	pfunc_f pfunc[3] = { mandlebrot, ordinary_square, not_a_circle };

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

	struct coordinate_plane_s *plane =
	    new_coordinate_plane(screen_x, screen_y);

	if (func_idx < 0 || func_idx > 2) {
		func_idx = 0;
	}

	for (unsigned long c = 0, i = 0; c != 'q'; ++i) {
		iterate_plane(plane, pfunc[func_idx]);
		print_coordinate_plane_ascii(plane);
		printf("%lu. <enter> to continue or 'q<enter>' to quit: ", i);
		c = getchar();
	}

	delete_coordinate_plane(plane);

	return 0;
}
