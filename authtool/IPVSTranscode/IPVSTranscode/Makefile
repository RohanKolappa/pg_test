ifneq ($(VERBOSE),1)
QUIET_CC = @echo "CC $@";
QUIET_DEP = @echo "DEP $@";
QUIET_BIN = @echo "BIN $@";
QUIET_LIB = @echo "LIB $@";
QUIET_CLEAN = @echo "CLEAN";
endif

CC=g++

CCSYSTEM = $(shell $(CC) -dumpmachine)
MINGW = 0
LIBEXT = so
ifeq ($(findstring mingw32, $(CCSYSTEM)), mingw32)
MINGW = 1
LIBEXT = a
endif

DEV_DIR=../../../dev/dev_i386

V2DLIBDIR = ../../../v2dlib
V2DLIBCFLAGS = -I ${V2DLIBDIR}
V2DLIBLIBS = -L ${V2DLIBDIR} -lv2d   
ifeq ($(MINGW),0)
V2DLIBLIBS += -Wl,-Bstatic -lpng -ljpeg -lz -Wl,-Bdynamic
endif

MATROSKADIR = ${DEV_DIR}/matroska
LIBMATROSKADIR = ${MATROSKADIR}/lib
MATROSKALIBS = -L ${LIBMATROSKADIR} -lmatroska -lebml

XERCESDIR = ${DEV_DIR}/xerces
LIBXERCESDIR = ${XERCESDIR}/lib
XERCESLIBS = -L ${LIBXERCESDIR} -lxerces-c
XERCESINCDIR = ${XERCESDIR}/include

MEDIAINDEXDIR = ../../../mediaindex
MEDIAINDEXCFLAGS = -I ${MEDIAINDEXDIR}
MEDIAINDEXLIBS = -L ${MEDIAINDEXDIR} -lmediaindex

AVISYNTHDIR = ${DEV_DIR}/avisynth
AVISYNTHCFLAGS = -I ${AVISYNTHDIR}/include
AVISYNTHLIBS = ${AVISYNTHDIR}/lib/avisynth.lib

FFMPEGDIR = ${DEV_DIR}/ffmpeg
FFMPEGCFLAGS = -I ${FFMPEGDIR}/include
FFMPEGLIBPATH = ${FFMPEGDIR}/lib
ifeq ($(MINGW),1)
FFMPEGLIBS = -L ${FFMPEGDIR}/lib -lavformat -lavcodec -lavutil -lswscale
else
FFMPEGLIBS = -L ${FFMPEGDIR}/lib -lavformat -lavcodec -lavutil -lswscale -lz -lbz2
endif

GTESTDIR = ${DEV_DIR}/gmock
GTESTCFLAGS = -I ${GTESTDIR}/include
GTESTLIB = ${GTESTDIR}/lib/gmock_main.a ${GTESTDIR}/lib/gtest_utils.a
ifeq ($(MINGW),0)
GTESTLIB += -lpthread
endif

BOOSTDIR = ${DEV_DIR}/boost
BOOSTINCDIR = ${BOOSTDIR}/include
LIBBOOSTDIR = ${BOOSTDIR}/lib
ifeq ($(MINGW),1)
BOOSTLIBS = -L ${LIBBOOSTDIR} -lboost_filesystem -lboost_system
else
BOOSTLIBS = -lboost_filesystem -lboost_system
endif

SAMPLERATEDIR = ${DEV_DIR}/libsamplerate
SAMPLERATECFLAGS = -I ${SAMPLERATEDIR}/include
SAMPLERATELIBPATH = ${SAMPLERATEDIR}/lib
SAMPLERATELIBFILE = ${SAMPLERATELIBPATH}/libsamplerate.a
SAMPLERATELIBS = -L ${SAMPLERATEDIR}/lib -lsamplerate

ifeq ($(MINGW),1)
UUIDLIBS = -lrpcrt4 -L ../../../boost/lib -lboost_filesystem  -lboost_system
else
UUIDLIBS = -luuid
endif

CFLAGS = -Wall -Wno-deprecated-declarations

CFLAGS += ${V2DLIBCFLAGS} ${MEDIAINDEXCFLAGS} ${FFMPEGCFLAGS} ${SAMPLERATECFLAGS}
LIBS += ${V2DLIBLIBS} ${MEDIAINDEXLIBS} ${FFMPEGLIBS} ${SAMPLERATELIBS} ${XERCESLIBS} ${MATROSKALIBS} \
     ${UUIDLIBS} ${BOOSTLIBS}

ifeq ($(MINGW),1)
LIBS += -lws2_32 -lwsock32 -lgdi32 -lwinmm
endif

ifeq ($(DEBUG),1)
CFLAGS += -g -DDEBUG=1 -O2
else
CFLAGS += -O2
endif

CODEOBJS = V2DFrameServer.o V2DAudioFrameServer.o AudioResampler.o

TESTOBJS = v2dframeserver_test.o v2daudioframeserver_test.o

TEST_OBJS = ${patsubst %.o, test/%.o, ${TESTOBJS}}

SRCS = $(CODEOBJS:.o=.cpp)
TEST_SRCS = $(TEST_OBJS:.o=.cpp)

PLUGINOBJS = ${CODEOBJS}

PLUGINTARGETS =
ifeq ($(MINGW),1)
PLUGINTARGETS = IPVSMediaPlugin.dll
endif

TESTTARGETS = TranscodeTests

all: ${PLUGINTARGETS} ${TESTTARGETS}

%.o:%.cpp
	${QUIET_CC} ${CC} -c ${CFLAGS} $< -o $@

${TEST_OBJS}: %.o: %.cpp
	${QUIET_CC} ${CC} -c ${CFLAGS} -I . ${GTESTCFLAGS} -o $@ $< 

IPVSMediaPlugin.dll : IPVSMediaPlugin.o ${PLUGINOBJS} ${AVISYNTHLIBS} 
	${QUIET_LIB} ${CC} -shared -static-libgcc -static-libstdc++ -o $@ $^ ${LIBS} 

IPVSMediaPlugin.o : IPVSMediaPlugin.cpp
	${QUIET_CC} ${CC} -Wall -O2 -c -o $@ ${AVISYNTHCFLAGS} ${CFLAGS} IPVSMediaPlugin.cpp
	
deploy:
	cp IPVSMediaPlugin.dll /e/temp

TranscodeTests: ${TEST_OBJS} ${CODEOBJS}
	${QUIET_BIN} ${CC} -o ${@} ${TEST_OBJS} ${CODEOBJS} ${LIBS} ${GTESTLIB} ${LFLAGS} 

clean:
	${QUIET_CLEAN} ${RM} Makefile.d *.d *.o test/*.o ${EXETARGETS} ${TESTTARGETS}

# Create dependencies files
%.d: %.cpp
	${QUIET_DEP} set -e; $(CC) -MM -MT $@ -MT ${patsubst %.d, %.o, $@} $(CFLAGS) $<  -o $@; \
	   [ -s $@ ] || rm -f $@

$(TEST_SRCS:.cpp=.d): %.d: %.cpp
	${QUIET_DEP} set -e; $(CC) -MM -MT $@ -MT ${patsubst %.d, %.o, $@} $(CFLAGS) $<  -I . ${GTESTCFLAGS} -o $@; \
	   [ -s $@ ] || rm -f $@

.PHONY: clean

ifneq ($(MAKECMDGOALS),clean)
-include $(SRCS:.cpp=.d)
-include $(TEST_SRCS:.cpp=.d)
endif

