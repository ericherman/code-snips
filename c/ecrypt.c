/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* ecrypt.c: demo of not echoing a password, and using crypt_r */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */
/* cc ./ecrypt.c -o ecrypt -lcrypt */

#define _GNU_SOURCE
#include <crypt.h>		/* Link with -lcrypt */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#define Salt_buf_size 80
#define Key_buf_size 4096

#ifndef eprintf
#define eprintf(format, ...) \
	do { \
		fflush(stdout); /* avoid POSIX undefined stdout+stderr [1] */ \
		fprintf(stderr, format, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
	} while (0)
#endif

/* prototypes */
static char *fgets_nocrlf(char *buf, size_t len, FILE *stream);

/* functions */
char *fgetpass(char *buf, size_t len, FILE *stream)
{

	int fno = fileno(stream);

	struct termios orig;
	int err = tcgetattr(fno, &orig);
	if (err) {
		eprintf("tcgetattr returned error %d for %d?", err, fno);
		return NULL;
	}

	struct termios next = orig;
	next.c_lflag &= ~ECHO;
	err = tcsetattr(fno, TCSAFLUSH, &next);
	if (err) {
		eprintf("tcsetattr returned error %d for %d?", err, fno);
		return NULL;
	}

	char *str = fgets_nocrlf(buf, len, stream);

	err = tcsetattr(fno, TCSAFLUSH, &orig);
	if (err) {
		eprintf("reset tcsetattr returned error %d for %d?", err, fno);
		return NULL;
	}

	return str;
}

int main(void)
{
	printf("input salt: ");
	char raw_salt_buf[Salt_buf_size];
	char *raw_salt = fgets_nocrlf(raw_salt_buf, Salt_buf_size, stdin);
	if (!raw_salt) {
		exit(EXIT_FAILURE);
	}

	char salt[100];
	const unsigned sha512 = 6;
	snprintf(salt, 100, "$%u$%s$", sha512, raw_salt);

	int diff = 0;
	char key_buf[Key_buf_size];
	char *key = NULL;
	do {
		if (diff) {
			printf("inputs did not match\n");
		}
		printf(" input passphrase: ");
		key = fgetpass(key_buf, Key_buf_size, stdin);
		if (!key) {
			exit(EXIT_FAILURE);
		}
		printf("\n");

		printf("repeat passphrase: ");
		char key_buf2[Key_buf_size];
		char *key2 = fgetpass(key_buf2, Key_buf_size, stdin);
		if (!key2) {
			exit(EXIT_FAILURE);
		}
		printf("\n");

		diff = strncmp(key, key2, Key_buf_size);
	} while (diff);

	struct crypt_data data;
	/* data->initialized = 0; */
	memset(&data, 0x00, sizeof(struct crypt_data));
	char *result = crypt_r(key, salt, &data);

	printf("%s\n", result);

	return 0;
}

/* boring stuff */
static char *chomp_crlf(char *str, size_t max)
{
	if (!str) {
		return NULL;
	}
	for (size_t i = 0; i < max && str[i] != '\0'; ++i) {
		if (str[i] == '\r' || str[i] == '\n') {
			str[i] = '\0';
			return str;
		}
	}
	return str;
}

static char *fgets_nocrlf(char *buf, size_t len, FILE *stream)
{
	return chomp_crlf(fgets(buf, len, stream), len);
}

/* [1] https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_05_01 */
