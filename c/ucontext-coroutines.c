/* ucontext-coroutines.c : coroutines using POSIX ucontext.h functions */

#include <ucontext.h>		/* getcontext makecontext swapcontext */

void die(const char *file, int line, const char *func, const char *msg);

#define swapcontext_or_die(from, to) do { \
	if (swapcontext(from, to) == -1) { \
		die(__FILE__, __LINE__, __func__, "swapcontext"); \
	} \
} while (0)

void make_coroutine(ucontext_t *uctx_return_to, ucontext_t *uctx_coroutine,
		    char *stack, size_t stack_size,
		    void (*pfunc)(void *func_ctx), void *func_ctx)
{
	/* The function signature of makecontext is archaic C:
	 *
	 * void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...)
	 *
	 * It is no longer allowed to decalre function pointers with empty
	 * arguments, but POSIX pragmatically elects to keep this.
	 *
	 * Since the actual function pointer does not use that archaic
	 * signature, so we cast it to that style, though it may result
	 * in a compiler warning: */
	void (*myfunc)() = pfunc;	/* I want my funk uncut */

	if (getcontext(uctx_coroutine) == -1) {
		die(__FILE__, __LINE__, __func__, "getcontext");
	}

	uctx_coroutine->uc_link = uctx_return_to;
	uctx_coroutine->uc_stack.ss_sp = stack;
	uctx_coroutine->uc_stack.ss_size = stack_size;

	makecontext(uctx_coroutine, myfunc, 1, func_ctx);
}

#include <stdio.h>
#include <stdlib.h>

struct our_coroutine_contxt {
	int *cnt;
	ucontext_t *us;
	ucontext_t *a;
	ucontext_t *b;
};

void coroutine1(void *user_context)
{
	struct our_coroutine_contxt *context = user_context;

	printf("coroutine1: started\n");
	while (*context->cnt) {
		printf("coroutine1: context->cnt: %d\n", *context->cnt);
		if (*context->cnt % 4 == 0) {
			printf("coroutine1: swap to 2\n");
			swapcontext_or_die(context->us, context->a);
		} else if (*context->cnt % 3 == 0) {
			printf("coroutine1: swap to 3\n");
			swapcontext_or_die(context->us, context->b);
		}
		--(*context->cnt);
	}
	printf("coroutine1: done\n");
}

void coroutine2(void *user_context)
{
	struct our_coroutine_contxt *context = user_context;

	printf("coroutine2: started\n");
	while (*context->cnt) {
		--(*context->cnt);
		printf("coroutine2: *context->cnt: %d\n", *context->cnt);
		if (*context->cnt % 3 == 0) {
			printf("coroutine2: swap to 3\n");
			swapcontext_or_die(context->us, context->b);
		} else if (*context->cnt % 2 == 0) {
			printf("coroutine2: swap to 1\n");
			swapcontext_or_die(context->us, context->a);
		}
	}
	printf("coroutine2: done\n");
}

void coroutine3(void *user_context)
{
	struct our_coroutine_contxt *context = user_context;

	printf("coroutine3: started\n");
	while (*context->cnt) {
		printf("coroutine3: *context->cnt: %d\n", *context->cnt);
		if (*context->cnt % 2 == 0) {
			printf("coroutine3: swap to 2\n");
			swapcontext_or_die(context->us, context->b);
		} else {
			printf("coroutine3: swap to 1\n");
			swapcontext_or_die(context->us, context->a);
		}
	}
	printf("coroutine3: done\n");
}

#define Coroutine_stack_size 16384
int main(int argc, char **argv)
{
	char coroutine1_stack[Coroutine_stack_size];
	char coroutine2_stack[Coroutine_stack_size];
	char coroutine3_stack[Coroutine_stack_size];
	struct our_coroutine_contxt context1;
	struct our_coroutine_contxt context2;
	struct our_coroutine_contxt context3;
	int our_count;
	ucontext_t uctx_main, uctx_cr1, uctx_cr2, uctx_cr3;

	context1.cnt = &our_count;
	context1.us = &uctx_cr1;
	context1.a = &uctx_cr2;
	context1.b = &uctx_cr3;

	context2.cnt = &our_count;
	context2.us = &uctx_cr2;
	context2.a = &uctx_cr1;
	context2.b = &uctx_cr3;

	context3.cnt = &our_count;
	context3.us = &uctx_cr3;
	context3.a = &uctx_cr1;
	context3.b = &uctx_cr2;

	make_coroutine(&uctx_main, &uctx_cr1, coroutine1_stack,
		       Coroutine_stack_size, coroutine1, &context1);
	make_coroutine(&uctx_cr1, &uctx_cr2, coroutine2_stack,
		       Coroutine_stack_size, coroutine2, &context2);
	make_coroutine(&uctx_cr2, &uctx_cr3, coroutine3_stack,
		       Coroutine_stack_size, coroutine3, &context3);

	our_count = argc > 1 ? abs(atoi(argv[1])) : 10;

	printf("main: swapcontext(&uctx_main, &uctx_cr1)\n");
	swapcontext_or_die(&uctx_main, &uctx_cr1);

	printf("main: exiting\n");
	exit(EXIT_SUCCESS);
}

void die(const char *file, int line, const char *func, const char *msg)
{
	fprintf(stderr, "%s +%d %s(): ", file, line, func);
	perror(msg);
	exit(EXIT_FAILURE);
}
