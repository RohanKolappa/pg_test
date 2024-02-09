# Copyright (C) 2000-2003 WIS Technologies, Inc.
# 
# $Id: Makefile,v 1.2 2007-01-10 20:58:09 sjain Exp $
#
# Makefile for Linux SDK and applications
# Target platform
export PLATFORM := i686

# Daredevil S3C2510 
export BOARD := matrix
export CONFIG := ipcam

# Cross Compiler and Tools
export TOOLS := /usr
export TOOLTYPE := redhat

ifeq ($(BOARD),matrix)
COMPONENTS := usbdrv apps
LIBS := *.so.?
endif

# Build system type
export BUILD=i386-pc-linux-gnu
export BUILDCC=gcc

# Source bases
export TOP=$(shell pwd)
export SRCBASE=$(shell (cd $(TOP)/.. && pwd -P))

# Cross-compile environment variables
ifeq ($(PLATFORM),arm)
export CROSS_COMPILE=arm-elf-
export CROSS=arm-elf-
#export CONFIGURE=./configure arm-linux --build=$(BUILD)
export LIBDIR=$(TOOLS)/arm-elf/lib
export USRLIBDIR=$(TOOLS)/arm-elf/usr/lib
export LINUXDIR=$(SRCBASE)/linux2510
export LINUXINCLUDE= $(LINUXDIR)/include
else
export CROSS_COMPILE=
export CROSS=
export CONFIGURE=./configure --build=$(BUILD)
export LIBDIR=$(TOOLS)/lib
export USRLIBDIR=$(TOOLS)/usr/lib
export LINUXDIR=/usr/src/linux-2.4
export LINUXINCLUDE= $(LINUXDIR)/include
endif

export CC=$(CROSS_COMPILE)gcc
export AR=$(CROSS_COMPILE)ar
export AS=$(CROSS_COMPILE)as
export LD=$(CROSS_COMPILE)ld
export NM=$(CROSS_COMPILE)nm
export RANLIB=$(CROSS_COMPILE)ranlib
export STRIP=$(CROSS_COMPILE)strip

# Install directory
export INSTALLDIR=$(TOP)/install-$(PLATFORM)

# kernel directory: RH9 is default
export KERNELVERSION=2.4.20
export KERNELDIR=$(LINUXDIR)
export FFMPEG_DIR = $(TOP)/Tools/ffmpeg-012304
#export LIVE_DIR=$(TOP)/Tools/live-040904
export WISLIVE_DIR=$(TOP)/Tools/wis-live
export WISLIVE_STAND_DIR=$(TOP)/Tools/wis-live-stand
export LAME_DIR=$(TOP)/Tools/lame-3.95.1


all: $(foreach component,$(COMPONENTS),$(component))

clean: $(foreach component,$(COMPONENTS),$(component)-clean)

config: $(foreach component,$(filter-out libopt,$(COMPONENTS)),$(component)-config)

install: $(foreach component,$(filter-out libopt,$(COMPONENTS)),$(component)-install)
ifneq ($(filter libopt,$(COMPONENTS)),)
	$(MAKE) libopt-install
endif


# Driver Support
usbdrv-config:
	@echo "Done usbconfig"

usbdrv:
	cd Src/Drv/gosbdrv/linux && make
	cd Src/Drv/board/wisboard/linux && make
	mkdir -p install
	cp Src/Drv/gosbdrv/linux/wisgo7007sb.o install
	cp Src/Drv/board/wisboard/linux/wis_sensor.o install

usbdrv-clean:
	rm -fr install
	cd Src/Drv/gosbdrv/linux && make clean
	cd Src/Drv/board/wisboard/linux && make clean

device-install dev-install:
	/bin/mknod /dev/wisgo7007_0 c 240 0
	/bin/mknod /dev/wisgo7007_1 c 240 1
	/bin/mknod /dev/wisgo7007_2 c 240 2
	/bin/mknod /dev/wisgo7007_3 c 240 3

device-remove dev-clean:	
	/bin/rm -f /dev/wisgo7007_0
	/bin/rm -f /dev/wisgo7007_1
	/bin/rm -f /dev/wisgo7007_2
	/bin/rm -f /dev/wisgo7007_3

# Supporting Application Libraries
apps-config:
#	cd Tools/jrtplib && ./configure 
#	cd $(LAME_DIR) && ./configure --prefix=$(LAME_DIR)/install \
#                         --enable-shared=no
#	cd $(FFMPEG_DIR) && ./configure --prefix=$(FFMPEG_DIR)/install \
#                         --enable-mp3lame \
#                         --extra-cflags=-I$(LAME_DIR)/install/include\
#                         --extra-ldflags="-L$(LAME_DIR)/install/lib -lmp3lame"
#	cd $(LIVE_DIR) && ./genMakefiles linux

apps:
#	cd Tools/jrtplib && make
#	cd $(LAME_DIR) && make && make install
#	cd $(FFMPEG_DIR) && make
#	cd $(LIVE_DIR) && make
#	cd $(WISLIVE_DIR) && make
#	cd $(WISLIVE_STAND_DIR) && make
	mkdir -p Lib
	cd Src/Lib/VideoCapture && make
#	cd Src/Lib/Auxiliary && make
#	cd Src/App/go-server && make
#	cd Src/App/live-server && make
#	cp Src/App/go-server/go-server install
#	cp Src/App/live-server/live-server install

apps-clean:
#	cd Tools/jrtplib && make clean
#	cd $(FFMPEG_DIR) && make distclean
#	cd $(LIVE_DIR) && make clean
#	cd $(WISLIVE_DIR) && make clean
#	cd $(WISLIVE_STAND_DIR) && make clean
#	cd $(LAME_DIR) && make distclean
	rm -fr Lib
	cd Src/Lib/VideoCapture && make clean
#	cd Src/Lib/Auxiliary && make clean
#	cd Src/App/go-server && make clean


load:
	/sbin/insmod install/wis_sensor.o
	/sbin/insmod install/wisgo7007sb.o


unload:
	/sbin/rmmod -r wisgo7007sb

