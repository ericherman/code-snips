#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	int sleep_seconds;
	struct timeval tval_before, tval_after, tval_result;

	if (argc > 1) {
		sleep_seconds = atoi(argv[1]);
	} else {
		sleep_seconds = 1;
	}

	gettimeofday(&tval_before, NULL);

	printf("will sleep %d seconds\n", sleep_seconds);
	sleep(sleep_seconds);

	gettimeofday(&tval_after, NULL);

	timersub(&tval_after, &tval_before, &tval_result);

	printf("Time elapsed: %ld.%06ld in %d sleep\n",
	       (long int)tval_result.tv_sec, (long int)tval_result.tv_usec,
	       sleep_seconds);

	return 0;
}
