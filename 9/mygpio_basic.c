/*                                                     
 * ID: mygpio.c 
 */                                                    
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/module.h>
#include <asm/uaccess.h>

#define MYGPIO_MAJOR  64
#define MYGPIO_MINOR   0
#define MYGPIO_CHS     1
#define MAXLEN       512

static struct cdev MyGpioDev;
struct file_operations mygpio_Fops;
static int devno;
const unsigned int gpio_len = 1;

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
    if (gpio_request(138, "gpio138") < 0)
      printk("Error requesting GPIO138\n");

    /*
     * Set GPIO Direction
     */

    gpio_direction_output(138, true);

    return 0;
}

static void mygpio_exit(void)
{
    printk(KERN_NOTICE "Removing Mygpio Module\n");

    gpio_free(138);
 
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

    printk("Opening MyGpio Device [major], [minor]: %i, %i\n", major, minor);

    if (!try_module_get(mygpio_Fops.owner)) // Get Module
      return -ENODEV;

    return 0;
}

int mygpio_release(struct inode *inode, struct file *filep)
{
    int minor, major;
    
    major = MAJOR(inode->i_rdev);
    minor = MINOR(inode->i_rdev);

    printk("Closing MyGpio Device [major], [minor]: %i, %i\n", major, minor);

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

    /**** Use copy_from_user to get data ****/
    if(copy_from_user(kbuf, ubuf, len))
        return -EFAULT;
	
    kbuf[len] = '\0';   // Pad null termination to string
    printk("string from user: %s\n", kbuf);

    /*
     * Convert string to int
     */
    sscanf(kbuf,"%i", &value);
    printk("value %i\n", value);
    
    value = value > 0 ? 1 : 0;

    /*
     * Set GPIO port
     * (if an output port)
     */
    gpio_set_value(138, value);

    *f_pos += count;
    return count;
}

ssize_t mygpio_read(struct file *filep, char __user *buf, 
                 size_t count, loff_t *f_pos)
{
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

    if(minor == 0)
      result = gpio_get_value(138);
    else
      return -1;

    /*
     * Convert int to string
     * using sprintf
     */
    len=sprintf(readBuf,"%i", result);
    len++; // To include null-termination

    printk(KERN_ALERT "testBuf: %s \n", readBuf);

    /*
     * Copy string result
     * to User Space
     */

    if(copy_to_user(buf, readBuf, len))
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
