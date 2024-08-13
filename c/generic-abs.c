#include <stdio.h>
#include <math.h>

#define ABS(x) _Generic((x),	\
	int: abs,		\
	long: labs,		\
	long long: llabs,	\
	float: fabsf,		\
	double: fabs,		\
	long double: fabsl	\
)(x)

int main()
{
	int i = -11;
	long l = -13L;
	long long ll = -17LL;
	float f = -19.23f;
	double d = -29.31;
	long double ld = -37.41L;

	printf("ABS(%d) = %d\n", i, ABS(i));
	printf("ABS(%ld) = %ld\n", l, ABS(l));
	printf("ABS(%lld) = %lld\n", ll, ABS(ll));
	printf("ABS(%f) = %f\n", f, ABS(f));
	printf("ABS(%lf) = %lf\n", d, ABS(d));
	printf("ABS(%Lf) = %Lf\n", ld, ABS(ld));

	return 0;
}
