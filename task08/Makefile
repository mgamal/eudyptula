ifneq ($(KERNELRELEASE),)
	obj-m := hello.o
else

	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
	CFLAGS_hello.o := -DDEBUG

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	rm -rf *.o *.ko *.mod.c *.cmd Module.symvers modules.order .tmp_versions

default: modules
all: modules

endif

