CC=$(CROSS_COMPILE)gcc
LD = ${CROSS_COMPILE}ld
DEVDIR=
CFLAGS=../dev/
ifneq (,$(findstring arm,$(CROSS_COMPILE)))
DEV_DIR=dev/dev_arm
else
DEV_DIR=
endif

STROPHEDIR = ../${DEV_DIR}/libstrophe
STROPHEINCDIR = ${STROPHEDIR}/usr/local/include
STROPHEINCDIR = ${STROPHEDIR}/usr/local/include
STROPHE_INCLUDE = -I ${STROPHEDIR}/usr/local/include
LIBSTROPHEDIR = ${STROPHEDIR}/usr/local/lib
STROPHELIBS = -L ${LIBSTROPHEDIR} -l strophe

LIBS=${STROPHELIBS} -lexpat -lssl -lcrypto -lz -lresolv 
CFLAGS=-I. ${STROPHE_INCLUDE}
default:bin/agent
bin/agent:src/agent.c
	${CC}  ${CFLAGS} -g -o $@ $? ${LIBS}
clean:
	${RM} bin/agent bin/agent.o
