#include <stdio.h>

int main(void)
{
	for (char c = '0'; c <= '9'; ++c) {
		int x = c & 0b00001111;
		printf("('%c' & 0b00001111) == %d ; ", c, x);

		char a = (char)(x | 0b00110000);
		printf("(%d | 0b00110000) == '%c'\n", x, a);
	}

	printf("\n");

	for (char u = 'A'; u <= 'Z'; ++u) {
		char l = u | 0b00100000;
		printf("('%c' | 0b00100000) == '%c' ; ", u, l);

		char r = l & 0b11011111;
		printf("('%c' & 0b11011111) == '%c'\n", l, r);
	}

	int errors = 0;
	for (int c = 0; c <= 0b01111111; ++c) {
		// DEL is ASCII code point 127
		const char delete = 0x7F;

		char result = (char)(c | delete);
		if (result != delete) {
			++errors;
		}
	}

	return errors ? 1 : 0;
}
