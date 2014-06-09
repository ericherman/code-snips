#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "simple_stats.h"

#include "ss_options.h"

#define MAX_LINE_LEN 2000
#define MAX_VALUE_LEN MAX_LINE_LEN

FILE *_ifp_open(const char *file_name, unsigned short *from_file)
{
	FILE *fp;
	const char *mode = "r";

	*from_file = (strcmp("-", file_name) != 0);

	if (*from_file) {
		fp = fopen(file_name, mode);
		if (fp == NULL) {
			fprintf(stderr, "can not open '%s'\n", file_name);
			exit(-1);
		}
	} else {
		fp = stdin;
	}
	return fp;
}

void _lex_col_val(char *line_buf, size_t * lex_pos, char *val_buf)
{
	size_t len = 0;
	unsigned char done = 0;
	char *c;

	while (!done) {
		c = (line_buf + *lex_pos + len++);
		switch (*c) {
		case '\0':
		case '\n':
		case ',':
			{
				done = 1;
			}
		default:
			break;
		}
	}
	strncpy(val_buf, line_buf + *lex_pos, len);
	val_buf[(!done || !len) ? len : len - 1] = '\0';
	*lex_pos += len;
}

void _run(ss_options_t * options)
{
	FILE *ifp;
	simple_stats_t stats[options->channels];
	unsigned short from_file;
	char line_buf[MAX_LINE_LEN];
	char val_buf[MAX_VALUE_LEN];
	unsigned int rows_skipped = 0;
	unsigned int cols_skipped = 0;
	unsigned int columns = options->channels + options->skip_cols;
	unsigned int channel;
	unsigned int lex_pos = 0;
	size_t i;
	double d;

	for (i = 0; i < options->channels; i++) {
		simple_stats_t_init(&stats[i]);
	}

	ifp = _ifp_open(options->file, &from_file);

	while (fgets(line_buf, MAX_LINE_LEN, ifp) != NULL) {
		if (rows_skipped++ < options->skip_rows) {
			continue;
		}
		cols_skipped = 0;
		lex_pos = 0;
		for (i = 0; i < columns; i++) {
			_lex_col_val(line_buf, &lex_pos, val_buf);
			if (cols_skipped++ < options->skip_cols) {
				continue;
			}
			channel = i - options->skip_cols;
			sscanf(val_buf, "%lf%*s", &d);
			simple_stats_t_append_val(&stats[channel], d);
		}
	}

	if (from_file) {
		fclose(ifp);
	}

	for (i = 0; i < options->channels; i++) {
		simple_stats_t_to_string(&stats[i], line_buf, MAX_LINE_LEN);
		printf("channel %u: %s\n", i, line_buf);
	}
}

int main(int argc, char *argv[])
{
	ss_options_t options;

	parse_cmdline_args(&options, argc, argv);

	_run(&options);

	return 0;
}
