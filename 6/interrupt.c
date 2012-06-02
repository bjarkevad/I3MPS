/*                                                     
 * ID: mygpio.c 
 */                                                    
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>

#define MYGPIO_MAJOR  62
#define MYGPIO_MINOR   0
#define MYGPIO_CHS     8
#define MAXLEN       512

int Minor; 				// choose GPIO by selecting minor number

static struct cdev MyGpioDev;
struct file_operations mygpio_Fops;

static int devno;

DECLARE_WAIT_QUEUE_HEAD(wait_queue);
int flag = 0;

const unsigned int gpio_len = 8;
struct gpioPort // define struct
{
  unsigned int num;
  enum direnu { in, out } dir;
  const char label[10]; 		// Very important to specify a size. Just [] causes havoc to gpio indexing
};

struct gpioPort gpio[] =    
{
  {130, in, "gpio0_0"},
  {131, in, "gpio0_1"},
  {132, in, "gpio0_2"},
  {133, in, "gpio0_3"},
  {134, out, "gpio0_4"},
  {135, out, "gpio0_5"},
  {136, out, "gpio0_6"},
  {137, out, "gpio0_7"}
};

irqreturn_t my_isr(int irq, void *dev_id)
{
	printk(KERN_ALERT "INTERRUPT!\n");
	flag = 1;	
	wake_up_interruptible(&wait_queue);
	return IRQ_HANDLED;
}


static int mygpio_init(void)
{
  int err; 
    
    printk(KERN_ALERT "Mygpio Module Inserted\n");

    /*
     * Allocate Major/Minor Numbers 
     */
    devno = MKDEV(MYGPIO_MAJOR, MYGPIO_MINOR);
    if((err=register_chrdev_region(devno, gpio_len, "myGpio"))<0){
      printk(KERN_ALERT "Can't Register Major no: %i\n", MYGPIO_MAJOR);
      return err;
    }

    /*
     * Create cdev
     */       
    cdev_init(&MyGpioDev, &mygpio_Fops);
    err = cdev_add(&MyGpioDev, devno, gpio_len);
    if (err) {
        printk (KERN_ALERT "Error %d adding MyGpio device\n", err);
        return -1;
    }

    /*
     * Request GPIO Ressources
     */
    if(gpio_request(137, "gpio0_7") < 0)
		printk("Error requesting GPIO %i\n", 131);
    /*
     * Set GPIO Direction
     */
    /*** YOUR CODE HERE ****/
	gpio_direction_input(137); // for one gpio
	request_irq(gpio_to_irq(137), my_isr, IRQF_TRIGGER_RISING, "myIRQ", NULL);
    return 0;
}

static void mygpio_exit(void)
{
     
    printk(KERN_NOTICE "Removing Mygpio Module\n");
		free_irq(gpio_to_irq(137), NULL);
    /*
     * Free GPIO Ressources 
     */
    gpio_free(131);
    /*
     * Delete cdev
     */
    unregister_chrdev_region(devno, gpio_len);
    cdev_del(&MyGpioDev);

}


int mygpio_open(struct inode *inode, struct file *filep)
{
    int major, minor;

    major = MAJOR(inode->i_rdev);
    minor = MINOR(inode->i_rdev);
    
    Minor = minor;
    
    printk("Opening MyGpio Device [major], [minor]: %i, %i\n", major, minor);

    /*
     *  Try to get the module sem
     */
    if (!try_module_get(mygpio_Fops.owner))
      return -ENODEV;


    return 0;
}

int mygpio_release(struct inode *inode, struct file *filep)
{
    int minor, major;
    
    major = MAJOR(inode->i_rdev);
    minor = MINOR(inode->i_rdev);

    printk("Closing MyGpio Device [major], [minor]: %i, %i\n", major, minor);

    /*
     * Put the module semaphore
     * to release it
     */
    module_put(mygpio_Fops.owner);
    
    return 0;


}



ssize_t mygpio_write(struct file *filep, const char __user *ubuf, 
                  size_t count, loff_t *f_pos)
{
    int minor, len, value;
    char kbuf[MAXLEN];
    
    /*
     * retrieve minor from file ptr
     */
    minor = MINOR(filep->f_dentry->d_inode->i_rdev);
    printk(KERN_ALERT "Writing to MyGpio [Minor] %i \n", minor);

    /*
     * Copy data from user space
     * to kernel space
     */  
    len = count < MAXLEN ? count : MAXLEN; // set len < MAXLEN

    /*
	 * Use copy_from_user to 
	 * get data 
	 */
    if(copy_from_user(kbuf, ubuf, len))
        return -EFAULT;
	
    kbuf[len] = '\0';   // Pad null termination to string
    printk("string from user: %s\n", kbuf);

    /*
     * Convert string to int
     * using sscanf
     */
    sscanf(kbuf,"%i", &value);
    printk("value %i\n", value);
    
    value = value > 0 ? 1 : 0;

    /*
     * Use gpio_set_value on appropriate port 
     * (if an output port)
     */
    if(Minor > 3 && Minor <= 7) // gpio's defined as output 
	gpio_set_value(131,value);

    else
      printk("IO %i is defined as input!- can't write\n", 131);
    
    *f_pos += count;
    return count;
}

ssize_t mygpio_read(struct file *filep, char __user *buf, 
                 size_t count, loff_t *f_pos)
{
   
   wait_event_interruptible(wait_queue, flag == 1);
   flag = 0;
   char readBuf[MAXLEN];
   int len, result=-1, minor;

    /*
     * retrieve minor from file ptr
     */
    minor = MINOR(filep->f_dentry->d_inode->i_rdev);
    printk(KERN_ALERT "Reading from MyGpio [Minor] %i \n", minor);

    /*
     * Read GPIO port
     * (if an input port)
     */
    if(Minor <= 3) // IO defined as input
	result = gpio_get_value(131);


    else
      printk("IO %i is defined as output! - Can't read!\n", 131);
	 
    /*
     * Convert int to string
     * using sprintf
     */
    len=sprintf(readBuf,"%i", result);
    len++; // To include null-termination
    readBuf[len] = 4;

    //printk(KERN_ALERT "testBuf: %s \n", readBuf);
    
    printk(KERN_ALERT "testBuf: %i \n", result);

    /*
     * Copy string result
     * to User Space
     */
    if(copy_to_user(buf, (readBuf), len))
        return -EFAULT;

    *f_pos += len;
    return len;    
}




struct file_operations mygpio_Fops = 
{
    .owner   = THIS_MODULE,
    .open    = mygpio_open,
    .release = mygpio_release,
    .write   = mygpio_write,
    .read    = mygpio_read,
};

module_init(mygpio_init);
module_exit(mygpio_exit);

MODULE_DESCRIPTION("My GPIO Module");
MODULE_AUTHOR("IHA <http://www.iha.dk>");
MODULE_LICENSE("GPL");
