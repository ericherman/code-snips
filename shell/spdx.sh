#!/bin/bash
# SPDX-License-Identifier: LGPL-2.1-or-later
# Copyright (C) 2020-2023 Eric Herman <eric@freesa.org>

if [ "_${VERBOSE}_" != "__" ] && [ "$VERBOSE" -gt 0 ]; then
	set -x
fi

IGNORE_PATTERN_DEFAULT='\.json\|LICENSE\|COPYING'

function usage() {
PROG=`basename $0`
cat << EOF
example usage:

	$PROG LGPL-2.1-or-later 'Jane Doe <jd@example.org>'

environment variables:

set USE_SPDX_FILE_COPYRIGHT=1 to enable "SPDX-FileCopyrightText:" prefix
set SKIP_COPYRIGHT_PREFIX=1 to skip 'Copyright (C)' in the attribution text
set IGNORE_PATTERN to tell grep to match different files than the default
	IGNORE_PATTERN_DEFAULT: '$IGNORE_PATTERN_DEFAULT'
set MARKDOWN_FRONTMATTER=1 to us YAML style in .md that begins with '---'
EOF
}

if [ "_${1}_" == "__" ] || [ "_${2}_" == "__" ]; then
	usage
	exit 1
fi

THIS_YEAR=$(date +%Y)

function add_spdx() {
	FILE=$1
	SPDX_ID=$2
	shift 2
	HOLDER="$@"
	PRESERVE_FIRST=0

	EXTENSION=${FILE##*.}
	if [ $EXTENSION == "py" ]; then
		COMMENT='# '
		ENDC=''
	elif [ $EXTENSION == "sh" ]; then
		PRESERVE_FIRST=1
		COMMENT='# '
		ENDC=''
	elif [ $EXTENSION == "js" ]; then
		COMMENT='// '
		ENDC=''
	elif [ $EXTENSION == "ts" ]; then
		COMMENT='// '
		ENDC=''
	elif [ $EXTENSION == 'html' ]; then
		COMMENT='<!-- '
		ENDC=' -->'
	elif [ $EXTENSION == 'scss' ]; then
		COMMENT='/* '
		ENDC=' */'
	elif [ $EXTENSION == 'sql' ]; then
		COMMENT='-- '
	elif [ $EXTENSION == 'tex' ]; then
		COMMENT='% '
		ENDC=''
	elif [ $EXTENSION == 'yml' ]; then
		COMMENT='# '
		ENDC=''
	elif [ $EXTENSION == 'yaml' ]; then
		COMMENT='# '
		ENDC=''
	elif [ $EXTENSION == 'md' ]; then
		if [ "_${MARKDOWN_FRONTMATTER}_" != "__" ] &&
			[ "${MARKDOWN_FRONTMATTER}" -gt 0 ] &&
			[ "_$(head -n1 $FILE)_" == "_---_" ]; then
			PRESERVE_FIRST=1
			COMMENT='# '
			ENDC=''
		else
			COMMENT='<!-- '
			ENDC=' -->'
		fi
	elif [ $EXTENSION == 'json' ]; then
		return
	else
		echo "unknown file type '$EXTENSION' for $FILE"
		return
	fi

	grep SPDX-License-Identifier $FILE
	if [ $? -eq 0 ]; then return; fi

	FROM_YEAR=$( git log --diff-filter=A --follow \
			--format='%ad' --date=format:%Y -1 -- $FILE )
	if [ "$FROM_YEAR" == "$THIS_YEAR" ]; then
		YEARS="$THIS_YEAR"
	else
		YEARS="${FROM_YEAR}-${THIS_YEAR}"
	fi

	if [ "_${USE_SPDX_FILE_COPYRIGHT}_" != "__" ] \
		&& [ "${USE_SPDX_FILE_COPYRIGHT}" -gt 0 ]; then
		SPDX_COPYRIGHT_PREFIX="SPDX-FileCopyrightText:"
	else
		SPDX_COPYRIGHT_PREFIX=""
	fi;
	if [ "_${SKIP_COPYRIGHT_PREFIX}_" == "__" ] \
		|| [ "${SKIP_COPYRIGHT_PREFIX}" -eq 0 ]; then
		COPYRIGHT_PREFIX="${SPDX_COPYRIGHT_PREFIX} Copyright (C)"
	else
		COPYRIGHT_PREFIX="${SPDX_COPYRIGHT_PREFIX}"
	fi

	touch $FILE.0
	FILE_LINES=$(cat $FILE | wc -l)
	if [ $FILE_LINES > 0 ]; then
		if [ $PRESERVE_FIRST -gt 0 ]; then
			head -n1 $FILE > $FILE.0
			FILE_LINES=$(( $FILE_LINES - 1 ))
		fi
	fi

	echo "${COMMENT}SPDX-License-Identifier: ${SPDX_ID}${ENDC}" >> $FILE.0
	echo "${COMMENT}${COPYRIGHT_PREFIX} ${YEARS} ${HOLDER}${ENDC}" >> $FILE.0

	tail -n$FILE_LINES $FILE >> $FILE.0
	cat $FILE.0 > $FILE
	rm $FILE.0
	git diff $FILE
}

function git_list_files() {
		# -r Recurse into sub-trees.
	git ls-tree \
		--full-tree \
		-r \
		--name-only \
		HEAD
}

if [ "_${IGNORE_PATTERN}_" == "__" ]; then
	IGNORE_PATTERN=$IGNORE_PATTERN_DEFAULT
fi

for FILE in $(git_list_files | grep --invert-match $IGNORE_PATTERN); do
	if ! [ -f $FILE ]; then
		continue
	fi
	if grep --quiet SPDX-License-Identifier $FILE; then
		continue
	fi
	add_spdx $FILE $@
done
