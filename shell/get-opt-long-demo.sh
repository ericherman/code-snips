#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Eric Herman <eric@freesa.org>

set -e
set -u
set -o pipefail

# $ help set
#	-e Exit immediately if a command exits with a non-zero status.
#	-u Treat unset variables as an error when substituting.
#	-o option-name
#	    Set the variable corresponding to option-name:
#	        pipefail     the return value of a pipeline is the status of
#	                     the last command to exit with a non-zero status,
#	                     or zero if no command exited with a non-zero status


VERSION=0.0.1

OUR_NAME=$(basename $(readlink -f $0))

VERBOSE=${VERBOSE:-0} # default to zero if not set

function usage()
{
	cat << EOF
$OUR_NAME version $VERSION
	An options demonstration.
	At least one option must be set when invoked.

OPTIONS:
	-f, --foo=[FOO]		sets FOO
	-b, --bar=[BAR]		sets BAR
	--whiz=[WHIZ]		sets WHIZ
	--bang			explodes
	-h, --help		prints this message, then exits
	-V, --version		prints version ( $VERSION ), then exits
	-v, --verbose

EXAMPLE USAGE:
$0 \\
	-f this \\
	--bar=that \\
	--whiz=bang
EOF
}

declare -A COMMON_OPTIONS
COMMON_OPTIONS=(
	["f:"]="foo:"
	["b:"]="bar:"
	["v"]="verbose"
	["V"]="version"
	["h"]="help"
)
LONG_ONLY_OPTIONS=(
	"whiz:"
	"bang"
)

function join-with()
{
	SEP=$1
	shift
	JOINED=$1
	shift
	for ELEM in $@; do
		JOINED="$JOINED$SEP$ELEM"
	done
	echo "$JOINED"
}

SHRT_OPTS=$(join-with '' ${!COMMON_OPTIONS[@]})
LONG_OPTIONS=$(join-with ',' ${COMMON_OPTIONS[@]} ${LONG_ONLY_OPTIONS[*]})

OPTIONS=$(getopt -o "$SHRT_OPTS" --long "$LONG_OPTIONS" -n "$0" -- "$@")

if [ $? -ne 0 ]; then
	echo 'Terminating...' >&2
	exit 1
fi

# Note the quotes around "$OPTIONS": they are essential!
eval set -- "$OPTIONS"
unset OPTIONS

while true; do
	case "$1" in
		'-f'|'--foo')
			FOO=$2
			shift 2
			continue
		;;
		'-b'|'--bar')
			BAR=$2
			shift 2
			continue
		;;
		'--whiz')
			WHIZ=$2
			shift 2
			continue
		;;
		'--bang')
			BANG=1
			shift 1
			continue
		;;
		'-v'|'--verbose')
			VERBOSE=$(( $VERBOSE + 1 ))
			shift 1
			continue
		;;
		'-V'|'--version')
			echo "$OUR_NAME $VERSION"
			exit 0
		;;
		'-h'|'--help')
			usage
			exit 0
		;;
		'--')
			shift
			break
		;;
		*)
			echo 'Internal error!' >&2
			exit 1
		;;
	esac
done

if [ "$VERBOSE" -gt 1 ]; then
	set -x
fi

if [ -z "${FOO:-}" ] &&
   [ -z "${BAR:-}" ] &&
   [ -z "${WHIZ:-}" ] &&
   [ -z "${BANG:-}" ] ; then
	usage
	exit 1
fi

if [ "$VERBOSE" -gt 0 ]; then
	DASH_V=-v
else
	DASH_V=
fi


PID=$$
NOW=$(date --utc +%Y%m%dT%H%M%SZ)
FILE=/tmp/$OUR_NAME.$USER.$PID.$NOW.log

function cleanup()
{
	rm $DASH_V -f $FILE
}
trap cleanup EXIT

echo "{ FOO => '${FOO:-}', BAR => '${BAR:-}' }" \
	> $FILE

cat $FILE

WHIZ=${WHIZ:-}
BANG=${BANG:-0}

if [ $BANG -gt 0 ]; then
	echo "BANG!"
	exit 1
fi

if [ -n "$WHIZ" ]; then
	echo "whiz $WHIZ!"
fi

