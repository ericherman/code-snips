/* Copyright (C) 2023 John Millikin <john@john-millikin.com> */
/* Copyright (C) 2023 Eric Herman <eric@freesa.org> */
/* SPDX-License-Identifier: 0BSD */
/* Origin:
    https://john-millikin.com/improved-unix-socket-networking-in-qemu-7.2#qemu-wrapper-c
    qemu-wrapper.c create a socket from a path for QEMU
    Improved UNIX socket networking in QEMU 7.2, 2023-04-11 04:17
*/
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int sock_fd, rc;
	char *sock_path;
	size_t sock_path_len;
	struct sockaddr_un sock_addr = { AF_UNIX, "" };

	if (argc < 3) {
		fprintf(stderr, "Usage: %s <socket> <program> [args...]\n",
			argv[0]);
		return 1;
	}

	sock_path = argv[1];
	sock_path_len = strlen(sock_path);
	if (sock_path_len >= sizeof sock_addr.sun_path) {
		fprintf(stderr, "Socket path \"%s\" too long\n", sock_path);
		return 1;
	}
	memcpy(sock_addr.sun_path, sock_path, sock_path_len + 1);

	sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		perror("Failed to create socket");
		return 1;
	}

	rc = connect(sock_fd, (struct sockaddr *)&sock_addr, sizeof sock_addr);
	if (rc == -1) {
		fprintf(stderr, "Failed to connect to socket \"%s\": ",
			sock_path);
		perror(NULL);
		return 1;
	}

	execv(argv[2], argv + 2);
	fprintf(stderr, "%s: ", argv[2]);
	perror(NULL);
	return 1;
}
