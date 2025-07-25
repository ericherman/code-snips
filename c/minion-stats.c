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
 *	./minion-stats --help
 */

#include <assert.h>
#include <getopt.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __linux__
#include <sys/random.h>
#else
#include <time.h>
#endif

#define Version_str "0.0.0"
#define Default_num_stats 6U
#define Default_characters 10U
#define Default_dice_to_roll 3U
#define Default_dice_to_keep 3U
#define Default_num_stats 6U
#define Default_die_sides 6U

struct charstats {
	uint16_t *stats;
	size_t stats_len;
	int total_bonuses;
};

static int bonus_for_stat(int stat_val)
{
	return (stat_val / 2) - 5;
}

static int u16cmpdesc(const void *vpa, const void *vpb)
{
	const uint16_t a = (*(const uint16_t *)vpa);
	const uint16_t b = (*(const uint16_t *)vpb);
	return (b > a) ? 1 : (a > b) ? -1 : 0;
}

static int zcmpdesc(const void *vpa, const void *vpb)
{
	const size_t a = (*(const size_t *)vpa);
	const size_t b = (*(const size_t *)vpb);
	return (b > a) ? 1 : (a > b) ? -1 : 0;
}

#ifdef __linux__
#define init_rand() ((void)0)
static size_t rand_z(void)
{
	size_t val = 0;
	ssize_t r = 0;
	do {
		size_t flags = 0;
		r = getrandom(&val, sizeof(val), flags);
	} while (r < (ssize_t)sizeof(val));
	return val;
}
#else
#define init_rand() srand(time(NULL))
#define rand_z(void) ((size_t)rand())
#endif

void roll_stats(struct charstats *minion, size_t *dice, size_t dice_to_roll,
		uint16_t die_sides, size_t dice_to_keep, int sort_stats)
{
	assert(minion != NULL);
	assert(dice != NULL);
	assert(dice_to_roll > 0);

	minion->total_bonuses = 0;
	for (size_t s = 0; s < minion->stats_len; ++s) {
		int stat = 0;
		for (size_t d = 0; d < dice_to_roll; ++d) {
			size_t r = rand_z();
			dice[d] = (r % die_sides) + 1;
		}

		/* sort to get the best dice_to_keep dice */
		assert(sizeof(dice[0]) == sizeof(size_t));
		qsort(dice, dice_to_roll, sizeof(size_t), zcmpdesc);
		for (size_t d = 0; d < dice_to_keep && d < dice_to_roll; ++d) {
			stat += (d < dice_to_roll ? dice[d] : 1);
		}

		minion->stats[s] = stat;
		minion->total_bonuses += bonus_for_stat(stat);
	}

	if (sort_stats) {
		assert(sizeof(minion->stats[0]) == sizeof(uint16_t));
		qsort(minion->stats, minion->stats_len, sizeof(uint16_t),
		      u16cmpdesc);
	}
}

int charstats_cmpdesc(const void *a, const void *b)
{
	const struct charstats *sa = (const struct charstats *)a;
	const struct charstats *sb = (const struct charstats *)b;

	uint16_t lastats[2 * Default_num_stats];
	uint16_t *lbstats = lastats + Default_num_stats;

	/* copy so we can compare sorted stats on the copy and compare */
	struct charstats ca = { lastats, sa->stats_len, sa->total_bonuses };
	struct charstats cb = { lbstats, sb->stats_len, sb->total_bonuses };

	assert(sa->stats_len == sb->stats_len);
	if (sa->stats_len > Default_num_stats) {
		size_t total_size = 2 * sa->stats_len;
		ca.stats = (uint16_t *)calloc(total_size, sizeof(uint16_t));
		cb.stats = ca.stats + sa->stats_len;
	}
	memcpy(ca.stats, sa->stats, sizeof(uint16_t) * sa->stats_len);
	memcpy(cb.stats, sb->stats, sizeof(uint16_t) * sb->stats_len);

	if (cb.total_bonuses != ca.total_bonuses) {
		return (cb.total_bonuses - ca.total_bonuses);
	}

	assert(sizeof(ca.stats[0]) == sizeof(uint16_t));
	qsort(ca.stats, ca.stats_len, sizeof(uint16_t), u16cmpdesc);

	assert(sizeof(cb.stats[0]) == sizeof(uint16_t));
	qsort(cb.stats, cb.stats_len, sizeof(uint16_t), u16cmpdesc);

	assert(ca.stats_len == cb.stats_len);
	int rv = 0;
	for (size_t i = 0; i < ca.stats_len && !rv; ++i) {
		rv = cb.stats[i] - ca.stats[i];
	}

	if (ca.stats != lastats) {
		cb.stats = NULL;
		free(ca.stats);
		ca.stats = NULL;
	}

	return rv;
}

