#include <stdio.h>
#include <string.h>

/*
  Compile with -O0

  gcc -O0 -o array-neg-index array-neg-index.c && ./array-neg-index
 */

int main(void)
{
	char foo;
	char buf[6];
	char bar;

	foo = 'a';
	bar = 'z';

	strcpy(buf, "hello");

	buf[-1] = 'X';

	printf("%c %s %c\n", foo, buf, bar);

	return 0;
}
