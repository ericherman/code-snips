/* SPDX-License-Identifier: GPL-3.0-or-later */
/* eherr.c easy hosted error logging */
/* Copyright (C) 2021 Eric Herman <eric@freesa.org> */

#include "eherr.h"

#include <stdio.h>
#include <stdlib.h>

void vlogerror(FILE *log, const char *file, int line, const char *func,
	       const char *format, va_list ap)
{
	/* avoid POSIX undefined stdout+stderr */
	fflush(stdout);
	/* https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_05_01 */

	fprintf(log, "%s:%d %s(): ", file, line, func);
	vfprintf(log, format, ap);
	fprintf(log, "\n");
	fflush(log);
}

void logerror(FILE *log, const char *file, int line, const char *func,
	      const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vlogerror(log, file, line, func, format, ap);
	va_end(ap);
}

void die(FILE *log, const char *file, int line, const char *func,
	 const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vlogerror(log, file, line, func, format, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

void *calloc_or_die(FILE *log, const char *file, int line, const char *func,
		    size_t nmemb, size_t size)
{
	void *p = calloc(nmemb, size);
	if (!p) {
		die(log, file, line, func, "calloc(%zu, %zu) returned NULL",
		    nmemb, size);
	}
	return p;
}
