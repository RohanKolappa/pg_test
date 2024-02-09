

ifneq ($(VERBOSE),1)
QUIET_CC = @echo "CC $@";
QUIET_DEP = @echo "DEP $@";
QUIET_LIB = @echo "LIB $@";
QUIET_BIN = @echo "BIN $@";
QUIET_CLEAN = @echo "CLEAN";
endif

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
DEV_DIR=dev/dev_arm
else
DEV_DIR=dev/dev_i386
endif

CC = $(CROSS_COMPILE)g++
CCSYSTEM = $(shell $(CC) -dumpmachine)
MINGW = 0
LIBEXT = so
ifeq ($(findstring mingw32, $(CCSYSTEM)), mingw32)
MINGW = 1
LIBEXT = a
endif

# Speed up compilation under mingw
ifeq ($(MINGW),1)
export MAKEFLAGS := -r
%.hh:
%.h:
%.cpp:
%.c:
%.hpp:
endif

MATROSKADIR = ../${DEV_DIR}/matroska
MATROSKAINCDIR = ${MATROSKADIR}/include
LIBMATROSKADIR = ${MATROSKADIR}/lib
MATROSKALIBS = -L ${LIBMATROSKADIR} -lmatroska -lebml

XERCESDIR = ../${DEV_DIR}/xerces
XERCESINCDIR = ${XERCESDIR}/include
LIBXERCESDIR = ${XERCESDIR}/lib
XERCESLIBS = -L ${LIBXERCESDIR} -lxerces-c


BOOSTDIR = ../${DEV_DIR}/boost
BOOSTINCDIR = ${BOOSTDIR}/include
LIBBOOSTDIR = ${BOOSTDIR}/lib
ifeq ($(MINGW),1)
BOOSTLIBS = -L ${LIBBOOSTDIR} -lboost_filesystem -lboost_system
else
ifneq (,$(findstring arm,$(CROSS_COMPILE)))
BOOSTLIBS = -L ${LIBBOOSTDIR} -lboost_filesystem -lboost_system
else
BOOSTLIBS = -lboost_filesystem
endif
endif

GTESTDIR = ../${DEV_DIR}/gmock
GTESTCFLAGS = -I ${GTESTDIR}/include
GTESTLIB = ${GTESTDIR}/lib/gmock_main.a ${GTESTDIR}/lib/gtest_utils.a 
ifeq ($(MINGW),1)
GTESTLIB += ${BOOSTLIBS} -lboost_system
else
ifneq (,$(findstring arm,$(CROSS_COMPILE)))
GTESTLIBS = -L ${BOOSTLIBS} -lboost_system 
else
GTESTLIB += -lpthread
endif
endif
UUIDDIR = ../${DEV_DIR}/uuid
UUIDINCDIR = ${UUIDDIR}/include
LIBUUIDDIR = ${UUIDDIR}/lib

ifeq ($(MINGW),1)
UUIDLIBS = -lrpcrt4
else
ifneq (,$(findstring arm,$(CROSS_COMPILE)))
UUIDLIBS = -L ${LIBUUIDDIR} -luuid 
else
UUIDLIBS = -luuid
endif
endif

CFLAGS = -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
CFLAGS += -Wall

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
CFLAGS += -D__ARM__ -DUSE_OSSP_UUID=1
endif

ifeq ($(DEBUG),1)
ifeq ($(COVERAGE),1)
CFLAGS += -g -fprofile-arcs -ftest-coverage
else
CFLAGS += -g
endif
else
CFLAGS += -O2
endif

CFLAGS += -I ${MATROSKAINCDIR} -I ${XERCESINCDIR}  
ifneq (,$(findstring arm,$(CROSS_COMPILE)))
CFLAGS += -I ${UUIDINCDIR} -I ${BOOSTINCDIR}
endif

ifeq ($(MINGW),1)
CFLAGS += -I ${BOOSTINCDIR} -DXML_LIBRARY
endif


LIBS += ${MATROSKALIBS} ${XERCESLIBS} ${UUIDLIBS} ${BOOSTLIBS} 

