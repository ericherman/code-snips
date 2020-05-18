/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* ecrypt.c: demo of not echoing a password, and using crypt_r */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */
/* cc ./ecrypt.c -o ecrypt -lcrypt */

/*
 * To test against your own passwd, get your salt:
 *
 *	sudo grep $USER /etc/shadow | cut -f2 -d':' | cut -d'$' -f3
 *
 * Then run the program with that salt and your password then compare
 * results with your full password data:
 *
 *	sudo grep $USER /etc/shadow | cut -f2 -d':'
 *
 */

#define _GNU_SOURCE
#include <crypt.h>		/* Link with -lcrypt */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <termios.h>

/*
 * Alpha-numeric random strings can be nice as they are easy to print,
 * however selecting a string from only the 62 alpha-numeric values
 * contained within the 256 possible bytes values greatly reduces the
 * the number of possible combinations.
 *
 * Eight alpha-numeric characters has fewer possible combinations than
 * six 256-value bytes has:
 *
 *    62^08:      218340105584896
 *   256^06:      281474976710656
 *
 *    62^10:   839299365868340224
 *   256^08: 18446744073709551616
 *    62^11: 52036560683837093888
 *
 * RFC#8018 describes using an 8 byte salt:
 * https://tools.ietf.org/html/rfc8018#section-4
 *
 * Thus, to equal-or-exceed the possible combinations of 8 octets, use
 * 11 alpha-numeric characters for salts:
 */
#define Suggested_salt_len 11
#define Max_salt_len 16

#define Salt_buf_size (Max_salt_len + 1 + 1)
#define Crypt_salt_buf_size (Salt_buf_size + 8)
#define Random_read_size (Max_salt_len * 4)
#define Passphrase_max_len (4 * 1024)

/* see the "Notes" section of "man 3 crypt" for glibc crypt_r
 * algorithm options */
#define CRYPT_MD5 '1'
#define CRYPT_SHA256 '5'
#define CRYPT_SHA512 '6'

#if !(Salt_buf_size > Suggested_salt_len)
#error Salt_buf_size must be grater than Suggested_salt_len
#endif

#ifndef eprintf
#define eprintf(format, ...) \
	do { \
		fflush(stdout); /* avoid POSIX undefined stdout+stderr [1] */ \
		fprintf(stderr, format, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
	} while (0)
#endif

/* prototypes */
char *chomp_crlf(char *str, size_t max);
void getpass(char *buf, size_t len, int confirm);
void getrandom_alphanumeric(char *buf, size_t len);
char *fgetpass(char *buf, size_t len, FILE *stream);
int is_english_alphanumeric(char c);

/* functions */
int main(void)
{
	printf("salts may be up to %u characters excluding '$' and ':'\n",
	       Max_salt_len);
	char plain_salt[Salt_buf_size];
	getrandom_alphanumeric(plain_salt, Suggested_salt_len + 1);
	printf("salt suggestion: %s\n", plain_salt);

	printf("input salt: ");
	char *r = fgets(plain_salt, Salt_buf_size, stdin);
	if (!r) {
		eprintf("fgets returned NULL reading buf of %zu?",
			Salt_buf_size);
		exit(EXIT_FAILURE);
	}
	chomp_crlf(plain_salt, Salt_buf_size);

	const char algo = CRYPT_SHA512;
	char algo_salt[Crypt_salt_buf_size];
	snprintf(algo_salt, Crypt_salt_buf_size, "$%c$%s$", algo, plain_salt);

	struct crypt_data data;
	/* data->initialized = 0; */
	memset(&data, 0x00, sizeof(struct crypt_data));
	char *encrypted = NULL;
	{
		int confirm = 1;
		char plaintext_passphrase[Passphrase_max_len];

		getpass(plaintext_passphrase, Passphrase_max_len, confirm);

		encrypted = crypt_r(plaintext_passphrase, algo_salt, &data);

		memset(plaintext_passphrase, 0x00, Passphrase_max_len);
	}

	printf("%s\n", encrypted);

	return 0;
}

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

	char *str = chomp_crlf(fgets(buf, len, stream), len);

	err = tcsetattr(fno, TCSAFLUSH, &orig);
	if (err) {
		eprintf("reset tcsetattr returned error %d for %d?", err, fno);
		return NULL;
	}

	return str;
}

void getpass(char *buf, size_t len, int confirm)
{
	int diff = 0;
	do {
		if (diff) {
			printf("inputs did not match\n");
		}
		printf(" input passphrase: ");
		char *r = fgetpass(buf, len, stdin);
		if (!r) {
			eprintf("fgetpass returned NULL reading buf of %zu?",
				len);
			exit(EXIT_FAILURE);
		}
		printf("\n");

		if (confirm) {
			printf("repeat passphrase: ");
			char buf2[len];
			r = fgetpass(buf2, len, stdin);
			if (!r) {
				eprintf
				    ("fgetpass returned NULL reading buf of %zu?",
				     len);
				exit(EXIT_FAILURE);
			}
			printf("\n");

			diff = strncmp(buf, buf2, len);
		}
	} while (diff);
}

void getrandom_alphanumeric(char *buf, size_t len)
{
	if (!buf || !len) {
		eprintf("buf: %p len: %zu?", (void *)buf, len);
		exit(EXIT_FAILURE);
	}
	size_t max = (len - 1);
	size_t pos = 0;
	do {
		char rnd_buf[Random_read_size];
		unsigned int flags = 0;
		ssize_t got = getrandom(rnd_buf, Random_read_size, flags);

		for (ssize_t i = 0; i < got && pos < max; ++i) {
			char c = rnd_buf[i];
			if (is_english_alphanumeric(c)) {
				buf[pos++] = c;
			}
		}
	} while (pos < max);
	buf[pos++] = '\0';
}

/* boring stuff */
char *chomp_crlf(char *str, size_t max)
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

/* the standard C libarary "isalnum" results may depend upon the locale */
/* https://www.cplusplus.com/reference/cctype/isalnum/ */
int is_english_alphanumeric(char c)
{
	if (c >= '0' && c <= '9') {
		return c;
	}
	if (c >= 'A' && c <= 'Z') {
		return c;
	}
	if (c >= 'a' && c <= 'z') {
		return c;
	}
	return 0;
}

/* [1] https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_05_01 */
