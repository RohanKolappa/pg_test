SOURCES = printedid.c \
		  setedid.c \
		  reg.c  \
		  testmdio.c \
		  temperature.c

OBJECTS = $(SOURCES:.c=.o)
EXECUTABLES = $(OBJECTS:.o=)
SCRIPTITEMS = $(OBJECTS:.o=.script)

## Compiler settings

CC = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld
LN = ln -s
STRIP = $(CROSS_COMPILE)strip

INCLUDES = -I../../driver/netvizxpi -I../../../linux-svn/include/media

CFLAGS = -Wall
  
## Main build rules

all: $(EXECUTABLES) mreg macreg drpreg 

$(EXECUTABLES): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $@.c $(INCLUDES) $(LIBS)

mreg: reg
	${LN} reg $@

macreg: reg
	${LN} reg $@

drpreg: reg
	${LN} reg $@

clean:
	rm -rf $(EXECUTABLES) mreg macreg drpreg

InitScript:
	@rm -f transferscript.sh
	@echo -n ipaddr=$$ > transferscript.sh
	@echo \{1-192.92.109.100\} >> transferscript.sh

.PHONY: $(SCRIPTITEMS)
$(SCRIPTITEMS):
	@echo $@	
	@echo "tftp -g -r Barco-new-irq/software/application/$(@:.script=) \$$ipaddr" >> transferscript.sh
	@echo "chmod +x $(@:.script=)" >> transferscript.sh

transferscript.sh: InitScript $(SCRIPTITEMS)
	@echo "tftp -g -r Barco-new-irq/software/libaio-0.3.107/debian/src/libaio.so.1.0.1 \$$ipaddr" >> transferscript.sh
	@echo "tftp -g -r Barco-new-irq/software/application/transferscript.sh \$$ipaddr" >> transferscript.sh

TransferScript: transferscript.sh

