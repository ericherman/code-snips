#!/bin/bash
fix-ssh "$@"
RV=$?
if [ $RV -eq 0 ]; then
	exit 0
fi

find $(echo "$PATH" | tr ':' ' ' | xargs readlink -f | sort -u ) \
	-name fix-ssh.env \
	2>/dev/null
echo
echo "is fix-ssh.env sourced?"
exit $RV
