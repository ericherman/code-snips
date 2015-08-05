#!/bin/bash

case "$1" in
	'one')
		echo "one will break"
		echo "1"
		;; # break

	'two' | '2')
		echo "two will fall through"
		echo "2"
		;& # fall-through

	[tT][hH][rR][eE][eE] | '3')
		echo "three will break"
		echo "3"
		;; # break

	*) # default
		echo "Usage: $0 [one|two|three]"
		;; # break (optional)
esac
