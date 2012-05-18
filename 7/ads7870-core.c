#include <linux/err.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include "ads7870-core.h"
#include "ads7870-spi.h"
#include <linux/spi/spi.h>
#include <plat/mcspi.h>

#define MAXLEN             512
#define MODULE_DEBUG 0

/* 
 * Pointer to SPI Device 
 */
static struct spi_device *ads7870_spi_device;

/* 
 * SPI Configuration Structure
 * Used when creating a new SPI Device
 */
static struct omap2_mcspi_device_config ads7870_mcspi_config = {
  .turbo_mode	        = 0,
  .single_channel	= 1,  /* 0: slave, 1: master */
};

struct spi_board_info omap3devkit8000_spi_board_info[] = {
  {
    .modalias		= "dac7612", //new alias
    .bus_num		= 1,
    .chip_select	= 3, //new chipselect
    .max_speed_hz	= 15000000,
    .controller_data	= &ads7870_mcspi_config,
    .mode               = SPI_MODE_3, // Clock polarity
  },

};

int ads7870_init(void) {
  int err;

  if((err=ads7870_spi_init(&ads7870_spi_device, 
			   &omap3devkit8000_spi_board_info[0])) < 0)
    return err;

  // Configure ADS7870
  err = ads7870_spi_write_reg8(ads7870_spi_device,
			       ADS7870_REFOSC, 
			       ADS7870_REFOSC_OSCR |
			       ADS7870_REFOSC_OSCE |
			       ADS7870_REFOSC_REFE |
			       ADS7870_REFOSC_BUFE |
			       ADS7870_REFOSC_R2V);

  return err;
}


int ads7870_exit(void) {
  return ads7870_spi_exit(ads7870_spi_device);
}

u16 ads7870_convert(u8 channel) {
  u16 i, result;
  
    /*
     * Start Conversion
     */
    ads7870_spi_write_reg8(ads7870_spi_device,
			   ADS7870_GAINMUX, 
			   ADS7870_GAIN_1X | 
			   ADS7870_CH_SINGLE_ENDED |
			   (channel & 0x07)| 
			   ADS7870_CONVERT);


    /*
     * Wait for it to complete, poll CONVERT flag
     * Dirty, dirty way....
     * 
     * !! Check how many cycles is used !!
     */
    i=0;
    while((ads7870_spi_read_reg8(ads7870_spi_device, 
				 ADS7870_GAINMUX) & ADS7870_CONVERT) > 0) {
      i++;
	  /*
	   * Schedule process (TASK_INTERRUPTIBLE==> Take it off the run queue)
	   * for xx jiffies to avoid busy-waiting
	   */
	  //schedule_timeout(ADS7870_CONVERT_TIME_JIFFIES);
	  }

    //    printk("Had to poll ADS7870 %i times before result was ready\n", i);

    /*
     * Get Result
     */
    result = ads7870_spi_read_reg16(ads7870_spi_device, ADS7870_RESULTLO);
    if(result & ADS7870_RESULTLO_OVR)
      printk(KERN_ALERT "ADS7870: Error! PGA Out of Range\n");
    result = result >> 4; // right-align result, lower 4-bits are zero
    if(MODULE_DEBUG)
      printk(KERN_DEBUG "ADS7870: Channel %i result: %i mV\n", channel, result);

    return result;
}

void dac_write(u16 channel, u16 value)
{
	u16 adr = 0;

	if(channel == 0)
		adr = 2;

	else	
		adr = 3;

	ads7870_spi_write_reg8(ads7870_spi_device, adr, value);
}


