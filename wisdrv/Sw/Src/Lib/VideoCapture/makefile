# makefile for module

CC=g++
AR=ar
CFLAGS=-g -D_LINUX -I../../Include -I../../../Include \
                   -I../../Drv/include \
                   -Wall -Wcast-align -Wcast-qual -O2

OBJ = 	Device.o 	\
		VideoCapture.o

SRC =   Device.cpp	\
		VideoCapture.cpp

AFLAGS=rv

all:    lib solib

.cpp.o:
	$(CC) $(CFLAGS) -o $@ -c $<

lib: $(OBJ)
	$(AR) $(AFLAGS) ../../../Lib/libVideoCapture.a $(OBJ)

solib: $(SRC)
	$(CC) $(CFLAGS) -shared -o ../../../Lib/libVideoCapture.so $(SRC)

clean:
	rm -f *.o *~ core

dep:
	$(CC) $(CFLAGS) -M $(SRC) > .depend



