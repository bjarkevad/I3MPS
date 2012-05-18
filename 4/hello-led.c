#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/ioport.h>
#include <asm/io.h>

MODULE_LICENSE("DUAL BSD/GPL");

// Output "Hello, Kernel" on module load
static int hello_init(void)
{
	printk(KERN_ALERT "LED-module inserted");
	
	volatile unsigned long *REG_GPIO6_OE;
	volatile unsigned long *REG_GPIO6_DIN; 
	volatile unsigned long *REG_GPIO6_DOUT;

	unsigned long temp;
	unsigned long ledTemp;

	if (request_mem_region (0x49058034, 12, "hello") == NULL) {
           printk ("Allocation for I/O memory range is failed\n");
           return 0;
     	}

	REG_GPIO6_OE = ioremap( 0x49058034, 1);
	REG_GPIO6_DIN = ioremap(0x49058038, 1);
	REG_GPIO6_DOUT = ioremap(0x4905803c, 1); 
	
	//Set LED to output
	temp = ioread32(REG_GPIO6_OE);

	while(1) {
		temp = ioread32(REG_GPIO6_DIN);
		ledTemp = ioread32(REG_GPIO6_DOUT);

		if((temp & 0x04000000) == 0)
			ledTemp |= 0x00000008;
		else
			ledTemp &= 0xfffffff7;
		
		iowrite32(ledTemp, REG_GPIO6_DOUT);
	}

	return 0;
}

// Output "Goodbye, Kernel" on module unload
static void hello_exit(void)
{
	printk(KERN_ALERT "LED-module removed\n");
	release_mem_region(0x49058034, 12); 
	release_mem_region(0x48310038, 12); 
}

module_init(hello_init);
module_exit(hello_exit);
