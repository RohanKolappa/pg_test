ifneq ($(VERBOSE),1)
QUIET_CC = @echo "CC $@";
QUIET_DEP = @echo "DEP $@";
QUIET_BIN = @echo "BIN $@";
QUIET_LIB = @echo "LIB $@";
QUIET_CLEAN = @echo "CLEAN";
QUIET_DOC = @echo "DOC $@";
QUIET = @ 
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

DO_GTEST=1
ifeq ($(DO_GTEST),1)
GTESTDIR = ../${DEV_DIR}/gmock
GTESTCFLAGS = -I ${GTESTDIR}/include
GTESTLIB = ${GTESTDIR}/lib/gmock_main.a ${GTESTDIR}/lib/gtest_utils.a 

ifeq ($(MINGW),0)
GTESTLIB += -lboost_filesystem
endif

ifeq ($(MINGW),1)
GTESTLIB += -L ../${DEV_DIR}/boost/lib -lboost_system -lboost_filesystem
else
GTESTLIB += -lpthread
ifneq (,$(findstring arm,$(CROSS_COMPILE)))
GTESTLIB += -L ../${DEV_DIR}/boost/lib -lboost_system
endif
endif
endif

USE_IPP_LIB=0
ifeq ($(USE_IPP_LIB),1)
IPPDIR = /home/rkale/intel/ipp/6.1.2.051/ia32
IPPFLAGS = -I ${IPPDIR}/include
IPPLIBPATH = ${IPPDIR}/lib
IPPLIB = -L ${IPPLIBPATH} -lippvcemerged -lippvcmerged \
	-lippjemerged -lippjmerged \
	-lippiemerged -lippimerged \
	-lippsemerged -lippsmerged \
	-lippccemerged -lippccmerged \
	-lippcore
endif

CFLAGS = -Wall -DSTREAMING_SERVER ${IPPFLAGS} 

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
CFLAGS += -D__XPI__ -I ../${DEV_DIR}/png/include -I ../${DEV_DIR}/jpeg/include
endif

ifeq ($(MINGW),0)
STATICLIBS = -Wl,-Bstatic -L../${DEV_DIR}/png/lib -lpng -L../${DEV_DIR}/jpeg/lib -ljpeg -lz -Wl,-Bdynamic ${IPPLIB}
endif

ifeq ($(MINGW),1)
LIBS += -lws2_32 -enable-auto-import --no-undefined --enable-runtime-pseudo-reloc 
endif

DEBUG=0
ifeq ($(DEBUG),1)
ifeq ($(COVERAGE),1)
CFLAGS += -g -DDEBUG=1 -fprofile-arcs -ftest-coverage
else
CFLAGS += -g -DDEBUG=1 
endif
else
CFLAGS += -O2 -g
endif

ifeq ($(MINGW),1)
CFLAGS += -DLOCAL_PRINT
endif

# Uncomment this for memory checking
# Do a make clean before building again
# TRACK_MEMORY = 1
ifeq ($(TRACK_MEMORY),1)
MMGR_OBJECT = mmgr.o
CFLAGS += -DTRACK_MEMORY
endif

ifeq ($(FULLWARNINGS),1)
CFLAGS += -W -Wstrict-prototypes -Wmissing-prototypes
CFLAGS += -Wmissing-declarations -Wmissing-noreturn  -Wmissing-format-attribute 
CFLAGS += -Wno-deprecated-declarations -Wnested-externs -Wredundant-decls
CFLAGS += -Wpacked -Wpadded -Winline -Wdisabled-optimization
CFLAGS += -Wfloat-equal -Wundef -Wshadow -Wpointer-arith 
CFLAGS += -Wbad-function-cast -Wcast-qual -Wcast-align
CFLAGS += -Wwrite-strings -Wsign-compare -Waggregate-return
#CFLAGS += -Wtraditional -Wconversion -Wunreachable-code
CC=gcc
endif

