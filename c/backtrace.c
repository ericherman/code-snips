/* backtrace.c: demo of backtrace(), backtrace_symbols_fd()
Copyright (C) 2016 Eric Herman <eric@freesa.org>
THIS IS PROVIDED WITHOUT WARRANTY, TO THE EXTENT PERMITTED BY APPLICABLE LAW.
Free Software license: LGPL version 3 or at your option any later version:
  https://www.gnu.org/licenses/lgpl.txt */
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
 otherwise, if "-g" is used, one could pass the address to addr2line:

$ gcc -g -D_POSIX_SOURCE backtrace.c -o backtrace
$ ./backtrace
./backtrace[0x804854c]
./backtrace[0x80485a7]
./backtrace[0x80485d1]
/lib/i386-linux-gnu/libc.so.6(__libc_start_main+0xf7)[0xb752a637]
./backtrace[0x8048441]
$ addr2line -e backtrace -a 0x80485a7 -f -p
0x080485a7: my_func at /home/eric/src/code-snips/c/backtrace.c:51
$
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
