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
