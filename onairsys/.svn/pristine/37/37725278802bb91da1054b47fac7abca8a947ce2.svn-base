CC=gcc

LCDOBJS=633_linux/633_packet.o 633_linux/show_packet.o 633_linux/serial.o

#all: misc frontpanel 3Ware csu E1000
all: frontpanel

3Ware:
	${MAKE} -C 3ware/driver -f Makefile.rh

csu:
	${MAKE} -C csu -f Makefile.i386

frontpanel:
	${MAKE} -C frontpanel

E1000:
	${MAKE} -C e1000/src KSP=../../../linux

misc:
	${MAKE} -C misc

clean:
	${MAKE} -C misc clean
	${MAKE} -C frontpanel clean
	${MAKE} -C csu -f Makefile.i386 clean
#	${MAKE} -C e1000/src clean KSP=../../../linux
	${MAKE} -C 3ware/driver -f Makefile.rh clean


.PHONY: misc 3Ware frontpanel E1000 csu
