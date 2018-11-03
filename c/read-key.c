/* read-key.c
   Copyright (C) 2015, 2018 Eric Herman <eric@freesa.org>

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
/* gcc -std=c89 -Wall -Wextra -pedantic -Werror -ggdb -o read-key read-key.c */

/* largely copied from http://www.unix.com/programming/3690-how-programm-tty-devices-under-unix-platform.html#post12226 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <string.h>

/* extern int errno; */

int main(void)
{
	struct termios orig, now;
	int c, rv, mode, errsave;
	FILE *stream;
	char *buf;
	size_t size;

	if (0) {
		stream = stdin;
		buf = NULL;
		mode = _IONBF;
		size = 0;

		rv = setvbuf(stream, buf, mode, size);
		if (rv) {
			errsave = errno;
			fprintf(stderr, "setvbuf failed; errno: %d: %s\n",
				errsave, strerror(errsave));
			return rv;
		}
	}

	tcgetattr(0, &orig);

	now = orig;
	now.c_lflag &= ~(ISIG | ICANON | ECHO);
	now.c_cc[VMIN] = 1;
	now.c_cc[VTIME] = 2;

	tcsetattr(0, TCSANOW, &now);

	c = 0;
	while (c != 'q') {
		printf("hit a key ('q' to quit): ");
		c = getchar();
		printf("%03X \n", c);
	}

	tcsetattr(0, TCSANOW, &orig);

	return 0;
}
