/* sscanf-example.c
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
/*
 gcc --std=c99 -o sscanf-example sscanf-example.c
*/
#include <stdio.h>

int main(void)
{
	double d;
	size_t i;

	const char *read_format = "%lf%*s";
	const char *write_format = "sscanf(\"%s\", \"%s\", &d); d == %lf\n";
	const char *strs[] = {
		"123.45 blah",
		"123.45,678",
		"123,456.78",
		"123.45",
	};

	for (i = 0; i < 4; i++) {
		sscanf(strs[i], read_format, &d);
		printf(write_format, strs[i], read_format, d);
	}

	return 0;
}
