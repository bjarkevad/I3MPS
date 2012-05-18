#ifndef ADS7870_SPI_H
#define ADS7870_SPI_H
#include <linux/spi/spi.h>
//#include <mach/mcspi.h>
#include <linux/input.h>


u16 ads7870_spi_read_reg8(struct spi_device *spi, u16 reg);
u16 ads7870_spi_read_reg16(struct spi_device *spi, u16 reg);
u16 ads7870_spi_write_reg8(struct spi_device *spi, u16 reg, u16 val);
int ads7870_spi_init(struct spi_device **ads7870_spi_device, 
			    struct spi_board_info *ads7870_spi_board_info);
int ads7870_spi_exit(struct spi_device *spi);

#endif