TESTOBJS = test_utils.o sliceencoder_test.o bitstream_test.o imagemanip_test.o \
		slicedecoder_test.o colortransform_test.o vframedecoder_test.o \
		vframeparser_test.o vframeencoder_test.o srd_test.o \
		vframeencodedecode_test.o sliceencodedecode_test.o scapprocessor_test.o \
		emulationprevention_test.o deemulation_test.o bitmap_test.o

TEST_OBJS = ${patsubst %.o, test/%.o, ${TESTOBJS}}
TEST_SRCS = $(TEST_OBJS:.o=.cpp)

SRCS = v2d.c \
       v2dtx.c \
       v2drx.c \
       srd.c \
       comp_utils.c \
       net_utils.c \
       rtp.c \
       splash.c \
       vframedecoder_c.c \
       vframeparser_c.c \
       svc.c \
       aligned_malloc.c \
       cvt.c \
       protocol_utils.c

CPPSRCS = bitstream.cpp \
          codec_tables.cpp \
          imagemanip.cpp \
          bitmap.cpp \
          slicedecoder.cpp \
          h264slicedecoder.cpp \
          svcslicedecoder.cpp \
          sliceencoder.cpp \
          standardsliceencoder.cpp \
          h264sliceencoder.cpp \
          svcsliceencoder.cpp \
          colortransform.cpp \
          vframedecoder.cpp \
          slicebuffer.cpp \
          rgbtoyuv.cpp \
          dct.cpp \
          png.cpp \
          vframeencoder.cpp \
          yuv4mpeg2writer.cpp \
          AudioFrameParser.cpp \
          AudioDecoder.cpp \
          WaveFile.cpp \
          ScapProcessor.cpp \
          vframeimager.cpp \
          cavlc.cpp \
          EmulationPrevention.cpp

MAIN_SRCS = tx.c relay.c 
MAIN_CPPSRCS = frame_creator.cpp slice_creator.cpp rx.cpp

V2DLIBOBJS = $(SRCS:.c=.o) $(CPPSRCS:.cpp=.o)

ifeq ($(MINGW),0)
all: libv2d.${LIBEXT} rx tx relay slice_creator frame_creator v2dLibTests
else
all: libv2d.${LIBEXT} slice_creator frame_creator v2dLibTests
endif

test: v2dLibTests
	export LD_LIBRARY_PATH=./:${GTESTLIBPATH} GTESTDATADIR=../avdata/; \
           ./v2dLibTests ${TESTPARAMS}

v2dLibTests: ${TEST_OBJS} libv2d.${LIBEXT}
	${QUIET_BIN} ${CC} -o $@ -L"./" ${TEST_OBJS} \
	    ${GTESTLIB} -lv2d ${STATICLIBS} ${LIBS}

