#ifndef LIBMEDIACONVERTER_H
#define LIBMEDIACONVERTER_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>


#include "MediaServer.hh"
#include "MediaWriter.hh"


#define IPVS_APP_VERSION_STRING "1.0"

#define IPVS_DATA_TRACK_TYPE        0x80
#define IPVS_BOOKMARKS_TRACK_TYPE   0x81
#define IPVS_THUMBNAILS_TRACK_TYPE  0x82

#define IPVS_FRAME_TYPE_MEDIA_DATA         0
#define IPVS_FRAME_TYPE_MEDIA_START        1
#define IPVS_FRAME_TYPE_MEDIA_STOP         2
#define IPVS_FRAME_TYPE_MEDIA_GAPINFO      3
#define IPVS_FRAME_TYPE_MEDIA_SNAPSHOT     4
#define IPVS_FRAME_TYPE_MEDIA_THUMBNAIL    5

#define IPVS_FRAME_HEADER_SIZE          4
#define IPVS_TIMESTAMP_SIZE             8
#define IPVS_IMAGE_HEADER_SIZE          4

#define IPVS_PRIVATE_DATA_VERSION_LENGTH       1
#define IPVS_PRIVATE_DATA_CURRENT_VERSION      1

#define G_IPVS_TIMECODESCALE               1000000      // nanoseconds
#define G_IPVS_MAX_FRAMES_PER_BLOCK        8            // count
#define G_IPVS_MAX_CLUSTER_SIZE            (5 * 1024 * 1024)         // bytes
#define G_IPVS_MAX_CLUSTER_DURATION        5000         // milliseconds
#define G_IPVS_CUEING_SIZE                 (1 * 1024 * 1024)         // bytes
#define G_IPVS_CUEING_PERIOD               1000         // milliseconds


namespace mediaindex
{
    class CMatroskaFileReader;
    class CMatroskaFileWriter;
    class CMinuteFileWriter;
    class CMatroskaFileParser;

    class CMatroskaFileReaderImpl;
    class CMatroskaFileWriterImpl;
    class CMinuteFileWriterImpl;
    class CMatroskaFileParserImpl;

    enum eLIBMC_ERROR
    {
        ELIBMC_OK = 0,
        ELIBMC_EBMLHEADER_NOT_FOUND = 10,
        ELIBMC_ERROR
    };

    class nameval_pair
    {
    public:
        std::string name;
        std::string val;

        nameval_pair(const std::string& a_name, const std::string& a_val)
        : name(a_name), val(a_val)
        {
        }

    };

}

//#define LIBMEDIACONVERTER_NAMESPACE libmediaconverter
//#define START_LIBMEDIACONVERTER_NAMESPACE namespace LIBMEDIACONVERTER_NAMESPACE {
//#define END_LIBMEDIACONVERTER_NAMESPACE   };

#endif //LIBMEDIACONVERTER_H

