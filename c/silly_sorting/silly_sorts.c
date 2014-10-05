#include "silly_sorts.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

typedef struct int_list_t_ {
	struct int_list_t_ *next_node;
	int val;
} int_list_t;

typedef struct int_list_head_t_ {
	int_list_t *first_node;
} int_list_head_t;

typedef struct pthread_push_val_context_t_ {
	int_list_head_t *head;
	pthread_mutex_t *mutex;
	int val;
} pthread_push_val_context_t;

void *pthread_push_val(void *arg)
{
	pthread_push_val_context_t *ctx;
	int_list_head_t *head;
	int_list_t *node;

	ctx = (pthread_push_val_context_t *) arg;

	head = ctx->head;

	sleep(ctx->val);

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

void sleep_sort(int *elements, size_t num_elements)
{
	size_t i;
	int rv;
	pthread_push_val_context_t *ctx;
	pthread_t threads[num_elements];
	int_list_head_t *int_list;
	int_list_t *node;
	pthread_mutex_t mutex;
	const pthread_mutexattr_t *attr = NULL;

	int_list = (int_list_head_t *) malloc(sizeof(int_list_head_t));
	if (!int_list) {
		exit(EXIT_FAILURE);
	}
	int_list->first_node = NULL;

	rv = pthread_mutex_init(&mutex, attr);
	if (rv) {
		fprintf(stderr, "pthread_mutex_init() returned: %d\n", rv);
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < num_elements; i++) {
		ctx = (pthread_push_val_context_t *)
		    malloc(sizeof(pthread_push_val_context_t));
		ctx->mutex = &mutex;
		ctx->head = int_list;
		ctx->val = elements[i];

		rv = pthread_create(&(threads[i]), NULL, pthread_push_val, ctx);
		if (rv) {
			fprintf(stderr, "pthread_create() returned: %d\n", rv);
			exit(EXIT_FAILURE);
		}
	}

	for (i = 0; i < num_elements; i++) {
		pthread_join(threads[i], NULL);
	}

	if (int_list->first_node) {
		node = int_list->first_node;
		for (i = 0; i < num_elements; i++) {
			elements[i] = node->val;
			node = node->next_node;
		}

		while (int_list->first_node->next_node) {
			node = int_list->first_node->next_node;
			int_list->first_node->next_node = node->next_node;
			free(node);
		}
		free(int_list->first_node);
	}
	free(int_list);
	rv = pthread_mutex_destroy(&mutex);
}

void shuffle(int *elements, size_t num_elements)
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

void random_sort(int *elements, size_t num_elements)
{
	size_t i;
	int last;
	int sorted = 0;

	while (!sorted) {
		shuffle(elements, num_elements);
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
