KERNEL_DIR = /usr/src/linux-headers-$(shell uname -r)
PWD := $(shell pwd)

.PHONY: clean

obj-m += md.o
md-objs = ./src/md.o ./src/cookie.o

all: build_modules
	@rm -rf .*.ko.cmd .*.mod.o.cmd .*.o.cmd .*.o.d *.mod.c *.mod.o *.o .*.*.cmd *.mod *.symvers *.order
	@rm -rf ./**/.*.ko.cmd ./**/.*.mod.o.cmd ./**/.*.o.cmd ./**/.*.o.d ./**/*.mod.c ./**/*.mod.o ./**/*.o .*.*.cmd *.mod *.symvers *.order

build_modules:
	@make -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	@make -C $(KERNEL_DIR) M=$(PWD) clean
