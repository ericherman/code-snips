#include "silly_sorts.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

enum reorder { SORT, SHUFFLE };

typedef struct int_list_t_ {
	struct int_list_t_ *next_node;
	int val;
} int_list_t;

typedef struct int_list_head_t_ {
	int_list_t *first_node;
} int_list_head_t;

typedef struct pthread_push_context_t_ {
	int_list_head_t *head;
	pthread_mutex_t *mutex;
	int val;
	unsigned int sleep_time;
} pthread_push_context_t;

void *_pthread_push(void *arg)
{
	pthread_push_context_t *ctx;
	int_list_head_t *head;
	int_list_t *node;

	ctx = (pthread_push_context_t *) arg;

	head = ctx->head;

	sleep(ctx->sleep_time);

	pthread_mutex_lock(ctx->mutex);
	if (head->first_node == NULL) {
		node = (int_list_t *) malloc(sizeof(int_list_t));
		node->next_node = NULL;

		head->first_node = node;
	} else {
		node = head->first_node;
		while (node->next_node) {
			node = node->next_node;
		}
		node->next_node = (int_list_t *) malloc(sizeof(int_list_t));
		node->next_node->next_node = NULL;

		node = node->next_node;
	}

	node->val = ctx->val;
	pthread_mutex_unlock(ctx->mutex);

	free(arg);

	return NULL;
}

void _sleep_reorder(enum reorder order, int *elements, size_t num_elements)
{
	size_t i;
	int rv;
	pthread_push_context_t *ctx;
	pthread_t *threads;
	int_list_head_t *pos_list, *neg_list;
	int_list_t *node;
	pthread_mutex_t *mutex;
	const pthread_mutexattr_t *attr = NULL;

	threads = (pthread_t *) malloc(num_elements * sizeof(pthread_t));
	if (!threads) {
		exit(EXIT_FAILURE);
	}

	pos_list = (int_list_head_t *) malloc(sizeof(int_list_head_t));
	if (!pos_list) {
		exit(EXIT_FAILURE);
	}
	pos_list->first_node = NULL;

	neg_list = (int_list_head_t *) malloc(sizeof(int_list_head_t));
	if (!neg_list) {
		exit(EXIT_FAILURE);
	}
	neg_list->first_node = NULL;

	mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	if (!mutex) {
		exit(EXIT_FAILURE);
	}
	rv = pthread_mutex_init(mutex, attr);
	if (rv) {
		fprintf(stderr, "pthread_mutex_init() returned: %d\n", rv);
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < num_elements; i++) {
		ctx = (pthread_push_context_t *)
		    malloc(sizeof(pthread_push_context_t));
		ctx->mutex = mutex;

		ctx->val = elements[i];

		switch (order) {
		case SHUFFLE:
			ctx->head = pos_list;
			ctx->sleep_time = rand() % num_elements;
			break;
		case SORT:
			if (elements[i] >= 0) {
				ctx->head = pos_list;
				ctx->sleep_time = elements[i];
			} else {
				ctx->head = neg_list;
				ctx->sleep_time = -1 * elements[i];
			}
			break;
		}

		rv = pthread_create(&(threads[i]), NULL, _pthread_push, ctx);
		if (rv) {
			fprintf(stderr, "pthread_create() returned: %d\n", rv);
			exit(EXIT_FAILURE);
		}
	}

	for (i = 0; i < num_elements; i++) {
		pthread_join(threads[i], NULL);
	}
	free(threads);
	rv = pthread_mutex_destroy(mutex);
	if (rv) {
		fprintf(stderr, "pthread_mutex_destroy() returned: %d\n", rv);
	}
	free(mutex);

	while (neg_list->first_node) {
		node = neg_list->first_node;
		neg_list->first_node = node->next_node;
		node->next_node = pos_list->first_node;
		pos_list->first_node = node;
	}
	free(neg_list);

	if (pos_list->first_node) {
		node = pos_list->first_node;
		for (i = 0; i < num_elements; i++) {
			elements[i] = node->val;
			node = node->next_node;
		}

		while (pos_list->first_node->next_node) {
			node = pos_list->first_node->next_node;
			pos_list->first_node->next_node = node->next_node;
			free(node);
		}
		free(pos_list->first_node);
	}
	free(pos_list);
}

void sleep_sort(int *elements, size_t num_elements)
{
	_sleep_reorder(SORT, elements, num_elements);
}

void _sleep_shuffle(int *elements, size_t num_elements)
{
	_sleep_reorder(SHUFFLE, elements, num_elements);
}

void _shuffle(int *elements, size_t num_elements)
{
	size_t i, pos;
	int swap;
	for (i = 0; i < num_elements; i++) {
		pos = rand() % num_elements;
		swap = elements[i];
		elements[i] = elements[pos];
		elements[pos] = swap;
	}
}

void _random_sort(void (*shuffle_func) (int *elements, size_t num_elements),
		  int *elements, size_t num_elements)
{
	size_t i;
	int last;
	int sorted = 0;

	while (!sorted) {
		shuffle_func(elements, num_elements);
		sorted = 1;
		last = INT_MIN;
		for (i = 0; i < num_elements; i++) {
			if (elements[i] < last) {
				sorted = 0;
			}
			last = elements[i];
		}
	}
}

void random_sort(int *elements, size_t num_elements)
{
	_random_sort(_shuffle, elements, num_elements);
}

void random_sleep_sort(int *elements, size_t num_elements)
{
	_random_sort(_sleep_shuffle, elements, num_elements);
}
