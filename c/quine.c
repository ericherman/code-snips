/* quine.c: a program to print its own source
 * Copyright (c) 2018 Eric Herman <eric@freesa.org>
 * License GPLv3 or later
 * https://www.gnu.org/licenses/gpl-3.0.txt */

#include <stdio.h>

#define LF 10
#define TAB 9
#define DQUOTE 34

int main(void)
{
	char *fmt;

	fmt = "/* quine.c: a program to print its own source%c * Copyright (c) 2018 Eric Herman <eric@freesa.org>%c * License GPLv3 or later%c * https://www.gnu.org/licenses/gpl-3.0.txt */%c%c#include <stdio.h>%c%c#define LF 10%c#define TAB 9%c#define DQUOTE 34%c%cint main(void)%c{%c%cchar *fmt;%c%c%cfmt = %c%s%c;%c%c%cprintf(fmt, LF, LF, LF, LF, LF, LF, LF, LF, LF, LF, LF, LF, LF, TAB, LF, LF, TAB, DQUOTE, fmt, DQUOTE, LF, LF, TAB, LF, LF, TAB, LF, LF, LF);%c%c%creturn 0;%c}%c";

	printf(fmt, LF, LF, LF, LF, LF, LF, LF, LF, LF, LF, LF, LF, LF, TAB, LF, LF, TAB, DQUOTE, fmt, DQUOTE, LF, LF, TAB, LF, LF, TAB, LF, LF, LF);

	return 0;
}
