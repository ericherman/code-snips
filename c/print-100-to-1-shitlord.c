/*
 *  http://www.thousandtyone.com/blog/EasierThanFizzBuzzWhyCantProgrammersPrint100To1.aspx
 *
 *  Print 100 to 1.
 *  That's it.
 *  That was the question.
 *  The Catch?
 *  You need to start with "for(int i=0;" and continue from there -
 *   you cannot write anything before "for(int i=0;" and
 *   you can't use two loops.
*/

#include <stdio.h>
int main(void)
{
	for (int i = 0;
/*
 * The "shitlord solution" was proposed by AEvar Bjarmason,
 * (https://github.com/avar) which truly made my day.
 * I thought I would actually code it up in C, just for fun.
 */
	     shitlord(&i); --i) {
		printf("%d\n", i);
	}
	return 0;
}

static int shitlord_init = 0;
int shitlord(int *i)
{
	if (!shitlord_init) {
		*i = 100;
		shitlord_init = 1;
	}
	return *i;
}
