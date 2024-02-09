ifneq ($(VERBOSE),1)
QUIET_CC = @echo "CC $@";
QUIET_DEP = @echo "DEP $@";
QUIET_BIN = @echo "BIN $@";
QUIET_CLEAN = @echo "CLEAN";
endif

CC = g++
CCSYSTEM = $(shell $(CC) -dumpmachine)
MINGW = 0
LIBEXT = so
ifeq ($(findstring mingw32, $(CCSYSTEM)), mingw32)
MINGW = 1
LIBEXT = a
endif

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
	DEV_DIR=dev/dev_arm
else
	DEV_DIR=dev/dev_i386
endif


MATROSKADIR = ../../../${DEV_DIR}/matroska
LIBMATROSKADIR = ${MATROSKADIR}/lib
MATROSKALIBS = -L ${LIBMATROSKADIR} -lmatroska -lebml

XERCESDIR = ../../../${DEV_DIR}/xerces
LIBXERCESDIR = ${XERCESDIR}/lib
XERCESLIBS = -L ${LIBXERCESDIR} -lxerces-c
XERCESINCDIR = ${XERCESDIR}/include

MEDIAINDEXDIR = ../../../mediaindex
MEDIAINDEXINCDIR = ${MEDIAINDEXDIR}/
MEDIAINDEXCFLAGS = -I ${MEDIAINDEXDIR}/  
MEDIAINDEXLIBPATH = ${MEDIAINDEXDIR}
MEDIAINDEXLIBS = -L ${MEDIAINDEXLIBPATH}/ -lmediaindex

STRMSRVDIR = ../../../strmsrv
STRMSRVINCDIR = ${STRMSRVDIR}/
STRMSRVCFLAGS = -I ${STRMSRVDIR}
STRMSRVLIBPATH = ${STRMSRVDIR}
STRMSRVLIBS = -L ${STRMSRVLIBPATH}/ -lstrmsrv

BCP_DIR		=	../../../VMP_DMS
HLS_CFLAGS	=	-I${BCP_DIR}/VMP_DMS/Src

V2DLIBDIR = ../../../v2dlib
V2DLIBCFLAGS = -I ${V2DLIBDIR}
V2DLIBLIBPATH = ${V2DLIBDIR}
V2DLIBLIBS = -L ${V2DLIBLIBPATH}/ -lv2d

RFBPROTODIR = ../../../rfbproto
RFBPROTOLIBPATH = ${RFBPROTODIR}
RFBPROTOLIBS = -L ${RFBPROTOLIBPATH}/ -lrfbproto

V2ODIR = ../../../V2O/modules
V2OLIBPATH = ${V2ODIR}
V2ODRVLIBPATH = ../../../V2O/modules/drivers
V2OLIBS = -L ${V2OLIBPATH} -L ${V2ODRVLIBPATH} -lusb -lv2o -lv2okernel

FFMPEGDIR = ../../../${DEV_DIR}/ffmpeg
FFMPEGLIBPATH = ${FFMPEGDIR}/lib
FFMPEGLIBS = -L ${FFMPEGDIR}/lib -lavformat -lavcodec -lavutil -lswscale -lz -lbz2

PORTAUDIODIR = ../../../${DEV_DIR}/portaudio
PORTAUDIOLIBPATH = ${PORTAUDIODIR}/lib
PORTAUDIOLIBS = -L ${PORTAUDIODIR}/lib -lportaudio

LOG4CDIR = ../../../${DEV_DIR}/log4c
LOG4CLIBPATH = ${LOG4CDIR}/lib
LOG4CLIBS = -L ${LOG4CLIBPATH} -llog4c

LIBXKBCOMMONDIR = ../../../${DEV_DIR}/libxkbcommon
LIBXKBCOMMONINCDIR = ${LIBXKBCOMMONDIR}/include
LIBXKBCOMMON_INCLUDE = -I ${LIBXKBCOMMONINCDIR}
LIBLIBXKBCOMMONDIR = ${LIBXKBCOMMONDIR}/lib
LIBXKBCOMMONLIBS = -L ${LIBLIBXKBCOMMONDIR} -lxkbcommon

BOOSTDIR = ../../../${DEV_DIR}/boost
BOOSTINCDIR = ${BOOSTDIR}/include
LIBBOOSTDIR = ${BOOSTDIR}/lib
ifeq ($(MINGW),1)
BOOSTLIBS = -L ${LIBBOOSTDIR} -lboost_program_options -lboost_filesystem -lboost_system
else
BOOSTLIBS = -lboost_program_options -lboost_filesystem
endif

SHAREDLIBS = ${MEDIAINDEXDIR}/libmediaindex.${LIBEXT}

ifeq ($(MINGW),1)
UUIDLIBS = -lrpcrt4
else
UUIDLIBS = -luuid
endif

CFLAGS = -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
CFLAGS += -Wall 

CFLAGS += ${MEDIAINDEXCFLAGS} ${STRMSRVCFLAGS} ${V2DLIBCFLAGS} ${HLS_CFLAGS}
ifeq ($(MINGW),1)
CFLAGS += -I ${BOOSTINCDIR}
endif

LIBS += ${MEDIAINDEXLIBS} ${MATROSKALIBS} ${XERCESLIBS} ${BOOSTLIBS} 

ifeq ($(MINGW),0)
LIBS += ${STRMSRVLIBS} ${V2DLIBLIBS} ${RFBPROTOLIBS} ${V2OLIBS} ${FFMPEGLIBS} ${PORTAUDIOLIBS} ${LOG4CLIBS} ${LIBXKBCOMMONLIBS} 
endif

LIBS += ${UUIDLIBS}

ifeq ($(DEBUG),1)
CFLAGS += -g -DDEBUG=1 -O2
else
CFLAGS += -O2
endif

MAINOBJS = mediaconverter.o 
SRCS = $(MAINOBJS:.o=.cpp) 	
EXETARGETS = mediaconverter

all: ${EXETARGETS}

%.o:%.cpp
	${QUIET_CC} ${CC} -c ${CFLAGS} $< -o $@

mediaconverter: ${MAINOBJS} ${SHAREDLIBS}
ifeq ($(MINGW),1)
	${QUIET_BIN} ${CC}  ${LFLAGS} ${MAINOBJS} ${LIBS} -static-libgcc -static-libstdc++ -o ${@}
else
	${QUIET_BIN} ${CC}  ${LFLAGS} ${MAINOBJS} ${LIBS} -o ${@}
endif

clean:
	${QUIET_CLEAN} ${RM} Makefile.d *.d *.o ${EXETARGETS}

# Create dependencies files
%.d: %.cpp
	${QUIET_DEP} set -e; $(CC) -MM -MT $@ -MT ${patsubst %.d, %.o, $@} $(CFLAGS) $<  -o $@; \
	   [ -s $@ ] || rm -f $@

.PHONY: clean

ifneq ($(MAKECMDGOALS),clean)
-include $(SRCS:.cpp=.d)
endif

