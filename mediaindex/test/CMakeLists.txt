cmake_minimum_required (VERSION 2.8)

project (MediaIndexTests)


set(GTEST_PATH ${DEV_PKG_PATH}/gmock)
include_directories(${GTEST_PATH}/include)

include_directories(${MediaIndex_SOURCE_DIR})

set(TEST_SRCS
    create_test.cpp
    editlist_test.cpp
    extract_test.cpp
    info_test.cpp
    parser_test.cpp
    mediaserver_test.cpp
#    mediawriter_test.cpp
)

add_executable(mediaindexLibTests ${TEST_SRCS})
target_link_libraries (mediaindexLibTests mediaindex)
target_link_libraries (mediaindexLibTests ${GTEST_PATH}/lib/gmock_main.a ${GTEST_PATH}/lib/gtest_utils.a)
target_link_libraries (mediaindexLibTests ${EXTERNAL_LIBS})

if(ARM_BUILD)
    target_link_libraries (mediaindexLibTests pthread)
endif()
