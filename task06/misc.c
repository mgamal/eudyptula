#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

#define ID_LENGTH 13

static const char id[] = "88a35103c0cc";

static ssize_t misc_read(struct file *filp,
		char __user *buffer,
		size_t len,
		loff_t *offset)
{
	ssize_t ret;

	ret = simple_read_from_buffer(buffer, len, offset, id, strlen(id));
	if (ret < 0)
		return ret;

	pr_info("misc: reading buffer %s, ret=%lu, len=%lu\n", id, ret, len);

	/* Append new line to buffer before returning */
	if (copy_to_user(buffer + (strlen(id)-1), "\n", 2) < 0)
		return -1;

	return ret;
}

static ssize_t misc_write(struct file *filp,
                const char __user *buffer,
                size_t len,
                loff_t *offset)
{
	ssize_t ret;

	char tmp[ID_LENGTH];

	memset(tmp, 0, sizeof(id));

	ret = simple_write_to_buffer(tmp, sizeof(id), offset, buffer, len - 1);

	if (ret < 0)
		return ret;

	pr_info("misc: writing buffer: %s\n", tmp);

	if (strncmp(id, tmp, len - 1) != 0)
		return -EINVAL;

	return len;
}

struct file_operations misc_fops = {
	.owner = THIS_MODULE,
	.read = misc_read,
	.write = misc_write
};

struct miscdevice misc_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &misc_fops
};

static int misc_init(void)
{
	pr_info("misc: Hello World!\n");
	return misc_register(&misc_dev);
}

static void misc_exit(void)
{
	misc_deregister(&misc_dev);
	pr_info("misc: Goodbye World!\n");
}

module_init(misc_init);
module_exit(misc_exit);

