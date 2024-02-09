obj-m := OmniTek_Ethernet_Driver.o Barco_Ethernet_Extension.o
OmniTek_Ethernet_Driver-objs := OmniTek_Ethernet.o
Barco_Ethernet_Extension-objs := Barco_Ethernet.o

#KERNELDIR ?= /lib/modules/$(shell uname -r)/build
#ARCH ?= x86
KERNELDIR ?= /home/pwang/linux-svn
LAST_BUILD = last_build.$(ARCH)
CUR_BUILD = $(wildcard last_build.*)


BUILD_OPTS :=

PWD       := $(shell pwd)


all: cleanlastarch
	@echo "Making for $(ARCH)"
	cp ../Module.symvers ./
	cat ../efdma/Module.symvers >> ./Module.symvers
	$(MAKE) -C $(KERNELDIR) M=$(PWD) CFLAGS=$(CFLAGS)
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
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions last_build.* modules.order Module.symvers

TARGET_MODULES = OmniTek_Ethernet_Driver.ko Barco_Ethernet_Extension.ko
TARGET_MODULES_INSTALL = $(TARGET_MODULES:%.ko=%.install)
KERNELRELEASE = `cat $(KERNELDIR)/include/config/kernel.release`
MODDIR = /home/pwang/ipvs_work/buildroot/output/target/lib/modules
BASEMODDIR = $(MODDIR)/$(KERNELRELEASE)
NETVIZMODDIR = $(BASEMODDIR)/netviz

.PHONY : $(TARGET_MODULES_INSTALL)
$(TARGET_MODULES_INSTALL) : $(TARGET_MODULES)
	@mkdir -p $(NETVIZMODDIR)
	@cp -p $(@:%.install=%.ko) $(NETVIZMODDIR)
#	@echo "/lib/modules/$(KERNELRELEASE)/netviz/$(@:%.install=%.ko):" >> $(BASEMODDIR)/modules.dep
	@echo "$(BASEMODDIR)/netviz/$(@:%.install=%.ko):" >> $(BASEMODDIR)/modules.dep

.PHONY: $(TARGET_MODULES_INSTALL)

modules_install: $(TARGET_MODULES) $(TARGET_MODULES_INSTALL)

