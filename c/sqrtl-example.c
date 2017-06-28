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
