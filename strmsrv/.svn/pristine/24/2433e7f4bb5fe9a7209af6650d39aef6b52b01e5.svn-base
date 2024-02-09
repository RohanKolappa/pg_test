

ifneq ($(VERBOSE),1)
QUIET_CC = @echo "CC $@";
QUIET_DEP = @echo "DEP $@";
QUIET_BIN = @echo "BIN $@";
QUIET_LIB = @echo "LIB $@";
QUIET_DIST = @echo "DIST";
QUIET_CLEAN = @echo "CLEAN";
QUIET_DOC = @echo "DOC $@";
QUIET = @ 
endif

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
DEV_DIR=dev/dev_arm
else
DEV_DIR=dev/dev_i386
endif

# Test for machine type
CC = $(CROSS_COMPILE)g++
CCSYSTEM = $(shell $(CC) -dumpmachine)
MINGW = 0
LIBEXT = so
ifeq ($(findstring mingw32, $(CCSYSTEM)), mingw32)
MINGW = 1
LIBEXT = a
endif

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
CFLAGS += -D__XPI__
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

USE_IPP_LIB=0
ifeq ($(USE_IPP_LIB),1)
IPPDIR = /home/rkale/intel/ipp/6.1.2.051/ia32
IPPCFLAGS = -I ${IPPDIR}/include
IPPLIBPATH = ${IPPDIR}/lib
IPPLIBS = -L ${IPPLIBPATH} -lippvcemerged -lippvcmerged \
	-lippjemerged -lippjmerged \
	-lippiemerged -lippimerged \
	-lippsemerged -lippsmerged \
	-lippccemerged -lippccmerged \
	-lippcore
endif

V2ODIR = ../V2O/modules
V2ODRVDIR = ../V2O/modules/drivers
V2OCFLAGS =  -I ${V2ODIR} -I ${V2ODRVDIR}
V2OLIBPATH = ${V2ODIR}
V2ODRVLIBPATH = ${V2ODRVDIR}
V2OLIBS = -L ${V2OLIBPATH} -L ${V2ODRVLIBPATH} -lv2o -lv2okernel
ifneq (,$(findstring arm,$(CROSS_COMPILE)))
V2OLIBS += -L../${DEV_DIR}/libusb/lib -lusb-1.0 
else
V2OLIBS += -lusb
endif

V2DLIBDIR = ../v2dlib
V2DLIBCFLAGS = -I ${V2DLIBDIR} 
V2DLIBLIBPATH = ${V2DLIBDIR}
V2DLIBLIBS = -L ${V2DLIBLIBPATH} -lv2d   
ifeq ($(MINGW),0)
V2DLIBLIBS += -L../${DEV_DIR}/libpng/lib -L../${DEV_DIR}/jpeg/lib -Wl,-Bstatic -lpng -ljpeg -lz -Wl,-Bdynamic
endif

RFBDIR = ../rfbproto
RFBCFLAGS = -I ${RFBDIR}/
RFBLIBPATH = ${RFBDIR}
RFBLIBS = -L ${RFBLIBPATH} -lrfbproto

MEDIAINDEXDIR = ../mediaindex
MEDIAINDEXCFLAGS = -I ${MEDIAINDEXDIR}/
MEDIAINDEXLIBPATH = ${MEDIAINDEXDIR}
MEDIAINDEXLIBS = -L ${MEDIAINDEXLIBPATH}/ -lmediaindex

BOOSTDIR = ../${DEV_DIR}/boost
BOOSTINCDIR = ${BOOSTDIR}/include
BOOSTLIBPATH = ${BOOSTDIR}/lib
ifeq ($(MINGW),1)
BOOSTLIBS = -L ${BOOSTLIBPATH}/ -lboost_filesystem -lboost_system
else
ifneq (,$(findstring arm,$(CROSS_COMPILE)))
BOOSTLIBS = -L ${BOOSTLIBPATH}/ -lboost_filesystem -lboost_system
else
BOOSTLIBS = -lboost_filesystem 
endif
endif

GTESTDIR = ../${DEV_DIR}/gmock
GTESTCFLAGS = -I ${GTESTDIR}/include
GTESTLIB = ${GTESTDIR}/lib/gmock_main.a ${GTESTDIR}/lib/gtest_utils.a -lboost_filesystem
ifeq ($(MINGW),1)
GTESTLIB += -L ../${DEV_DIR}/boost/lib -lboost_system
endif

XERCESDIR = ../${DEV_DIR}/xerces
XERCESCFLAGS = -I ${XERCESDIR}/include
XERCESLIBPATH = ${XERCESDIR}/lib
XERCESLIBS = -L ${XERCESLIBPATH} -lxerces-c

