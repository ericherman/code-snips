#include <alloca.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

/*
Program for determining the default (thread) stack size
slightly modified from:
http://lists.gnu.org/archive/html/bug-coreutils/2009-10/msg00262.html

$ gcc -std=c89 -O0 -Wall -Wextra -Wpedantic -Werror \
    -o stack-size stack-size.c -pthread
$ ./stack-size
Allocated 7444992 bytes from stack ...Segmentation fault (core dumped)
$ ./stack-size -t
Creating thread.
Allocated 7452288 bytes from thread stack ...Segmentation fault (core dumped)
$ ulimit -s
8192
*/

void *eat_stack(void *p)
{
	const char *s = *((int *)p) == 1 ? "thread " : "";
	int n = 0;
	for (;;) {
		printf("\rAllocated %d bytes from %sstack ...", n, s);
		fflush(stdout);
		n += 128;
		*((volatile char *)alloca(128)) = 0;
	}
}

int main(int argc, char *argv[])
{
	int of_thread;
	pthread_t thread;
	if ((argc > 1) && ((strcmp(argv[1], "--thread") == 0)
			   || (strcmp(argv[1], "-t") == 0))) {
		of_thread = 1;
	} else {
		of_thread = 0;
	}

	if (of_thread) {
		printf("Creating thread.\n");
		pthread_create(&thread, NULL, eat_stack, &of_thread);
		for (;;) {
		}
	} else {
		eat_stack(&of_thread);
	}

	return 0;
}
