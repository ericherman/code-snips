#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *endptr;
	char *dec;
	int i, base, save_errno;
	unsigned long result;

	base = 0;		/* special: decimal, octal, or hex */

	for (i = 1; ((i == 1) || (i < argc)); ++i) {
		dec = (argc > i) ? argv[i] : "508882404962533554393";
		printf("input: '%s'\n", dec);
		errno = 0;
		result = strtoul(dec, &endptr, base);
		save_errno = errno;
		if (endptr && strlen(endptr) > 0) {
			printf("endptr '%s'\n", endptr);
		}
		if (save_errno) {
			printf("errno %d: '%s'\n", save_errno,
			       strerror(save_errno));
		}
		printf("result: %lu\n", result);
	}

	return 0;
}
