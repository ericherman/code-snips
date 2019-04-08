/* feature-tests.c
   Copyright (C) 2016, 2018 Eric Herman <eric@freesa.org>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

	https://www.gnu.org/licenses/lgpl-3.0.txt
	https://www.gnu.org/licenses/gpl-3.0.txt
 */
/* feature-tests.c

eric@titan:~/src/code-snips/c$ uname -a
Linux titan 4.4.0-45-generic #66-Ubuntu SMP Wed Oct 19 14:12:05 UTC 2016 i686 i686 i686 GNU/Linux
eric@titan:~/src/code-snips/c$ /lib/i386-linux-gnu/libc.so.6 | head -n1
GNU C Library (Ubuntu GLIBC 2.23-0ubuntu4) stable release version 2.23, by Roland McGrath et al.
eric@titan:~/src/code-snips/c$ gcc --version | head -n1
gcc (Ubuntu 5.4.0-6ubuntu1~16.04.2) 5.4.0 20160609
eric@titan:~/src/code-snips/c$ gcc -std=gnu11 -Dthread_local=__thread  -o feature-tests feature-tests.c
eric@titan:~/src/code-snips/c$ ./feature-tests
__STDC__ defined
__STDC_VERSION__ defined: 201112L
__STDC_NO_THREADS__ defined
__STDC_IEC_559__ defined
__STDC_IEC_559_COMPLEX__ defined
__GNUC__ defined
__GNU_LIBRARY__ defined
_POSIX_SOURCE defined
_POSIX_C_SOURCE defined: 200809L
_DEFAULT_SOURCE defined
_ATFILE_SOURCE defined
_SC_PASS_MAX defined: 88L
_SC_XOPEN_XCU_VERSION defined: 90L
eric@titan:~/src/code-snips/c$

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
/* #include <threads.h> */
/* #include <stdatomic.h> */
/* #include <complex.h> */