MATROSKADIR = ../${DEV_DIR}/matroska
MATROSKAINCDIR = ${MATROSKADIR}/include
MATROSKALIBPATH = ${MATROSKADIR}/lib
MATROSKALIBS = -L ${MATROSKALIBPATH} -lmatroska -lebml

ASOUNDLIBPATH = ../${DEV_DIR}/alsa-lib/lib
ASOUNDLIBLIBS = -L ${ASOUNDLIBPATH} -lasound

LOG4CDIR = ../${DEV_DIR}/log4c
LOG4CCFLAGS = -I ${LOG4CDIR}/include
LOG4CLIBPATH = ${LOG4CDIR}/lib
LOG4CLIBS = -L ${LOG4CLIBPATH}  -llog4c

LIBXKBCOMMONDIR = ../${DEV_DIR}/libxkbcommon
LIBXKBCOMMONINCDIR = ${LIBXKBCOMMONDIR}/include
LIBXKBCOMMON_INCLUDE = -I ${LIBXKBCOMMONINCDIR}
LIBLIBXKBCOMMONDIR = ${LIBXKBCOMMONDIR}/lib
LIBXKBCOMMONLIBS = -L ${LIBLIBXKBCOMMONDIR} -lxkbcommon


#BLMDIR = ../${DEV_DIR}/BLM
#BLMINCDIR = ${BLMDIR}/
#BLM_INCLUDE = -I ${BLMINCDIR}
#LIBBLMDIR = ${BLMDIR}/lib
#BLMLIBS = -L ${LIBBLMDIR} -lblm

LIVEMEDIADIR = ../${DEV_DIR}/live
LIVEMEDIACFLAGS = -I ${LIVEMEDIADIR}/UsageEnvironment/include \
                   -I ${LIVEMEDIADIR}/groupsock/include \
                   -I ${LIVEMEDIADIR}/liveMedia/include \
                   -I ${LIVEMEDIADIR}/BasicUsageEnvironment/include

LIVEMEDIALIBS =    ${LIVEMEDIADIR}/liveMedia/libliveMedia.a \
                   ${LIVEMEDIADIR}/groupsock/libgroupsock.a \
                   ${LIVEMEDIADIR}/BasicUsageEnvironment/libBasicUsageEnvironment.a \
                   ${LIVEMEDIADIR}/UsageEnvironment/libUsageEnvironment.a

FFMPEGDIR = ../${DEV_DIR}/ffmpeg
FFMPEGCFLAGS = -I ${FFMPEGDIR}/include
FFMPEGLIBPATH = ${FFMPEGDIR}/lib
SPEEXDIR = ../${DEV_DIR}/speex
ifeq ($(MINGW),0)
#FFMPEGLIBS = -L ${FFMPEGDIR}/lib -lavformat -lavcodec -lavutil -lswscale -lz -lbz2
FFMPEGLIBS = -L ${FFMPEGDIR}/lib -lavformat -lavcodec -lavutil -lswscale -lz -L../${DEV_DIR}/libbz2/lib -lbz2 
else
FFMPEGLIBS = -L ${FFMPEGDIR}/lib -lavformat -lavcodec -lavutil -lswscale -lavdevice -lavfilter -L ${SPEEXDIR}/lib -lspeex -lavifil32
endif

PORTAUDIODIR = ../${DEV_DIR}/portaudio
PORTAUDIOCFLAGS = -I ${PORTAUDIODIR}/include
PORTAUDIOLIBPATH = ${PORTAUDIODIR}/lib
PORTAUDIOLIBS = -L ${PORTAUDIODIR}/lib -lportaudio

SAMPLERATEDIR = ../${DEV_DIR}/libsamplerate
SAMPLERATECFLAGS = -I ${SAMPLERATEDIR}/include
SAMPLERATELIBPATH = ${SAMPLERATEDIR}/lib
SAMPLERATELIBFILE = ${SAMPLERATELIBPATH}/libsamplerate.a
SAMPLERATELIBS = -L ${SAMPLERATEDIR}/lib -lsamplerate

GDDIR = ../${DEV_DIR}/gd
GDCFLAGS = -I ${GDDIR}/include
GDLIBPATH = ${GDDIR}/lib
GDLIBS = -L ${GDDIR}/lib -lgd

JPEGDIR = ../${DEV_DIR}/jpeg
JPEGCFLAGS = -I ${JPEGDIR}/include
JPEGLIBPATH = ${JPEGDIR}/lib
JPEGLIBS = -L ${JPEGDIR}/lib -ljpeg

CURLDIR = ../${DEV_DIR}/curl
CURLCFLAGS = -I ${CURLDIR}/include
CURLLIBPATH = ${CURLDIR}/lib
ifneq ($(MINGW),1)
CURLLIBS = -L ${CURLDIR}/lib -lcurl
else
CURLLIBS = -L ${CURLDIR}/lib -lcurldll
endif

