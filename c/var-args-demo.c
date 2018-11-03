/* var-args-demo.c
   Copyright (C) 2016, 2018 Eric Herman <eric@freesa.org>

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
/*
 A demo of using va_list in this pretend c89 backport of snprintf

 $ gcc -std=c89 -Wall -Wextra -Wpedantic -Werror -ggdb -O0 \
      -o var-args-demo var-args-demo.c
 $ ./var-args-demo
 100% awesome!

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

int a_str(char *buf, size_t len, const char *str);
int a_int(char *buf, size_t len, int x);
int c89_fake_snprintf(char *buf, size_t buflen, const char *fmt, ...);

#define BUF_LEN 80
int main(int argc, char **argv)
{
	int i;
	const char *s;
	char buf[BUF_LEN];

	i = (argc > 1) ? atoi(argv[1]) : 100;
	s = (argc > 2) ? argv[2] : "awesome";

	c89_fake_snprintf(buf, BUF_LEN, "%d%% %s!", i, s);

	printf("%s\n", buf);

	return 0;
}

/* just a demos the use of varargs, not a real snprintf impl */
int c89_fake_snprintf(char *buf, size_t buflen, const char *fmt, ...)
{
	va_list ap;
	size_t fpos, bpos, max, len;
	const char *s;
	int d;
	char c;

	va_start(ap, fmt);

	fpos = 0;
	bpos = 0;
	max = buflen - 1;

	if (buflen) {
		buf[0] = '\0';
	}
	while (fmt[fpos] != '\0') {
		if (fmt[fpos] == '%') {
			c = fmt[fpos + 1];
			len = max - bpos;
			switch (c) {
			case '%':
				if ((bpos + 1) < max) {
					buf[bpos++] = '%';
					buf[bpos] = '\0';
				} else {
					++bpos;
				}
				break;
			case 'd':
				d = va_arg(ap, int);
				bpos += a_int(buf + bpos, len, d);
				break;
			case 's':
				s = va_arg(ap, const char *);
				bpos += a_str(buf + bpos, max, s);
				break;
			default:
				if ((bpos + 2) < max) {
					buf[bpos++] = fmt[fpos++];
					buf[bpos++] = fmt[fpos++];
					buf[bpos] = '\0';
				} else {
					bpos += 2;
				}
			}
			fpos += 2;
		} else {
			if ((bpos + 1) < max) {
				buf[bpos++] = fmt[fpos++];
				buf[bpos] = '\0';
			} else {
				++bpos;
			}
		}
	}

	va_end(ap);

	return bpos;
}

int a_str(char *buf, size_t len, const char *str)
{
	size_t i;
	for (i = 0; str[i] != '\0'; ++i) {
		if (len > i) {
			buf[i] = str[i];
		}
	}

	buf[(len > i) ? i : len - 1] = '\0';
	return i;
}

int a_int(char *buf, size_t len, int x)
{
	size_t p;
	int i, y, d;

	i = x;
	p = 0;
	for (d = 1000000000; d >= 10; d /= 10) {
		if (x >= d) {
			if (len > p) {
				y = (i / d);
				buf[p++] = '0' + y;
			} else {
				++p;
			}
			i = i % d;
		}
	}
	if (len > p) {
		buf[p++] = '0' + i;
	} else {
		++p;
	}

	buf[(len > p) ? p : len - 1] = '\0';
	return p;
}
