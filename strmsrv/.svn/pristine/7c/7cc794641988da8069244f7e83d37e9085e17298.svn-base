/*
 * CCheckPTS.cpp
 *
 *  Created on: Jun 4, 2013
 *      Author: durga
 */

#include "strmsrv_compatibility.h"

#include "FFmpegHeaders.hh"
#include "FFmpegUtils.hh"
#include "CheckPTS.h"


CCheckPTS::CCheckPTS(std::string a_strInputFile)
{
    m_pInputFmtContext = NULL;

    m_pVideoCodecCtx = NULL;
    m_pAudioCodecCtx = NULL;

    m_nVideoIndex = -1;
    m_nAudioIndex = -1;

    m_strInputFileName = a_strInputFile;

    Initialize();
}

CCheckPTS::~CCheckPTS()
{
    if (m_pInputFmtContext)
        avformat_free_context(m_pInputFmtContext);
}

void CCheckPTS::Initialize()
{
    FFmpegUtils::Initialize();

    m_pInputFmtContext = avformat_alloc_context();
    int ret = av_open_input_file(&m_pInputFmtContext, m_strInputFileName.c_str(), NULL, 0, NULL);
    if (ret != 0) {
        printf("Could not open input file. \n");
        return;
    }

    for( unsigned int i = 0; i < m_pInputFmtContext->nb_streams; i++) {
        if (m_pInputFmtContext->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) {
            printf("Found Video stream index is = %d\n", i);
            m_nVideoIndex = i;

            m_pVideoCodecCtx = m_pInputFmtContext->streams[i]->codec;
            printf("Extradata size = %d \n", m_pVideoCodecCtx->extradata_size);
        }
        else if (m_pInputFmtContext->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
            printf("Found Audio Stream \n");
            m_nAudioIndex = i;

            m_pAudioCodecCtx = m_pInputFmtContext->streams[i]->codec;
            printf("AUDIO SAMPLE SIZE is %d \n", m_pAudioCodecCtx->frame_size);
        }
    }
}

void CCheckPTS::RetrievePTSAndDTS(bool a_bVideo, bool a_bAudio)
{
    int nRead = 0;

    do
    {
        AVPacket packet;

        nRead = av_read_frame(m_pInputFmtContext, &packet);
        if (nRead < 0) {
            return;
        }

        if (packet.stream_index == (int)m_nAudioIndex) {
            if (a_bAudio)
                printf("Audio sample PTS = %"PRIu64", DTS = %"PRIu64" \n", packet.pts, packet.dts);
        }
        else if (packet.stream_index == (int)m_nVideoIndex) {
            if (a_bVideo)
                printf("Video Frame PTS = %"PRIu64", DTS = %"PRIu64" starttime = %"PRIu64" \n", packet.pts, packet.dts,
                        m_pInputFmtContext->streams[m_nVideoIndex]->codec->timecode_frame_start);
        }

        av_free_packet(&packet);
    }while (!nRead);
}
/*Test PTS value by implement one test case like this.

TEST_F(FFTest, CheckPTSValue) {
    CCheckPTS *pCheckPTS = new CCheckPTS("/home/durga/workspace/strmsrv/output.mp4");

    pCheckPTS->Initialize();

    delete pCheckPTS;
}
*/
