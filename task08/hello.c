#include <linux/module.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/semaphore.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

#define ID_LENGTH 13

static const char id[] = "88a35103c0cc";
static char foo[PAGE_SIZE];

static struct dentry *parent_dir;

DEFINE_MUTEX(foo_mutex);

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

static ssize_t foo_read(struct file *filp,
		char __user *buffer,
		size_t len,
		loff_t *offset)
{
	ssize_t ret;

	mutex_lock(&foo_mutex);

	ret = simple_read_from_buffer(buffer, len, offset, foo, PAGE_SIZE);

	mutex_unlock(&foo_mutex);

	return ret;
}

static ssize_t foo_write(struct file *filp,
		const char __user *buffer,
		size_t len,
		loff_t *offset)
{
	ssize_t ret;

	if (len > PAGE_SIZE) {  
		pr_err("hello: Can't write more than %lu bytes\n", PAGE_SIZE);
		return -EINVAL;
	}

	mutex_lock(&foo_mutex);

	memset(foo, 0, sizeof(foo));
	ret = simple_write_to_buffer(foo, PAGE_SIZE, offset, buffer, len);

	mutex_unlock(&foo_mutex);

	return ret;
}

static struct file_operations id_ops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write
};

static struct file_operations foo_ops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write
};

static int hello_init(void)
{
	struct dentry* id_entry;
	struct dentry* foo_entry;

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
		pr_err("Failed to create file 'id'\n");
		return -1;
	}

	if (!debugfs_create_u64("jiffies", 0444, parent_dir, (u64 *)&jiffies))
		return -1;

	foo_entry = debugfs_create_file("foo", 0666, parent_dir, NULL, &foo_ops);

	if (!foo_entry) {
		pr_err("Failed to create file 'foo'\n");
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
