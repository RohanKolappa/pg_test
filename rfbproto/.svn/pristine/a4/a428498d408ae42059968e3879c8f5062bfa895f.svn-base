cmake_minimum_required (VERSION 2.8)

project (RFBProto)

if(ARM_BUILD)
	SET(DEV_PKG_DIR "dev/dev_arm")
else()
	SET(DEV_PKG_DIR "dev/dev_i386")
endif()

set(DEV_PKG_PATH "${RFBProto_SOURCE_DIR}/../${DEV_PKG_DIR}")


if(UNIX)
	SET(BUILD_SHARED_LIBS ON)
endif(UNIX)

add_definitions(-Wall)
add_definitions(-DSTREAMINGSERVER)

set(LIB_SRCS
    sockets.c
    vncauth.c
    d3des.c
    rfbio.c
    rfbclient.c
)


add_library(rfbproto ${LIB_SRCS})

add_executable(rfbtest rfbtest.c)
target_link_libraries (rfbtest rfbproto)

set(CMAKE_BUILD_TYPE Distribution)
set(CMAKE_CXX_FLAGS_DISTRIBUTION "-O2")
