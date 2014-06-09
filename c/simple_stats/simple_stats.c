/* simple_stats_t.c - Eric Herman 2014 */

#include "simple_stats.h"
#include <stdio.h>
#include <math.h>
#include <float.h>

void simple_stats_t_init(simple_stats_t *stats)
{
	stats->cnt = 0;
	stats->min = DBL_MAX;
	stats->max = -DBL_MAX;
	stats->sum = 0.0;
	stats->sum_of_squares = 0.0;
}

void simple_stats_t_append_val(simple_stats_t *stats, double val)
{
	stats->cnt++;
	if (stats->min > val) {
		stats->min = val;
	}
	if (stats->max < val) {
		stats->max = val;
	}
	stats->sum += val;
	stats->sum_of_squares += (val * val);
}

double simple_stats_t_average(simple_stats_t *stats)
{
	return stats->sum / stats->cnt;
}

double simple_stats_t_variance(simple_stats_t *stats)
{
	return stats->sum_of_squares / stats->cnt;
}

double simple_stats_t_std_dev(simple_stats_t *stats)
{
	return sqrt(simple_stats_t_variance(stats));
}

void simple_stats_t_to_string(simple_stats_t *stats, char *buf, size_t buflen)
{
	snprintf(buf, buflen,
		 "{ cnt: %u, min: %f, max: %f, avg: %f, std-dev: %f }",
		 stats->cnt, stats->min, stats->max,
		 simple_stats_t_average(stats), simple_stats_t_std_dev(stats));
}
