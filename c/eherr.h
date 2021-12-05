/* SPDX-License-Identifier: GPL-3.0-or-later */
/* eherr.h easy hosted error logging */
/* Copyright (C) 2021 Eric Herman <eric@freesa.org> */

#ifndef EHERR_H
#define EHERR_H

#include <stdarg.h>
#include <stdio.h>

void vlogerror(FILE *log, const char *file, int line, const char *func,
	       const char *format, va_list ap);

void logerror(FILE *log, const char *file, int line, const char *func,
	      const char *format, ...);
#define Logerror(format, ...) \
	logerror(stderr, __FILE__, __LINE__, __func__, \
		format __VA_OPT__(,) __VA_ARGS__)

void die(FILE *log, const char *file, int line, const char *func,
	 const char *format, ...);

#define Die(format, ...) \
	die(stderr, __FILE__, __LINE__, __func__, \
	    format __VA_OPT__(,) __VA_ARGS__)

void *calloc_or_die(FILE *log, const char *file, int line, const char *func,
		    size_t nmemb, size_t size);

#define Calloc_or_die(nmemb, size) \
	calloc_or_die(stderr, __FILE__, __LINE__, __func__, nmemb, size)

#endif /* EHERR_H */
