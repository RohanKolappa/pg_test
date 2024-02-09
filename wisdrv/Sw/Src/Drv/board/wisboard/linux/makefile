# makefile for module

CC = gcc
LD = ld 
# Note that the linux include directoy path below has been changed
CFLAGS = -I../../../../../../../linux/include -I../../../include -I../../../../Include -Wall -O2 -g
DEFINES = -D__KERNEL__ -DMODULE -DDEBUG -D_LINUX -DDRIVER

OBJ =	board_module.o				\
		../Board_MatrixII.o			\
		../Board_MatrixReload.o		\
		../Board_TVTuner.o			\
		../Board_Twister.o			\
		../Board_XMen.o				\
		../Board_XMenII.o			\
		../Board_XMenIII.o			\
		../Board_MatrixRevolution.o	\
		../Board_ISpy.o			    \
		../Board_ISpyII.o			    \
		../BoardEntry.o				\
		../../../platform/linux/platform.o	\
		../../../common/debug.o

all: wis_sensor.o

.c.o:
	$(CC) $(DEFINES) $(CFLAGS) -o $@ -c $<

wis_sensor.o: $(OBJ)
	$(LD) -r -o $@ $(OBJ)

clean:
	rm -f *.o *~ core
	rm -f ../*.o
	rm -f ../../../common/*.o
	rm -f ../../../platform/linux/*.o

