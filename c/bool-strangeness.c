#include <stdio.h>
int main(int argc, char **argv)
{
	printf("althouth _Bool is technically an integer type,\n");
	printf(" conversion to _Bool does not always work the\n");
	printf(" same as conversion to other integer types:\n");
	printf("  ((int) 0.5) ? 1 : 0 == %d\n", ((int)0.5) ? 1 : 0);
	printf("((_Bool) 0.5) ? 1 : 0 == %d\n", ((_Bool) 0.5) ? 1 : 0);

	return ((int)0.5) || ((_Bool) 0.5);
}
