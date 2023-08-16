#!/bin/bash

if [ "_${VERBOSE}_" != "__" ] && [ "$VERBOSE" -gt 0 ]; then
set -x
fi

which lsb_release > /dev/null &&
	lsb_release -d 2>/dev/null | sed 's/Description:[[:space:]]*//'
arch
uname -r
lscpu | grep 'Model name:\|^CPU(s):\|Thread(s) per core:\|Core(s) per socket:\|Socket(s):\|CPU max MHz:'
which inxi > /dev/null && inxi -M | grep 'Type: '
head -n1 /proc/meminfo
echo
cat /proc/cmdline
echo
lspci | grep VGA
echo
lsmod | grep "kms\|drm"
echo
glxinfo | grep -i "vendor\|rendering"
echo
grep LoadModule /var/log/Xorg.0.log
echo
egrep -i " connected|card detect|primary dev|Setting driver" /var/log/Xorg.0.log
echo
lspci -k | grep -EA3 'VGA|3D|Display'
