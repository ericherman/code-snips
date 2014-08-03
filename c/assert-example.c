#include <stdio.h>
#include <assert.h>

void foo(int x, int y)
{
	printf("demonstrating assert(%d < %d)\n", x, y);
	assert(x < y);
}

int main(int argc, char *argv[])
{
	int x, y;

	if (argc >= 3) {
		y = atoi(argv[2]);
	}
	if (argc >= 2) {
		x = atoi(argv[1]);
	}

	foo(x, y);

	return 0;
}
