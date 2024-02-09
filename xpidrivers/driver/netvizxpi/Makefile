#
# make CROSS_COMPILE=arm-none-linux-gnueabi- ARCH=arm
#
obj-m := Barco_NetvizXpi.o \
	Barco_Adv7604.o \
	Barco_Adv7612.o \
	Barco_Sii7172.o
Barco_NetvizXpi-objs := netvizxpi.o lut_table.o slice_size.o 
Barco_Adv7604-objs := adv7604.o 
Barco_Adv7612-objs := adv7612.o 
Barco_Sii7172-objs := sii7172.o 

#KERNELDIR ?= /lib/modules/$(shell uname -r)/build
#ARCH ?= x86

LAST_BUILD = last_build.$(ARCH)
CUR_BUILD = $(wildcard last_build.*)

BUILD_OPTS :=

PWD       := $(shell pwd)


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

.PHONY : $(TARGET_MODULES_INSTALL)
$(TARGET_MODULES_INSTALL) : $(TARGET_MODULES)
	@mkdir -p $(NETVIZMODDIR)
	@cp -p $(@:%.install=%.ko) $(NETVIZMODDIR)

.PHONY : $(TARGET_MODULES_INSTALL)

modules_install: $(TARGET_MODULES) $(TARGET_MODULES_INSTALL)

