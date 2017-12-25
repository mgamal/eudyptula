#include <linux/module.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

#define ID_LENGTH 13

static const char id[] = "88a35103c0cc";

static struct dentry *parent_dir;

static ssize_t id_read(struct file *filp,
                char __user *buffer,
                size_t len,
                loff_t *offset)
{
        ssize_t ret;

        ret = simple_read_from_buffer(buffer, len, offset, id, strlen(id));
        if (ret < 0)
                return ret;

        pr_debug("misc: reading buffer %s, ret=%lu, len=%lu\n", id, ret, len);

        /* Append new line to buffer before returning */
        if (copy_to_user(buffer + (strlen(id)-1), "\n", 2) < 0)
                return -1;

        return ret;
}

static ssize_t id_write(struct file *filp,
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

        pr_debug("misc: writing buffer: %s\n", tmp);

        if (strncmp(id, tmp, len - 1) != 0)
                return -EINVAL;

        return len;
}

static struct file_operations id_ops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write
};
static int hello_init(void)
{
	struct dentry* id_entry;

	pr_debug("Hello World!\n");

	parent_dir = debugfs_create_dir("eudyptula", NULL);

	if (parent_dir == ERR_PTR(-ENODEV)) {
		pr_err("CONFIG_DEBUG_FS not enabled\n");
		return -1;
	}

	if (!parent_dir) {
		pr_err("Failed to create directory");
		return -1;
	}

	id_entry = debugfs_create_file("id", 0644, parent_dir, NULL, &id_ops);

	if (!id_entry) {
		pr_err("Failed to create file 'id' \n");
		return -1;
	}

	return 0;
}

static void hello_exit(void)
{
	debugfs_remove_recursive(parent_dir);
	pr_debug("Goodbye World!\n");
}

module_init(hello_init);
module_exit(hello_exit);

