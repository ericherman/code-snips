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