PNGDIR = ../${DEV_DIR}/png
PNGCFLAGS = -I ${PNGDIR}/include

ifeq ($(MINGW),1)
UUIDLIBS = -lrpcrt4
else
UUIDLIBS = -L../${DEV_DIR}/uuid/lib -luuid
endif


# -----------------------------------------------------------------------------

VMP_DIR		:=	$(CURDIR)/../VMP_DMS
HLS_CFLAGS	:=	-I${VMP_DIR}/VMP_DMS/Src/
HLS_LIBS	:=	-L${VMP_DIR}/BFC_Base64/ -lBFC_Base64	\
			-L${VMP_DIR}/BFC_DL/ -lBFC_DL	\
			-L${VMP_DIR}/BFC_Dyn/ -lBFC_Dyn	\
			-L${VMP_DIR}/BFC_IO/ -lBFC_IO	\
			-L${VMP_DIR}/BFC_Image/ -lBFC_Image	\
			-L${VMP_DIR}/BFC_Light/ -lBFC_Light	\
			-L${VMP_DIR}/BFC_LightRCom -lBFC_LightRCom	\
			-L${VMP_DIR}/BFC_MT/ -lBFC_MT	\
			-L${VMP_DIR}/BFC_Msg/ -lBFC_Msg	\
			-L${VMP_DIR}/BFC_Net/ -lBFC_Net	\
			-L${VMP_DIR}/BFC_Net_Log/ -lBFC_Net_Log	\
			-L${VMP_DIR}/BFC_Plugins/ -lBFC_Plugins	\
			-L${VMP_DIR}/BFC_RegEx/ -lBFC_RegEx	\
			-L${VMP_DIR}/BFC_Sound/ -lBFC_Sound	\
			-L${VMP_DIR}/BFC_VFS/ -lBFC_VFS	\
			-L${VMP_DIR}/BFC_XML/ -lBFC_XML	\
			-L${VMP_DIR}/BFC_DOM/ -lBFC_DOM	\
			-L${VMP_DIR}/Sys_DL/ -lSys_DL	\
			-L${VMP_DIR}/Sys_MT/ -lSys_MT	\
			-L${VMP_DIR}/VMP_DMS/ -lVMP_DMS	\
			-L${VMP_DIR}/VMP/ -lVMP	\
			-L${VMP_DIR}/VMP_GPC/ -lVMP_GPC	\
			-L${VMP_DIR}/VMP_L2/ -lVMP_L2	\
			-L${VMP_DIR}/VMP_L3/ -lVMP_L3	\
			-L${VMP_DIR}/VMP_UI/ -lVMP_UI	\
			-L${VMP_DIR}/VMP_AAC/ -lVMP_AAC	\
			-L${VMP_DIR}/VMP_M2S/ -lVMP_M2S	\
			-L${VMP_DIR}/VMP_M4S/ -lVMP_M4S	\
			-L${VMP_DIR}/VMP_AVC/ -lVMP_AVC	\
			-L${VMP_DIR}/VMP_HLS/ -lVMP_HLS	\
			-L${VMP_DIR}/VMP_RTP/ -lVMP_RTP	\
			-L${VMP_DIR}/VMP_SegUtils/ -lVMP_SegUtils	\
			-Wl,-E	\
			-Wl,-rpath,${VMP_DIR}/BFC_Base64/	\
			-Wl,-rpath,${VMP_DIR}/BFC_DL/	\
			-Wl,-rpath,${VMP_DIR}/BFC_Dyn/	\
			-Wl,-rpath,${VMP_DIR}/BFC_IO/	\
			-Wl,-rpath,${VMP_DIR}/BFC_Image/	\
			-Wl,-rpath,${VMP_DIR}/BFC_Light/	\
			-Wl,-rpath,${VMP_DIR}/BFC_LightRCom/	\
			-Wl,-rpath,${VMP_DIR}/BFC_MT/	\
			-Wl,-rpath,${VMP_DIR}/BFC_Msg/	\
			-Wl,-rpath,${VMP_DIR}/BFC_Net/	\
			-Wl,-rpath,${VMP_DIR}/BFC_Net_Log/	\
			-Wl,-rpath,${VMP_DIR}/BFC_Plugins/	\
			-Wl,-rpath,${VMP_DIR}/BFC_RegEx/	\
			-Wl,-rpath,${VMP_DIR}/BFC_Sound/	\
			-Wl,-rpath,${VMP_DIR}/BFC_VFS/	\
			-Wl,-rpath,${VMP_DIR}/BFC_XML/	\
			-Wl,-rpath,${VMP_DIR}/BFC_DOM/	\
			-Wl,-rpath,${VMP_DIR}/Sys_DL/	\
			-Wl,-rpath,${VMP_DIR}/Sys_MT/	\
			-Wl,-rpath,${VMP_DIR}/VMP_DMS/	\
			-Wl,-rpath,${VMP_DIR}/VMP/	\
			-Wl,-rpath,${VMP_DIR}/VMP_GPC/	\
			-Wl,-rpath,${VMP_DIR}/VMP_L2/	\
			-Wl,-rpath,${VMP_DIR}/VMP_L3/	\
			-Wl,-rpath,${VMP_DIR}/VMP_UI/	\
			-Wl,-rpath,${VMP_DIR}/VMP_AAC/	\
			-Wl,-rpath,${VMP_DIR}/VMP_M2S/	\
			-Wl,-rpath,${VMP_DIR}/VMP_M4S/	\
			-Wl,-rpath,${VMP_DIR}/VMP_AVC/	\
			-Wl,-rpath,${VMP_DIR}/VMP_HLS/	\
			-Wl,-rpath,${VMP_DIR}/VMP_RTP/	\
			-Wl,-rpath,${VMP_DIR}/VMP_SegUtils/

