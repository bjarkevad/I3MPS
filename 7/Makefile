
# To build modules outside of the kernel tree, we run "make"
# in the kernel source tree; the Makefile these then includes this
# Makefile once again.
# This conditional selects whether we are being included from the
# kernel Makefile or not.
ifeq ($(KERNELRELEASE),)

    # Assume the source tree is where the running kernel was built
    # You should set KERNELDIR in the environment if it's elsewhere
#    KERNELDIR = $(shell echo $(TARGETKERNEL))
    KERNELDIR = ~/sources/linux-3.2.6
    # The current directory is passed to sub-makes as argument
    PWD := $(shell pwd)
    TEMP := $(shell echo $(TARGETKERNEL))

modules:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-angstrom-linux-gnueabi- -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-angstrom-linux-gnueabi- -C $(KERNELDIR) M=$(PWD) modules_install

temp:
	echo $(KERNELDIR)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

.PHONY: modules modules_install clean

else
    # called from kernel build system: just declare what our modules are
#    obj-m := ads7870.o #ads7870-core.o ads7870-spi.o
    obj-m += ads7870mod.o
    ads7870mod-objs := ads7870.o ads7870-core.o ads7870-spi.o

endif


