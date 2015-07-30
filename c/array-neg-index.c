#include <stdio.h>
#include <string.h>
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
