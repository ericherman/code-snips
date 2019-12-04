/* SPDX-License-Identifier: GPL-3.0-or-later */
/* minion-stats.c : a commandline utility for generating D&D 5e stats */
/* Copyright (C) 2019 Eric Herman <eric@freesa.org> */

/*
 * building requires a modern-ish c compiler as it uses some c99 code
 * constructs but is otherwise straightforward:
 *	cc -o minion-stats ./minion-stats.c
 * If you wish you can dail-up the cflags, but you're unlikely to notice
 * a difference:
 *	gcc -Wall -Wextra -g -O2 -DNDEBUG -o minion-stats ./minion-stats.c
 *
 * usage:
 *	./minion-stats $NUM_MINIONS $NUM_D6 $SORT_STATS
 *
 * defaults for parameters: 10 3 0
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct charstats_s {
	int stats[6];
	int total_bonuses;
};

int bonus_for_stat(int stat_val)
{
	return (stat_val / 2) - 5;
}

int icmpdesc(const void *a, const void *b)
{
	return (*(const int *)b - *(const int *)a);
}

void roll_stats(struct charstats_s *minion, int nd6, int sort_stats)
{
	assert(minion != NULL);
	assert(nd6 > 0);

	minion->total_bonuses = 0;
	for (int s = 0; s < 6; ++s) {
		int dice[nd6];
		for (int d = 0; d < nd6; ++d) {
			int r = rand();
			dice[d] = (r % 6) + 1;
		}

		/* sort to get the best 3 dice */
		qsort(dice, nd6, sizeof(int), icmpdesc);
		int stat = 0;
		for (int d = 0; d < 3; ++d) {
			stat += (d < nd6 ? dice[d] : 1);
		}

		minion->stats[s] = stat;
		minion->total_bonuses += bonus_for_stat(stat);
	}

	if (sort_stats) {
		qsort(minion->stats, 6, sizeof(int), icmpdesc);
	}
}

int charstats_cmpdesc(const void *a, const void *b)
{
	const struct charstats_s *sa = a;
	const struct charstats_s *sb = b;

	/* copy so we can compare sorted stats on the copy and compare */
	struct charstats_s ca = *sa;
	struct charstats_s cb = *sb;

	if (cb.total_bonuses != ca.total_bonuses) {
		return (cb.total_bonuses - ca.total_bonuses);
	}

	qsort(ca.stats, 6, sizeof(int), icmpdesc);
	qsort(cb.stats, 6, sizeof(int), icmpdesc);

	return (cb.stats[0] != ca.stats[0]) ? (cb.stats[0] - ca.stats[0]) :
	    (cb.stats[1] != ca.stats[1]) ? (cb.stats[1] - ca.stats[1]) :
	    (cb.stats[2] != ca.stats[2]) ? (cb.stats[2] - ca.stats[2]) :
	    (cb.stats[3] != ca.stats[3]) ? (cb.stats[3] - ca.stats[3]) :
	    (cb.stats[4] != ca.stats[4]) ? (cb.stats[4] - ca.stats[4]) :
	    (cb.stats[5] - ca.stats[5]);
}

char *charstats_to_string(char *buf, size_t buf_len, struct charstats_s *minion)
{
	assert(buf);
	assert(buf_len);
	assert(minion);

	int printed = 0;
	size_t total_printed = 0;
	buf[0] = '\0';
	for (int s = 0; s < 6; ++s) {
		int stat = minion->stats[s];
		int bonus = bonus_for_stat(stat);
		const char *plus = bonus >= 0 ? "+" : "";
		printed = snprintf(buf + total_printed, buf_len - total_printed,
				   "%2d (%s%d)  ", stat, plus, bonus);

		/* snprintf returns negative on error; if total_printed equals
		 * (or exceeds) buf_len, then data trucation occured */
		assert(printed > 0);
		total_printed += printed;
		assert(total_printed < buf_len);
	}
	const char *plus = minion->total_bonuses >= 0 ? "+" : "";
	printed = snprintf(buf + total_printed, buf_len - total_printed,
			   " ((%s%d))", plus, minion->total_bonuses);

	assert(printed > 0);
	total_printed += printed;
	assert(total_printed < buf_len);

	return buf;
}

#ifndef Make_valgrind_happy
#ifdef NDEBUG
#define Make_valgrind_happy 0
#endif
#endif

#ifndef Make_valgrind_happy
#define Make_valgrind_happy 1
#endif

int main(int argc, char **argv)
{
	int nd6 = 0;
	int num_minions = 0;
	int sort_stats = 0;
	struct charstats_s *minion_stats = NULL;

	/* the first argument is number of minions, default to 10 */
	if (argc > 1) {
		num_minions = atoi(argv[1]);
	}
	if (num_minions < 1) {
		num_minions = 10;
	}

	/* second argument is number of d6 to roll per stat, default to 3 */
	if (argc > 2) {
		nd6 = atoi(argv[2]);
	}
	if (nd6 <= 0) {
		nd6 = 3;
	}

	/* the third is whether or not to sort the stats high to low */
	if (argc > 3) {
		sort_stats = atoi(argv[3]);
	}

	/* seed the bad random number generator */
	srand(time(NULL));

	/* make space for minions */
	minion_stats = malloc(num_minions * sizeof(struct charstats_s));
	if (!minion_stats) {
		return EXIT_FAILURE;
	}

	/* roll stats for all the minions */
	for (int i = 0; i < num_minions; ++i) {
		roll_stats(&minion_stats[i], nd6, sort_stats);
	}

	/* sort the minions from best to worst */
	qsort(minion_stats, num_minions, sizeof(struct charstats_s),
	      charstats_cmpdesc);

	/* print each minion */
	for (int i = 0; i < num_minions; ++i) {
		char buf[80];
		printf("%s\n", charstats_to_string(buf, 80, &minion_stats[i]));
	}

	if (Make_valgrind_happy) {
		free(minion_stats);
	}

	return 0;
}
