VERSION = $(shell uname -r)

all:
	$(MAKE) -C /lib/modules/${VERSION}/build M=$$PWD
clean:
	$(MAKE) -C /lib/modules/${VERSION}/build M=$$PWD clean
