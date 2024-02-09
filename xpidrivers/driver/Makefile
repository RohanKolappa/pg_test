#
# export KERNELDIR=/home/onair/ipvs_work/buildroot/output/build/linux-svn
# make CROSS_COMPILE=arm-none-linux-gnueabi- ARCH=arm 
#
obj-m := OmniTek_Driver.o
OmniTek_Driver-objs :=  OmniTekResources_linux.o OmniTek_linux.o OmniTekDriver_linux.o OmniTekFPGA_linux.o OmniTekFops_linux.o OmniTekDma.o OmniTekDmaOperations.o OmniTekRequestQueue.o OmniTekDMACore.o OmniTekDMARequest.o OmniTekResources_sysfs.o OmniTekInterrupt.o

#KERNELDIR ?= /lib/modules/$(shell uname -r)/build
#ARCH ?= x86
export KERNELDIR=${PWD}/../../dev/dev_arm/linux-kernel-headers

LAST_BUILD = last_build.$(ARCH)
CUR_BUILD = $(wildcard last_build.*)

BUILD_OPTS :=

PWD       := $(shell pwd)


all: cleanlastarch
	@echo "Making for $(ARCH)"
	$(MAKE) -C $(KERNELDIR) M=$(PWD) CFLAGS=$(CFLAGS)
	$(MAKE) -C efdma M=$(PWD) CFLAGS=$(CFLAGS)
	$(MAKE) -C ethernet M=$(PWD) CFLAGS=$(CFLAGS)
	$(MAKE) -C ptp M=$(PWD) CFLAGS=$(CFLAGS)
	$(MAKE) -C framebuffer M=$(PWD) CFLAGS=$(CFLAGS)
	$(MAKE) -C netvizxpi M=$(PWD) CFLAGS=$(CFLAGS)
	touch last_build.$(ARCH)

.PHONY : cleanlastarch
cleanlastarch: 
	@echo "Checking if last built version was $(ARCH)"
	@echo "LAST_BUILD: '$(LAST_BUILD)'"
	@echo "CUR_BUILD : '$(CUR_BUILD)'"
    ifneq ($(CUR_BUILD),$(LAST_BUILD))
	@echo "Last build was for different architecture, cleaning"
	@$(MAKE) clean
    endif

clean:
	$(MAKE) -C efdma clean
	$(MAKE) -C ethernet clean
	$(MAKE) -C ptp clean
	$(MAKE) -C framebuffer clean
	$(MAKE) -C netvizxpi clean
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions last_build.* modules.order Module.symvers

TARGET_MODULE = OmniTek_Driver.ko
KERNELRELEASE = `cat $(KERNELDIR)/include/config/kernel.release`
BASEMODDIR = $(MODDIR)/$(KERNELRELEASE)
NETVIZMODDIR = $(BASEMODDIR)/netviz

modules_install: $(TARGET_MODULE)
	@mkdir -p $(NETVIZMODDIR)
	@cp -p $(TARGET_MODULE) $(NETVIZMODDIR)
#	@echo "/lib/modules/$(KERNELRELEASE)/netviz/$(TARGET_MODULE):" >> $(BASEMODDIR)/modules.dep
	@echo "$(MODDIR)/$(KERNELRELEASE)/netviz/$(TARGET_MODULE):" >> $(BASEMODDIR)/modules.dep
	$(MAKE) -C efdma modules_install
	$(MAKE) -C ethernet modules_install
	$(MAKE) -C ptp modules_install
	$(MAKE) -C framebuffer modules_install
	$(MAKE) -C netvizxpi modules_install

