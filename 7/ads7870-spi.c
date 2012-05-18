#include <linux/err.h>
#include <plat/mcspi.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/spi/spi.h>
#include "ads7870-core.h"
#include <linux/module.h>
#define ADS7870_SPI_HZ 1500000

#define MODULE_DEBUG 0


u16 ads7870_spi_read_reg8(struct spi_device *spi, u16 reg)
{
	struct spi_transfer t[2];
	struct spi_message m;
	u16 data = 0, cmd;

	cmd = (reg & 0x1f) | (1<<6); // Mask R/Wn 16/8n

	memset(t, 0, sizeof(t));
	spi_message_init(&m);
	m.spi = spi;

	t[0].tx_buf = &cmd;
	t[0].rx_buf = NULL;
	t[0].len = 1;
	spi_message_add_tail(&t[0], &m);

	t[1].tx_buf = NULL;
	t[1].rx_buf = &data;
	t[1].len = 1;
	spi_message_add_tail(&t[1], &m);


	spi_sync(m.spi, &m);
	if(MODULE_DEBUG)
	  printk(KERN_DEBUG "ADS7870: Read Reg8 Addr 0x%02x Data: 0x%02x\n", cmd, data);

	return data;
}

u16 ads7870_spi_read_reg16(struct spi_device *spi, u16 reg)
{
	struct spi_transfer t[2];
	struct spi_message m;
	u16 data = 0, cmd;

	cmd = (reg & 0x1f) | (1<<6) | (1<<5); // Mask R/Wn 16/8n

	memset(t, 0, sizeof(t));
	spi_message_init(&m);
	m.spi = spi;

	t[0].tx_buf = &cmd;
	t[0].rx_buf = NULL;
	t[0].len = 1;
	if(MODULE_DEBUG)
	  printk("requesting data from addr 0x%x\n", cmd);
	spi_message_add_tail(&t[0], &m);

	t[1].tx_buf = NULL;
	t[1].rx_buf = &data;
	t[1].len = 2;
	spi_message_add_tail(&t[1], &m);


	spi_sync(m.spi, &m);

	if(MODULE_DEBUG)
	  printk(KERN_DEBUG "ADS7870: Read Reg16 Addr 0x%02x Data: 0x%04x\n", cmd, data);

	return data;
}


void ads7870_spi_write_reg8(struct spi_device *spi, u16 reg, u16 val)
{
  struct spi_transfer t[2];
  struct spi_message m;
  u8 data[2];
  u16 valA, regA;

  valA = val & 0xff;
  regA = reg & 0x1f;

  /* Now we prepare the command for transferring */
  data[0] = (u8)((reg << 4) | (val >> 8));
  data[1] = (u8)(val);


  memset(&t, 0, sizeof(t)); 
  spi_message_init(&m);
  m.spi = spi;

  if(MODULE_DEBUG)
    printk(KERN_DEBUG "ADS7870: Write Reg8 Addr 0x%x Data 0x%02x\n", data[0], data[1]); 

/*   t.tx_buf = data; */
/*   t.rx_buf = NULL; */
/*   t.len = 2; // one because single byte (bits_pr_word = 8-bit) */
/*   spi_message_add_tail(&t, &m); */


  t[0].tx_buf = &regA;
  t[0].rx_buf = NULL;
  t[0].len = 2;
  spi_message_add_tail(&t[0], &m);

  t[1].tx_buf = &valA;
  t[1].rx_buf = NULL;
  t[1].len = 1;
  spi_message_add_tail(&t[1], &m);


  spi_sync(m.spi, &m);
}

/*
 * ADS7870 Probe
 * Used by the SPI Master to probe the device
 * When the SPI device is registered. 
 */
static int __devinit ads7870_spi_probe(struct spi_device *spi)
{

  spi->bits_per_word = 8;  
  spi_setup(spi);

  printk(KERN_DEBUG "Probing ADS7870, ADS7870 Revision %i\n", 
	 ads7870_spi_read_reg8(spi, ADS7870_ID));

  return 0;
}

static int __devexit ads7870_remove(struct spi_device *spi)
{
  return 0;
}

static struct spi_driver ads7870_spi_driver = {
  .driver = {
    .name = "ads7870",
    .bus = &spi_bus_type,
    .owner = THIS_MODULE,
  },
  .probe = ads7870_spi_probe,
  .remove = __devexit_p(ads7870_remove),
};


/*
 * Init / Exit routines called from 
 * character driver. Init registers the spi driver
 * and the spi host probes the device upon this.
 * Exit unregisters the driver and the spi host
 * calls _remove upon this
 */

int ads7870_spi_init(struct spi_device **ads7870_spi_device, 
		     struct spi_board_info *ads7870_spi_board_info)
{
  int err, bus_num;
  struct spi_master *ads7870_spi_master;

  /* Add the ads7870 device to the SPI bus *
   *
   * Using this method we actually add a device 
   * before adding a driver. A second way would be to
   * register the driver and probe the devices in _probe
   * this is how it's done in I2C, where multiple devices
   * may be available for one driver. In this case
   * it would be overkill. If done in _probe we couldn't
   * use the spi_device parameter, as it wouldn't be 
   * assigned yet.
   */ 
  bus_num = ads7870_spi_board_info->bus_num;
  ads7870_spi_master = spi_busnum_to_master(bus_num);
  *ads7870_spi_device = spi_new_device(ads7870_spi_master,
				      ads7870_spi_board_info);

  if((err=spi_register_driver(&ads7870_spi_driver))<0)
    printk (KERN_ALERT "Error %d registering the ads7870 SPI driver\n", err);
  else
	printk(KERN_ALERT "Successfully registered the DAC driver");

  ads7870_spi_write_reg8(*ads7870_spi_device, 1, 1500);
  return err;
}

int ads7870_spi_exit(struct spi_device *spi)
{
  /*
   * Un-register spi driver and device
   * Spi host calls _remove upon this
   */
  ads7870_spi_write_reg8(spi, 1, 2000);
  spi_unregister_driver(&ads7870_spi_driver); 
  spi_unregister_device(spi);

  return 0;
}


