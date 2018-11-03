/* pipe-open.c
   Copyright (C) 2014, 2018 Eric Herman <eric@freesa.org>

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
/* pipe-open.c, 2018 Eric Herman 2014 */

/* pipe-open takes a command and optionally reverses the output */

#include <stdio.h>
#include <string.h>
#include <getopt.h>		/* if you don't hate getopt, you should */
#include <stdlib.h>
#include <linux/limits.h>	/* ARG_MAX */

#define PIPE_OPEN_DEMO_VERSION "1"
#define DEFAULT_COMMAND_TO_TRY "ls -ltr"

#define BUF_LEN ARG_MAX		/* better  idea? */

void _parse_args(int argc, char *argv[], char *cmd, size_t len, int *rev);

int main(int argc, char *argv[])
{
	char cmd[ARG_MAX];
	int reverse_flag;
	char buf[BUF_LEN];
	char reversed[BUF_LEN];
	size_t i, len;
	FILE *pipe;
	const char *mode = "r";

	/* initialize and set vars from command line */
	_parse_args(argc, argv, cmd, ARG_MAX, &reverse_flag);

	pipe = popen(cmd, mode);
	if (!pipe) {
		fprintf(stderr, "Could not open pipe (%s)\n", cmd);
		exit(1);
	}

	while (fgets(buf, BUF_LEN, pipe) != NULL) {
		len = strlen(buf);
		if (reverse_flag) {
			for (i = 0; i < len - 1; i++) {
				reversed[i] = buf[len - i - 2];
			}
			reversed[len - 1] = '\n';
			reversed[len] = '\0';

		}
		fprintf(stdout, "%s", reverse_flag ? reversed : buf);
	}

	if (pclose(pipe) != 0) {
		/* weird, but not really fatal */
		fprintf(stderr, "Could not close pipe (%s)\n", cmd);
	}

	return 0;
}

void _parse_args(int argc, char *argv[], char *cmd, size_t len, int *rev)
{
	int ver;
	int help;
	int opt_char;
	int option_index;

	/* yes, optstirng is horrible */
	const char *optstring = "vhc:r";

	struct option long_options[] = {
		{"version", no_argument, 0, 'v'},
		{"help", no_argument, 0, 'h'},
		{"command", required_argument, 0, 'c'},
		{"reverse", no_argument, 0, 'r'},
		{0, 0, 0, 0}
	};

	ver = 0;
	help = 0;
	cmd[0] = '\0';
	*rev = 0;
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
		case 'v':	/* --version | -v */
			ver = 1;
			break;
		case 'h':	/* --help | -h */
			help = 1;
			break;
		case 'r':	/* --reverse | -r */
			*rev = 1;
			break;
		case 'c':	/* --commanad | -c */
			{
				/* basically, optarg is all wrong */
				snprintf(cmd, len, "%s", optarg);
				break;
			}
		}
	}

	if (ver) {
		printf("pipe-open version %s\n", PIPE_OPEN_DEMO_VERSION);
		exit(0);
	}

	if (help) {
		printf("pipe-open version %s options\n",
		       PIPE_OPEN_DEMO_VERSION);
		printf("  %s   %s\n", "-v, --version                ",
		       "Print version information");
		printf("  %s   %s\n", "-h, --help                   ",
		       "Print option help");
		printf("  %s   %s\n", "-c, --command <shell command>",
		       "Command to execute");
		printf("  %s   %s\n", "-r, --reverse                ",
		       "Print the command output reversed");
		exit(0);
	}

	if (strcmp("", cmd) == 0) {
		sprintf(cmd, DEFAULT_COMMAND_TO_TRY);
	}
}
