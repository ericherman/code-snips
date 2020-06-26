/*
export MDB_BASE=${HOME}/builds/mariadb-10.5
export MDB_INC=${MDB_BASE}/include/mysql
export MDB_LIB=${MDB_BASE}/lib/plugin
gcc -g -Wall -Werror -Wextra -Wpedantic -fPIC -shared -pipe \
	-Wno-unused-parameter \
	-DNDEBUG \
	-I${MDB_INC} -I. \
	sigproc-udf.c \
	-o sigproc.so
cp -v sigproc.so ${MDB_LIB}/sigproc.so

MariaDB [(none)]> use test;
DROP TRIGGER IF EXISTS foo_insert_trigger;
DROP TRIGGER IF EXISTS foo_update_trigger;
DROP FUNCTION IF EXISTS sigproc;
DROP TABLE IF EXISTS `foo`;
show global variables like '%plugin_dir%';
CREATE FUNCTION sigproc RETURNS int SONAME 'sigproc.so';
select * from mysql.func;

CREATE TABLE `foo` (
  `foo_id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `foo_dat` int NOT NULL,
  PRIMARY KEY (`foo_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SHOW CREATE TABLE `foo`\G

DELIMITER $
CREATE TRIGGER foo_insert_trigger
AFTER INSERT ON `foo`
FOR EACH ROW
BEGIN
DECLARE result int(10);
SET result = sigproc('/tmp/foo.pid');
END;
$
DELIMITER ;

DELIMITER $
CREATE TRIGGER foo_update_trigger
AFTER UPDATE ON `foo`
FOR EACH ROW
BEGIN
DECLARE result int(10);
SET result = sigproc('/tmp/foo.pid');
END;
$
DELIMITER ;
SHOW TRIGGERS\G

-- expect signal:
INSERT INTO `foo`(`foo_dat`) VALUES ('1');

-- expect no signal:
UPDATE `foo` SET `foo_dat` = 2 WHERE `foo_id`=2;

-- expect signal:
UPDATE `foo` SET `foo_dat` = 2 WHERE `foo_id`=1;

*/

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>		/* UDF_INIT, UDF_ARGS */

#ifndef DEFAULT_SIG_PROC_PID_PATH
#define DEFAULT_SIG_PROC_PID_PATH "/var/run/sig_proc_pid.pid"
#endif

#ifndef MYSQL_ERRMSG_SIZE
#define MYSQL_ERRMSG_SIZE 512
#endif

#ifndef Our_debug
#ifdef NDEBUG
#define Our_debug 0
#endif
#endif

#ifndef Our_debug
#define Our_debug 1
#endif

#ifndef Errorf
#define Errorf(buf, len, fmt, ...) \
	errorf(__FILE__, __LINE__, buf, len, fmt, __VA_ARGS__)
#endif

#ifndef DEBUG_TRACE
#define DEBUG_TRACE 0
#endif

#if DEBUG_TRACE
#define Trace_point() \
	if (DEBUG_TRACE) { \
		fprintf(stderr, "%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__)\
	}
#else
#define Our_no_op do { (void)0; } while (0)
#define Trace_point() Our_no_op
#endif

int signal_pid_at_path(const char *pidfile_path, int sig, char *ebuf,
		       size_t len);
int pid_from_file(long *pid, const char *pidfile_path, char *ebuf, size_t len);

int errorf(const char *file, int line, char *buf, size_t len,
	   const char *format, ...);

bool sigproc_init(UDF_INIT *init, UDF_ARGS *args, char *errmsg)
{
	Trace_point();
	if (args->arg_count > 2) {
		strcpy(errmsg, "sigproc(['path/to/pidfile'], [signal_num])");
		return 1;
	}
	if (args->arg_count > 0 && args->arg_type[0] != STRING_RESULT) {
		strcpy(errmsg, "argument must be a string (pidfile path)");
		return 1;
	}
	if (args->arg_count > 1 && args->arg_type[1] != INT_RESULT) {
		strcpy(errmsg, "argument must be an int (signal)");
		return 1;
	}
	return 0;
}

void sigproc_deinit(UDF_INIT *init)
{
	Trace_point();
}

int sigproc(UDF_INIT *init, UDF_ARGS *args, char *result,
	    unsigned long *length, unsigned char *is_null, unsigned char *error)
{
	Trace_point();
	const char *path = DEFAULT_SIG_PROC_PID_PATH;
	if (args->arg_count > 0) {
		path = args->args[0];
	}

	int sig = SIGUSR1;
	if (args->arg_count > 1) {
		sig = *((long long *)args->args[1]);
	}

	size_t buf_len = MYSQL_ERRMSG_SIZE;
	char buf[buf_len];
	memset(buf, 0x00, MYSQL_ERRMSG_SIZE);

	int err = signal_pid_at_path(path, sig, buf, buf_len);
	if (err) {
		// I do not understand why setting "error" seems
		// to cause serious problems, but it does. --eric
#if (0)
		if (error) {
			*error = 1;
		}
#endif
		if (Our_debug) {
			fflush(stdout);
			fprintf(stderr, "%s", buf);
		}
	}

	return err ? 1 : 0;
}

int signal_pid_at_path(const char *pidfile_path, int sig, char *ebuf,
		       size_t len)
{
	Trace_point();
	long pid = LONG_MIN;
	int err = pid_from_file(&pid, pidfile_path, ebuf, len);
	if (err) {
		return 1;
	} else if (pid < 2) {
		Errorf(ebuf, len, "PID == %ld?", pid);
		return 1;
	}

	err = kill(pid, sig);
	if (err) {
		int save_errno = errno;
		Errorf(ebuf, len, "Could not send %d to PID %ld (%s)", sig, pid,
		       strerror(save_errno));
		return 1;
	}

	return 0;
}

int pid_from_file(long *pid, const char *pidfile_path, char *ebuf, size_t len)
{
	Trace_point();
	FILE *pid_file = fopen(pidfile_path, "r");
	if (!pid_file) {
		int save_errno = errno;
		Errorf(ebuf, len, "ERROR %d (%s): fopen(%s)", save_errno,
		       strerror(save_errno), pidfile_path);
		return 1;
	}
	int matched = fscanf(pid_file, "%ld", pid);
	fclose(pid_file);
	if (matched != 1) {
		Errorf(ebuf, len, "ERROR: could not parse pid_file %s",
		       pidfile_path);
		return 1;
	}
	return 0;
}

int errorf(const char *file, int line, char *buf, size_t len,
	   const char *format, ...)
{
	Trace_point();
	int printed1 = snprintf(buf, len, "%s:%d: ", file, line);
	if ((printed1 < 0) || (((size_t)printed1) >= len)) {
		return printed1;
	}
	va_list args;
	va_start(args, format);
	int printed2 = vsnprintf(buf + printed1, len - printed1, format, args);
	va_end(args);
	if (printed2 < 0) {
		return printed2;
	}
	return printed1 + printed2;
}
