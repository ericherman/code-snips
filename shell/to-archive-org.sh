#!/bin/bash
# SPDX-License-Identifier: LGPL-2.1-or-later
# to-archive-org.sh : send referenced URLs to the wayback machine
# Copyright (C) 2020 Eric Herman <eric@freesa.org>

# TODO FIXXXME: use something better than bash

rm -f urls.txt

# find . -type f -name '*.md' -print0 |
# while IFS= read -r -d '' FILE; do

BRANCH_NAME=$(git branch | grep \* | cut -d ' ' -f2)
for FILE in $(git ls-tree -r --name-only $BRANCH_NAME); do
	# URLs inside parens
	grep '(http[s]\?:' "$FILE" \
		| sed -e's/.*[(]\(http[s]*:[^)]*\).*/\1/' \
		>> urls.txt
	# URLs inside angle brackets
	grep '<http[s]\?:' "$FILE" \
		| sed -e's/.*<\(http[s]*:[^>]*\).*/\1/' \
		>> urls.txt
	# URLs inside double-quotes
	grep '"http[s]\?:' "$FILE" \
		| sed -e's/.*"\(http[s]*:[^"]*\).*/\1/' \
		>> urls.txt
done

cat urls.txt | cut -f1 -d'#' | sort -u > urls-sorted.txt

# cat urls-sorted.txt

echo "sending to the wayback machine ..."
#PAUSE_TIME=1.5
PAUSE_TIME=2.5
for URL in $(cat urls-sorted.txt); do
	sleep $PAUSE_TIME
	wget  --post-data "url=$URL" https://web.archive.org/save
done
echo "done"
