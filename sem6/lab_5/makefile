TARGETS := testCIO testKernelIO testFFIO

all: $(TARGETS)

$(TARGETS): %: %.c
	gcc -std=c11 -o $@ -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $^ -lpthread

clean:
	rm -f $(TARGETS)