int main(int argc, char *argv[])
{

	int verbose = (argc > 1) ? atoi(argv[1]) : 0;

#ifdef  __STDC__
	printf("__STDC__ defined (%ld)\n", (long)__STDC__);
#else
	if (verbose) {
		printf("__STDC__ not defined\n");
	}
#endif

#ifdef  __STDC_VERSION__
	printf("__STDC_VERSION__ defined: %ldL\n", (long)__STDC_VERSION__);
#else
	if (verbose) {
		printf("__STDC_VERSION__ not defined\n");
	}
#endif

#ifdef  __STDC_ANALYZABLE__
	printf("__STDC_ANALYZABLE__ defined (%ld)\n",
	       (long)__STDC_ANALYZABLE__);
#endif

#ifdef  __STDC_LIB_EXT1__
	printf("__STDC_LIB_EXT1__ defined (%ld)\n", (long)__STDC_LIB_EXT1__);
#endif

#ifdef  __STDC_VERSION__
#ifdef  __STDC_NO_THREADS__
	printf("__STDC_NO_THREADS__ defined (%ld)\n",
	       (long)__STDC_NO_THREADS__);
#else
	if (verbose) {
		printf("__STDC_NO_THREADS__ not defined\n");
	}
#endif
#endif

#ifdef  __STDC_VERSION__
#ifdef  __STDC_NO_ATOMICS__
	printf("__STDC_NO_ATOMICS__ defined (%ld)\n",
	       (long)__STDC_NO_ATOMICS__);
#else
	if (verbose) {
		printf("__STDC_NO_ATOMICS__ not defined\n");
	}
#endif
#endif

#ifdef  __STDC_IEC_559__
	printf("__STDC_IEC_559__ defined (%ld)\n", (long)__STDC_IEC_559__);
#endif

#ifdef  __STDC_IEC_559_COMPLEX__
	printf("__STDC_IEC_559_COMPLEX__ defined (%ld)\n",
	       (long)__STDC_IEC_559_COMPLEX__);
#endif

#ifdef  __STDC_VERSION__
#ifdef  __STDC_NO_COMPLEX__
	printf("__STDC_NO_COMPLEX__ defined (%ld)\n",
	       (long)__STDC_NO_COMPLEX__);
#else
	if (verbose) {
		printf("__STDC_NO_COMPLEX__ not defined\n");
	}
#endif
#endif

#ifdef  __STDC_VERSION__
#ifdef  __STDC_NO_VLA__
	printf("__STDC_NO_VLA__ defined (%ld)\n", (long)__STDC_NO_VLA__);
#else
	if (verbose) {
		printf("__STDC_NO_VLA__ not defined\n");
	}
#endif
#endif

#ifdef  __GNUC__
	printf("__GNUC__ defined (%ld)\n", (long)__GNUC__);
#endif

#ifdef  __GNU__
	printf("__GNU__ defined (%ld)\n", (long)__GNU__);
#endif

#ifdef  __USE_GNU
	printf("__USE_GNU defined (%ld)\n", (long)__USE_GNU);
#endif

#ifdef  __GNU_LIBRARY__
	printf("__GNU_LIBRARY__ defined (%ld)\n", (long)__GNU_LIBRARY__);
#endif

#ifdef _POSIX_SOURCE
	printf("_POSIX_SOURCE defined (%ld)\n", (long)_POSIX_SOURCE);
#endif

#ifdef _POSIX_C_SOURCE
	printf("_POSIX_C_SOURCE defined: %ldL\n", (long)_POSIX_C_SOURCE);
#else
	if (verbose) {
		printf("_POSIX_C_SOURCE not defined\n");
	}
#endif

#ifdef _ISOC99_SOURCE
	printf("_ISOC99_SOURCE defined (%ld)\n", (long)_ISOC99_SOURCE);
#endif

#ifdef _ISOC11_SOURCE
	printf("_ISOC11_SOURCE defined (%ld)\n", (long)_ISOC11_SOURCE);
#endif

#ifdef _XOPEN_SOURCE
	printf("_XOPEN_SOURCE defined: %ld\n", (long)_XOPEN_SOURCE);
#endif

#ifdef _XOPEN_SOURCE_EXTENDED
	printf("_XOPEN_SOURCE_EXTENDED defined (%ld)\n",
	       (long)_XOPEN_SOURCE_EXTENDED);
#endif

#ifdef _LARGEFILE64_SOURCE
	printf("_LARGEFILE64_SOURCE defined (%ld)\n",
	       (long)_LARGEFILE64_SOURCE);
#endif

#ifdef _FILE_OFFSET_BITS
	printf("_FILE_OFFSET_BITS defined: %ld\n", (long)_FILE_OFFSET_BITS);
#endif

#ifdef _BSD_SOURCE
	printf("_BSD_SOURCE defined (%ld)\n", (long)_BSD_SOURCE);
#endif

#ifdef _SVID_SOURCE
	printf("_SVID_SOURCE defined (%ld)\n", (long)_SVID_SOURCE);
#endif

#ifdef _DEFAULT_SOURCE
	printf("_DEFAULT_SOURCE defined (%ld)\n", (long)_DEFAULT_SOURCE);
#endif

#ifdef _ATFILE_SOURCE
	printf("_ATFILE_SOURCE defined (%ld)\n", (long)_ATFILE_SOURCE);
#endif

#ifdef _GNU_SOURCE
	printf("_GNU_SOURCE defined (%ld)\n", (long)_GNU_SOURCE);
#endif

#ifdef _REENTRANT
	printf("_REENTRANT defined (%ld)\n", (long)_REENTRANT);
#endif

#ifdef _THREAD_SAFE
	printf("_THREAD_SAFE defined (%ld)\n", (long)_THREAD_SAFE);
#endif

#ifdef _FORTIFY_SOURCE
	printf("_FORTIFY_SOURCE defined (%ld)\n", (long)_FORTIFY_SOURCE);
#endif

#ifdef _ALL_SOURCE
	printf("_ALL_SOURCE (%ld)\n", (long)_ALL_SOURCE);
#endif

#ifdef _ALL_SOURCE_NO_THREADS
	printf("_ALL_SOURCE_NO_THREADS (%ld)\n", (long)_ALL_SOURCE_NO_THREADS);
#endif

#ifdef _FP_MODE_VARIABLE
	printf("_FP_MODE_VARIABLE (%ld)\n", (long)_FP_MODE_VARIABLE);
#endif

#ifdef _IEEEV1_COMPATIBILITY
	printf("_IEEEV1_COMPATIBILITY (%ld)\n", (long)_IEEEV1_COMPATIBILITY);
#endif

#ifdef _LARGE_FILES
	printf("_LARGE_FILES (%ld)\n", (long)_LARGE_FILES);
#endif

#ifdef _LARGE_MEM
	printf("_LARGE_MEM (%ld)\n", (long)_LARGE_MEM);
#endif

#ifdef _LARGE_TIME_API
	printf("_LARGE_TIME_API (%ld)\n", (long)_LARGE_TIME_API);
#endif

#ifdef _LONGMAP
	printf("_LONGMAP (%ld)\n", (long)_LONGMAP);
#endif

#ifdef _MSE_PROTOS
	printf("_MSE_PROTOS (%ld)\n", (long)_MSE_PROTOS);
#endif

#ifdef _NOISOC99_SOURCE
	printf("_NOISOC99_SOURCE (%ld)\n", (long)_NOISOC99_SOURCE);
#endif

#ifdef _OE_SOCKETS
	printf("_OE_SOCKETS (%ld)\n", (long)_OE_SOCKETS);
#endif

#ifdef _OPEN_DEFAULT
	printf("_OPEN_DEFAULT (%ld)\n", (long)_OPEN_DEFAULT);
#endif

#ifdef _OPEN_MSGQ_EXT
	printf("_OPEN_MSGQ_EXT (%ld)\n", (long)_OPEN_MSGQ_EXT);
#endif

#ifdef _OPEN_SOURCE
	printf("_OPEN_SOURCE defined:%ldL\n", (long)_OPEN_SOURCE);
#endif

#ifdef _OPEN_SYS
	printf("_OPEN_SYS (%ld)\n", (long)_OPEN_SYS);
#endif

#ifdef _OPEN_SYS_DIR_EXT
	printf("_OPEN_SYS_DIR_EXT (%ld)\n", (long)_OPEN_SYS_DIR_EXT);
#endif

#ifdef _OPEN_SYS_EXT
	printf("_OPEN_SYS_EXT (%ld)\n", (long)_OPEN_SYS_EXT);
#endif

#ifdef _OPEN_SYS_FILE_EXT
	printf("_OPEN_SYS_FILE_EXT (%ld)\n", (long)_OPEN_SYS_FILE_EXT);
#endif

#ifdef _OPEN_SYS_IF_EXT
	printf("_OPEN_SYS_IF_EXT (%ld)\n", (long)_OPEN_SYS_IF_EXT);
#endif

#ifdef _OPEN_SYS_IPC_EXTENSIONS
	printf("_OPEN_SYS_IPC_EXTENSIONS (%ld)\n",
	       (long)_OPEN_SYS_IPC_EXTENSIONS);
#endif

#ifdef _OPEN_SYS_MAP_EXTENTION
	printf("_OPEN_SYS_MAP_EXTENTION (%ld)\n",
	       (long)_OPEN_SYS_MAP_EXTENTION);
#endif

#ifdef _OPEN_SYS_MUTEX_EXT
	printf("_OPEN_SYS_MUTEX_EXT (%ld)\n", (long)_OPEN_SYS_MUTEX_EXT);
#endif

#ifdef _OPEN_SYS_PTY_EXTENSIONS
	printf("_OPEN_SYS_PTY_EXTENSIONS (%ld)\n",
	       (long)_OPEN_SYS_PTY_EXTENSIONS);
#endif

#ifdef _OPEN_SYS_SOCK_EXT
	printf("_OPEN_SYS_SOCK_EXT (%ld)\n", (long)_OPEN_SYS_SOCK_EXT);
#endif

#ifdef _OPEN_SYS_SOCK_EXT2
	printf("_OPEN_SYS_SOCK_EXT2 (%ld)\n", (long)_OPEN_SYS_SOCK_EXT2);
#endif

#ifdef _OPEN_SYS_SOCK_EXT3
	printf("_OPEN_SYS_SOCK_EXT3 (%ld)\n", (long)_OPEN_SYS_SOCK_EXT3);
#endif

#ifdef _OPEN_SYS_SOCK_EXT4
	printf("_OPEN_SYS_SOCK_EXT4 (%ld)\n", (long)_OPEN_SYS_SOCK_EXT4);
#endif

#ifdef _OPEN_SYS_SOCK_IPV6
	printf("_OPEN_SYS_SOCK_IPV6 (%ld)\n", (long)_OPEN_SYS_SOCK_IPV6);
#endif

#ifdef _OPEN_THREADS
	printf("_OPEN_THREADS defined: %ldL\n", (long)_OPEN_THREADS);
#endif

#ifdef _POSIX1_SOURCE
	printf("_POSIX1_SOURCE defined: %ldL\n", (long)_POSIX1_SOURCE);
#endif

#ifdef _SC_2_
	printf("_SC_2_ (%ld)\n", (long)_SC_2_);
#endif

#ifdef _SC_PASS_MAX
	printf("_SC_PASS_MAX defined: %ldL\n", (long)_SC_PASS_MAX);
#endif

#ifdef _SC_XOPEN_XCU_VERSION
	printf("_SC_XOPEN_XCU_VERSION defined: %ldL\n",
	       (long)_SC_XOPEN_XCU_VERSION);
#endif

#ifdef _SHARE_EXT_VARS
	printf("_SHARE_EXT_VARS (%ld)\n", (long)_SHARE_EXT_VARS);
#endif

#ifdef _SHR_
	printf("_SHR_ (%ld)\n", (long)_SHR_);
#endif

#ifdef _SHR_DAYLIGHT
	printf("_SHR_DAYLIGHT (%ld)\n", (long)_SHR_DAYLIGHT);
#endif

#ifdef _SHR_ENVIRON
	printf("_SHR_ENVIRON (%ld)\n", (long)_SHR_ENVIRON);
#endif

#ifdef _SHR_H_ERRNO
	printf("_SHR_H_ERRNO (%ld)\n", (long)_SHR_H_ERRNO);
#endif

#ifdef _SHR_LOC1
	printf("_SHR_LOC1 (%ld)\n", (long)_SHR_LOC1);
#endif

#ifdef _SHR_LOC2
	printf("_SHR_LOC2 (%ld)\n", (long)_SHR_LOC2);
#endif

#ifdef _SHR_LOCS
	printf("_SHR_LOCS (%ld)\n", (long)_SHR_LOCS);
#endif

#ifdef _SHR_OPTARG
	printf("_SHR_OPTARG (%ld)\n", (long)_SHR_OPTARG);
#endif

#ifdef _SHR_OPTERR
	printf("_SHR_OPTERR (%ld)\n", (long)_SHR_OPTERR);
#endif

#ifdef _SHR_OPTIND
	printf("_SHR_OPTIND (%ld)\n", (long)_SHR_OPTIND);
#endif

#ifdef _SHR_OPTOPT
	printf("_SHR_OPTOPT (%ld)\n", (long)_SHR_OPTOPT);
#endif

#ifdef _SHR_SIGNGAM
	printf("_SHR_SIGNGAM (%ld)\n", (long)_SHR_SIGNGAM);
#endif

#ifdef _SHR_TIMEZONE
	printf("_SHR_TIMEZONE (%ld)\n", (long)_SHR_TIMEZONE);
#endif

#ifdef _SHR_TZNAME
	printf("_SHR_TZNAME (%ld)\n", (long)_SHR_TZNAME);
#endif

#ifdef _SHR_T_ERRNO
	printf("_SHR_T_ERRNO (%ld)\n", (long)_SHR_T_ERRNO);
#endif

#ifdef _SHR__LOC1
	printf("_SHR__LOC1 (%ld)\n", (long)_SHR__LOC1);
#endif

#ifdef _TR1_C99
	printf("_TR1_C99 (%ld)\n", (long)_TR1_C99);
#endif

#ifdef _UNIX03_SOURCE
	printf("_UNIX03_SOURCE (%ld)\n", (long)_UNIX03_SOURCE);
#endif

#ifdef _UNIX03_THREADS
	printf("_UNIX03_THREADS (%ld)\n", (long)_UNIX03_THREADS);
#endif

#ifdef _UNIX03_WITHDRAWN
	printf("_UNIX03_WITHDRAWN (%ld)\n", (long)_UNIX03_WITHDRAWN);
#endif

#ifdef _VARARG_EXT_
	printf("_VARARG_EXT_ (%ld)\n", (long)_VARARG_EXT_);
#endif

#ifdef __IBM_DFP
	printf("__IBM_DFP (%ld)\n", (long)__IBM_DFP);
#endif

#ifdef __LIBASCII
	printf("__LIBASCII (%ld)\n", (long)__LIBASCII);
#endif

#ifdef __STDC_CONSTANT_MACROS
	printf("__STDC_CONSTANT_MACROS (%ld)\n", (long)__STDC_CONSTANT_MACROS);
#endif

#ifdef __STDC_FORMAT_MACROS
	printf("__STDC_FORMAT_MACROS (%ld)\n", (long)__STDC_FORMAT_MACROS);
#endif

#ifdef __STDC_LIMIT_MACROS
	printf("__STDC_LIMIT_MACROS (%ld)\n", (long)__STDC_LIMIT_MACROS);
#endif

#ifdef __STDC_WANT_DEC_FP__
	printf("__STDC_WANT_DEC_FP__ (%ld)\n", (long)__STDC_WANT_DEC_FP__);
#endif

	if (verbose) {
#ifdef  __linux__
		printf("__linux__ defined (%ld)\n", (long)__linux__);
#endif
	}

	return EXIT_SUCCESS;
}
