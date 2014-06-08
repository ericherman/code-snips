/*
 gcc --std=c99 -o sscanf-example sscanf-example.c
*/
#include <stdio.h>

int main(void)
{
	double d;
	size_t i;

	const char *read_format = "%lf%*s";
	const char *write_format = "sscanf(\"%s\", \"%s\", &d); d == %lf\n";
	const char *strs[] = {
		"123.45 blah",
		"123.45,678",
		"123,456.78",
		"123.45",
	};

	for (i = 0; i < 4; i++) {
		sscanf(strs[i], read_format, &d);
		printf(write_format, strs[i], read_format, d);
	}

	return 0;
}