# -----------------------------------------------------------------------------

SHAREDLIBS = ${V2DLIBDIR}/libv2d.${LIBEXT} ${MEDIAINDEXDIR}/libmediaindex.${LIBEXT} \
              ${RFBDIR}/librfbproto.${LIBEXT} 
ifneq ($(MINGW),1)
SHAREDLIBS += ${V2ODIR}/libv2o.${LIBEXT} ${V2ODRVDIR}/libv2okernel.${LIBEXT}
endif

CFLAGS = -Wall -Wno-deprecated-declarations -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
LIBS += ${V2DLIBLIBS} ${RFBLIBS} ${MEDIAINDEXLIBS} ${BOOSTLIBS} ${XERCESLIBS} \
      ${LIVEMEDIALIBS} ${FFMPEGLIBS} ${PORTAUDIOLIBS} ${SAMPLERATELIBS} \
      ${MATROSKALIBS} ${UUIDLIBS} ${ASOUNDLIBLIBS}
else
LIBS += ${V2DLIBLIBS} ${RFBLIBS} ${MEDIAINDEXLIBS} ${BOOSTLIBS} ${XERCESLIBS} \
      ${LIVEMEDIALIBS} ${FFMPEGLIBS} ${PORTAUDIOLIBS} ${SAMPLERATELIBS} \
      ${MATROSKALIBS} ${UUIDLIBS}
endif

ifeq ($(MINGW),0)
LIBS += -lrt -L../${DEV_DIR}/freetype/lib -lfreetype -L../${DEV_DIR}/fontconfig/lib -lfontconfig -Wl,-Bstatic -lgd -Wl,-Bdynamic -lcurl

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
LIBS += ${V2OLIBS} ${IPPLIBS} ${LOG4CLIBS} ${CURLLIBS} ${LIBXKBCOMMONLIBS} -L../${DEV_DIR}/png/lib -lpng ${GDLIBS} ${JPEGLIBS} 
else
LIBS += ${V2OLIBS} ${IPPLIBS} ${LOG4CLIBS} ${LIBXKBCOMMONLIBS} 
endif
else
LIBS += -lpthread 
LIBS += ${CURLLIBS}
endif
ifeq ($(MINGW),1)
CFLAGS += -DXML_LIBRARY
endif

ifeq ($(MINGW),1)
LIBS += -lws2_32 -lwsock32 -lgdi32 -lwinmm
endif

ifneq ($(MINGW),1)
FFCREATORLIBS = -lpthread -lrt
else
FFCREATORLIBS = -lpthread -lws2_32 -lwsock32 
endif

STRMSRV_LIBS = ${LIBS} ${HLS_LIBS}

STRMSRV_DEVICE_LIBS = ${LIBS}

INCLUDECFLAGS = ${V2OCFLAGS} ${V2DLIBCFLAGS} ${IPPCFLAGS} ${CODECCFLAGS} \
	${RFBCFLAGS} ${MEDIAINDEXCFLAGS} ${XERCESCFLAGS} ${LOG4CCFLAGS} \
	${LIVEMEDIACFLAGS} ${FFMPEGCFLAGS} ${PORTAUDIOCFLAGS} \
	${SAMPLERATECFLAGS} ${GDCFLAGS} ${CURLCFLAGS} ${PNGCFLAGS} ${HLS_CFLAGS}

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
INCLUDECFLAGS += ${JPEGCFLAGS} 
endif

ifeq (,$(findstring arm,$(CROSS_COMPILE)))
#INCLUDECFLAGS += ${BLM_INCLUDE}
endif


ifeq ($(MINGW),1)
INCLUDECFLAGS += ${CURLCFLAGS}
endif

ifneq ($(MINGW),1)
CFLAGS += -DHAVE_V2O
endif

