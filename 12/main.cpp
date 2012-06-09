#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE -1)

int main(void) 
{
	int fd;
	void *oe_base, *dout_base, *oe_base_virt, *dout_base_virt;

	off_t GPIO_OE = 0x49056034,
		  GPIO_DOUT = 0x4905603C;

	printf("Pagesize: %i\n", getpagesize());
	if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
		printf("Could not open /dev/mem.\n");
		perror("open");
	} else {
		printf("/dev/mem opened.\n");
	}

	oe_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_OE & ~MAP_MASK);
	if(oe_base == (void*) -1) {
		printf("Memory map of oe_base failed.\n");
		perror("mmap");
		return -1;
	} else {
		printf("Memory mapped at address %p.\n", oe_base);
	}

	oe_base_virt = oe_base + (GPIO_OE & MAP_MASK);

	*(unsigned char*) oe_base_virt = 0xFFFFFF0F;
	dout_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_DOUT & ~MAP_MASK);
	if(dout_base == (void*) -1) {
		printf("Memory map of dout_base failed.\n");
		perror("mmap");
		return -1;
	} else {
		printf("Memory mapped at address %p.\n", dout_base);
	}

	dout_base_virt = dout_base + (GPIO_DOUT & MAP_MASK);

	while(1) {
	*(unsigned char*) dout_base_virt = 16;
	sleep(1);
	*(unsigned char*) dout_base_virt = 32;
	sleep(1);
	*(unsigned char*) dout_base_virt = 64;
	sleep(1);
	*(unsigned char*) dout_base_virt = 128;
	sleep(1);
	}
}
