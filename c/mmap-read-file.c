/* memory allocation by mmap requires _GNU_SOURCE since it is linux specific */
#define _GNU_SOURCE
#include <sys/mman.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	int fd;
	struct stat fst;
	void *mem;
	char *filename;

	filename = argc > 1 ? argv[1] : "mmap-read-file.c";

	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "open(\"%s\", O_RDONLY)\n", filename);
		return -1;
	}

	if (fstat(fd, &fst) == -1) {
		fprintf(stderr, "fstat(fd, &fst)\n");
		return -2;
	}

	if (!fst.st_size) {
		fprintf(stderr, "size is 0\n");
	} else {
		mem =
		    mmap(0, fst.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE,
			 fd, 0);
	}

	if (fd != -1) {
		close(fd);
	}

	if (!mem) {
		fprintf(stderr, "mem is NULL\n");
	} else if (mem == MAP_FAILED) {
		fprintf(stderr, "mem == MAP_FAILED\n");
	} else {
		fprintf(stderr, "%s", (char *)mem);
	}

	if (mem) {
		munmap(mem, fst.st_size);
	}

	return fd;
}
