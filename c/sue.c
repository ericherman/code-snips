#include <stdio.h>
#include <limits.h>
#include <time.h>

int main()
{
	long long_time = (long)time(NULL);
	printf("love you %ld\n", long_time);
	return 0;
}
