#!/bin/bash

THIS_YEAR=$(date +%Y)

function add_spdx() {
	FILE=$1
	SPDX_ID=$2
	HOLDER=$3

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
	FROM_YEAR=$( git log --pretty=format:'%ad' --date=format:%Y $FILE \
		| sort -u \
		| head -n1 )
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
