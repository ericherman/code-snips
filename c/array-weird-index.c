/* array-weird-index.c
   Copyright (C) 2016, 2018 Eric Herman <eric@freesa.org>

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
#include <string.h>

int main(void)
{
	char buf[6];

	strcpy(buf, "foobar");

	printf("       buf == '%s'\n", buf);
	printf("*(buf + 4) == '%c'\n", *(4 + buf));
	printf("    buf[4] == '%c'\n", buf[4]);
	printf("    4[buf] == '%c'\n", 4[buf]);

	return 0;
}

/*
So in C,
	x[y]
must turn in to
	*(x + y)
soo...
	(1+1)[foo]
would become:
	*((1+1) + foo)
Thus, I think the foo[bar] must be *pure* syntactic sugar.
*/
