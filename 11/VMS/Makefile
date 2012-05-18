ifeq ($(KERNELRELEASE),)
ifeq ($(TARGETKERNEL),)
	KERNELDIR = /lib/modules/$(shell uname -r)/build
else
	KERNELDIR = $(shell echo $(TARGETKERNEL))
endif
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules	

target:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-angstrom-linux-gnueabi- -C ~/sources/linux-3.2.6 M=$(PWD) modules


clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules clean
else
  obj-m := vms.o
endif