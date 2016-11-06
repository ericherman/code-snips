/*
backtrace.c: demo of backtrace(), backtrace_symbols_fd()
Copyright (C) 2016 Eric Herman <eric@freesa.org>

THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.

Free Software license: LGPL version 3 or at your option any later version:
  https://www.gnu.org/licenses/lgpl.txt
*/
/*
$ gcc -std=c89 -Wall -Werror -Wextra -Wpedantic \
  -D_POSIX_SOURCE \
  -rdynamic \
  backtrace.c -o backtrace
$ ./backtrace
./backtrace(log_backtrace+0x31)[0x804876c]
./backtrace(my_func+0x11)[0x80487c7]
./backtrace(main+0x24)[0x80487f1]
/lib/i386-linux-gnu/libc.so.6(__libc_start_main+0xf7)[0xb75f9637]
./backtrace[0x8048661]

 fileno(): _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE

 backtrace() and backtrace_symbols_fd() are GNU extensions.

 the "-rdynamic" option is needed to see the function names in the output,
 (otherwise one could pass the address to addr2line)
*/

#include <stdio.h>		/* stdout fileno */
#include <execinfo.h>		/* backtrace backtrace_symbols_fd */

void log_backtrace(FILE *log)
{
	void *buf[4000];
	size_t len;

	len = backtrace(buf, 4000);
	backtrace_symbols_fd(buf, len, fileno(log));
}

void my_func(FILE *log)
{
	log_backtrace(log);
}

int main(void)
{
	FILE *log = stdout;

	my_func(log);

	return 0;
}
