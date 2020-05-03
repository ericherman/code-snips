
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

MariaDB [test]> DROP TRIGGER IF EXISTS foo_insert_trigger;
DROP TRIGGER IF EXISTS foo_update_trigger;
DROP FUNCTION IF EXISTS sigproc;
DROP TABLE IF EXISTS `foo`;
CREATE FUNCTION sigproc RETURNS int SONAME 'sigproc.so';

MariaDB [test]> CREATE TABLE `foo` (
  `foo_id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `foo_dat` int NOT NULL,
  PRIMARY KEY (`foo_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

MariaDB [test]> DELIMITER $
CREATE TRIGGER foo_insert_trigger
AFTER INSERT ON `foo`
FOR EACH ROW
BEGIN
DECLARE result int(10);
SET result = sigproc();
END;
$
DELIMITER ;

MariaDB [test]> DELIMITER $
CREATE TRIGGER foo_update_trigger
AFTER UPDATE ON `foo`
FOR EACH ROW
BEGIN
DECLARE result int(10);
SET result = sigproc();
END;
$
DELIMITER ;


-- expect signal:
INSERT INTO `foo`(`foo_dat`) VALUES ('1');

-- expect no signal:
UPDATE `foo` SET `foo_dat` = 2 WHERE `foo_id`=2;

-- expect signal:
UPDATE `foo` SET `foo_dat` = 2 WHERE `foo_id`=1;

*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#include <mysql.h>
#include <stdbool.h>

#ifndef SIG_PROC_PID_PATH
#define SIG_PROC_PID_PATH "/var/run/sig_proc_pid.pid"
#endif

#ifndef EDEBUG
#ifdef NDEBUG
#define EDEBUG 0
#endif
#else
#define EDEBUG 1
#endif

#define Errorf(ebuf, ebuf_len, fmt, ...) \
	do { \
		if (EDEBUG && ebuf && ebuf_len) { \
			(ebuf)[0] = '\0'; \
			size_t _ebuf_len = (ebuf_len); \
			int _ebuf_pos = snprintf((ebuf), _ebuf_len, \
					"%s:%d: ", __FILE__, __LINE__); \
			if (_ebuf_pos > 0 && \
				(unsigned long long)_ebuf_pos < _ebuf_len) { \
				size_t _ebuf_left = _ebuf_len - _ebuf_pos; \
				snprintf((ebuf) + _ebuf_pos, _ebuf_left, \
					fmt, __VA_ARGS__); \
			} \
			fflush(stdout); \
			fprintf(stderr, "%s\n", (ebuf)); \
		} \
	} while (0)

int signal_pid_at_path(const char *pidfile_path, int sig, char *ebuf,
		       size_t len);
int pid_from_file(long *pid, const char *pidfile_path, char *ebuf, size_t len);

bool sigproc_init(UDF_INIT *init, UDF_ARGS *args, char *errmsg)
{
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
}

int sigproc(UDF_INIT *init, UDF_ARGS *args, char *result,
	    unsigned long *length, unsigned char *is_null, unsigned char *error)
{
	const char *path = SIG_PROC_PID_PATH;
	if (args->arg_count > 0) {
		path = args->args[0];
	}

	int sig = SIGUSR1;
	if (args->arg_count > 1) {
		sig = *((long long *)args->args[1]);
	}

	size_t buf_len = 1024;
	char buf[buf_len];
	buf[0] = '\0';

	int err = signal_pid_at_path(path, sig, buf, buf_len);
	if (err) {
		*error = 1;
	}

	return err ? 1 : 0;
}

int signal_pid_at_path(const char *pidfile_path, int sig, char *ebuf,
		       size_t len)
{
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
		Errorf(ebuf, len, "Could not send %d to PID %ld", sig, pid);
	}

	return err;
}

int pid_from_file(long *pid, const char *pidfile_path, char *ebuf, size_t len)
{
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
