#!/usr/bin/env bash

VERSION=0.0.0
BUILD_DIR_DEFAULT="./build"
FOO_DEFAULT="foo"

if [ -z "${VERBOSE:-}" ]; then
	VERBOSE=0
fi
if ! [[ "$VERBOSE" =~ ^-?[0-9]+$ ]]; then
	echo >&2 "VERBOSE='$VERBOSE' not an integer"
	exit 1
fi

function usage()
{
	local NAME=$(basename "$0")
	cat <<EOF
Usage: $NAME [OPTION]... [ARG1] [ARG2]

  -b, --build-dir[=/to/build]   path to build
				[DEFAULT=$BUILD_DIR_DEFAULT]
  -f, --foo[=bar]		path to build
				[DEFAULT=$FOO_DEFAULT]
  -v, --verbose			explain what is being done
  -h, --help			display this help and exit
  -V, --version			output version information ($VERSION) and exit

This is a demo.

EOF
}

# long options that require an argument, space‑separated
LONG_OPTION_REQUIRES_ARG="build-dir foo"

EXCLUDE_DIR=()
while getopts b:f:hvV-: OPT; do
	if [ "$OPT" = "-" ]; then
		# extract long option name
		OPT="${OPTARG%%=*}"
		# extract long option argument (may be empty)
		OPTARG="${OPTARG#"$OPT"}"
		OPTARG="${OPTARG#=}"

		# argument consumption for required‑argument options
		case " $LONG_OPTION_REQUIRES_ARG " in
			*" $OPT "*)
				ERR="Error: option --$OPT requires an argument"
				if [ -z "$OPTARG" ]; then
					# no argument after '=',
					# try to take the next word
					if [ -n "${!OPTIND}" ] \
					&& [[ ! "${!OPTIND}" =~ ^- ]]; then
						OPTARG="${!OPTIND}"
						OPTIND=$((OPTIND + 1))
					else
						echo >&2 "$ERR"
						usage
						exit 1
					fi
				fi
				;;
		esac
	fi

	case "$OPT" in
		b | build-dir )
			BUILD_DIR="$OPTARG"
			;;
		f | foo )
			FOO="$OPTARG"
			;;
		h | help )
			usage
			exit 0
			;;
		v | verbose )
			# optional argument: if provided, use it; otherwise increment
			if [ -n "$OPTARG" ]; then
				VERBOSE="$OPTARG"
			else
				VERBOSE=$((VERBOSE + 1))
			fi
			;;
		V | version )
			echo "$VERSION"
			if [ -n "$OPTARG" ]; then
				echo >&2 "Error: unexpected argument '$OPTARG' for --$OPT"
				usage
				exit 1
			fi
			exit 0
			;;
		\? )
			# bad short option (error reported via getopts)
			usage
			exit 1
			;;
		* )
			echo >&2 "Illegal option --$OPT"
			usage
			exit 1
			;;
	esac
done
shift $((OPTIND-1))

if [ "$VERBOSE" -gt 1 ]; then
	set -x
fi

BUILD_DIR="${BUILD_DIR:-$BUILD_DIR_DEFAULT}"
FOO="${FOO:-$FOO_DEFAULT}"
mkdir -pv "$BUILD_DIR"
echo "BUILD_DIR=$BUILD_DIR"
echo "FOO=$FOO"
echo "VERBOSE=$VERBOSE"
echo "Additional args: $@"
