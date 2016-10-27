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
	printf("__STDC__ defined\n");
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
	printf("__STDC_ANALYZABLE__ defined\n");
#endif

#ifdef  __STDC_LIB_EXT1__
	printf("__STDC_LIB_EXT1__ defined\n");
#endif

#ifdef  __STDC_VERSION__
#ifdef  __STDC_NO_THREADS__
	printf("__STDC_NO_THREADS__ defined\n");
#else
	if (verbose) {
		printf("__STDC_NO_THREADS__ not defined\n");
	}
#endif
#endif

#ifdef  __STDC_VERSION__
#ifdef  __STDC_NO_ATOMICS__
	printf("__STDC_NO_ATOMICS__ defined\n");
#else
	if (verbose) {
		printf("__STDC_NO_ATOMICS__ not defined\n");
	}
#endif
#endif

#ifdef  __STDC_IEC_559__
	printf("__STDC_IEC_559__ defined\n");
#endif

#ifdef  __STDC_IEC_559_COMPLEX__
	printf("__STDC_IEC_559_COMPLEX__ defined\n");
#endif

#ifdef  __STDC_VERSION__
#ifdef  __STDC_NO_COMPLEX__
	printf("__STDC_NO_COMPLEX__ defined\n");
#else
	if (verbose) {
		printf("__STDC_NO_COMPLEX__ not defined\n");
	}
#endif
#endif

#ifdef  __STDC_VERSION__
#ifdef  __STDC_NO_VLA__
	printf("__STDC_NO_VLA__ defined\n");
#else
	if (verbose) {
		printf("__STDC_NO_VLA__ not defined\n");
	}
#endif
#endif

#ifdef  __GNUC__
	printf("__GNUC__ defined\n");
#endif

#ifdef  __GNU__
	printf("__GNU__ defined\n");
#endif

#ifdef  __USE_GNU
	printf("__USE_GNU defined\n");
#endif

#ifdef  __GNU_LIBRARY__
	printf("__GNU_LIBRARY__ defined\n");
#endif

#ifdef _POSIX_SOURCE
	printf("_POSIX_SOURCE defined\n");
#endif

#ifdef _POSIX_C_SOURCE
	printf("_POSIX_C_SOURCE defined: %ldL\n", (long)_POSIX_C_SOURCE);
#else
	if (verbose) {
		printf("_POSIX_C_SOURCE not defined\n");
	}
#endif

#ifdef _ISOC99_SOURCE
	printf("_ISOC99_SOURCE defined\n");
#endif

#ifdef _ISOC11_SOURCE
	printf("_ISOC11_SOURCE defined\n");
#endif

#ifdef _XOPEN_SOURCE
	printf("_XOPEN_SOURCE defined: %d\n", _XOPEN_SOURCE);
#endif

#ifdef _XOPEN_SOURCE_EXTENDED
	printf("_XOPEN_SOURCE_EXTENDED defined\n");
#endif

#ifdef _LARGEFILE64_SOURCE
	printf("_LARGEFILE64_SOURCE defined\n");
#endif

#ifdef _FILE_OFFSET_BITS
	printf("_FILE_OFFSET_BITS defined: %d\n", _FILE_OFFSET_BITS);
#endif

#ifdef _BSD_SOURCE
	printf("_BSD_SOURCE defined\n");
#endif

#ifdef _SVID_SOURCE
	printf("_SVID_SOURCE defined\n");
#endif

#ifdef _DEFAULT_SOURCE
	printf("_DEFAULT_SOURCE defined\n");
#endif

#ifdef _ATFILE_SOURCE
	printf("_ATFILE_SOURCE defined\n");
#endif

#ifdef _GNU_SOURCE
	printf("_GNU_SOURCE defined\n");
#endif

#ifdef _REENTRANT
	printf("_REENTRANT defined\n");
#endif

#ifdef _THREAD_SAFE
	printf("_THREAD_SAFE defined\n");
#endif

#ifdef _FORTIFY_SOURCE
	printf("_FORTIFY_SOURCE defined\n");
#endif

#ifdef _ALL_SOURCE
	printf("_ALL_SOURCE\n");
#endif

#ifdef _ALL_SOURCE_NO_THREADS
	printf("_ALL_SOURCE_NO_THREADS\n");
#endif

#ifdef _FP_MODE_VARIABLE
	printf("_FP_MODE_VARIABLE\n");
#endif

#ifdef _IEEEV1_COMPATIBILITY
	printf("_IEEEV1_COMPATIBILITY\n");
#endif

#ifdef _LARGE_FILES
	printf("_LARGE_FILES\n");
#endif

#ifdef _LARGE_MEM
	printf("_LARGE_MEM\n");
#endif

#ifdef _LARGE_TIME_API
	printf("_LARGE_TIME_API\n");
#endif

#ifdef _LONGMAP
	printf("_LONGMAP\n");
#endif

#ifdef _MSE_PROTOS
	printf("_MSE_PROTOS\n");
#endif

#ifdef _NOISOC99_SOURCE
	printf("_NOISOC99_SOURCE\n");
#endif

#ifdef _OE_SOCKETS
	printf("_OE_SOCKETS\n");
#endif

#ifdef _OPEN_DEFAULT
	printf("_OPEN_DEFAULT\n");
#endif

#ifdef _OPEN_MSGQ_EXT
	printf("_OPEN_MSGQ_EXT\n");
#endif

#ifdef _OPEN_SOURCE
	printf("_OPEN_SOURCE defined:%ldL\n", (long)_OPEN_SOURCE);
#endif