#include <stdio.h>
char *charstats_to_string(char *buf, size_t buf_len, struct charstats *minion)
{
	int printed = 0;
	size_t total_printed = 0;
	size_t s = 0;
	const char *plus = NULL;

	assert(buf);
	assert(buf_len);
	assert(minion);

	buf[0] = '\0';
	for (s = 0; s < minion->stats_len; ++s) {
		int stat = minion->stats[s];
		int bonus = bonus_for_stat(stat);
		plus = bonus >= 0 ? "+" : "";
		printed = snprintf(buf + total_printed, buf_len - total_printed,
				   "%2d (%s%d)  ", stat, plus, bonus);

		/* snprintf returns negative on error; if total_printed equals
		 * (or exceeds) buf_len, then data trucation occured */
		assert(printed > 0);
		total_printed += printed;
		assert(total_printed < buf_len);
	}
	plus = minion->total_bonuses >= 0 ? "+" : "";
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
#else
#define Make_valgrind_happy 1
#endif
#endif

struct stats_options {
	size_t characters;
	size_t dice_to_roll;
	size_t dice_to_keep;
	size_t num_stats;
	size_t die_sides;
	uint8_t sort_stats;
	uint8_t verbose;
	uint8_t help;
	uint8_t version;
	int option_index;
};

static void setz_if_positive(size_t *val, int i)
{
	if (i > 0) {
		*val = (size_t)i;
	}
}

static void usage(FILE *out, const char *argv0)
{
	fprintf(out, "Usage: %s [OPTION]...\n", argv0);
	fprintf(out, "Print results of dice stat-rolls\n");
	fprintf(out, "\t-c, --characters=VAL    "
		"number of sets of stats to roll [DEFAULT %u]\n",
		Default_characters);
	fprintf(out, "\t-r, --dice-to-roll=VAL  "
		"number of dice rolled per stat [DEFAULT %u]\n",
		Default_dice_to_roll);
	fprintf(out, "\t-k, --dice-to-keep=VAL  "
		"number of dice to keep per stat [DEFAULT %u]\n",
		Default_dice_to_keep);
	fprintf(out, "\t-n, --num-stats=VAL     "
		"number of stats per character [DEFAULT %u]\n",
		Default_num_stats);
	fprintf(out, "\t-d, --die-sides=VAL     "
		"number of faces per die [DEFAULT %u]\n", Default_die_sides);
	fprintf(out, "\t-s, --sort-stats        "
		"whether to sort the resulting stats\n");
	fprintf(out, "\t-h, --help              "
		"print this message and exit\n");
	fprintf(out, "\t-v, --verbose           " "verbose output\n");
	fprintf(out, "\t-V, --version           "
		"print version (%s), and exit\n", Version_str);
}

static void parse_args_(struct stats_options *options, int argc, char **argv)
{
	assert(options);

	const char *optstring = "c:r:k:n:d:shvV";

	struct option long_options[] = {
		{ "characters", required_argument, 0, 'c' },
		{ "dice-to-roll", required_argument, 0, 'r' },
		{ "dice-to-keep", required_argument, 0, 'k' },
		{ "num-stats", required_argument, 0, 'n' },
		{ "die-sides", required_argument, 0, 'd' },
		{ "sort-stats", no_argument, 0, 's' },
		{ "help", no_argument, 0, 'h' },
		{ "verbose", no_argument, 0, 'v' },
		{ "version", no_argument, 0, 'V' },
		{ 0, 0, 0, 0 }
	};

	int ch;
	while (1) {
		options->option_index = 0;
		ch = getopt_long(argc, argv, optstring, long_options,
				 &options->option_index);

		/* Detect the end of the options. */
		if (ch == -1)
			break;

		switch (ch) {
		case 0:
			break;
		case 'c':
			setz_if_positive(&options->characters, atoi(optarg));
			break;
		case 'r':
			setz_if_positive(&options->dice_to_roll, atoi(optarg));
			break;
		case 'k':
			setz_if_positive(&options->dice_to_keep, atoi(optarg));
			break;
		case 'n':
			setz_if_positive(&options->num_stats, atoi(optarg));
			break;
		case 'd':
			setz_if_positive(&options->die_sides, atoi(optarg));
			break;
		case 's':
			options->sort_stats = 1;
			break;
		case 'h':
			options->help = 1;
			break;
		case 'v':
			options->verbose += 1;
			break;
		case 'V':
			options->version = 1;
			break;
		}
	}
}

#define info(verbose_expr, ...) do { \
	if (verbose_expr) { \
		fprintf(stdout, __VA_ARGS__); \
		fprintf(stdout, "\n"); \
	} \
} while (0)

