
/*
export MDB_BASE=${HOME}/builds/mariadb-10.5
export MDB_INC=${MDB_BASE}/include/mysql
export MDB_LIB=${MDB_BASE}/lib
gcc -g -Wall -Werror -Wextra -Wpedantic -fPIC -shared -pipe \
	-Wno-unused-parameter \
	-I${MDB_INC} -I. \
	sigproc-udf.c \
	-o sigproc.so
cp -v sigproc.so ${MDB_LIB}/sigproc.so
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#ifndef SIG_PROC_PID_PATH
#define SIG_PROC_PID_PATH "/var/run/sig_proc_pid.pid"
#endif

#define Eprintf(fmt, ...) \
	do { \
		fflush(stdout); \
		fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
		fprintf(stderr, fmt, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
	} while (0)

int pid_from_file(long *pid, const char *pidfile);
int signal_pid_at_path(const char *pidfile_path, int sig);

#ifdef SIGPROC_MAIN
int main(int argc, char **argv)
{

	if (argc < 2) {
		printf("usage: %s /path/to/pid_file\n", argv[0]);
		return 1;
	}

	const char *pidfile_path = argv[1];
	int sig = argc > 2 ? atoi(argv[2]) : SIGUSR1;

	int err = signal_pid_at_path(pidfile_path, sig);
	return err ? 1 : 0;
}
#else /* #ifdef SIGPROC_MAIN */
#include <mysql.h>
#include <stdbool.h>
bool sigproc_init(UDF_INIT *init, UDF_ARGS *args, char *message)
{
	if (args->arg_count != 0) {
		strcpy(message, "sigproc does not expect arguments");
		return 1;
	}
	return 0;
}

void sigproc_deinit(UDF_INIT *init)
{
}

int sigproc(UDF_INIT *init, UDF_ARGS *args, char *result, unsigned long *length,
	    unsigned char *is_null, unsigned char *error)
{
	signal_pid_at_path(SIG_PROC_PID_PATH, SIGUSR1);
	return 0;
}
#endif /* #ifdef SIGPROC_MAIN */

#ifndef Errorf
#define Errorf Eprintf
#endif

int signal_pid_at_path(const char *pidfile_path, int sig)
{
	long pid = LONG_MIN;
	int err = pid_from_file(&pid, pidfile_path);
	if (err) {
		return 1;
	} else if (pid < 2) {
		Errorf("PID == %ld?", pid);
		return 1;
	}

	err = kill(pid, sig);
	if (err) {
		Errorf("Could not send %d to PID %ld", sig, pid);
	}

	return err;
}

int pid_from_file(long *pid, const char *pidfile_path)
{
	FILE *pid_file = fopen(pidfile_path, "r");
	if (!pid_file) {
		int save_errno = errno;
		Errorf("ERROR %d (%s): fopen(%s)", save_errno,
		       strerror(save_errno), pidfile_path);
		return 1;
	}
	int matched = fscanf(pid_file, "%ld", pid);
	fclose(pid_file);
	if (matched != 1) {
		Errorf("ERROR: could not parse pid_file %s", pidfile_path);
		return 1;
	}
	return 0;
}
