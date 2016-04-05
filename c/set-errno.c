#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char buf[10];

	if (argc != 2) {
		fprintf(stderr, "Usage: perror errno\n");
	}

	errno = argc > 1 ? atoi(argv[1]) : 0;

	snprintf(buf, 10, "errno %d", errno);
	perror(buf);

	return errno;
}
