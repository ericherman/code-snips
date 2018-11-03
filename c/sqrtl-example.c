/* sqrtl-example.c
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
/*
gcc -o sqrtl-example sqrtl-example.c -lm
*/

#include <stdio.h>
#include <math.h>

int main(void)
{
	long double x = 17999999998987831969ULL;
	long double sr = sqrtl(x);
	unsigned long long z =
	    ((unsigned long long)sr) * ((unsigned long long)sr);
	printf("sqrt(%Lf) == %Lf\n", x, sr);
	printf("%Lf * %Lf == %llu\n", sr, sr, z);
	return 0;
}