#ifdef _OPEN_SYS
	printf("_OPEN_SYS\n");
#endif

#ifdef _OPEN_SYS_DIR_EXT
	printf("_OPEN_SYS_DIR_EXT\n");
#endif

#ifdef _OPEN_SYS_EXT
	printf("_OPEN_SYS_EXT\n");
#endif

#ifdef _OPEN_SYS_FILE_EXT
	printf("_OPEN_SYS_FILE_EXT\n");
#endif

#ifdef _OPEN_SYS_IF_EXT
	printf("_OPEN_SYS_IF_EXT\n");
#endif

#ifdef _OPEN_SYS_IPC_EXTENSIONS
	printf("_OPEN_SYS_IPC_EXTENSIONS\n");
#endif

#ifdef _OPEN_SYS_MAP_EXTENTION
	printf("_OPEN_SYS_MAP_EXTENTION\n");
#endif

#ifdef _OPEN_SYS_MUTEX_EXT
	printf("_OPEN_SYS_MUTEX_EXT\n");
#endif

#ifdef _OPEN_SYS_PTY_EXTENSIONS
	printf("_OPEN_SYS_PTY_EXTENSIONS\n");
#endif

#ifdef _OPEN_SYS_SOCK_EXT
	printf("_OPEN_SYS_SOCK_EXT\n");
#endif

#ifdef _OPEN_SYS_SOCK_EXT2
	printf("_OPEN_SYS_SOCK_EXT2\n");
#endif

#ifdef _OPEN_SYS_SOCK_EXT3
	printf("_OPEN_SYS_SOCK_EXT3\n");
#endif

#ifdef _OPEN_SYS_SOCK_EXT4
	printf("_OPEN_SYS_SOCK_EXT4\n");
#endif

#ifdef _OPEN_SYS_SOCK_IPV6
	printf("_OPEN_SYS_SOCK_IPV6\n");
#endif

#ifdef _OPEN_THREADS
	printf("_OPEN_THREADS defined: %ldL\n", (long)_OPEN_THREADS);
#endif

#ifdef _POSIX1_SOURCE
	printf("_POSIX1_SOURCE defined: %ldL\n", (long)_POSIX1_SOURCE);
#endif

#ifdef _SC_2_
	printf("_SC_2_\n");
#endif

#ifdef _SC_PASS_MAX
	printf("_SC_PASS_MAX defined: %ldL\n", (long)_SC_PASS_MAX);
#endif

#ifdef _SC_XOPEN_XCU_VERSION
	printf("_SC_XOPEN_XCU_VERSION defined: %ldL\n",
	       (long)_SC_XOPEN_XCU_VERSION);
#endif

#ifdef _SHARE_EXT_VARS
	printf("_SHARE_EXT_VARS\n");
#endif

#ifdef _SHR_
	printf("_SHR_\n");
#endif

#ifdef _SHR_DAYLIGHT
	printf("_SHR_DAYLIGHT\n");
#endif

#ifdef _SHR_ENVIRON
	printf("_SHR_ENVIRON\n");
#endif

#ifdef _SHR_H_ERRNO
	printf("_SHR_H_ERRNO\n");
#endif

#ifdef _SHR_LOC1
	printf("_SHR_LOC1\n");
#endif

#ifdef _SHR_LOC2
	printf("_SHR_LOC2\n");
#endif

#ifdef _SHR_LOCS
	printf("_SHR_LOCS\n");
#endif

#ifdef _SHR_OPTARG
	printf("_SHR_OPTARG\n");
#endif

#ifdef _SHR_OPTERR
	printf("_SHR_OPTERR\n");
#endif

#ifdef _SHR_OPTIND
	printf("_SHR_OPTIND\n");
#endif

#ifdef _SHR_OPTOPT
	printf("_SHR_OPTOPT\n");
#endif

#ifdef _SHR_SIGNGAM
	printf("_SHR_SIGNGAM\n");
#endif

#ifdef _SHR_TIMEZONE
	printf("_SHR_TIMEZONE\n");
#endif

#ifdef _SHR_TZNAME
	printf("_SHR_TZNAME\n");
#endif

#ifdef _SHR_T_ERRNO
	printf("_SHR_T_ERRNO\n");
#endif

#ifdef _SHR__LOC1
	printf("_SHR__LOC1\n");
#endif

#ifdef _TR1_C99
	printf("_TR1_C99\n");
#endif

#ifdef _UNIX03_SOURCE
	printf("_UNIX03_SOURCE\n");
#endif

#ifdef _UNIX03_THREADS
	printf("_UNIX03_THREADS\n");
#endif

#ifdef _UNIX03_WITHDRAWN
	printf("_UNIX03_WITHDRAWN\n");
#endif

#ifdef _VARARG_EXT_
	printf("_VARARG_EXT_\n");
#endif

#ifdef __IBM_DFP
	printf("__IBM_DFP\n");
#endif

#ifdef __LIBASCII
	printf("__LIBASCII\n");
#endif

#ifdef __STDC_CONSTANT_MACROS
	printf("__STDC_CONSTANT_MACROS\n");
#endif

#ifdef __STDC_FORMAT_MACROS
	printf("__STDC_FORMAT_MACROS\n");
#endif

#ifdef __STDC_LIMIT_MACROS
	printf("__STDC_LIMIT_MACROS\n");
#endif

#ifdef __STDC_WANT_DEC_FP__
	printf("__STDC_WANT_DEC_FP__\n");
#endif

	if (verbose) {
#ifdef  __linux__
		printf("__linux__ defined\n");
#endif
	}

	return EXIT_SUCCESS;
}
