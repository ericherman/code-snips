/* ternary-weird.c
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
/*
 from kernel/exit.c:

                panic("Attempted to kill init! exitcode=0x%08x\n",
                        father->signal->group_exit_code ?: father->exit_code);

 warning: ISO C forbids omitting the middle term of a ?: expression [-Wpedantic]


 from https://gcc.gnu.org/onlinedocs/gcc-5.2.0/gcc/Conditionals.html :

 The middle operand in a conditional expression may be omitted. Then if the
 first operand is nonzero, its value is the value of the conditional
 expression.

 Therefore, the expression

     x ? : y

 has the value of x if that is nonzero; otherwise, the value of y.

 This example is perfectly equivalent to

     x ? x : y

 In this simple case, the ability to omit the middle operand is not
 especially useful. When it becomes useful is when the first operand
 does, or may (if it is a macro argument), contain a side effect. Then
 repeating the operand in the middle would perform the side effect twice.
 Omitting the middle operand uses the value already computed without the
 undesirable effects of recomputing it.


 See also: https://en.wikipedia.org/wiki/Elvis_operator
*/

#include <stdio.h>

int main(void)
{
	int a, b;

	a = 0 ?: 7;

	b = 3 ?: 17;

	printf("a = 0 ?: 7; a == %d\n", a); /* 7 */
	printf("b = 3 ?: 17; b == %d\n", b); /* 3 */

	return (a == 7 && b == 3) ? 0 : 1;
}
