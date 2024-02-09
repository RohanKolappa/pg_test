
#KERNELDIR ?= /lib/modules/$(shell uname -r)/build
#ARCH ?= x86

LAST_BUILD = last_build.$(ARCH)
CUR_BUILD = $(wildcard last_build.*)

## Get local svn revision
MODULE_BASE = fb
SVN_VERSION = $(shell setlocalversion)
LOCAL_VERSION = $(MODULE_BASE)$(SVN_VERSION)


BUILD_OPTS :=

PWD       := $(shell pwd)

obj-m := $(MODULE_BASE).o
$(MODULE_BASE)-objs := netvizfb.o


all: cleanlastarch
	@echo "Making for $(ARCH)"
	cp ../Module.symvers ./
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

KERNELRELEASE = `cat $(KERNELDIR)/include/config/kernel.release`
MODDIR = /home/pwang/ipvs_work/buildroot/output/target/lib/modules
BASEMODDIR = $(MODDIR)/$(KERNELRELEASE)
NETVIZMODDIR = $(BASEMODDIR)/netviz
TARGET_MODULE = $(MODULE_BASE).ko

modules_install: $(TARGET_MODULE)
	@mkdir -p $(NETVIZMODDIR)
	@cp -p $(TARGET_MODULE) $(NETVIZMODDIR)
#	@echo "/lib/modules/$(KERNELRELEASE)/netviz/$(TARGET_MODULE):" >> $(BASEMODDIR)/modules.dep
	@echo "$(BASEMODDIR)/netviz/$(TARGET_MODULE):" >> $(BASEMODDIR)/modules.dep
