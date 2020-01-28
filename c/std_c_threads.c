/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* std_c_threads.c - example of C threads, atomics, thread_local variables */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */
/*
   This example reflects some of my understanding of modern C threads,
   atomics, and thread_local variables.
   As of this writing, using GCC we still need to explicitly link to
   pthreads, e.g.:

   gcc -o std_c_threads std_c_threads.c -lpthread

   My impression so far: the year is 2020 and it looks like glibc with C11
   threads and atomics support is finally available in standard linux dists
   and using the more portable standard C threading instead of pthreads isn't
   terrible ... unless I'm holding it wrong ... which maybe I am.
*/

#include <threads.h>		// thrd_create, thrd_join thread_local
#include <stdatomic.h>		// atomic_ulong atomic_fetch_add

#include <stdio.h>
#include <stdlib.h>		// rand malloc
#include <string.h>		// strlen
#include <sys/time.h>		// gettimeofday
#include <unistd.h>		// usleep

#define usec_per_sec (1000 * 1000)

// globals (considered harmful, yes)
atomic_ulong task_total_usec;

// thread locals (less bad than globals, still kind of smelly code)
thread_local unsigned long task_start_usec;
thread_local unsigned long task_end_usec;

// types
struct task_s {
	size_t task_id;
	double seconds_to_sleep;
};

// prototypes
static unsigned long time_in_usec(void);
static void die_on_thread_create_failure(int error, size_t our_id);

// example function to run as a thread
int task_do_sleep(struct task_s *task)
{
	printf("task %zu sleeping for %7.5f seconds\n", task->task_id,
	       task->seconds_to_sleep);

	// record start-time by assignment to a thread_local
	// there's no reason to use a thread_local in this case,
	// except to demonstrates the thread-local property
	task_start_usec = time_in_usec();

	int error = usleep(task->seconds_to_sleep * usec_per_sec);

	// record end-time assignment to a thread_local
	// again, only to demo thread_local-ness
	task_end_usec = time_in_usec();

	unsigned long duration_usec = task_end_usec - task_start_usec;

	// log the total time in an atomic variable
	atomic_fetch_add(&task_total_usec, duration_usec);

	double actual = duration_usec / (1.0 * usec_per_sec);
	double diff = actual - task->seconds_to_sleep;
	char buf[80];
	sprintf(buf, "task %zu slept for %7.5f seconds (diff %7.5f)",
		task->task_id, actual, diff);
	if (error) {
		sprintf(buf + strlen(buf), ", returning error: %d", error);
	}
	printf("%s\n", buf);

	return error;
}

int task_thread_func(void *context)
{
	struct task_s *task = (struct task_s *)context;
	return task_do_sleep(task);
}

int main(int argc, char **argv)
{
	size_t num_threads = argc > 1 ? (size_t)strtoul(argv[1], NULL, 10) : 10;


	// allocate some per-thread memory
	thrd_t *thread_ids = malloc(sizeof(thrd_t) * num_threads);
	struct task_s *contexts = malloc(sizeof(struct task_s) * num_threads);

	// initialize rand()
	srand(time(NULL));

	// initialize our global atomics
	// since we are initializing to 0, this is not needed
	atomic_store(&task_total_usec, 0);

	for (size_t i = 0; i < num_threads; ++i) {
		contexts[i].task_id = i;
		double fraction_of_a_second = rand() * (1.0 / RAND_MAX);
		contexts[i].seconds_to_sleep = fraction_of_a_second;
	}

	unsigned long begin = time_in_usec();
	printf("starting %zu threads\n", num_threads);
	for (size_t i = 0; i < num_threads; ++i) {
		void *context = &(contexts[i]);
		thrd_start_t thread_func = task_thread_func;
		thrd_t *thread_id = &(thread_ids[i]);

		int error = thrd_create(thread_id, thread_func, context);
		if (error) {
			die_on_thread_create_failure(error, i);
		}
	}

	printf("joining threads\n");
	for (size_t i = 0; i < num_threads; ++i) {
		thrd_join(thread_ids[i], NULL);
		printf("joined thread %zu\n", i);
	}
	unsigned long end = time_in_usec();

	double wallclock_secs = (end - begin) / (1.0 * usec_per_sec);
	double cpu_secs = atomic_load(&task_total_usec) / (1.0 * usec_per_sec);

	printf("done in %7.5f wallclock seconds (cpu seconds: %7.5f)\n",
	       wallclock_secs, cpu_secs);

	// Make_valgrind_happy
	free(thread_ids);
	free(contexts);

	return 0;
}

static unsigned long time_in_usec(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long time_in_micros = (usec_per_sec * tv.tv_sec) + tv.tv_usec;
	return time_in_micros;
}

// we don't really need this boiler-plate to be this verbose
static void die_on_thread_create_failure(int error, size_t our_id)
{
	switch (error) {
	case thrd_success:
		break;
	case thrd_nomem:
		fprintf(stderr, "thrd_create %zu returned thrd_nomem\n",
			our_id);
		exit(EXIT_FAILURE);
		break;
	case thrd_error:
		fprintf(stderr, "thrd_create %zu returned thrd_error\n",
			our_id);
		exit(EXIT_FAILURE);
		break;
	case thrd_timedout:
		fprintf(stderr,
			"thrd_create %zu returned unexpected thrd_timedout\n",
			our_id);
		exit(EXIT_FAILURE);
		break;
	case thrd_busy:
		fprintf(stderr,
			"thrd_create %zu returned unexpected thrd_busy\n",
			our_id);
		exit(EXIT_FAILURE);
		break;
	default:
		fprintf(stderr,
			"thrd_create %zu returned unexpected value %d\n",
			our_id, error);
		exit(EXIT_FAILURE);
		break;
	}
}
