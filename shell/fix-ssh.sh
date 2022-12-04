#!/bin/bash

# sourcing this file creates the "fix-ssh" command
#
# running fix-ssh will:
# 1) symlink ~/.ssh_auth_sock to the socket with the most recent ctime
# 2) set SSH_AUTH_SOCK=~/.ssh_auth_sock
#
# atime (Access timestamp): last time a file was accessed
# mtime (Modified timestamp): last time a file's contents were modified
# ctime (Change timestamp): last time a file's metadata was changed
# stat(1) uses %X for atime, %Y for mtime, %Z for ctime
#
# if any argument is added to fix-ssh, it will be in verbose mode

function fix-ssh() {

	if [ "_${1}_" == "__" ]; then
		fix_ssh_verbose=0
	else
		fix_ssh_verbose=1
	fi

	if [ $fix_ssh_verbose -gt 0 ]; then
		LN="ln -v"
	else
		LN=ln
	fi

	possible_sockets=$(ls -t $SSH_AUTH_SOCK /tmp/ssh*/*agent* 2>/dev/null)

	best_ctime=0
	best_sock=""

	for file in $possible_sockets; do
		sfile=$(readlink -f $file)

		if [ $fix_ssh_verbose -gt 0 ]; then
			ls -l $sfile
		fi

		if [ -S "$sfile" ]; then
			ctime=$(stat --format=%Z "$sfile")

			if [ $fix_ssh_verbose -gt 0 ]; then
				echo "  ctime: $ctime ($(date -d @$ctime))"
			fi

			if [ $ctime -gt $best_ctime ]; then
				best_sock=$sfile
				best_ctime=$ctime
			fi
		fi
	done

	if [ "_${best_sock}_" == "__" ]; then
		echo "no sockets found"
		exit 1
	fi

	$LN -sf $best_sock ~/.ssh_auth_sock &&
	export SSH_AUTH_SOCK=~/.ssh_auth_sock

	if [ $fix_ssh_verbose -gt 0 ]; then
		echo SSH_AUTH_SOCK=$SSH_AUTH_SOCK
	fi
}
