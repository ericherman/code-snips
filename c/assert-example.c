/* assert-example.c
   Copyright (C) 2014, 2018 Eric Herman <eric@freesa.org>

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
#include <assert.h>

void foo(int x, int y)
{
	printf("demonstrating assert(%d < %d)\n", x, y);
	assert(x < y);
}

int main(int argc, char *argv[])
{
	int x, y;

	if (argc >= 3) {
		y = atoi(argv[2]);
	}
	if (argc >= 2) {
		x = atoi(argv[1]);
	}

	foo(x, y);

	return 0;
}
