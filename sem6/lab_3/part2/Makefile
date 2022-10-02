.PHONY: clean

obj-m += md1.o md2.o md3.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules > /dev/null ;\
	rm -rf .*.ko.cmd .*.mod.o.cmd .*.o.cmd .*.o.d *.mod.c *.mod.o *.o .*.*.cmd *.mod *.symvers *.order

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean > /dev/null
