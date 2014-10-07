#include "ts_options.h"
#include <getopt.h>		/* if you don't hate getopt, you should */
#include <stdlib.h>

void _init_options(ts_options_t *options)
{
	options->num_elements = 4;
	options->element_max_value = 12;
	options->negate_one_in = 3;
	options->help = 0;
	options->version = 0;
}

void parse_cmdline_args(ts_options_t *options, int argc, char *argv[])
{
	int opt_char;
	int option_index;

	/* yes, optstirng is horrible */
	const char *optstring = "h::v::e::m::n::";

	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"elements", optional_argument, 0, 'e'},
		{"max_val", optional_argument, 0, 'm'},
		{"negate", optional_argument, 0, 'n'},
		{0, 0, 0, 0}
	};

	_init_options(options);

	while (1) {
		option_index = 0;

		opt_char =
		    getopt_long(argc, argv, optstring, long_options,
				&option_index);

		/* Detect the end of the options. */
		if (opt_char == -1)
			break;

		switch (opt_char) {
		case 0:
			break;
		case 'h':	/* --help | -h */
			options->help = 1;
			break;
		case 'v':	/* --version | -v */
			options->version = 1;
			break;
		case 'e':	/* --elements | -e */
			options->num_elements = atoi(optarg);
			break;
		case 'm':	/* --max_val | -m */
			options->element_max_value = atoi(optarg);
			break;
		case 'n':	/* --negate | -n */
			options->negate_one_in = atoi(optarg);
			break;
		}
	}
}

void show_usage(FILE * s, char *argv0)
{
	fprintf(s, "Usage: %s [option...]\n", argv0);
	fprintf(s, "\nOptions:\n");
	fprintf(s, "\t-h, --help        print this message and exit\n");
	fprintf(s, "\t-v, --version     print version and exit\n");
	fprintf(s, "\t-e, --elements    number of elements to sort\n");
	fprintf(s, "\t-m, --max_val     max absolute value of elements\n");
	fprintf(s, "\t-n, --negate      one in how many to negate\n");
}
