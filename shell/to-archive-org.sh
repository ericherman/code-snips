#!/bin/bash

# TODO FIXXXME: use something better than bash

rm -f urls.txt

# find . -type f -name '*.md' -print0 |
# while IFS= read -r -d '' FILE; do

BRANCH_NAME=$(git branch | grep \* | cut -d ' ' -f2)
for FILE in $(git ls-tree -r --name-only $BRANCH_NAME); do
	grep '(http[s]\?:' "$FILE" |
		sed -e's/.*(\(http[s]*:[^)]*\).*/\1/' >> urls.txt
	grep '<http[s]\?:' "$FILE" |
		sed -e's/.*<\(http[s]*:[^>]*\).*/\1/' >> urls.txt
	grep '"http[s]\?:' "$FILE" |
		sed -e's/.*"\(http[s]*:[^"]*\).*/\1/' >> urls.txt
done

cat urls.txt | sort -u > urls-sorted.txt

PAUSE_TIME=1.5
for URL in $(cat urls-sorted.txt); do
	sleep $PAUSE_TIME
	ARCHIVE_URL="http://web.archive.org/save/$URL"
	echo $ARCHIVE_URL
	curl $ARCHIVE_URL
done
