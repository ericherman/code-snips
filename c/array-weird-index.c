#include <stdio.h>
#include <string.h>

int main(void)
{
	char buf[6];

	strcpy(buf, "foobar");

	printf("       buf == '%s'\n", buf);
	printf("*(buf + 4) == '%c'\n", *(4 + buf));
	printf("    buf[4] == '%c'\n", buf[4]);
	printf("    4[buf] == '%c'\n", 4[buf]);

	return 0;
}

/*
So in C,
	x[y]
must turn in to
	*(x + y)
soo...
	(1+1)[foo]
would become:
	*((1+1) + foo)
Thus, I think the foo[bar] must be *pure* syntactic sugar.
*/
