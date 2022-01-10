/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* anon-stack-alloc.c creating "stack_new" functions for known structs */
/* Copyright (C) 2022 Eric Herman <eric@freesa.org> */

#include <stddef.h>

/* assuming we know can create structs on the stack,
 * meaning the definition is known, not opaque */
struct sbuf {
	size_t size;
	size_t start;
	size_t end;
	char *str;
};

struct sbuf *sbuf_init(struct sbuf *sb, char *buf, size_t len);

/* then we can create pointers to anonymous instances */
/* so we can pass it to an init function designed to be called after a malloc */
/* and create a handy "stack_new()" function for stack allocation */
#define sbuf_stack_new(buf, len) \
	sbuf_init(&((struct sbuf) {0,0,0,NULL}), buf, len)

/* with no need to call free() for the pointer, because no malloc() */
/* (naturally, this only makes sense for objects which do not "escape" and */
/* which are not too large) */

/* In truth the gain here is *very* small, compared to the confusion it can
 * potentially create, thus we should instead create the struct on the stack,
 * and also create a pointer to it. */

int main(void)
{
	const size_t buf_len = 80;
	char buf[buf_len];
	struct sbuf *sb = sbuf_stack_new(buf, buf_len);
	return sb->size == buf_len ? 0 : 1;
}

#include <string.h>

struct sbuf *sbuf_clear(struct sbuf *sb)
{
	sb->start = 0;
	sb->end = 0;
	memset(sb->str, 0x00, sb->size);
	return sb;
}

struct sbuf *sbuf_init(struct sbuf *sb, char *buf, size_t len)
{
	sb->size = len;
	sb->str = buf;
	return sbuf_clear(sb);
}
