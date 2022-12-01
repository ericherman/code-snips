#!/bin/bash

# sourcing this file creates the "fix-ssh" command

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

	best_ctime=0
	for file in $(ls -t $SSH_AUTH_SOCK /tmp/ssh*/*agent*); do
		sfile=$(readlink -f $file)
		if [ $fix_ssh_verbose -gt 0 ]; then
			ls -l $sfile
		fi
		if [ -S "$sfile" ]; then
			ctime=$(stat --format=%Z "$sfile")
			if [ $fix_ssh_verbose -gt 0 ]; then
				echo "ctime: $ctime"
			fi
			if [ $ctime -gt $best_ctime ]; then
				$LN -sf $sfile ~/.ssh_auth_sock &&
				export SSH_AUTH_SOCK=~/.ssh_auth_sock &&
				best_ctime=$ctime
			fi
		fi
	done
	if [ $fix_ssh_verbose -gt 0 ]; then
		echo SSH_AUTH_SOCK=$SSH_AUTH_SOCK
		echo "'~/.ssh_auth_sock' -> '$(readlink -f ~/.ssh_auth_sock)'"
	fi
}
