/* lldiv.c
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
#include <stdlib.h>		/* div, div_t */

int main(int argc, char **argv)
{
	long long numerator = (argc > 1) ? atol(argv[1]) : 5088824049625;
	long long denominator = (argc > 2) ? atol(argv[2]) : 33554393;
	lldiv_t result = lldiv(numerator, denominator);

	printf("lldiv_t result = lldiv(%llu,%llu);\n", numerator, denominator);
	printf("result.quot: %llu, result.remainder: %llu\n",
	       result.quot, result.rem);

	return 0;
}
