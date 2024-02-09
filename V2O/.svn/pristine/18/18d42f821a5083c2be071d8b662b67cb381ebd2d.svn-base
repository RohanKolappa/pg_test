
# Makefile for V2O build

ifneq ($(VERBOSE),1)
QUIET_CC = @echo "CC $@";
QUIET_DEP = @echo "DEP $@";
QUIET_BIN = @echo "BIN $@";
QUIET_CLEAN = @echo "CLEAN";
QUIET_GEN = @echo "GEN $@";
QUIET_STRIP = @echo "STRIP $@";
QUIET_LIB = @echo "LIB $@";
QUIET = @ 
endif

WARNFLAGS  := -Wall -Wstrict-prototypes -Wmissing-prototypes
CPPWARNFLAGS  := -Wall
MOPTFLAGS  := -O
UOPTFLAGS  := -g -O
I386FLAGS  := 
DEFFLAGS   := -DMODULE -D__KERNEL__


ifeq ($(DEBUG),1)
UOPTFLAGS += -DV2ODEBUG=1
endif

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld
STRIP = $(CROSS_COMPILE)strip

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
	DEV_DIR=dev/dev_arm
else
	DEV_DIR=dev/dev_i386
endif

MOD_FLAGS  := ${DEFFLAGS} ${WARNFLAGS} ${MOPTFLAGS} ${I386FLAGS}

.PHONY: clean 

PWD := $(shell pwd)

ifeq (,$(findstring arm,$(CROSS_COMPILE)))
all: driver libv2okernel.so

KDIR := ../../../${DEV_DIR}/linux-kernel-headers

KERNELHEADERSARCHIVE := ../pkg_3party_images/pkg_3party_moddev_binaries.tgz

${KDIR}: ../../${KERNELHEADERSARCHIVE}
	rm -rf ${KDIR}
	mkdir -p ${KDIR}
	cd ${KDIR}; tar xzmf ../../${KERNELHEADERSARCHIVE}

driver: ${KDIR}
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean: ${KDIR}
	-$(MAKE) -C $(KDIR) M=$(PWD) clean
	${QUIET_CLEAN} /bin/rm -f *.so *.o

else
all: libv2okernel.so
clean:
	${QUIET_CLEAN} /bin/rm -f *.so *.o
endif

LIB_FLAGS  := ${WARNFLAGS} ${MOPTFLAGS} ${I386FLAGS}

libv2okernel.so: slice_size_lib.o
	${QUIET_LIB} ${CC} -shared -o $@ slice_size_lib.o

slice_size_lib.o: slice_size.c
	${QUIET_CC} ${CC} ${LIB_FLAGS} -c -o slice_size_lib.o $^

