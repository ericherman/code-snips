/* test-time.c
   Copyright (C) 2017, 2018 Eric Herman <eric@freesa.org>

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
