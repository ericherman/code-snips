#include <stdio.h>
#include <stdlib.h>

/*
   I think it is interesting that there is no compiler warning, for this
   struct casting, so I guess this does not qualify as "type-punning" and
   does not require strict aliasing?

   gcc -std=c89 -O2 -Wall -Wextra -pedantic -Werror
   gcc -std=c11 -O2 -Wall -Wextra -pedantic -Werror
*/

struct point {
	int x;
	int y;
};

struct point3d {
	int x;
	int y;
	int z;
};

void move_point(struct point *pt, int x, int y)
{
	pt->x += x;
	pt->y += y;
}

void print_point(struct point *pt)
{
	printf("[%d,%d]\n", pt->x, pt->y);
}

int main(int argc, char **argv)
{
	struct point pt1;
	struct point3d pt2;
	struct point *pt;

	pt1.x = argc > 1 ? atoi(argv[1]) : 2;
	pt1.y = argc > 2 ? atoi(argv[2]) : 3;

	pt2.x = argc > 3 ? atoi(argv[3]) : 5;
	pt2.y = argc > 4 ? atoi(argv[4]) : 7;
	pt2.z = argc > 5 ? atoi(argv[5]) : 11;

	pt = &pt1;
	move_point(pt, 13, 17);
	print_point(pt);

	pt = (struct point *)&pt2;
	move_point(pt, 23, 27);
	print_point(pt);

	return 0;
}
