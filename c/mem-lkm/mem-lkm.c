/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Copyright (C) 2021 Eric Herman */

#include <linux/fs.h>		/* struct file_operations */
#include <linux/init.h>		/* __init __exit */
#include <linux/kernel.h>
#include <linux/module.h>	/* module_init module_exit */
#include <linux/slab.h>		/* kmalloc */
#include <linux/uaccess.h>	/* copy_from_user */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eric Herman");
MODULE_DESCRIPTION("a sample kernel module accessing a byte array");
MODULE_VERSION("0.1");

static const char mem_name[] = "mem_buf";

/* globals */
static int mem_major = -1;
static char *mem_buf = NULL;
static uint mem_buf_size = 100;
module_param(mem_buf_size, uint, S_IRUGO);
MODULE_PARM_DESC(mem_buf_size, "defaults to 100");

int mem_open(struct inode *n, struct file *f);
int mem_release(struct inode *n, struct file *f);
ssize_t mem_read(struct file *f, char *buf, size_t count, loff_t *pos);
ssize_t mem_write(struct file *f, const char *buf, size_t count, loff_t *pos);

/* there are a boatload of these in fs.h, we only need 4 */
struct file_operations mem_access_funcs = {
	.owner = THIS_MODULE,
	.read = mem_read,
	.write = mem_write,
	.open = mem_open,
	.release = mem_release
};

int __init mem_init(void)
{
	int err = 0;
	const struct file_operations *fops = &mem_access_funcs;

	printk(KERN_INFO "mem_buf[%u] init\n", mem_buf_size);

	if (!mem_buf_size) {
		printk(KERN_WARNING "%s:%d:%s() INIT FAILED "
		       "mem_buf_size of zero\n", __FILE__, __LINE__, __func__);
		return -1;
	}

	err = register_chrdev(0, mem_name, fops);
	if (err < 0) {
		printk(KERN_WARNING "%s:%d:%s() INIT FAILED "
		       "register_chrdev(%u, %s, fops) == %d\n",
		       __FILE__, __LINE__, __func__, mem_major, mem_name, err);
		return err;
	}
	mem_major = err;
	printk(KERN_NOTICE
	       "mem_buf[%u] character device %d\n", mem_buf_size, mem_major);

	mem_buf = kmalloc(mem_buf_size, GFP_KERNEL);
	if (!mem_buf) {
		printk(KERN_WARNING "%s:%d:%s() INIT FAILED "
		       "kmalloc(%u, GFP_KERNEL) == NULL\n",
		       __FILE__, __LINE__, __func__, mem_buf_size);
		unregister_chrdev(mem_major, mem_name);
		return -ENOMEM;
	}

	memset(mem_buf, 0x00, mem_buf_size);

	return 0;
}

void mem_exit(void)
{
	if (mem_major >= 0) {
		unregister_chrdev(mem_major, mem_name);
		mem_major = -1;
	}
	if (mem_buf) {
		kfree(mem_buf);
		mem_buf = NULL;
	}
	printk(KERN_INFO "mem_buf[%u] exit\n", mem_buf_size);
}

module_init(mem_init);
module_exit(mem_exit);

int mem_open(struct inode *n, struct file *f)
{
	(void)n;
	(void)f;
	return 0;
}

int mem_release(struct inode *n, struct file *f)
{
	(void)n;
	(void)f;
	return 0;
}

ssize_t mem_read(struct file *f, char *buf, size_t count, loff_t *pos)
{
	unsigned long copied;
	size_t bytes = count;

	(void)f;

	printk(KERN_INFO "mem_buf[%u] read(f, buf, %zu, %zu)\n", mem_buf_size,
	       count, (size_t)*pos);

	if (!count) {
		return 0;
	}

	if (*pos >= mem_buf_size) {
		return 0;
	}

	if ((mem_buf_size - *pos) < count) {
		bytes = mem_buf_size - *pos;
	}

	copied = copy_to_user(buf, mem_buf + *pos, bytes);
	if (copied != 0) {
		printk(KERN_WARNING "%s:%d:%s() READ FAILED "
		       "copy_to_user(buf, ptr, %zu) == %lu\n",
		       __FILE__, __LINE__, __func__, bytes, copied);
		return -EFAULT;
	}

	*pos += bytes;

	return bytes;
}

ssize_t mem_write(struct file *f, const char *buf, size_t count, loff_t *pos)
{
	unsigned long copied;
	size_t bytes = count;

	(void)f;

	printk(KERN_INFO "mem_buf[%u] write(f, buf, %zu, %zu)\n",
	       mem_buf_size, count, (size_t)*pos);

	if (!count) {
		return 0;
	}

	if (*pos >= mem_buf_size) {
		return 0;
	}

	if ((mem_buf_size - *pos) < count) {
		bytes = mem_buf_size - *pos;
	}

	copied = copy_from_user(mem_buf + *pos, buf, bytes);
	if (copied) {
		printk(KERN_WARNING "%s:%d:%s() WRITE FAILED "
		       "copy_from_user(ptr, buf, %zu) == %lu\n",
		       __FILE__, __LINE__, __func__, bytes, copied);
		return -EFAULT;
	}

	return bytes;
}