DEBUG=0
ifeq ($(DEBUG),1)
ifeq ($(COVERAGE),1)
CFLAGS += -g -DSSDEBUG=1 -fprofile-arcs -ftest-coverage
else
CFLAGS += -g -DSSDEBUG=1 -O2
endif
else
CFLAGS += -g -O2
endif

ifeq ($(DEBUGSTDOUT),1)
CFLAGS += -DLOG_TO_STDOUT=1
endif

CFLAGS += ${INCLUDECFLAGS}


STRMSRV_CFLAGS = ${CFLAGS} -DHAVE_DISKIO=1

STRMSRV_DEVICE_CFLAGS = ${CFLAGS}

THREADOBJS = appthread.o \
             streamerthr.o \
             writerthr.o \
             rcvrthr.o \
             ctrlthr.o \
             senderthr.o \
             readerthr.o \
             diskiothr.o \
             writerdiskiothr.o \
             writerdiskioffthread.o \
             readerdiskiothr.o \
             readerquerythr.o \
             vncencoderthr.o \
             processorthr.o \
             v2ddecoderthr.o \
             rtpvideodecoderthr.o \
             rtpaudiodecoderthr.o 

ifeq ($(MINGW),0)
THREADOBJS += \
             deleterdiskiothr.o
endif

QUEUEOBJS = utils.o \
            address.o \
            socket.o \
            mutex.o \
            condition.o \
            squeue.o \
            sframe.o \
            sbuffer.o \
            rtputils.o


DBOBJS = command.o \
         perf.o \
         timekeeper.o \
         streamer.o \
         shaper.o \
         ratecontroller.o \
         meter.o \
         notify.o

RCVROBJS = \
           receiver.o \
           v2drx.o \
           rtprx.o \
           udprx.o \
           RTPSession.o \
           RTPFrameFilter.o \
           RTPFrameUtils.o \
           scheduler.o \
           mpegtsrx.o \
           MPEGTSMuxFilter.o \
           rfbhandler.o \
           fbrx.o \
           vncrx.o

ifeq ($(MINGW),0)
RCVROBJS += \
           fpgarx.o \
           fpgaaudiorx.o 
else
RCVROBJS += \
           winfbrx.o 
endif


SENDEROBJS = sender.o \
             v2dtx.o \
             mpegtstx.o \
             rtptx.o \
             udptx.o \
             v2dff.o \
             v2dplayertx.o \
             rtpplayertx.o

ifeq ($(MINGW),0)
SENDEROBJS += \
             fpgatx.o \
             fpgaaudiotx.o
endif

WRITEROBJS = writer.o imagewriter.o

ifeq ($(MINGW),0)
WRITEROBJS += \
             deleter.o
endif

READEROBJS = reader.o

UTILOBJS = V2DUtils.o \
           portuse.o \
           averager.o \
           cpustats.o \
           MulticastUtils.o \
           FFmpegUtils.o \
           URLFetcher.o \
           RTPParser.o \
           H264VideoRTPParser.o \
           MPEG4AudioRTPParser.o \
           MPEG4VideoRTPParser.o \
           MPEG4LATMAudioRTPParser.o \
           AudioRenderer.o \
           pa_ringbuffer.o \
           GenerateUniqueID.o 

ifeq ($(MINGW),0)
UTILOBJS += \
           splash.o \
           vsplashframe.o \
           netutils.o \
           hidutils.o \
           KbmIPCUtils.o \
		   USBHid.o \
		   USB.o \
		   RawToUsb.o \
           RFBToUSB.o
endif

ifeq ($(MINGW),1)
UTILOBJS += \
           winscreenscraper.o \
           wineventhandler.o \
           keymap.o 
endif

FFPROCESSOROBJS = FFMediaProcessor.o FFTrackWriter.o FFCodec.o FFV2d.o FFSnapshot.o FFRtp.o FFMpegts.o FFV2dTrack.o FFUtils.o ThumbnailGenerator.o CodecHandler.o \
					ThumbnailHandler.o RTPMediaHandler.o SnapshotHandler.o MpegtsMediaHandler.o StandardFormatHandler.o PTSGenerator.o averager.o CheckPTS.o \
					MPEGTSPktParser.o TimeStampAdjuster.o AdaptiveMediaGenerator.o DataDropper.o FFMediaProcessorErrorHandler.o

MAINOBJS = strmsrv.o

EXTERNALOBJS = streamingserver.o

TESTOBJS = queue_test.o \
           command_test.o \
           thread_test.o \
           v2dutils_test.o \
           v2dencoderthread_test.o \
           ff_test.o \
           ffutils_test.o \
           idprovider_test.o \
           pts_test.o \
           tsadjuster_test.o \
           mpegts_parser_test.o \
           hls_test.o

