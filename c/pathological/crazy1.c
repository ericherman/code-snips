#include <stdio.h>
#include <string.h>

/*

See:
http://www.cl.cam.ac.uk/~pes20/cerberus/notes64-wg14.html

Experiment found in:
  DRAFT - WORK IN PROGRESS
  C memory object and value semantics:
  the space of de facto and ISO standards
  [This revises and extends WG14 N2013]
  Revision: 1571 2016-03-17
  David Chisnall, Justus Matthiesen, Kayvan Memarian,
  Peter Sewell, Robert N. M. Watson
  University of Cambridge
  http://www.cl.cam.ac.uk/~pes20/cerberus/notes30.pdf


$ gcc -std=c89 -Wall -Wextra -Wpedantic -Werror -ggdb \
      -O3 -o crazy1 crazy1.c &&
  ./crazy1
Addresses: p=0x804a028 q=0x804a028
x=1 y=2 *p=11 *q=2
$
$ gcc -std=c89 -Wall -Wextra -Wpedantic -Werror -ggdb \
      -O1 -o crazy1 crazy1.c &&
  ./crazy1
Addresses: p=0x804a028 q=0x804a028
x=1 y=11 *p=11 *q=11
$
$ gcc -std=c89 -Wall -Wextra -Wpedantic -Werror -ggdb \
      -O0 -o crazy1 crazy1.c &&
  ./crazy1
Addresses: p=0x804a02c q=0x804a024
$

*/

int y = 2, x = 1;
int main()
{
	int *p = &x + 1;
	int *q = &y;
	printf("Addresses: p=%p q=%p\n", (void *)p, (void *)q);
	if (memcmp(&p, &q, sizeof(p)) == 0) {
		*p = 11;	/* does this have undefined behaviour? */
		printf("x=%d y=%d *p=%d *q=%d\n", x, y, *p, *q);
	}
	return 0;
}
