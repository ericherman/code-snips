#include <stdio.h>

#define eprints(msg) \
	fprintf(stderr, "%s %d %s: %s\n", \
		__FILE__, __LINE__, __func__, msg)

namespace foo {
	class bar {
	public:
		static void baz(const char *msg) {
			eprints(msg);
		}
	};
}

int main(void)
{
	foo::bar::baz("hello, world");

	return 0;
}
