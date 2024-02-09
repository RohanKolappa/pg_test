
CC=gcc
RM=rm -rf

GSTREAMER_DIR=../dev/dev_i386/libgstreamer
GSTREAMER_INCLUDE=${GSTREAMER_DIR}/include
GSTREAMER_LIB_PATH=${GSTREAMER_DIR}/lib

GSTREAMER_CFLAGS = -I ${GSTREAMER_DIR}/include/gstreamer-1.0 \
				   -I ${GSTREAMER_DIR}/include/glib-2.0 \
				   -I ${GSTREAMER_DIR}/lib/glib-2.0/include

GSTREAMER_LIBS = -L ${GSTREAMER_LIB_PATH} -lgstreamer-1.0 -lglib-2.0 -lgobject-2.0 -lgstbase-1.0 -lstdc++ -lgstvideo-1.0

V2DLIB_DIR = ../v2dlib
V2DLIB_CFLAGS = -I ${V2DLIB_DIR}

STRMSRV_DIR = ../strmsrv
STRMSRV_INCDIR = ${STRMSRV_DIR}/
STRMSRV_CFLAGS = -I ${STRMSRV_DIR}
STRMSRV_LIB_PATH = ${STRMSRV_DIR}
STRMSRV_LIBS = -L ${STRMSRV_LIB_PATH}/ -lstrmsrv

BCP_DIR		=	../VMP_DMS
HLS_CFLAGS	=	-I ${BCP_DIR}/VMP_DMS/Src/

MATROSKA_LIB_PATH = ../dev/dev_i386/matroska/lib
XERCES_LIB_PATH = ../dev/dev_i386/xerces/lib

MEDIAINDEX_DIR = ../mediaindex/
MEDIAINDEX_INCDIR = ${MEDIAINDEX_DIR}
MEDIAINDEX_CFLAGS = -I ${MEDIAINDEX_INCDIR}
MEDIAINDEX_LIBS = -L ${MEDIAINDEX_DIR} -lmediaindex -L ${MATROSKA_LIB_PATH} -lmatroska -lebml -L ${XERCES_LIB_PATH} -lxerces-c -luuid

CFLAGS=${GSTREAMER_CFLAGS} ${V2DLIB_CFLAGS} ${STRMSRV_CFLAGS} ${MEDIAINDEX_CFLAGS} ${HLS_CFLAGS}

IMPORTER_LIBS = ${GSTREAMER_LIBS} ${MEDIAINDEX_LIBS} ${STRMSRV_LIBS} 

CFLAGS += -DHAVE_CONFIG_H
CFLAGS += -Wall   

IMPORTEROBJS = mediawriter_cwrapper.o ipvsmediawriter.o 

IPVSRTPMUXEROBJS = ipvsrtpmuxer.o mediawriter_cwrapper.o 

all: libipvsmediawriter.so libipvsrtpmuxer.so

%.o:%.c
	${CC} -c ${CFLAGS} $< -o $@

%.o:%.cpp
	${CC} -c ${CFLAGS} $< -o $@

libipvsmediawriter.so: ${IMPORTEROBJS}
	${CC} -shared -o $@ ${IMPORTEROBJS} ${IMPORTER_LIBS} 

libipvsrtpmuxer.so: ${IPVSRTPMUXEROBJS}
	${CC} -shared -o $@ ${IPVSRTPMUXEROBJS} ${IMPORTER_LIBS}

clean: 
	${RM} *.o *.so