int main(int argc, char **argv)
{
	struct stats_options options;

	options.characters = Default_characters;
	options.dice_to_roll = Default_dice_to_roll;
	options.dice_to_keep = Default_dice_to_keep;
	options.num_stats = Default_num_stats;
	options.die_sides = Default_die_sides;
	options.sort_stats = 0;
	options.verbose = 0;
	options.help = 0;
	options.version = 0;
	options.option_index = 0;

	parse_args_(&options, argc, argv);

	if (options.help) {
		usage(stdout, argv[0]);
		return EXIT_SUCCESS;
	}
	if (options.version) {
		fprintf(stdout, "%s %s\n", argv[0], Version_str);
		return EXIT_SUCCESS;
	}

	size_t num_minions = options.characters;
	info(options.verbose, "characters: %zu", num_minions);

	size_t dice_to_roll = options.dice_to_roll;
	info(options.verbose, "dice to roll: %zu", dice_to_roll);

	size_t dice_to_keep = options.dice_to_keep;
	info(options.verbose, "dice to keep: %zu", dice_to_keep);

	uint16_t die_sides = options.die_sides;
	info(options.verbose, "sides per die: %zu", (size_t)die_sides);

	size_t num_stats = options.num_stats;
	info(options.verbose, "num stats: %zu", num_stats);

	int sort_stats = options.sort_stats;
	info(options.verbose, "sort stats: %s", sort_stats ? "yes" : "no");

	info(options.verbose > 1, "make space for characters");
	struct charstats *minion_stats = NULL;
	minion_stats =
	    (struct charstats *)calloc(num_minions, sizeof(struct charstats));
	if (!minion_stats) {
		return EXIT_FAILURE;
	}
	size_t stats_size = sizeof(uint16_t) * num_stats;
	uint8_t *stats = (uint8_t *)calloc(num_minions, stats_size);
	if (!stats) {
		return EXIT_FAILURE;
	}
	for (size_t i = 0; i < num_minions; ++i) {
		minion_stats[i].stats = (uint16_t *)(stats + (i * stats_size));
		minion_stats[i].stats_len = num_stats;
	}

	size_t ldice[1 + Default_dice_to_roll];
	size_t *dice = ldice;
	info(options.verbose > 2, "check space for dice");
	if (dice_to_roll > sizeof(ldice) / sizeof(ldice[0])) {
		info(options.verbose > 1, "make space for dice");
		dice = (size_t *)calloc(dice_to_roll, sizeof(size_t));
		if (!dice) {
			return EXIT_FAILURE;
		}
	} else {
		memset(ldice, 0x00, sizeof(ldice));
	}

	info(options.verbose > 1, "roll stats for all the minions");
	init_rand();
	for (size_t i = 0; i < num_minions; ++i) {
		roll_stats(&minion_stats[i], dice, dice_to_roll, die_sides,
			   dice_to_keep, sort_stats);
	}

	info(options.verbose > 1, "sort the minions from best to worst");
	qsort(minion_stats, num_minions, sizeof(struct charstats),
	      charstats_cmpdesc);

	info(options.verbose > 2, "print each minion");
	char buf[80];
	for (size_t i = 0; i < num_minions; ++i) {
		printf("%s\n", charstats_to_string(buf, 80, &minion_stats[i]));
	}

	info(Make_valgrind_happy && options.verbose > 2, "freeing memory");
	if (Make_valgrind_happy) {
		if (dice != ldice) {
			free(dice);
			dice = NULL;
		}
		free(stats);
		stats = NULL;
		free(minion_stats);
		minion_stats = NULL;
	}

	info(options.verbose > 2, "done");
	return 0;
}
