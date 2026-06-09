#!/bin/bash
set -ex

PROCS=$( grep -c ^processor /proc/cpuinfo )
JOBS=$(( 2 + $PROCS ))
echo "JOBS=$JOBS"

if [ -z "$NICE" ]; then
	if ! command -v nice >/dev/null 2>&1; then
		echo >&2 "no nice? is coreutiles NOT installed?"
	else
		NICE="nice -n 10"
	fi
fi

if [ -z "$IONICE" ]; then
	if ! command -v ionice >/dev/null 2>&1 ; then
		echo >&2 "no ionice? maybe install util-linux?"
	else
		# man ionice
		# 0 for none, 1 for realtime, 2 for best-effort, 3 for idle.
		IONICE="ionice -c 3"
	fi
fi

# -DCMAKE_CFLAGS=-Wno-dev
# -DENABLE_GCOV=ON

# -DWITHOUT_CSV=True
# -DWITHOUT_INNODB=True
# -DWITHOUT_PERFSCHEMA=True
# -DWITHOUT_TOKUDB=True

time {
	rm -rf build
	mkdir -pv build
	cd build
	cmake .. \
		-DCMAKE_BUILD_TYPE=Debug \
		-DWITHOUT_CONNECT=True \
		-DWITHOUT_GROONGA=True \
		-DWITHOUT_MROONGA=True \
		-DWITHOUT_OQGRAPH=True \
		-DWITHOUT_ROCKSDB=True \
		"$@"

	$NICE $IONICE make -j $JOBS

	echo YAY
}
