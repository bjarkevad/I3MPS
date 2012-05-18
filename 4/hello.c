#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("DUAL BSD/GPL");

// Output "Hello, Kernel" on module load
static int hello_init(void)
{
	printk(KERN_ALERT "Hello, Kernel!\n");
	return 0;
}

// Output "Goodbye, Kernel" on module unload
static void hello_exit(void)
{
	printk(KERN_ALERT "Goodbye, Kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);
