#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/i2c-dev.h>
#include </usr/src/linux-headers-3.2.0-24-generic/include/linux/i2c.h>
#include <sys/ioctl.h>

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE -1)

int main(void) 
{
	int fd, i;
	int lm75_addr = 0x48, lm75_slave_addr = 0x00;
	void *lm75_base, *lm75_virt;
	char buf[10];
	
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
	
	read(fd, buf, 1);
		  
	printf("%i %i\n", buf[5], buf[6]);

	return 0;
}