TEST_OBJS = ${patsubst %.o, test/%.o, ${TESTOBJS}}
ifneq (,$(findstring arm,$(CROSS_COMPILE)))
CODEOBJS = ${QUEUEOBJS} ${DBOBJS} ${RCVROBJS} ${SENDEROBJS} ${WRITEROBJS} \
		   ${READEROBJS} ${UTILOBJS} ${EXTERNALOBJS} ${FFPROCESSOROBJS}  
else
CODEOBJS = ${QUEUEOBJS} ${DBOBJS} ${RCVROBJS} ${SENDEROBJS} ${WRITEROBJS} \
		   ${READEROBJS} ${UTILOBJS} ${EXTERNALOBJS} ${FFPROCESSOROBJS} ${THREADOBJS}
endif

STRMSRVOBJS = ${CODEOBJS} ${MAINOBJS}
FFCREATOROBJS = ffcreator.o ${FFPROCESSOROBJS} ${QUEUEOBJS} ratecontroller.o timekeeper.o FFmpegUtils.o \
							MPEG4VideoRTPParser.o H264VideoRTPParser.o RTPParser.o appthread.o \
							MPEG4AudioRTPParser.o MPEG4LATMAudioRTPParser.o URLFetcher.o

STRMSRVXPIOBJS = $(filter-out writer.o reader.o AdaptiveMediaGenerator.o, $(STRMSRVOBJS)) 

THREADOBJSFORDEVICE = $(filter-out writerthr.o writerdiskiothr.o writerdiskioffthread.o readerthr.o readerdiskiothr.o, $(THREADOBJS)) writerthrDevice.o readerthrDevice.o

STRMSRVXPIOBJS += writerthrDevice.o readerthrDevice.o

STRMSRVDEVICEOBJS = $(filter-out writerthr.o writer.o writerdiskiothr.o writerdiskioffthread.o readerthr.o reader.o readerdiskiothr.o AdaptiveMediaGenerator.o, $(STRMSRVOBJS)) writerthrDevice.o readerthrDevice.o

FFCREATOROBJSFORDEVICE = $(filter-out AdaptiveMediaGenerator.o, $(FFCREATOROBJS))

ifneq ($(MINGW),1)
SRCS = $(STRMSRVOBJS:.o=.cpp) $(FFCREATOROBJS:.o=.cpp) ThreadTest.cpp
TEST_SRCS = $(TEST_OBJS:.o=.cpp)
endif


ifeq ($(PROFILE),1)
   CFLAGS += -pg
   LFLAGS += -pg
endif

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
ifeq ($(MINGW),0)
all: StreamingServerDevice FFCreator libthreadobjs.${LIBEXT} libstrmsrv.${LIBEXT}
else
all: StreamingServer StreamingServerTests FFCreator ThreadTest ScreenTest libstrmsrv.${LIBEXT}
endif
else
ifeq ($(MINGW),0)
all: StreamingServer StreamingServerDevice FFCreator libstrmsrv.${LIBEXT} StreamingServerTests ThreadTest FFmpegTest AudioTest
else
all: StreamingServerDevice libstrmsrv.${LIBEXT} 
endif
endif

ifeq ($(MINGW),0)
    CFLAGS += -rdynamic
endif

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
%.o:%.cpp
	${QUIET_CC} ${CC} -c ${STRMSRV_DEVICE_CFLAGS} $< -o $@
else
ifeq ($(MINGW), 0)
%.o:%.cpp
	${QUIET_CC} ${CC} -c ${STRMSRV_CFLAGS} $< -o $@
else
%.o:%.cpp
	${QUIET_CC} ${CC} -c ${STRMSRV_DEVICE_CFLAGS} $< -o $@
endif
endif

writerthrDevice.o:writerthr.cpp
	${QUIET_CC} ${CC} -c ${STRMSRV_DEVICE_CFLAGS} $< -o $@

readerthrDevice.o:readerthr.cpp
	${QUIET_CC} ${CC} -c ${STRMSRV_DEVICE_CFLAGS} $< -o $@


ifneq (,$(findstring arm,$(CROSS_COMPILE)))
StreamingServerDevice: libthreadobjs.${LIBEXT} ${STRMSRVXPIOBJS} ${SHAREDLIBS}
	${QUIET_BIN} ${CC} ${LFLAGS} ${STRMSRVXPIOBJS} ${STRMSRV_DEVICE_LIBS} -L. -lthreadobjs -o ${@}
else
StreamingServerDevice:	${STRMSRVDEVICEOBJS} ${SHAREDLIBS}
	${QUIET_BIN} ${CC} ${LFLAGS} ${STRMSRVDEVICEOBJS} ${STRMSRV_DEVICE_LIBS} -o ${@}
endif

