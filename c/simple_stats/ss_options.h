
/* ss_options.h - Eric Herman 2014 */
#ifndef _SS_OPTIONS_H_
#define _SS_OPTIONS_H_

#include <stdio.h>
#include <stdint.h>

typedef struct ss_options_ {
	char file[FILENAME_MAX];
	uint8_t help;
	uint8_t version;
	unsigned int channels;
	unsigned int skip_cols;
	unsigned int skip_rows;
} ss_options_t;

void parse_cmdline_args(ss_options_t *options, int argc, char *argv[]);

#endif /* _SS_OPTIONS_H_ */
