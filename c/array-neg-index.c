/* array-neg-index.c
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
#include <stdio.h>
#include <string.h>

/*
  Compile with -O0

  gcc -O0 -o array-neg-index array-neg-index.c && ./array-neg-index
 */

int main(void)
{
	char foo;
	char buf[6];
	char bar;

	foo = 'a';
	bar = 'z';

	strcpy(buf, "hello");

	buf[-1] = 'X';

	printf("%c %s %c\n", foo, buf, bar);

	return 0;
}
