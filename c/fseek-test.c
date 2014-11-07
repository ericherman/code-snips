/*

Copyright (C) 2005 TJ Walls <tjwalls@mindspring.nospam.com>
Copyright (C) 2013 Eric Herman <eric@freesa.org>

Code modified from "TJ Walls" Nov 14 '05 question:
http://bytes.com/topic/c/answers/218188-fseek-speed
(no license specified)
Eric's changes licensed LGPL v2.1

This code demonstrates that fseek(fp, 0, SEEK_CUR) is slow
even in situations like seeking to the current position,
if fseek needs to flush the buffer.

Arguments:

first argument of 0 indicates a write test, 1 a read test
second argument of 0 indicates no seek, 1 indicates seek
third argument (optional) is file_size in bytes


Results:

$ time { ./fseek-test 0 0; }
Will write 4194304 bytes.
Will not seek before each write.
4194304 of 4194304 bytes written.

real	0m0.214s
user	0m0.146s
sys	0m0.004s

$ time { ./fseek-test 1 0; }
Will read 4194304 bytes.
Will not seek before each read.
4194304 of 4194304 bytes read.

real	0m0.183s
user	0m0.136s
sys	0m0.000s

$ time { ./fseek-test 0 1; }
Will write 4194304 bytes.
Will seek before each write.
4194304 of 4194304 bytes written.

real	0m6.034s
user	0m0.767s
sys	0m5.257s

$ time { ./fseek-test 1 1; }
Will read 4194304 bytes.
Will seek before each read.
4194304 of 4194304 bytes read.

real	0m0.411s
user	0m0.227s
sys	0m0.183s

compiled using gcc 4.8.2 for x86_64 linux on ext4:

gcc --std=c89 -Os -Werror -Wall -Wextra -pedantic -o fseek-test fseek-test.c

*/

#include <stdio.h>
#include <stdlib.h>

void write_test(int should_seek, unsigned long file_size)
{
	FILE *fp;
	unsigned long i;
	char c;
	size_t nmemb, size;

	printf("Will write %lu bytes.\n", file_size);
	printf("Will %sseek before each write.\n", should_seek ? "" : "not ");

	fp = NULL;
	/* w+ Open for reading and writing.
	   The file is created if it does not exist, otherwise it is
	   truncated.  The stream  is  positioned  at the beginning
	   of the file. */
	if ((fp = fopen("/tmp/junk.bin", "w+b")) == NULL) {
		fprintf(stderr, "Could not open /tmp/junk.bin for writing.\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < file_size; i++) {
		if (i % 1024 == 0) {
			printf("\r%lu of %lu bytes written ...", i, file_size);
		}
		if (should_seek) {
			fseek(fp, 0, SEEK_CUR);
		}
		size = 1;	/* size of a elments to write (char) */
		nmemb = 1;	/* number of elements to write */
		c = '0' + ((char)(i % 10));
		fwrite(&c, size, nmemb, fp);
	}
	printf("\r%lu of %lu bytes written.    \n", i, file_size);

	fclose(fp);
}

void read_test(int should_seek, unsigned long file_size)
{
	FILE *fp;
	unsigned long i;
	char c, e;
	size_t nmemb, size, items;

	fp = NULL;

	printf("Will read %lu bytes.\n", file_size);
	printf("Will %sseek before each read.\n", should_seek ? "" : "not ");

	/* r+ Open for reading and writing.
	   The stream is positioned at the beginning of the file. */
	if ((fp = fopen("/tmp/junk.bin", "r+b")) == NULL) {
		fprintf(stderr, "Could not open /tmp/junk.bin for writing.\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < file_size; i++) {
		e = '0' + ((char)(i % 10));
		if (i % 1024 == 0) {
			printf("\r%lu of %lu bytes read ...", i, file_size);
		}
		if (should_seek) {
			fseek(fp, 0, SEEK_CUR);
		}
		size = 1;	/* size of a elments to write (char) */
		nmemb = 1;	/* number of elements to write */
		items = fread(&c, size, nmemb, fp);
		if (items != nmemb) {
			fprintf(stderr, "Expected to read %lu but read %lu.\n",
				(unsigned long)nmemb, (unsigned long)items);
			exit(EXIT_FAILURE);
		}
		if (c != e) {
			fprintf(stderr, "Expected %c but was %c.\n", e, c);
			exit(EXIT_FAILURE);
		}
	}
	printf("\r%lu of %lu bytes read.    \n", i, file_size);

	fclose(fp);
}

int main(int argc, char *argv[])
{
	int read, should_seek;
	unsigned long file_size;

	read = (argc >= 2) ? atoi(argv[1]) : 0;
	should_seek = (argc >= 3) ? atoi(argv[2]) : 0;
	file_size = (argc >= 4) ? atoi(argv[3]) : 4 * 1024 * 1024;

	if (read) {
		read_test(should_seek, file_size);
	} else {
		write_test(should_seek, file_size);
	}

	return EXIT_SUCCESS;
}
