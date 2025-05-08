#!/bin/bash
set -ex

PROCS=$( grep -c ^processor /proc/cpuinfo )
if [ $PROCS -lt 4 ]; then
	JOBS=3
else
	JOBS=$(( $PROCS - 1 ))
fi
echo "JOBS=$JOBS"

time {
	rm -rf build
	mkdir -pv build
	cd build
	cmake .. \
		-DCMAKE_BUILD_TYPE=Debug \
		-DWITHOUT_ROCKSDB=True \
		-DWITHOUT_GROONGA=True \
		-DWITHOUT_CONNECT=True \
		-DWITHOUT_OQGRAPH=True \
		-DWITHOUT_MROONGA=True
	make -j $JOBS
	echo YAY
}