ifneq ($(MINGW),1)
StreamingServer:  ${STRMSRVOBJS} ${SHAREDLIBS}
	${QUIET_BIN} ${CC} ${LFLAGS} ${STRMSRVOBJS} ${STRMSRV_LIBS} -o ${@}
endif

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
FFCreator: ${FFCREATOROBJSFORDEVICE} ${SHAREDLIBS}
	${QUIET_BIN} ${CC}  ${LFLAGS} ${FFCREATOROBJSFORDEVICE} ${STRMSRV_DEVICE_LIBS} ${FFCREATORLIBS}  -o ${@}
else
FFCreator: ${FFCREATOROBJS} ${SHAREDLIBS}
	${QUIET_BIN} ${CC}  ${LFLAGS} ${FFCREATOROBJS} ${STRMSRV_LIBS} ${FFCREATORLIBS}  -o ${@}
endif

ifeq ($(MINGW),0)
libstrmsrv.${LIBEXT}: ${CODEOBJS} ${SHAREDLIBS}
else
libstrmsrv.${LIBEXT}: ${STRMSRVDEVICEOBJS} ${SHAREDLIBS}
endif

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
ifeq ($(MINGW),0)
	${QUIET_LIB} ${CC} -shared -o $@ ${CODEOBJS} ${STRMSRV_DEVICE_LIBS}  -Wl,-rpath,'$$ORIGIN'
else
	${QUIET_LIB} ar cr $@ ${CODEOBJS} ${LIVEMEDIALIBS} ; \
	ar -x ${SAMPLERATELIBFILE} ; \
	ar -rs $@ samplerate.o src_sinc.o src_zoh.o src_linear.o 
endif
else
ifeq ($(MINGW),0)
	${QUIET_LIB} ${CC} -shared -o $@ ${CODEOBJS} ${STRMSRV_LIBS}  -Wl,-rpath,'$$ORIGIN'
else
	${QUIET_LIB} ar cr $@ ${STRMSRVDEVICEOBJS} ${SHAREDLIBS} ; \
	ar -x ${SAMPLERATELIBFILE} ; \
	ar -rs $@ samplerate.o src_sinc.o src_zoh.o src_linear.o
endif
endif

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
libthreadobjs.${LIBEXT}: ${THREADOBJSFORDEVICE} ${SHAREDLIBS}
	${QUIET_LIB} ${CC} -shared -o $@ ${THREADOBJSFORDEVICE} ${STRMSRV_DEVICE_LIBS} -Wl,-rpath,'$$ORIGIN'
endif

ifneq (,$(findstring arm,$(CROSS_COMPILE)))	
ThreadTest: ThreadTest.o ${CODEOBJS} libthreadobjs.${LIBEXT}
	${QUIET_BIN} ${CC} ${LFLAGS} $^ -o ${@} ${STRMSRV_DEVICE_LIBS} -lpthread -L. -lthreadobjs
else	
ThreadTest: ThreadTest.o ${CODEOBJS}
	${QUIET_BIN} ${CC} ${LFLAGS} $^ -o ${@} ${STRMSRV_LIBS}
endif


ScreenTest: ScreenTest.o ${CODEOBJS}
	${QUIET_BIN} ${CC} ${LFLAGS} $^ -o ${@} ${STRMSRV_LIBS} 

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
FFmpegTest: FFmpegTest.o ${CODEOBJS} libthreadobjs.${LIBEXT}
	${QUIET_BIN} ${CC} ${LFLAGS} -o ${@} ${CODEOBJS} FFmpegTest.o ${LIBS} -lpthread -L. -lthreadobjs
else
FFmpegTest: FFmpegTest.o ${CODEOBJS}
	${QUIET_BIN} ${CC} ${LFLAGS} -o ${@} ${CODEOBJS} FFmpegTest.o ${STRMSRV_LIBS} -lpthread
endif	


GSTREAMER_ROOT = ../gstreamer
GLIB_ROOT = ../glib
GSTREAMER_CFLAGS = -I ${GLIB_ROOT}/include/glib-2.0 \
                   -I ${GLIB_ROOT}/lib/glib-2.0/include \
                   -I ${GSTREAMER_ROOT}/include/gstreamer-0.10 \
                   -I /usr/include/libxml2
GSTREAMER_LIB = -L ${GLIB_ROOT}/lib \
                -L ${GSTREAMER_ROOT}/lib \
				-lgstreamer-0.10 \
				-lgstsdp-0.10 \
				-lgobject-2.0 \
				-lgmodule-2.0 

GStreamerTest: GStreamerTest.cpp
	${QUIET_BIN} ${CC} -o $@ ${CFLAGS} ${GSTREAMER_CFLAGS} GStreamerTest.cpp ${GSTREAMER_LIB}

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
AudioTest: AudioTest.cpp ${CODEOBJS} libthreadobjs.${LIBEXT}
	${QUIET_BIN} ${CC} -o ${@}  ${CFLAGS} AudioTest.cpp ${CODEOBJS} ${LIBS} -L. -lthreadobjs
