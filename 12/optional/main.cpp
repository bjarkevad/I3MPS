#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/i2c-dev.h>
#include </home/bjarke/sources/linux-3.2.6/include/linux/i2c.h>
#include <sys/ioctl.h>

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE -1)

int main(void) 
{
	int fd;
	int lm75_addr = 0x48;
	void *lm75_base, *lm75_virt;
	
	__s32 res;

	if((fd = open("/dev/i2c-3", O_RDONLY | O_SYNC)) == -1) {
		printf("Could not open /dev/i2c-3.\n");
		perror("open");
		exit(1);
	} 

	if(ioctl(fd, I2C_SLAVE, lm75_addr) < 0) {
		printf("ioctl error\n");
		exit(1);
	}
// DOES NOT WORK!! 
// This function should be present in header: linux/i2c.h - it's not.
// Calling read() works
	res = i2c_smbus_read_word_data(fd, lm75_addr);
		  
	printf("%X\n", res);

	return 0;
}
