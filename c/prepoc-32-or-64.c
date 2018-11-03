/* prepoc-32-or-64.c
   Copyright (C) 2015, 2018 Eric Herman <eric@freesa.org>

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
#include <stdio.h>

#ifndef __WORDSIZE
#define DIFINED_WORDSIZE "not_defined"
#else
#if ( __WORDSIZE == 64 )
#define DIFINED_WORDSIZE "found_64"
#else
#if ( __WORDSIZE == 32 )
#define DIFINED_WORDSIZE "found_32"
#else
#define DIFINED_WORDSIZE "not_known"
#endif
#endif
#endif

int main(void)
{
	printf("DIFINED_WORDSIZE: %s\n", DIFINED_WORDSIZE);
	return sizeof(void *);
}
