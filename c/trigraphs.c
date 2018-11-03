/* trigraphs.c
   Copyright (C) 2016, 2018 Eric Herman <eric@freesa.org>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

	https://www.gnu.org/licenses/lgpl-3.0.txt
	https://www.gnu.org/licenses/gpl-3.0.txt
 */
/* gcc -std=c89 -o trigraphs trigraphs.c */
??=include <stdio.h>
int main(int argc, char *argv??(??))
??<
	printf("Trigraphs? Huh?!?!??!??/n");
	printf("??/n");
	printf("??/t???/?(: %c??/n", '??(');
	printf("??/t???/?): %c??/n", '??)');
	printf("??/t???/?<: %c??/n", '??<');
	printf("??/t???/?>: %c??/n", '??>');
	printf("??/t???/?=: %c??/n", '??=');
	printf("??/t???/?/: %c??/n", '??/??/');
	printf("??/t???/?': %c??/n", '??'');
	printf("??/t???/?!: %c??/n", '??!');
	printf("??/t???/?-: %c??/n", '??-');
	printf("??/n");
	printf("Rationale for??/n");
	printf("International Standard--??/n");
	printf("Programming Languages--??/n");
	printf("C??/n");
	printf("Revision 5.10??/n");
	printf("April-2003??/n");
	printf("??/n");
	printf("section: 5.2.1.1 Trigraph sequences??/n");
	printf("??/n");

	return 0;
??>
