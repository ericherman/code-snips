/* simple_stats_t.h - Eric Herman 2014 */
#ifndef _SIMPLE_STATS_H_
#define _SIMPLE_STATS_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct simple_stats_t_ {
	unsigned int cnt;
	double min;
	double max;
	double sum;
	double sum_of_squares;
} simple_stats_t;

void simple_stats_t_init(simple_stats_t *stats);

void simple_stats_t_append_val(simple_stats_t *stats, double val);

double simple_stats_t_average(simple_stats_t *stats);
double simple_stats_t_variance(simple_stats_t *stats);
double simple_stats_t_std_dev(simple_stats_t *stats);

void simple_stats_t_to_string(simple_stats_t *stats, char *buf, size_t buflen);

#ifdef __cplusplus
}
#endif

#endif /* _SIMPLE_STATS_H_ */
