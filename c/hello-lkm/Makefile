# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2021 Eric Herman

# sudo apt-get install \
#	$(apt-cache search linux-headers-$(uname -r) | cut -f1 -d' ')

SUDO ?= sudo

default: all
all: modules

Module_name = hello-lkm
Module_ko = $(Module_name).ko

obj-m = $(Module_name).o

modules: hello-lkm.c
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

check: test

test: modules
	$(SUDO) insmod $(Module_ko)
	lsmod | grep hello
	modinfo $(Module_ko)
	$(SUDO) rmmod $(Module_ko)
	$(SUDO) insmod $(Module_ko) who="$(USER)"
	$(SUDO) rmmod $(Module_ko)
	$(SUDO) grep hello /var/log/syslog
	if [ $$(lsmod | grep -c 'hello') -gt 0 ]; then \
		echo "still loaded?"; \
		lsmod; \
		exit 1; \
	fi

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
Lindent=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0
# see also: https://www.kernel.org/doc/Documentation/process/coding-style.rst
tidy:
	$(Lindent) *.c
