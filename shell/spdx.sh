#!/bin/bash
# SPDX-License-Identifier: LGPL-2.1-or-later
# Copyright (C) 2020 Eric Herman <eric@freesa.org>

THIS_YEAR=$(date +%Y)

function add_spdx() {
	FILE=$1
	SPDX_ID=$2
	shift 2
	HOLDER="$@"

	EXTENSION=${FILE##*.}
	if [ $EXTENSION == "py" ]; then
		COMMENT='# '
		ENDC=''
	elif [ $EXTENSION == "js" ]; then
		COMMENT='// '
		ENDC=''
	elif [ $EXTENSION == 'html' ]; then
		COMMENT='<!-- '
		ENDC=' -->'
	elif [ $EXTENSION == 'scss' ]; then
		COMMENT='/* '
		ENDC=' */'
	elif [ $EXTENSION == 'json' ]; then
		return
	else
		echo "unknown file type '$EXTENSION' for $FILE"
		return
	fi

	grep SPDX-License-Identifier $FILE
	if [ $? -eq 0 ]; then return; fi

	echo "${COMMENT}SPDX-License-Identifier: ${SPDX_ID}${ENDC}" > $FILE.0
	FROM_YEAR=$( git log --diff-filter=A --follow \
			--format='%ad' --date=format:%Y -1 -- $FILE )
	if [ "$FROM_YEAR" == "$THIS_YEAR" ]; then
		YEARS="$THIS_YEAR"
	else
		YEARS="$FROM_YEAR - $THIS_YEAR"
	fi
	echo "${COMMENT}Copyright (C) $YEARS ${HOLDER}${ENDC}" >> $FILE.0
	cat $FILE >> $FILE.0
	cat $FILE.0 > $FILE
	rm $FILE.0
	git diff $FILE
}

add_spdx $@
