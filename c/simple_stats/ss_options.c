#include "ss_options.h"
#include <getopt.h>		/* if you don't hate getopt, you should */
#include <stdlib.h>

void _init_options(ss_options_t *options)
{
	/* initialize to stdin */
	snprintf(options->file, FILENAME_MAX, "-");
	options->help = 0;
	options->version = 0;
	options->channels = 1;
	options->skip_cols = 0;
	options->skip_rows = 0;
}

void parse_cmdline_args(ss_options_t *options, int argc, char *argv[])
{
	int opt_char;
	int option_index;

	/* yes, optstirng is horrible */
	const char *optstring = "hvf::c::x::y::";

	struct option long_options[] = {
		{"help", no_argument, 0, 1},
		{"version", no_argument, 0, 1},
		{"file", optional_argument, 0, 'f'},
		{"channels", optional_argument, 0, 'c'},
		{"skip_cols", optional_argument, 0, 'x'},
		{"skip_rows", optional_argument, 0, 'y'},
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
		case 'f':	/* --file | -f */
			{
				/* basically, optarg is all wrong */
				snprintf(options->file, FILENAME_MAX, "%s",
					 optarg);
				break;
			}
		case 'c':	/* --channels | -opt_char */
			options->channels = atoi(optarg);
			break;
		case 'x':	/* --skip_cols | -x */
			options->skip_cols = atoi(optarg);
			break;
		case 'y':	/* --skip_rows | -y */
			options->skip_rows = atoi(optarg);
			break;
		}
	}
}
