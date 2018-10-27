/* dem0o-mmap-alloc.c : example of how to hand-roll a malloc function on posix
 * Copyright (c) 2018 Eric Herman <eric@freesa.org>
 * License: LGPLv2.1 or later
 */

/* POSIX.1-2001, POSIX.1-2008, SVr4, 4.4BSD. */
#include <sys/mman.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef Block_size
#define Block_size (4UL * 1024UL)
#endif

void *mmap_alloc_blocks(unsigned num_blocks, int *err)
{
	void *mem;
	size_t len;
	int prot, flags, fd, rv;
	off_t offset;

	if (!err) {
		err = &rv;
	}

	prot = PROT_READ | PROT_WRITE;
	flags = MAP_PRIVATE | MAP_ANONYMOUS;
	fd = -1;
	offset = 0;
	len = num_blocks * Block_size;

	/* On  success, mmap() returns a pointer to the mapped area.
	 * On error, the value MAP_FAILED (that is, (void *) -1) is
	 * returned, and errno is set to indicate the cause of the
	 * error.  */
	mem = mmap(NULL, len, prot, flags, fd, offset);

	if (mem == MAP_FAILED) {
		*err = 1;
	} else if (!mem) {
		*err = 2;
	} else if (((ssize_t)mem) < 0) {
		*err = 3;
	} else {
		*err = 0;
	}

	if (0 != (*err)) {
		return NULL;
	}

	return mem;
}

int munmap_free_blocks(void *mem, unsigned num_blocks)
{
	size_t len;

	if (!mem) {
		return 1;
	}

	len = num_blocks * Block_size;

	/* On success, munmap() returns 0.
	 * On failure, it returns -1, and errno is set to indicate
	 * the cause of the error (probably to EINVAL).  */
	return munmap(mem, len);
}

int main(void)
{
	int err;
	unsigned blocks;
	size_t i, num_ints;
	uint32_t *ints;
	uint32_t sum, expected;

	blocks = 1;
	ints = mmap_alloc_blocks(blocks, &err);
	if (!ints) {
		return err;
	}

	expected = 0;
	num_ints = (Block_size) / (sizeof(uint32_t));
	for (i = 0; i < num_ints; ++i) {
		ints[i] = i;
		expected += i;
	}

	sum = 0;
	for (i = 0; i < num_ints; ++i) {
		sum += ints[i];
	}

	if (sum != expected) {
		return 4;
	}

	return munmap_free_blocks(ints, blocks);
}
