QUIET_CC = @echo "CC $@";
QUIET_CLEAN = @echo "CLEAN";
QUIET = @


CC=g++
RM=rm -rf

V2DLIB_DIR = ../v2dlib
V2DLIB_CFLAGS = -I ${V2DLIB_DIR}

V2DRX_INCLUDE = -I ../include
V2DRX_LIBS = -L ../v2drxlib -lv2drx

CFLAGS=${V2DLIB_CFLAGS}

CFLAGS += -DHAVE_CONFIG_H
CFLAGS += -Wall ${INCDIR}   

HEADERFILES = ../bitmap.hh \
              ../bitstream.h \
              ../codecdefs.h \
              ../colortransform.h \
              ../ipputils.h \
              ../listutils.h \
              ../pgetopt.h \
              ../ScapProcessor.hh \
              ../slicedecoder.hh \
              ../srd.h \
              ../v2dcommon.h \
              ../v2d.h \
              ../v2dlib_compatibility.h \
              ../vframedecoder.hh

V2DRXLIBOBJS = ../v2d.c \
               ../v2drx.c \
               ../srd.c \
               ../comp_utils.c \
               ../net_utils.c \
               ../svc.c \
               ../aligned_malloc.c \
               ../cvt.c \
               ../protocol_utils.c \
               ../bitstream.o \
               ../codec_tables.o \
               ../imagemanip.o \
               ../bitmap.o \
               ../slicedecoder.o \
               ../h264slicedecoder.o \
               ../svcslicedecoder.o \
               ../colortransform.o \
               ../vframedecoder.o \
               ../slicebuffer.o \
               ../dct.o \
               ../ScapProcessor.o \
               ../EmulationPrevention.o 

V2DRXOBJS =../v2drx_test/rx.cpp ../pgetopt.o

all: libv2drx.so  rx image

%.o:%.c
	${QUIET_CC} ${CC} -c ${CFLAGS} $< -o $@

%.o:%.cpp
	${QUIET_CC} ${CC} -c ${CFLAGS} $< -o $@

libv2drx.so: ${V2DRXLIBOBJS}
	${QUIET_CC} ${CC} -shared -o $@ ${V2DRXLIBOBJS} 

RXINCLUDE: 
	${QUIET} mkdir -p ../include
	${QUIET} cp ${HEADERFILES} ../include

rx:  RXINCLUDE ${V2DRXOBJS} libv2drx.so 
	${QUIET_CC} ${CC} ${V2DRX_INCLUDE} ${V2DRX_LIBS} -o rx ${V2DRXOBJS} 

image:
	 ${QUIET} mkdir -p ../src
	 ${QUIET} mkdir -p ../lib
	 ${QUIET} mkdir -p ../bin

	 ${QUIET} cp ReadMe.txt ../docs/
	 ${QUIET} cp rx ../bin
	 ${QUIET} cp libv2drx.so ../lib
	 ${QUIET} cp ../v2drx_test/rx.cpp ../src
	 ${QUIET} cp ../pgetopt.c  ../src
	 ${QUIET} tar czfP ../v2drxlib_$(shell svn info | grep "Revision" | cut -d':' -d " "  -f2).tar ../bin ../lib ../src ../include ../docs/ReadMe.txt

	 ${QUIET} rm -rf ../bin
	 ${QUIET} rm -rf ../lib
	 ${QUIET} rm -rf ../include
	 ${QUIET} rm -rf ../src
	 ${QUIET} rm -rf ../docs/ReadMe.txt	
 
clean: 
	${RM} *.o *.so rx ../v2drx*.tar

