KERNELDIR = ~/sources/linux-3.2.6
PWD := $(shell pwd)

ifneq ($(KERNELRELEASE),)
	obj-m := hello.o

else

default:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-angstrom-linux-gnueabi- -C $(KERNELDIR) M=$(PWD) modules

endif		
