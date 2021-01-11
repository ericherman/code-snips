/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* globdemo.c: demo glob usage */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct str_list {
	char **strs;
	size_t used;
	size_t len;
};
int str_list_add(struct str_list *l, const char *str);
void str_list_clear(struct str_list *l);

int get_paths(struct str_list *paths, const char *pattern)
{
	int (*errfunc)(const char *epath, int eerrno) = NULL;

	glob_t threads;

	int err = glob(pattern, GLOB_MARK, errfunc, &threads);
	if (err) {
		return 1;
	}

	for (size_t i = 0; i < threads.gl_pathc; ++i) {
		err += str_list_add(paths, threads.gl_pathv[i]);
	}

	globfree(&threads);

	return err;
}

int main(int argc, char **argv)
{
	struct str_list paths = { NULL, 0, 0 };

	const char *pidstr = argc > 1 ? argv[1] : "1";
	char pattern[FILENAME_MAX];

	strcpy(pattern, "/proc/");
	strcat(pattern, pidstr);
	strcat(pattern, "/task/*/stat");

	int err = get_paths(&paths, pattern);

	for (size_t i = 0; i < paths.used; ++i) {
		printf("%s\n", paths.strs[i]);
	}

	str_list_clear(&paths);

	return err;
}

int str_list_add(struct str_list *l, const char *str)
{
	if (!l) {
		return 1;
	}
	char *copy = strdup(str);
	if (!copy) {
		return 2;
	}
	if (l->used >= l->len) {
		size_t old_len = l->len;
		char **old_strs = l->strs;
		size_t new_len = old_len < 64 ? 64 : old_len * 2;
		char **new_strs = calloc(new_len, sizeof(char *));
		if (!new_strs) {
			free(copy);
			return 3;
		}
		l->strs = new_strs;
		l->len = new_len;
		for (size_t i = 0; i < old_len; ++i) {
			l->strs[i] = old_strs[i];
		}
		free(old_strs);
	}
	l->strs[l->used++] = copy;
	return 0;
}

void str_list_clear(struct str_list *l)
{
	if (!l || !l->strs) {
		return;
	}
	for (size_t i = 0; i < l->used; ++i) {
		free(l->strs[i]);
		l->strs[i] = NULL;
	}
	free(l->strs);
	l->strs = NULL;
	l->used = 0;
	l->len = 0;
}