LIBOBJS = MediaIndexFactory.o \
          MediaPortalUtils.o \
          ClipSourceIndex.o \
          ClipDestIndex.o \
          IndexData.o \
          MediaServer.o \
          MinuteFileMediaServer.o \
          MatroskaMediaServer.o \
          SnapshotMediaServer.o \
          MediaWriter.o \
          MatroskaFileParser.o \
          MatroskaUtils.o \
          XMLUtils.o \
          SystemUtils.o \
          FileSystemUtils.o \
          Std64IOCallback.o \
          EditList.o \
          MatroskaFileInfo.o \
          MinuteFileWriter.o \
          MatroskaFileWriter.o \
          MatroskaFileWriterImpl.o


SRCS = $(LIBOBJS:.o=.cpp) index_media.cpp

TESTOBJS = parser_test.o \
           editlist_test.o \
           info_test.o \
           extract_test.o \
           create_test.o \
           mediaserver_test.o \
           mediawriter_test.o


TEST_OBJS = ${patsubst %.o, test/%.o, ${TESTOBJS}}
TEST_SRCS = $(TEST_OBJS:.o=.cpp)

all : libmediaindex.${LIBEXT}  testserver index_media show_gaps mediaindexLibTests

testserver : testserver.cpp libmediaindex.${LIBEXT}
	${QUIET_BIN} ${CC} ${CFLAGS} -o testserver testserver.cpp libmediaindex.${LIBEXT} ${LIBS}

libmediaindex.${LIBEXT}: ${LIBOBJS}
ifeq ($(MINGW),0)
	${QUIET_LIB} ${CC}  -shared -o $@ $^
else
	${QUIET_LIB} ar cr $@ $^
endif

index_media: index_media.cpp
	${QUIET_BIN} ${CC} ${CFLAGS} -o index_media index_media.cpp

show_gaps : show_gaps.cpp libmediaindex.${LIBEXT}
	${QUIET_BIN} ${CC} ${CFLAGS} -o show_gaps show_gaps.cpp libmediaindex.${LIBEXT} ${LIBS}

%.o:%.cpp
	${QUIET_CC} ${CC} -c ${CFLAGS} $< -o $@
${TEST_OBJS}: %.o: %.cpp
	${QUIET_CC} ${CC} -c ${CFLAGS} -I . ${GTESTCFLAGS} -o $@ $< 

clean :
	${QUIET_CLEAN} rm -f *.o  *.${LIBEXT} *.exe *.d  test/*.o test/*.d index_media testserver show_gaps mediaindexLibTests

lint:
	cppcheck -D__GNUC__ -I ${MATROSKAINCDIR} -I ${XERCESINCDIR} ${GTESTCFLAGS} \
	--suppress="*:../gmock/include/*" --suppress="*:../matroska/include/*" \
	--suppress="*:../xerces/include/*" --enable=all -q .


test: mediaindexLibTests
	 export LD_LIBRARY_PATH=.:${LIBXERCESDIR}:${LIBMATROSKADIR}; \
         ./mediaindexLibTests ${TESTPARAMS}

mediaindexLibTests: ${TEST_OBJS} libmediaindex.${LIBEXT}
	${QUIET_BIN} ${CC} -o $@ -L"./" ${TEST_OBJS} \
	     ${GTESTLIB} -lmediaindex ${LIBS} -lpthread


# Create dependencies files
%.d: %.cpp
	${QUIET_DEP} set -e; $(CC) -MM $(CFLAGS) $< | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	   [ -s $@ ] || rm -f $@

$(TEST_SRCS:.cpp=.d): %.d: %.cpp
	${QUIET_DEP} set -e; $(CC) -MM -MT $@ -MT ${patsubst %.d, %.o, $@} $(CFLAGS) $<  -I . ${GTESTCFLAGS} -o $@; \
	   [ -s $@ ] || rm -f $@

ifneq ($(MAKECMDGOALS),clean)
-include $(SRCS:.cpp=.d)
-include $(TEST_SRCS:.cpp=.d)
endif

.PHONY: test clean