clean: docsclean
	${QUIET_CLEAN} /bin/rm -f rx tx relay libv2d.${LIBEXT} libv2dcodec.${LIBEXT} \
	*.o *.d test/*.o test/*.d v2dLibTests slice_creator frame_creator *.exe

docsclean:
	${QUIET} /bin/rm -f docs/*.html docs/*.pdf docs/images/gen_*.png

# This target used for debugging using gdb when we need sources on the box
sourcedist:
	mkdir -p v2dlib_sources
	-cp *.c *.h v2dlib_sources

${TEST_OBJS}: %.o: %.cpp
	${QUIET_CC} ${CC} -c ${CFLAGS} -I . ${GTESTCFLAGS} -o $@ $< 

tx: tx.o libv2d.${LIBEXT}
	${QUIET_BIN} ${CC} -o $@  -L"./" -lv2d ${STATICLIBS} tx.o 
rx: rx.o pgetopt.o libv2d.${LIBEXT}
	${QUIET_BIN} ${CC} -o $@  -L"./" -lv2d ${STATICLIBS} rx.o pgetopt.o

slice_creator: slice_creator.o libv2d.${LIBEXT}
	${QUIET_BIN} ${CC} -o $@ slice_creator.o -L"./" -lv2d  ${STATICLIBS}

frame_creator: frame_creator.o pgetopt.o  libv2d.${LIBEXT}
	${QUIET_BIN} ${CC} -o $@ frame_creator.o pgetopt.o  -L"./" -lv2d ${STATICLIBS}

relay: relay.o libv2d.${LIBEXT}
	${QUIET_BIN} ${CC} -o $@ relay.o -L"./" -lv2d ${STATICLIBS} 

libv2d.${LIBEXT}: ${V2DLIBOBJS} ${MMGR_OBJECT}
ifeq ($(MINGW),0)
	${QUIET_LIB} ${CC} -shared -o $@ $^ ${LIBS} 
else
	${QUIET_LIB} ar cr $@ $^
endif

ALTCOPTS = -D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200112 -D_FILE_OFFSET_BITS=64 \
    -D_LARGEFILE_SOURCE -std=c99 -fomit-frame-pointer \
    -g -Wdeclaration-after-statement -Wall \
    -Wno-switch -Wdisabled-optimization -Wpointer-arith -Wredundant-decls \
    -Wno-pointer-sign -Wcast-qual -Wwrite-strings -Wundef \
    -O3 -fno-math-errno -fno-tree-vectorize -fno-strict-aliasing

fdct_assembly.o:fdct_assembly.c
	gcc -c ${ALTCOPTS} $< -o $@


%.o:%.c
	${QUIET_CC} ${CC} -c ${CFLAGS} $< -o $@

%.o:%.cpp
	${QUIET_CC} ${CC} -c ${CFLAGS} $< -o $@

DOCUMENTS = V2DLibAPI.txt \
            SliceReductionModule.txt \
            V2DCodecSpecificationV2.txt \
            CompilingV2DLib.txt \
            CompilingV2DLibraryMSVC.txt \
            V2DRxLibAPI.txt

DOC_SRCS = ${patsubst %.txt, docs/%.txt, ${DOCUMENTS}}
DOC_HTML = $(DOC_SRCS:.txt=.html)

${DOC_HTML}: %.html: %.txt
	${QUIET_DOC} asciidoc -a max-width=55em -a toc -a toclevels=3 $^
	${QUIET} a2x -a toc -a toclevels=3 -a docinfo --fop $^

docs: ${DOC_HTML}

ifdef DOCSINSTALLBASEDIR
DOCS_INSTALL_BASEDIR = ${DOCSINSTALLBASEDIR}
else
DOCS_INSTALL_BASEDIR = ${HOME}/public_html/docs
endif

DOCS_INSTALL_DIR = ${DOCS_INSTALL_BASEDIR}/v2dlib

docsinstall: docs
	${QUIET} echo "INSTALLDOCS"
	${QUIET} mkdir -p ${DOCS_INSTALL_DIR} ${DOCS_INSTALL_DIR}/images
	${QUIET} /bin/cp -f docs/*.txt docs/*.pdf docs/*.html ${DOCS_INSTALL_DIR}
	${QUIET} for file in `/bin/ls docs/images/*.png 2>/dev/null`; \
		do /bin/cp  $${file} ${DOCS_INSTALL_DIR}/images; done

# Create dependencies files
%.d: %.c
	${QUIET_DEP} set -e; $(CC) -MM $(CFLAGS) $< | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	   [ -s $@ ] || rm -f $@

%.d: %.cpp
	${QUIET_DEP} set -e; $(CC) -MM $(CFLAGS) $< | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	   [ -s $@ ] || rm -f $@

$(TEST_SRCS:.cpp=.d): %.d: %.cpp
	${QUIET_DEP} set -e; $(CC) -MM -MT $@ -MT ${patsubst %.d, %.o, $@} $(CFLAGS) $<  -I . ${GTESTCFLAGS} -o $@; \
	   [ -s $@ ] || rm -f $@

.PHONY: sourcedist test clean docs docsinstall docsclean

ifneq ($(MAKECMDGOALS),clean)
-include $(SRCS:.c=.d)
-include $(CPPSRCS:.cpp=.d)
-include $(MAIN_SRCS:.c=.d)
-include $(MAIN_CPPSRCS:.cpp=.d)
-include $(TEST_SRCS:.cpp=.d)
-include pgetopt.d 
endif

