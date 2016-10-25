#include <stdio.h>
#include <unistd.h>

int main(void)
{
	int cpus = sysconf(_SC_NPROCESSORS_ONLN);
	printf("_SC_NPROCESSORS_ONLN: %d\n", cpus);
	return 0;
}
