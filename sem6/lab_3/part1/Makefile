.PHONY: clean

obj-m += md.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules && \
	rm -rf .*.ko.cmd .*.mod.o.cmd .*.o.cmd .*.o.d *.mod.c \
		*.mod.o *.o .*.*.cmd *.mod *.symvers *.order

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
