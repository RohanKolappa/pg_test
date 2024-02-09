
#include "v2dlib_compatibility.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "test_utils.hh"

#include "vframedecoder.hh"
#include "vframeparser_c.h"
#include "codecdefs.h"
#include "colortransform.h"
#include "imagemanip.h"
#include "bitmap.hh"
#include "v2d.h"
#include "memory_tracker.h"

using namespace std;

#define VIDEO_PACKET_SIZE SEND_BUFFER_SIZE

TEST(CParserTest, Parse) {

    string strDataDir = GTestUtils::GetDataDir("..");
    if(strDataDir == "")
    {
        FAIL() << "Data dir not found, aborting test ...";
        return;
    }

    string strBitFile = strDataDir + "/video/frames/authstation/" +
        "authstation_bitstream_1280x1024_slicesize32_422.dat";
    string strOffsetFile = strDataDir + "/video/frames/authstation/" +
        "authstation_bitstream_frame_offsets.dat";

    ifstream bitstream(strBitFile.c_str(), ios_base::in | ios_base::binary);
    if(bitstream.fail())
    {
        FAIL() << "Unable to open bitstream file " << strBitFile << 
            ", aborting test...";
        return;
    }

    ifstream offsetstream(strOffsetFile.c_str(), ios_base::in);
    if(offsetstream.fail())
    {
        FAIL() << "Unable to open frame offset file " << strOffsetFile << 
            ", aborting test...";
        return;
    }

    // Read the frame sizes into a array
    vector<size_t> vecFrames;
    copy(istream_iterator<size_t>(offsetstream), 
        istream_iterator<size_t>(),
        back_inserter(vecFrames));

    offsetstream.close();

    v2dParserHandle pParser = v2d_parser_init();

    uint8_t chReadBuf[VIDEO_PACKET_SIZE];
    size_t nBytesRead = 0;
    const uint8_t* pFrameBuf = 0;
    unsigned int nFrames = 0;
    int nFrameSize = 0;
  
    vector<size_t>::iterator vecIter = vecFrames.begin();

    while(!bitstream.eof() && 
            bitstream.read((char *)chReadBuf, VIDEO_PACKET_SIZE))
    {    
        nBytesRead = bitstream.gcount();
        uint8_t *pReadBuf = chReadBuf;

        while(nBytesRead > 0)
        {
            size_t nBytesParsed = 
                v2d_parse_frame(pParser, &pFrameBuf, &nFrameSize, 
                    pReadBuf, nBytesRead);
            nBytesRead -= nBytesParsed;
            pReadBuf += nBytesParsed;

            if(nFrameSize)
            {
                ASSERT_EQ( *vecIter++, static_cast<size_t>(nFrameSize) );
                nFrames++;
            }
        }
    }

    ASSERT_EQ(vecFrames.size(), nFrames);

    bitstream.close();
    v2d_parser_close(pParser);
}

