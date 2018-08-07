#include <stdio.h>

int main(int argc, char **argv) {
	char *s="#include <stdio.h>%c%cint main(int argc, char **argv) {%c%cchar *s=%c%s%c;%c%cprintf(s,10,10,10,9,34,s,34,10,9,10,9,10,10,10);%c%creturn 0;%c}%c";
	printf(s,10,10,10,9,34,s,34,10,9,10,9,10,10,10);
	return 0;
}
