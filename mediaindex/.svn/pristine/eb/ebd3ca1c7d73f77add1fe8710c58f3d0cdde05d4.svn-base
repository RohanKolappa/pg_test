cmake_minimum_required(VERSION 2.8)

project(MediaIndex)


if(ARM_BUILD)
	SET(DEV_PKG_DIR "dev/dev_arm")
else()
	SET(DEV_PKG_DIR "dev/dev_i386")
endif()

set(DEV_PKG_PATH "${MediaIndex_SOURCE_DIR}/../${DEV_PKG_DIR}")
set(MATROSKA_PATH ${DEV_PKG_PATH}/matroska)
include_directories(${MATROSKA_PATH}/include)
link_directories(${MATROSKA_PATH}/lib)

set(XERCES_PATH ${DEV_PKG_PATH}/xerces)
include_directories(${XERCES_PATH}/include)
link_directories(${XERCES_PATH}/lib)

if(ARM_BUILD)
    set(BOOST_PATH ${DEV_PKG_PATH}/boost)
    include_directories(${BOOST_PATH}/include)
    link_directories(${BOOST_PATH}/lib)

    set(UUID_PATH ${DEV_PKG_PATH}/uuid)
    include_directories(${UUID_PATH}/include)
    link_directories(${UUID_PATH}/lib)

    add_definitions(-DUSE_OSSP_UUID)
    add_definitions(-D__ARM__)
endif()

set(EXTERNAL_LIBS matroska ebml xerces-c boost_filesystem)
if(ARM_BUILD)
	list(APPEND EXTERNAL_LIBS boost_system)
endif()

if(UNIX)
	SET(BUILD_SHARED_LIBS ON)
	list(APPEND EXTERNAL_LIBS uuid)
else(UNIX)
	SET(BUILD_SHARED_LIBS OFF)
	list(APPEND EXTERNAL_LIBS boost_system rpcrt4)
	add_definitions(-DXML_LIBRARY)
	set(BOOST_PATH ${DEV_PKG_PATH}/boost)
	link_directories(${BOOST_PATH}/lib)
	include_directories(${BOOST_PATH}/include)
endif(UNIX)

add_definitions(-Wall)

set(LIB_SRCS
    MediaServer.cpp
    MediaWriter.cpp
    MediaIndexFactory.cpp
    MediaPortalUtils.cpp
    ClipSourceIndex.cpp
    ClipDestIndex.cpp
    IndexData.cpp
    MatroskaFileParser.cpp
    MatroskaUtils.cpp
    XMLUtils.cpp
    SystemUtils.cpp
    FileSystemUtils.cpp
    Std64IOCallback.cpp
    EditList.cpp
    MatroskaFileInfo.cpp
    MinuteFileWriter.cpp
    MatroskaFileWriter.cpp
    MatroskaFileWriterImpl.cpp
    MinuteFileMediaServer.cpp
    MatroskaMediaServer.cpp
    SnapshotMediaServer.cpp
)

add_subdirectory(test)

add_library(mediaindex ${LIB_SRCS})

#set_target_properties(mediaindex PROPERTIES COMPILE_FLAGS "-Wall")
if(ARM_BUILD)	
	set_target_properties(mediaindex PROPERTIES COMPILE_DEFINITIONS "_LARGEFILE_SOURCE;_LARGEFILE64_SOURCE;_FILE_OFFSET_BITS=64")
endif()

add_executable(testserver testserver.cpp)
target_link_libraries(testserver mediaindex)
target_link_libraries(testserver ${EXTERNAL_LIBS})

if(ARM_BUILD)
	add_executable(index_media index_media.cpp)
else
	add_executable(index_media index_media.cpp)
	target_link_libraries (index_media mediaindex)
	target_link_libraries (index_media ${EXTERNAL_LIBS})
endif()

add_executable(show_gaps show_gaps.cpp)
target_link_libraries(show_gaps mediaindex)
target_link_libraries(show_gaps ${EXTERNAL_LIBS})

#set(CMAKE_BUILD_TYPE Release)

#install(TARGETS mediaindex DESTINATION lib)
if (ARM_BUILD)
	set(CMAKE_BUILD_TYPE Distribution)
	set(CMAKE_CXX_FLAGS_DISTRIBUTION "-O2")
else	
	set(CMAKE_BUILD_TYPE Release)
endif()	


