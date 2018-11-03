/* dev-full.c
   Copyright (C) 2017, 2018 Eric Herman <eric@freesa.org>

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
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *filename;
	char buf[256];
	FILE *outfile;
	int save_errno, bytes_written;

	filename = (argc > 1) ? argv[1] : "/dev/full";
	outfile = fopen(filename, "w");

	errno = 0;
	bytes_written = fprintf(outfile, "%s", "hello, world");
	save_errno = errno;
	if (save_errno) {
		fprintf(stderr, "failure writing\n");
		goto print_error;
	}
	fflush(outfile);
	if (errno) {
		save_errno = errno;
		fprintf(stderr, "failure flushing\n");
		goto print_error;
	}

	return 0;

print_error:
	bytes_written =
	    snprintf(buf, 256, "errno %d writing to %s; bytes_written: %d\n",
		     save_errno, filename, bytes_written);
	/* perror(buf); */
	strerror_r(save_errno, buf + bytes_written, 256 - bytes_written);
	fprintf(stderr, "%s\n", buf);
	return EXIT_FAILURE;
}