else
AudioTest: AudioTest.cpp ${CODEOBJS}
	${QUIET_BIN} ${CC} -o ${@}  ${CFLAGS} AudioTest.cpp ${CODEOBJS} ${STRMSRV_LIBS}
endif

test: StreamingServerTests
	 export LD_LIBRARY_PATH=./:${XERCESLIBPATH}:${V2DLIBLIBPATH}:${MEDIAINDEXLIBPATH}:${RFBLIBPATH}:${V2OLIBPATH}:${V2ODRVLIBPATH}:${FFMPEGLIBPATH}:${LOG4CLIBPATH}:${PORTAUDIOLIBPATH}:${MATROSKALIBPATH}:${LIBLIBXKBCOMMONDIR}; ./StreamingServerTests ${TESTPARAMS}


# This target used for debugging using gdb when we need sources on the box
sourcedist:
	mkdir -p strmsrv_sources
	cp *.cpp *.hh strmsrv_sources

${TEST_OBJS}: %.o: %.cpp
	${QUIET_CC} ${CC} -c ${STRMSRV_CFLAGS} -I . ${GTESTCFLAGS} -o $@ $< 
ifneq (,$(findstring arm,$(CROSS_COMPILE)))
StreamingServerTests: libthreadobjs.${LIBEXT} ${TEST_OBJS} ${CODEOBJS}
	${QUIET_BIN} ${CC} ${LFLAGS} ${TEST_OBJS} ${CODEOBJS} ${LIBS} ${GTESTLIB} -lpthread -L. -lthreadobjs -o ${@}
else
StreamingServerTests: ${TEST_OBJS} ${CODEOBJS}
	${QUIET_BIN} ${CC} -o ${@} ${TEST_OBJS} ${CODEOBJS} ${STRMSRV_LIBS} ${GTESTLIB} ${LFLAGS} 
endif	
clean: docsclean
	${QUIET_CLEAN} ${RM} *.o *.so *.bb *.bbg StreamingServer \
	*.d test/*.o test/*.d StreamingServerTests ThreadTest FFmpegTest AudioTest

docsclean:
	${QUIET} /bin/rm -f docs/*.html docs/*.pdf docs/images/gen_*.png

lint:
	cppcheck -D__GNUC__=4 -D__GNUC_MINOR__=1 ${INCLUDECFLAGS} ${GTESTCFLAGS} --suppress="*:../gmock/include/*" --enable=all -q .


DOCUMENTS = StreamingServer.txt \
            BuildingStreamingServer.txt

DOC_SRCS = ${patsubst %.txt, docs/%.txt, ${DOCUMENTS}}
DOC_HTML = $(DOC_SRCS:.txt=.html)

${DOC_HTML}: %.html: %.txt
	${QUIET_DOC} asciidoc -a max-width=55em -a toc -a toclevels=3 $^
	${QUIET} a2x -a toc -a toclevels=3 --fop $^

docs: ${DOC_HTML}

ifdef DOCSINSTALLBASEDIR
DOCS_INSTALL_BASEDIR = ${DOCSINSTALLBASEDIR}
else
DOCS_INSTALL_BASEDIR = ${HOME}/public_html/docs
endif

DOCS_INSTALL_DIR = ${DOCS_INSTALL_BASEDIR}/strmsrv

docsinstall: docs
	${QUIET} echo "INSTALLDOCS"
	${QUIET} mkdir -p ${DOCS_INSTALL_DIR} ${DOCS_INSTALL_DIR}/images
	${QUIET} /bin/cp -f docs/*.txt docs/*.pdf docs/*.html ${DOCS_INSTALL_DIR}
	${QUIET} for file in `/bin/ls docs/images/*.png 2>/dev/null`; \
		do /bin/cp  $${file} ${DOCS_INSTALL_DIR}/images; done

doxygen:
	doxygen Doxyfile

# Create dependencies files
%.d: %.cpp
	${QUIET_DEP} set -e; $(CC) -MM -MT $@ -MT ${patsubst %.d, %.o, $@} $(CFLAGS) $<  -o $@; \
	   [ -s $@ ] || rm -f $@

$(TEST_SRCS:.cpp=.d): %.d: %.cpp
	${QUIET_DEP} set -e; $(CC) -MM -MT $@ -MT ${patsubst %.d, %.o, $@} $(CFLAGS) $<  -I . ${GTESTCFLAGS} -o $@; \
	   [ -s $@ ] || rm -f $@



.PHONY: sourcedist clean docs docsinstall docsclean doxygen

ifneq ($(MAKECMDGOALS),clean)
-include $(SRCS:.cpp=.d)
-include $(TEST_SRCS:.cpp=.d)
endif
