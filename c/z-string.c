/* This demonstrates using C11's `_Generic` and `_Static_assert` to get
 * type-safety at compile-time.
 *
 * To run the example, showing the runtime segfault:
 * cc -DDEMO_BROKEN_CODE -DSKIP_STATIC_ASSERTS c/z-string.c && ./a.out
 *
 * To run the example showing _Static_assert compile-time error message:
 * cc -DDEMO_BROKEN_CODE c/z-string.c
 *
 * To run the example without the bug:
 * cc c/z-string.c && ./a.out
 *
 * The example is intended to be somewhat realistic (library functions
 * to create string buffers that are compatible with traditional C char
 * pointers) to illustrate a desire for the compile-time type-safety.
 * Noteworthy is that z_string_delete can take either a `char **` or a
 * `const char **` and thus is declared to take a `void *` which can
 * be compile-time checked to be of one of these two types.
 *
 * z_string buffers have the declared max_len at the front of the
 * allocated buffer, followed by the zero-terminated string. The pointer
 * returned from z_string_new is just the zero-terminated string
 * portion, so the other functions look in front of the zero-terminated
 * string to find the declared size.
 *
 * The size can be read with the z_string_max_len function, which
 * returns the space available for character content; the space
 * available is exclusive of the extra space which was allocated for the
 * NULL terminator and the max_len information.
 *
 * If a string is provided to the constructor, the buffer will be filled
 * with the contents of that string.
 */

#ifndef Z_STRING_H
#define Z_STRING_H 1

#include <stddef.h>

/* Returns a `char *` of size `max_len`, plus 1 for the NULL terminator
 * if `str` is non-null, the contents (up to size max_len) are copied
 * into the pointer.
 * The resulting pointer must be `free`-ed with `z_string_delete`. */
char *z_string_size_new(size_t max_len, const char *str);

/* Returns a copy of the string passed in.
 * The resulting pointer must be `free`-ed with `z_string_delete`. */
#define z_string_new(str) \
	z_string_size_new(str ? strlen(str) : 0, str)

/* A type-unsafe function which assumes the pointer passed in is a
 * pointer to a pointer allocated via `z_string_size_new`.
 * After `free`-ing the z_string pointer, the passed in pointer is
 * set to `NULL`.
 * Use `z_string_delete` for more type safety. */
void z_string_delete_ref(void *ref);

#ifdef SKIP_STATIC_ASSERTS
#define z_string_delete(ref) z_string_delete_ref((void *)ref)
#else

#ifndef Static_is_type
#define Static_is_type(x, T) _Generic((x), T: 1, default: 0)	/* C11 */
#endif

/* A more type-safe wrapper for `z_string_delete_ref`.
 * At compile-time, `ref` is checked to be of type `char **` or
 * `const char **` before passing to `z_string_delete_ref`.
 *
 * The bit of type-safety provided will not protect against passing
 * a char * that was not allocated by z_string, however it will
 * protect from the likely real-world error of passing the `char *`
 * rather than the address of the 'char *' or `const char *` to a
 * function which is `void *` in order to deal with the two similar
 * types of pointers. */
#define z_string_delete(ref) do { \
	_Static_assert(Static_is_type(ref, char **) /* C11 */ \
		    || Static_is_type(ref, const char **) \
		     , #ref " is not a char **"); \
	z_string_delete_ref((void *)ref); \
} while (0)
#endif /* SKIP_STATIC_ASSERTS */

/* When passed a `char *` or `const char *` allocated via
 * `z_string_size_new`, returns the max_len that was originally
 * provided. */
size_t z_string_max_len(const char *str);

/* *************************************************************** */
/* Memory allocation function pointers used by z_string functions, */
/* which default to `stdlib.h` `calloc` and `free`.
 * Can be customized, but are global. */
extern void *(*z_string_calloc)(size_t nmemb, size_t size);
extern void (*z_string_free)(void *ptr);

#endif /* Z_STRING_H */

/* z-string-demo.c */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	printf("const char *s1 = z_string_new(\"foo\");\n");
	const char *s1 = z_string_new("foo");
	assert(s1);
	printf("s1 == '%s'\n", s1);
	printf("z_string_max_len(s1) == %zu\n", z_string_max_len(s1));
#ifdef DEMO_BROKEN_CODE
	printf("z_string_delete(s1); /* this is a bug! */\n");
	z_string_delete(s1);
#else
	printf("z_string_delete(&s1);\n");
	z_string_delete(&s1);
#endif

	printf("s1 == '%s'\n", s1);
	assert(!s1);

	printf("\n");

	printf("char *s2 = z_string_size_new(40, NULL);\n");
	char *s2 = z_string_size_new(40, NULL);
	assert(s2);
	printf("s2 == '%s'\n", s2);
	printf("z_string_max_len(s2) == %zu\n", z_string_max_len(s2));
	printf("z_string_delete(&s2);\n");
	z_string_delete(&s2);
	printf("s2 == '%s'\n", s2);
	assert(!s2);

	return EXIT_SUCCESS;
}

/* z-string.c */
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void *(*z_string_calloc)(size_t nmemb, size_t size) = calloc;
void (*z_string_free)(void *ptr) = free;
void *(*z_string_memcpy)(void *restrict dest, const void *restrict src,
			 size_t n) = memcpy;

char *z_string_size_new(size_t max_len, const char *str)
{
	assert(max_len < (SIZE_MAX - (sizeof(size_t) + 1)));

	/* allocate enough space for to record the size and the NULL */
	const size_t buf_size = sizeof(size_t) + max_len + 1;
	size_t *buf = (size_t *)z_string_calloc(1, buf_size);
	if (!buf) {
		return NULL;
	}

	/* record the size in the start of the buffer */
	buf[0] = max_len;

	/* advance the buffer past the size data */
	size_t *remainder = &(buf[1]);

	/* cast to a string that can be return to the caller */
	char *s = (char *)remainder;

	/* copy the contents in, if provied */
	if (str) {
		size_t len = strlen(str);
		if (len > max_len) {
			len = max_len;
		}
		z_string_memcpy(s, str, len);
	}

	return s;
}

void z_string_delete_ref(void *ref)
{
	/* cast to a pointer to char pointer */
	const char **pstr = (const char **)ref;

	/* extract the char pointer that was allocated by z_string_size_new */
	const char *str = *pstr;

	/* cast to a size_t pointer so we can go index by size_t sizes */
	size_t *remainder = (size_t *)str;

	/* go backwards one to get to the location of the original buffer */
	size_t *buf = &(remainder[-1]);

	/* free the buffer that was allocated */
	z_string_free(buf);

	/* set the pointer to NULL, since it is no longer valid */
	*pstr = NULL;
}

size_t z_string_max_len(const char *str)
{
	/* cast to a size_t pointer so we can go index by size_t sizes */
	const size_t *remainder = (const size_t *)str;

	/* go backwards one to get to the location of the original buffer */
	const size_t *buf = &(remainder[-1]);

	/* extract the size data that was stored there */
	size_t buf_size = buf[0];

	return buf_size;
}
