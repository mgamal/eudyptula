#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

static int hello_init(void)
{
	pr_debug("Hello World!\n");
	return 0;
}

static void hello_exit(void)
{
	pr_debug("Goodbye World!\n");
}

module_init(hello_init);
module_exit(hello_exit);

