/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Copyright (C) 2021 Eric Herman */

#include <linux/init.h>		/* __init __exit */
#include <linux/module.h>	/* module_init module_exit */
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eric Herman");
MODULE_DESCRIPTION("a 'hello, world' kernel module");
MODULE_VERSION("0.1");

static char *who = "world";
module_param(who, charp, S_IRUGO);
MODULE_PARM_DESC(who, "default of 'world'");

static int __init hello_init(void)
{
	printk(KERN_INFO "hello %s init\n", who);
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "hello %s exit\n", who);
}

module_init(hello_init);
module_exit(hello_exit);
