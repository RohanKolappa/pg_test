#Makefile for Crystalfontz ./test633
#Please read the README for details on using the 'test633' utility and 'make' options

CC = gcc
DEBUGFLAG = -g
LIBS = -lstdc++
DEBUGFLAG = -g
CFLAGS = $(DEBUGFLAG)
LDFLAGS = $(DEBUGFLAG)

SRCS = 633_packet.c show_packet.c serial.c test633.c
OBJS = $(SRCS:.c=.o)
TARGET = test633

all:	$(TARGET)

$(TARGET):	$(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)

distribution:	clean
	tar -C .. -cvf ../633_linux.tar 633_linux
