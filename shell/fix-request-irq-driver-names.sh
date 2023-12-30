#!/bin/bash

if [ "_${VERBOSE}_" != "__" ] && [ $VERBOSE -gt 0 ]; then
	set -x
fi

# https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap03.html#tag_03_282
FILENAME_CHARS='a-zA-Z0-9\._\-'

# single line
for FILE in $(git grep -l 'request_irq([^")]*"[^"]* '); do
	KEEP_SEARCHING=1
	while [ $KEEP_SEARCHING -ne 0 ]; do
		MATCH=$(grep 'request_irq([^")]*"[^"]* ' $FILE \
			| grep --only-matching '"[^"]* [^"]*"' \
			| head -n1)
		if [ "_${MATCH}_" == "__" ]; then
			KEEP_SEARCHING=0
		else
			STR2=$( echo -n "$MATCH" \
				| tr --complement '"'"$FILENAME_CHARS" "_")
			sed -i -e "s/$MATCH/$STR2/g" $FILE
		fi
	done
done

# multi-line
for FILE in $( git grep -A1 'request_irq([^")]*$' \
			| grep -B1 '"[^",)]* ' \
			| grep '\.c:' \
			| cut -f1 -d: \
			| sort -u ); do
	KEEP_SEARCHING=1
	while [ $KEEP_SEARCHING -ne 0 ]; do
		MATCH=$(grep -A1 'request_irq([^")]*$' $FILE \
			| grep --only-matching '"[^"]* [^"]*"' \
			| head -n1)
		if [ "_${MATCH}_" == "__" ]; then
			KEEP_SEARCHING=0
		else
			STR2=$( echo -n "$MATCH" \
				| tr --complement '"'"$FILENAME_CHARS" "_")
			sed -i -e "s/$MATCH/$STR2/g" $FILE
		fi
	done
done
