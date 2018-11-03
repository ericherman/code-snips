/* complex.c
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
/* gcc -o complex complex.c -lm */
#include <stdio.h>
#include <complex.h>

char *url1 = "https://mitpress.mit.edu/sicp/full-text/sicp/book/node43.html";
char *url2 = "http://en.cppreference.com/w/c/numeric/complex";
int main(void)
{
	complex float z = 23.0 + 5.0 * I;

	printf("z = %.1f + %.1fI\n", creal(z), cimag(z));
	printf("magnitue(z) = %f\n", cabs(z));
	printf("phase angle(z) = %f\n", carg(z));
	printf("thus %.1f%+.1fi cartesian as polar becomes rho=%f theta=%f\n",
	       creal(z), cimag(z), cabs(z), carg(z));

	printf("\nSee also:\n%s\n%s\n", url1, url2);
}
