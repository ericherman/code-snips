#!/bin/bash

if [ "_${VERBOSE}_" != "__" ] && [ $VERBOSE -gt 0 ]; then
	set -x
fi

# We could restrict to Portable Filename characters ('a-zA-Z0-9\._\-')
# as defined by POSIX[1], but really, getting rid of spaces is good enough.
#
# The replace is a bit overly-broad, for example in sound/mips/snd-n64.c
# we see:
# -MODULE_DESCRIPTION("N64 Audio");
# +MODULE_DESCRIPTION("N64_Audio");
# and:
# - strcpy(card->driver, "N64 Audio");
# - strcpy(card->shortname, "N64 Audio");
# - strcpy(card->longname, "N64 Audio");
# + strcpy(card->driver, "N64_Audio");
# + strcpy(card->shortname, "N64_Audio");
# + strcpy(card->longname, "N64_Audio");
#
# But again, this seems okay-enough to me.
#
# There are probably some that are missed, any that have a string defined
# to a variable and the variable used in the request_irq parameters.
#
# There will still be some slightly goofy directory names with commas and
# colons and parens and square brackets, but that's probably fine.
#
# [1] https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap03.html#tag_03_282

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
			STR2=$( echo -n "$MATCH" | sed 's/ /_/g' )
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
			STR2=$( echo -n "$MATCH" | sed 's/ /_/g' )
			sed -i -e "s/$MATCH/$STR2/g" $FILE
		fi
	done
done
