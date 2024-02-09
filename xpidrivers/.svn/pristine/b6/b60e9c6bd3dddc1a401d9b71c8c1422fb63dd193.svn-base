# File : Makefile
# Author : Eldridge M. Mount IV (eldridge.mount@labxtechnologies.com)
# Description : Makefile for the Lab X PTP demo application(s).

# Include project-specific make options
export PROJ_TOP = $(CURDIR)
include ./build/options.mk

# Local-only definitions
CC             = $(CROSS)gcc
CXX            = $(CROSS)g++
OBJCOPY        = $(CROSS)objcopy
ECHO           = echo
INC_DIRS       = $(LINUX_INC)
INC_DIRS      += ./libLabXFc/include
INC_DIRS      += ./libLabXAvb/include
LIB_DIRS       = ./libLabXFc
LIB_DIRS      += ./libLabXAvb
#LIB_DIRS      += $(LINUX_PATH)/uClibc/lib/
#LIB_DIRS      += $(LINUX_PATH)/lib/uClibc++/src/

# Build the FPGA API as well as the IDL layers
export BUILD_AVB_FPGA = true
export BUILD_IDL      = true

# Library order is important here:

# * PROJ_LIBS must begin with base libraries and end with ones which depend
#   upon the preceding ones
PROJ_LIBS  = LabXFc
PROJ_LIBS += LabXAvb

# Recursive utility for reversing list order
reverse = $(if $(1),$(call reverse,$(wordlist 2,$(words $(1)),$(1)))) $(firstword $(1))

# * LINK_LIBS must be the reverse order of dependency for symbol resolution
#   during the link step
LINK_LIBS  = $(call reverse,$(PROJ_LIBS))
LINK_LIBS += pthread
#LINK_LIBS += uClibc++
LINK_LIBS += m
LDFLAGS    = -Wl,-elf2flt

VPATH += ./libLabXFc ./libLabXAvb

# Default target
all: ptp_demo

# Labrinth-specific AVB Audio control daemon application
# Bump the stack size
ptp_demo: ptp_demo.cpp $(PROJ_LIBS:%=lib%.a)
	$(CXXFRONT) $(CXX) $(INC_DIRS:%=-I %) $(LIB_DIRS:%=-L %) -o $@ $< $(LINK_LIBS:%=-l%) $(LDFLAGS)
#	mb-flthdr -s 0x4000 $@

# Lab X Foundation Classes library
libLabXFc.a:
	$(ECHO) "Building Lab X Foundation Classes"
	cd libLabXFc; $(MAKE) $(MFLAGS)

# Lab X AVB library
libLabXAvb.a:
	$(ECHO) "Building Lab X AVB Classes"
	cd libLabXAvb; $(MAKE) $(MFLAGS)

clean: 
	rm -f ptp_demo *.o *.gdb
	cd libLabXFc; $(MAKE) clean
	cd libLabXAvb; $(MAKE) clean

# Phony "always build" targets
.PHONY: libLabXFc.a libLabXAvb.a
