/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* setjmp-try-catch.c : demo of setjump/longjump */
/* Copyright (C) 2022 Eric Herman <eric@freesa.org> */

/* to try it out:
   cc setjmp-try-catch.c -o ./setjmp-try-catch
   ./setjmp-try-catch
   ./setjmp-try-catch 1
   ./setjmp-try-catch 2
 */

/*
 * This is _not_ a serious implementation of exception handling, rather
 * it sketches out how setjmp and longjmp work in a way which gives a
 * rough idea of the utility of setjump/longjump.
 */

#include <setjmp.h>

struct try_context {
#if _POSIX_C_SOURCE
	sigjmp_buf jbuf;
#else
	jmp_buf jbuf;
#endif
	const char *file;
	const char *func;
	int line;
	int error;
	const char *msg;
};

/* embedded is unlikely to have/want threads
 * but if we're hosted and have a modern C, we typically want a
 * _Thread_local for "global-like" variables, which means we
 * will have a malloc/free behind the scenes */
#if ((__STDC_HOSTED__) && (__STDC_VERSION__ >= 201112L))
_Thread_local
#endif
struct try_context tctx;	// could be a list

#if _POSIX_C_SOURCE
static int savesigs = 1;
#define Try() (!sigsetjmp(tctx.jbuf, savesigs))
#else
#define Try() (!setjmp(tctx.jbuf))
#endif

void throw_it(const char *file, const char *func, int line,
	      int error, const char *msg)
{
	tctx.file = file;
	tctx.func = func;
	tctx.line = line;
	tctx.error = error;
	tctx.msg = msg;

#if _POSIX_C_SOURCE
	siglongjmp(tctx.jbuf, tctx.error);
#else
	longjmp(tctx.jbuf, tctx.error);
#endif
}

#define Throw(err, msg) throw_it(__FILE__, __func__, __LINE__, err, msg)

#include <stdio.h>
void things(int x)
{
	printf("start things\n");

	if (x == 1) {
		Throw(x, "argument is one");
	}
	if (x) {
		Throw(x, "argument is non-zero");
	}

	printf("end things\n");
}

#include <stdlib.h>
int main(int argc, char **argv)
{
	int x = argc > 1 ? atoi(argv[1]) : 0;

	if (Try()) {
		things(x);
	} else if (tctx.error == 1) {
		fprintf(stderr, "%s +%d %s() threw '%s' error\n",
			tctx.file, tctx.line, tctx.func, tctx.msg);
		return 1;
	} else {
		fprintf(stderr, "%s +%d %s() threw error %d: '%s'\n",
			tctx.file, tctx.line, tctx.func, tctx.error, tctx.msg);
		return 1;
	}

	return 0;
}
