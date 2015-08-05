#!/bin/bash

case "$1" in
	'one')
		echo "one will break"
		echo "1"
		;;
	'two' | '2')
		echo "two will fall through"
		echo "2"
		;&
	[tT][hH][rR][eE][eE] | '3')
		echo "three will break"
		echo "3"
	;;
	*) # default
		echo "Usage: $0 [one|two|three]"
	;;
esac
