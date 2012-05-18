#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/module.h>
#include "ads7870-core.h"


#define ADS7870_MAJOR       64
#define ADS7870_MINOR        0
#define MAXLEN             512
#define NBR_ADC_CH           8

#define MODULE_DEBUG 0
#define USECDEV 0

static struct cdev ads7870Dev;
struct file_operations ads7870_Fops;
static int devno;

static int __init ads7870_cdrv_init(void)
{
  int err; 
  
  printk("ads7870 driver initializing\n");  

  /* Register Char Device */
  cdev_init(&ads7870Dev, &ads7870_Fops);
	/* NEXT VERSION
	 * Leave out the following two lines
	 * LDD3 p.56
	 */
  ads7870Dev.owner = THIS_MODULE;
  ads7870Dev.ops   = &ads7870_Fops;
  devno           = MKDEV(ADS7870_MAJOR, ADS7870_MINOR);
  err = register_chrdev_region(devno, NBR_ADC_CH, "adConverter");
  err = cdev_add(&ads7870Dev, devno, NBR_ADC_CH);
  if (err) {
    printk (KERN_ALERT "Error %d adding ADS7870 device\n", err);
    return err;
  }

  err = ads7870_init();
  
  return err;
}

static void __exit ads7870_cdrv_exit(void)
{
  int err;
  printk("ads7870 driver Exit\n");
  err = ads7870_exit();
  unregister_chrdev_region(devno, NBR_ADC_CH);
  cdev_del(&ads7870Dev);
}


int ads7870_cdrv_open(struct inode *inode, struct file *filep)
{
    int major, minor;

    major = MAJOR(inode->i_rdev);
    minor = MINOR(inode->i_rdev);
	
	/* NEXT VERSION
	 * LDD3 p.55
	major = imajor(inode);
	minor = iminor(inode);
	*/

    printk("Opening ADS7870 Device [major], [minor]: %i, %i\n", major, minor);

    if (minor > NBR_ADC_CH-1)
    {
      printk("Minor no out of range (0-%i): %i\n", NBR_ADC_CH, minor);
        return -ENODEV;
    }

    if (!try_module_get(ads7870_Fops.owner)) // Get Module
	return -ENODEV;

    return 0;
}

int ads7870_cdrv_release(struct inode *inode, struct file *filep)
{
    int minor, major;
    
    major = MAJOR(inode->i_rdev);
    minor = MINOR(inode->i_rdev);

    printk("Closing ADS7870 Device [major], [minor]: %i, %i\n", major, minor);

    if (minor > NBR_ADC_CH-1)
      return -ENODEV;
    
    module_put(ads7870_Fops.owner); // Release Module
    
    return 0;
}



ssize_t ads7870_cdrv_write(struct file *filep, const char __user *ubuf, 
		      size_t count, loff_t *f_pos)
{
    int minor, len, value;
    char kbuf[MAXLEN];    
    
    minor = MINOR(filep->f_dentry->d_inode->i_rdev);
    if (minor != ADS7870_MINOR) {
        printk(KERN_ALERT "ads7870 Write to wrong Minor No:%i \n", minor);
        return 0; }
    printk(KERN_ALERT "Writing to ads7870 [Minor] %i \n", minor);
    
    len = count < MAXLEN ? count : MAXLEN;
    if(copy_from_user(kbuf, ubuf, len))
        return -EFAULT;
	
    kbuf[len] = '\0';   // Pad null termination to string
	
	dac_write((u16)minor, (u16)value);

    if(MODULE_DEBUG)
      printk("string from user: %s\n", kbuf);
    sscanf(kbuf,"%i", &value);
    if(MODULE_DEBUG)
      printk("value %i\n", value);

    /*
     * Write something here
     */

    return count;
}

ssize_t ads7870_cdrv_read(struct file *filep, char __user *ubuf, 
                  size_t count, loff_t *f_pos)
{
  int minor;
  char resultBuf[5];
  u16 result;
    
    minor = MINOR(filep->f_dentry->d_inode->i_rdev);
    if (minor > NBR_ADC_CH-1) {
      printk(KERN_ALERT "ads7870 read from wrong Minor No:%i \n", minor);
      return 0; }
    if(MODULE_DEBUG)
      printk(KERN_ALERT "Reading from ads7870 [Minor] %i \n", minor);
    
    /*
     * Start Conversion
     */
    result = ads7870_convert(minor & 0xff);

    /*
     * Convert to string and copy to user space
     */
    snprintf (resultBuf, sizeof resultBuf, "%d", result);
    if(copy_to_user(ubuf, resultBuf, sizeof(resultBuf)))
      return -EFAULT;

    return count;
}

struct file_operations ads7870_Fops = 
{
    .owner   = THIS_MODULE,
    .open    = ads7870_cdrv_open,
    .release = ads7870_cdrv_release,
    .write   = ads7870_cdrv_write,
    .read    = ads7870_cdrv_read,
};

module_init(ads7870_cdrv_init);
module_exit(ads7870_cdrv_exit);

MODULE_AUTHOR("Peter Hoegh Mikkelsen <phm@iha.dk>");
MODULE_LICENSE("GPL");

