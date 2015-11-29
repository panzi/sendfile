#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#include "parse.h"

static void usage(int argc, char *argv[]) {
	printf(
		"usage: %s [options] [input] [output]\n"
		"\n"
		"OPTIONS:\n"
		"\t-h, --help          print this message\n"
		"\t-o, --offset BYTES  read offset\n"
		"\t-c, --count BYTES   number of bytes to transfer\n",
		argc > 0 ? argv[0] : "sendfile");
}

int main(int argc, char *argv[]) {
	int status = 0;
	int in_fd  = -1;
	int out_fd = -1;
	off_t  offset = 0;
	size_t count  = SIZE_MAX;
	const char *in_file  = NULL;
	const char *out_file = NULL;

	static struct option long_options[] = {
		{"help",   no_argument,       0, 'h'},
		{"offset", required_argument, 0, 'o'},
		{"count",  required_argument, 0, 'c'},
		{0,        0,                 0,  0 }
	};

	for (;;) {
		int c = getopt_long(argc, argv, "ho:c:", long_options, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'h':
			usage(argc, argv);
			return 0;

		case 'o':
			if (!parse_offset(optarg, &offset)) {
				perror(optarg);
				return 1;
			}
			break;

		case 'c':
			if (!parse_size(optarg, &count)) {
				perror(optarg);
				return 1;
			}
			break;

		case '?':
		default:
			return 1;
		}
	}

	if (optind < argc) {
		in_file = argv[optind];

		++ optind;
		if (optind < argc) {
			out_file = argv[optind];
		}
	}

	if (in_file && strcmp(in_file, "-") != 0) {
		in_fd = open(in_file, O_RDONLY | O_LARGEFILE);
		if (in_fd < 0) {
			perror(in_file);
			goto error;
		}
	}
	else {
		in_file = "<stdin>";
		in_fd   = STDIN_FILENO;
	}
	
	if (out_file && strcmp(out_file, "-") != 0) {
		out_fd = open(out_file, O_WRONLY | O_CREAT | O_LARGEFILE, 0664);
		if (out_fd < 0) {
			perror(out_file);
			goto error;
		}
	}
	else {
		out_file = "<stdout>";
		out_fd   = STDOUT_FILENO;
	}

	ssize_t sent = sendfile(out_fd, in_fd, &offset, count);

	if (sent < 0) {
		perror(out_file);
		goto error;
	}

	goto cleanup;

error:
	status = 1;

cleanup:
	if (in_fd != -1) {
		close(in_fd);
	}
	
	if (out_fd != -1) {
		close(out_fd);
	}

	return status;
}
