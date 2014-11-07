/*

Copyright (C) 2005 TJ Walls
Copyright (C) 2013 Eric Herman <eric@freesa.org>

Code modified from "TJ Walls" Nov 14 '05 question:
http://bytes.com/topic/c/answers/218188-fseek-speed
(no license specified)
Eric's changes licensed LGPL v2.1

This code demonstrates that fseek(fp, 0, SEEK_CUR) is slow
even in situations like seeking to the current position.

Results:

$ time { ./fseek-test 0 ; }
Will write 4194304 bytes.
Will not seek before each write.
4194304 of 4194304 bytes written.

real	0m0.215s
user	0m0.138s
sys	0m0.004s

$ time { ./fseek-test 1 ; }
Will write 4194304 bytes.
Will seek before each write.
4194304 of 4194304 bytes written.

real	0m6.020s
user	0m0.863s
sys	0m5.147s



compiled using gcc 4.8.2 for x86_64 linux on ext4:

gcc --std=c89 -Os -Werror -Wall -Wextra -pedantic -o fseek-test fseek-test.c

*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FILE *fp;
	unsigned long i, file_size;
	char c;
	size_t nmemb, size;
	int should_seek;

	should_seek = (argc >= 2) ? atoi(argv[1]) : 0;
	file_size = (argc >= 3) ? atoi(argv[2]) : 4 * 1024 * 1024;

	printf("Will write %lu bytes.\n", file_size);
	printf("Will %sseek before each write.\n", should_seek ? "" : "not ");

	fp = NULL;
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

	return EXIT_SUCCESS;
}
