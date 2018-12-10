#!/bin/bash

# For this task, on a 4 core system, the single-threaded version is
# expected to be faster and is provided here to give a sense of the
# locking costs.
#
# (Well, single-threaded is faster at least to 10^10 -- maybe at
# 10^12 with 32 cores, parallel could win, I don't know, if you have
# a big machine like that, I'd be interested in seeing the output)
#
# On my 4 core (8 thread) system the atomics are up to 5x faster
# than using pthread's mutexes, and only a small penalty compared to
# single-threaded.

while getopts ":s:e:" opt; do
	case $opt in
	s)
		STRENGTH="$OPTARG"
		;;
	e)
		EXPONENT="$OPTARG"
		;;
	\?)
		echo "Unknown Option: -$OPTARG" >&2
		exit 1
		;;
	:)
		echo "Option -$OPTARG requires an argument" >&2
		exit 1
		;;
	esac
done

if [ "_${STRENGTH}_" == "__" ]; then
	STRENGTH=3
fi

if [ "_${EXPONENT}_" == "__" ]; then
	EXPONENT=10
fi

echo "compiling sieve-of-eratosthenes (single threaded)"
gcc -g -Wall -Werror -O2 -DNDEBUG \
	-o sieve-of-eratosthenes \
	sieve-of-eratosthenes.c

echo "compiling sieve-of-eratosthenes-pthreads (mutexed)"
gcc -g -Wall -Werror -O2 -DNDEBUG \
	-o sieve-of-eratosthenes-pthreads \
	sieve-of-eratosthenes-pthreads.c \
	-pthread

echo "compiling sieve-of-eratosthenes-atomic (pthreads and __atomic)"
gcc -g -Wall -Werror -O2 -DNDEBUG \
	-o sieve-of-eratosthenes-atomic \
	sieve-of-eratosthenes-atomic.c \
	-pthread

for E in $(seq 1 $EXPONENT); do
	echo ""
	POW=$(echo "10^$E" | bc)
	for RUN in $(seq 1 $STRENGTH); do
		echo ""
		echo "Trial: $RUN, (primes up to: $POW)"
		echo ""
		echo "./sieve-of-eratosthenes $POW >/dev/null"
		{ time { ./sieve-of-eratosthenes $POW >/dev/null; } ; } 2>&1
		echo ""
		echo "./sieve-of-eratosthenes-pthreads $POW >/dev/null"
		{ time { ./sieve-of-eratosthenes-pthreads $POW >/dev/null; } ; } 2>&1
		echo ""
		echo "./sieve-of-eratosthenes-atomic $POW >/dev/null"
		{ time { ./sieve-of-eratosthenes-atomic $POW >/dev/null; } ; } 2>&1
	done
done
