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
		READLINK="readlink -v"
	else
		LN=ln
		READLINK=readlink
	fi

	possible_sockets=$(ls -t $SSH_AUTH_SOCK /tmp/ssh*/*agent* 2>/dev/null)
	if [ $fix_ssh_verbose -gt 0 ]; then
		echo "possible sockets:"
		echo "$possible_sockets"
	fi

	best_ctime=0
	best_sock=""

	for possible in $possible_sockets; do
		if [ $fix_ssh_verbose -gt 0 ]; then
			echo
			echo $possible;
		fi

		# if a link, follow it and get the canonical file path
		canonical=$($READLINK -f $possible)

		if [ $fix_ssh_verbose -gt 0 ]; then
			ls -l $canonical
		fi

		if ! [ -w "$canonical" ]; then
			if [ $fix_ssh_verbose -gt 0 ]; then
				echo "$canonical is not writable"
			fi
			continue
		fi

		if ! [ -S "$canonical" ]; then
			if [ $fix_ssh_verbose -gt 0 ]; then
				echo "$canonical is not a socket"
			fi
			continue
		fi

		ctime=$(stat --format=%Z "$canonical")
		if [ $fix_ssh_verbose -gt 0 ]; then
			echo "  ctime: $ctime ($(date -d @$ctime))"
		fi

		if [ $ctime -gt $best_ctime ]; then
			best_sock=$canonical
			best_ctime=$ctime
		fi
	done

	if [ "_${best_sock}_" == "__" ]; then
		echo "no sockets found"
		exit 1
	fi

	if [ $fix_ssh_verbose -gt 0 ]; then
		echo
		echo "best sock: $best_sock"
		echo "  ctime: $best_ctime ($(date -d @$best_ctime))"
		echo
	fi

	$LN -sf $best_sock ~/.ssh_auth_sock &&
	export SSH_AUTH_SOCK=~/.ssh_auth_sock

	if [ $fix_ssh_verbose -gt 0 ]; then
		echo SSH_AUTH_SOCK=$SSH_AUTH_SOCK
	fi
}
