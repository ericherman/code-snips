/* mmap-cp.c
   Copyright (C) 2015, 2018 Eric Herman <eric@freesa.org>

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
/* in_memory allocation by mmap requires _GNU_SOURCE since it is linux specific */
#define _GNU_SOURCE
#include <sys/mman.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <stdio.h>

int main(int argc, char **argv)
{
	int fd;
	struct stat fst;
	void *in_mem, *out_mem;
	char *in_filename, *out_filename;

	in_filename = argc > 1 ? argv[1] : "mmap-cp.c";
	out_filename = argc > 2 ? argv[2] : "mmap-cp.c.copy";

	fd = open(in_filename, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "open(\"%s\", O_RDONLY)\n", in_filename);
		return -1;
	}

	if (fstat(fd, &fst) == -1) {
		fprintf(stderr, "fstat(fd, &fst) == -1\n");
		return -2;
	}

	if (!fst.st_size) {
		fprintf(stderr, "size is 0\n");
	} else {
		in_mem =
		    mmap(0, fst.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE,
			 fd, 0);
	}

	if (fd != -1) {
		close(fd);
	}

	if (!in_mem) {
		fprintf(stderr, "in_mem is NULL\n");
	} else if (in_mem == MAP_FAILED) {
		fprintf(stderr, "in_mem == MAP_FAILED\n");
	} else {
		if (0) {
			fprintf(stderr, "%s", (char *)in_mem);
		}
	}

	fd = open(out_filename, O_RDWR | O_CREAT | O_TRUNC,
		  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1) {
		fprintf(stderr, "open(\"%s\", O_WRONLY | O_CREATE)\n",
			out_filename);
		return -3;
	}

	if (lseek(fd, fst.st_size - 1, SEEK_SET) == -1) {
		fprintf(stderr, "lseek 1");
	} else if (write(fd, "", 1) != 1) {
		fprintf(stderr, "write byte");
	} else if (lseek(fd, 0, SEEK_SET) == -1) {
		fprintf(stderr, "lseek 2");
	}

	out_mem =
	    mmap(0, fst.st_size, PROT_WRITE, MAP_FILE | MAP_SHARED, fd, 0);

	if (!out_mem) {
		fprintf(stderr, "out_mem is NULL\n");
	} else if (out_mem == MAP_FAILED) {
		fprintf(stderr, "out_mem == MAP_FAILED\n");
	} else if (in_mem) {
		memcpy(out_mem, in_mem, fst.st_size);
	} else {
		fprintf(stderr, "no in_mem to copy.\n");
	}

	if (in_mem) {
		munmap(in_mem, fst.st_size);
	}
	if (out_mem) {
		munmap(out_mem, fst.st_size);
	}

	if (fd != -1) {
		close(fd);
	}

	return fd;
}
