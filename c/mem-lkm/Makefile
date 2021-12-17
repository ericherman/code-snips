# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2021 Eric Herman

# sudo apt-get install \
#	$(apt-cache search linux-headers-$(uname -r) | cut -f1 -d' ')

SUDO ?= sudo

default: all
all: modules

Module = mem-lkm
Grep_string = 'mem_buf\|mem.lkm'

obj-m = $(Module).o

modules: $(Module).c
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

$(Module).ko: modules
	ls -l $@

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm *~

load: $(Module).ko
	$(SUDO) insmod ./$(Module).ko

unload:
	$(SUDO) rmmod ./$(Module).ko

check: test

test: $(Module).ko
	-( $(SUDO) rmmod $(Module).ko )
	$(SUDO) insmod $(Module).ko
	modinfo $(Module).ko
	lsmod | grep $(Grep_string)
	$(SUDO) rmmod $(Module).ko
	$(SUDO) insmod $(Module).ko mem_buf_size=200
	$(SUDO) rm -fv /dev/mem_buf
	$(SUDO) mknod /dev/mem_buf c \
		$$(cat /proc/devices | grep mem_buf | cut -f1 -d' ') \
		0
	$(SUDO) chmod 666 /dev/mem_buf
	$(SUDO) ls -l /dev/mem_buf
	echo "foo" >/dev/mem_buf
	if [ $$(cat /dev/mem_buf) != "foo" ]; then \
		echo "expected 'foo' but was $$(cat /dev/mem_buf)"; exit 1; \
	fi
	$(SUDO) rmmod $(Module).ko
	$(SUDO) grep $(Grep_string) /var/log/syslog | tail -n 10
	if [ $$(lsmod | grep -c $(Grep_string)) -gt 0 ]; then \
		echo "still loaded?"; \
		lsmod; \
		exit 1; \
	fi
	$(SUDO) rm -fv /dev/mem_buf

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
Lindent=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0
# see also: https://www.kernel.org/doc/Documentation/process/coding-style.rst
tidy:
	$(Lindent) \
		-T size_t -T ssize_t \
		-T inode -T file \
		-T loff_t \
		*.c
