# makefile for module

CC = gcc
LD = ld 
# Note that the linux include directoy path below has been changed
CFLAGS = -I../../../../../../linux/include -I../../../../Include -I../../include -I../../../Include -Wall -O2 -g
DEFINES = -D__KERNEL__ -DMODULE -DDEBUG -D_LINUX -DDRIVER -D_SDK_PACGEN_ 

OBJ =							\
		../../common/AudioStreamBuffer.o	\
		../../common/CfgFileParser.o		\
		../../common/AVSync.o			\
		../../common/debug.o			\
		../../common/function.o			\
		../../common/gpio.o			\
		../../common/hw_base.o			\
		../../common/I2C.o			\
		../../common/PacGen_7007SB.o		\
		../../common/StreamBuffer.o		\
		../../common/motion_detection.o		\
		../../platform/linux/usb_base.o		\
		../../platform/linux/platform.o		\
		../../common/osd.o			\
		WisGoDrv.o				\
		DeviceCtl.o				\
		config_simple.o


all: wisgo7007sb.o

.c.o:
	$(CC) $(DEFINES) $(CFLAGS) -o $@ -c $<

wisgo7007sb.o: $(OBJ)
	$(LD) -r -o $@ $(OBJ)

mod:
	-sudo /sbin/rmmod wisgo7007
	-sudo /sbin/modprobe usb-ohci
	-sudo /sbin/insmod -m wisgo7007.o > ldmd.map
	cat /proc/ksyms > sav-ksyms
	cat /proc/modules > sav-modules

clean:
	rm -f *.o *~ core
	rm -f ../../common/*.o
	rm -f ../../platform/linux/*.o
	rm -f ldmd.map
	rm -f sav-ksyms
	rm -f sav-modules

