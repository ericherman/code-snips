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
