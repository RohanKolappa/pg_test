
ifneq ($(VERBOSE),1)
QUIET_CC = @echo "CC $@";
QUIET_DEP = @echo "DEP $@";
QUIET_BIN = @echo "BIN $@";
QUIET_LIB = @echo "LIB $@";
QUIET_CLEAN = @echo "CLEAN";
QUIET = @ 
endif

CC = ${CROSS_COMPILE}gcc
CCSYSTEM = $(shell $(CC) -dumpmachine)
MINGW = 0
LIBEXT = so
ifeq ($(findstring mingw32, $(CCSYSTEM)), mingw32)
MINGW = 1
LIBEXT = a
endif

CFLAGS = -Wall -DSTREAMING_SERVER
#CFLAGS = -Wall
#DEBUG = 1

ifeq ($(DEBUG),1)
CFLAGS += -g -DDEBUG=1 
else 
CFLAGS += -O2
endif


ifeq ($(MINGW),1)
LIBS += -lws2_32 -lwsock32
endif

LIBSRCS = sockets.c vncauth.c d3des.c rfbio.c rfbclient.c

TESTSRCS = rfbtest.c

LIBOBJS=$(LIBSRCS:.c=.o)

SRCS = ${LIBSRCS} ${TESTSRCS}

all: librfbproto.${LIBEXT} rfbtest

clean:
	${QUIET_CLEAN} /bin/rm -f librfbproto.${LIBEXT} rfbtest Makefile.d *.o *.d

librfbproto.${LIBEXT}: ${LIBOBJS}
ifeq ($(MINGW),0)
	${QUIET_LIB} ${CC} -shared -o $@ $^ ${LIBS} 
else
	${QUIET_LIB} ar cr $@ $^
endif

rfbtest: rfbtest.o librfbproto.${LIBEXT}
	${QUIET_BIN} ${CC} -o $@   rfbtest.o -L"./" -lrfbproto ${LIBS}

%.o:%.c
	${QUIET_CC} ${CC} -c ${CFLAGS} $< -o $@

# Create dependencies files
%.d: %.c
	${QUIET_DEP} set -e; $(CC) -MM $(CFLAGS) $< | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	   [ -s $@ ] || rm -f $@


ifneq ($(MAKECMDGOALS),clean)
-include $(SRCS:.c=.d)
endif

-include Makefile.d
