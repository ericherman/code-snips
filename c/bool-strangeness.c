/* bool-strangeness.c
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

char *url = "http://www.open-std.org/jtc1/sc22/wg14/www/C99RationaleV5.10.pdf";

int main(int argc, char **argv)
{
	printf("\nQuote from C99RationaleV5.10.pdf[1] Section: 6.3.1.2:\n");
	printf("\n\talthouth _Bool is technically an integer type,\n");
	printf("\tconversion to _Bool does not always work the\n");
	printf("\tsame as conversion to other integer types\n");
	printf("\nThis is easy to show:\n");
	printf("\t  ((int) 0.1) ? T : F == %c\n", ((int)0.1) ? 'T' : 'F');
	printf("\t((_Bool) 0.1) ? T : F == %c\n", ((_Bool) 0.1) ? 'T' : 'F');
	printf("\t        (0.1) ? T : F == %c\n", (0.1) ? 'T' : 'F');
	printf("\n[1] %s\n\n", url);

	return ((int)0.1) || ((_Bool) 0.1);
}
