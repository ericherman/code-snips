/*
 from kernel/exit.c:

                panic("Attempted to kill init! exitcode=0x%08x\n",
                        father->signal->group_exit_code ?: father->exit_code);

 warning: ISO C forbids omitting the middle term of a ?: expression [-Wpedantic]

 ... but with GCC it works ... and seems to resolve to "the item tested or"
*/

#include <stdio.h>

int main(void)
{
	int a, b;

	a = 0 ?: 7;

	b = 3 ?: 17;

	printf("a = 0 ?: 7; a == %d\n", a); /* 7 */
	printf("b = 3 ?: 17; b == %d\n", b); /* 3 */

	return (a == 7 && b == 3) ? 0 : 1;
}
