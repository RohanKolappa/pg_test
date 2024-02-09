SOURCES = OmniTek_BAR_RegAccess.c   \
          OmniTek_test_BAR_IOCtl.c  \
          OmniTek_test_MDMA.c       \
          omnitek_debug.c           \
          OmniTek_test_BAR_ByteAccess.c  \
          OmniTek_test_BAR_MultiAccess.c \
          OmniTek_test_FDMA.c            \
          OmniTek_test_kernel_MDMA.c    \
          OmniTek_test_EFDMA.c

OBJECTS = $(SOURCES:.c=.o)
EXECUTABLES = $(OBJECTS:.o=)
SCRIPTITEMS = $(OBJECTS:.o=.script)

## Compiler settings

LDFLAGS = -laio
CFLAGS = -Wall
  
## Main build rules

all: $(EXECUTABLES)

$(EXECUTABLES): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $@.c $(INCLUDES) $(LIBS) $(LDFLAGS)

clean:
	rm -rf $(EXECUTABLES)

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

