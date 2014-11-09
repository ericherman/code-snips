#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BITS_UNSIGNED_LONG (sizeof(unsigned long) * 8)
#define BITS_UNSIGNED_INT (sizeof(unsigned int) * 8)
#define BITS_UI_BUF_LEN (BITS_UNSIGNED_INT + 1)

/* this syntax uses only a few bits per field */
typedef struct {
	unsigned int v:3;
	unsigned int w:2;
	int x:3;
	int y:2;
	int z:1;
} teeny_t;

#if _POSIX_C_SOURCE < 200809L
size_t strnlen(char *str, size_t buf_size)
{
	size_t len = strlen(str);
	if (len > buf_size) {
		exit(EXIT_FAILURE);
	}
	return len;
}
#endif

void revstr(char *str, size_t buf_size)
{
	size_t i, j, len;
	char swap;

	len = strnlen(str, buf_size);
	for (i = 0, j = len - 1; i < j; i++, j--) {
		swap = str[i];
		str[i] = str[j];
		str[j] = swap;
	}
}

/*
  unsigned to binary
  represents a binary value as a string of zeros and ones
*/
char *utob(char *buf, size_t len, unsigned long val, int bits, int lil_endian)
{
	size_t i, shift, str_pos;
	if (bits < 1) {
		bits = BITS_UNSIGNED_LONG;
	}

	str_pos = 0;
	for (i = bits; i > 0; i--) {
		if (str_pos >= len) {
			exit(EXIT_FAILURE);
		}
		shift = (i - 1);
		buf[str_pos++] = ((val >> shift) & 1) ? '1' : '0';
	}
	buf[str_pos] = '\0';
	if (lil_endian) {
		revstr(buf, len);
	}
	return buf;
}

int main(int argc, char *argv[])
{
	int a;
	teeny_t s;
	char buf[BITS_UI_BUF_LEN];
	int le;

	a = (argc > 1) ? atoi(argv[1]) : 5;
	le = (argc > 2) ? atoi(argv[2]) : 0;

	s.v = a;
	s.w = a;
	s.x = a;
	s.y = a;
	s.z = a;

	printf("         int a   = %33u (decimal)\n", a);
	printf("         int a   = %33s (binary)\n\n",
	       utob(buf, BITS_UI_BUF_LEN, a, BITS_UNSIGNED_INT, le));

	printf("unsigned int v:3 = %33u (decimal)\n", s.v);
	printf("unsigned int v:3 = %33s (binary)\n\n",
	       utob(buf, BITS_UI_BUF_LEN, s.v, 3, le));

	printf("unsigned int w:2 = %33u (decimal)\n", s.w);
	printf("unsigned int w:2 = %33s (binary)\n\n",
	       utob(buf, BITS_UI_BUF_LEN, s.w, 2, le));

	printf("         int x:3 = %33d (decimal)\n", s.x);
	printf("         int x:3 = %33s (binary)\n\n",
	       utob(buf, BITS_UI_BUF_LEN, s.x, 3, le));

	printf("         int y:2 = %33d (decimal)\n", s.y);
	printf("         int y:2 = %33s (binary)\n\n",
	       utob(buf, BITS_UI_BUF_LEN, s.y, 2, le));

	printf("         int z:1 = %33d (decimal)\n", s.z);
	printf("         int z:1 = %33s (binary)\n\n",
	       utob(buf, BITS_UI_BUF_LEN, s.z, 1, le));

	printf("sizeof struct is %u bytes\n", sizeof(teeny_t));
	return 0;
}
