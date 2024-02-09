/*
 * CImageWriter.h
 *
 *  Created on: Mar 15, 2013
 *      Author: durga
 */

#ifndef CIMAGEWRITER_H_
#define CIMAGEWRITER_H_

#include "streamer.hh"
#include "portuse.hh"

class CFFMediaProcessor;

class CImageWriter : public CStreamer {
public:
    CImageWriter(int a_nId,
                 int a_nIOPort,
                 char *a_pMediaDir,
                 char *a_pStreamType,
                 bool a_bEnableTNs,
                 bool a_bEnableSnapshot,
                 bool a_bEnableLargeTN,
                 bool a_bEnableSmallTN,
                 int a_nSmallTNWidth, int a_nSmallHeight,
                 int a_nLargeTNWidth, int a_nLargeTNHeight,
                 int a_nThumbnailInterval,
                 char * a_pSDPFilePath,
                 char * a_publishURL);


    int ProcessStream();

    int StartStream(int nQueueId);
    int StopStream();
    void DeleteStream();

    int Update(CCommand *a_pCmd);

    char * GetMediaDir() {
        return m_pMediaDir;
    }
    void UpdateCustomStatus(CCommand *a_pCmd);

private:
    CFFMediaProcessor * m_pFFMediaProcessor;

    bool                m_IsStartNeeded;
    char *              m_pMediaDir;
    char *              m_pStreamType;
    char *              m_pSDPFilePath;

    bool                m_bEnableTNs;
    bool                m_bEnableNative;
    bool                m_bEnableLargeTN;
    bool                m_bEnableSmallTN;

    int                 m_nLargeTN_Width;
    int                 m_nLargeTN_Height;

    int                 m_nSmallTN_Width;
    int                 m_nSmallTN_Height;

    int                 m_nThumbnailInterval;
    int                 m_nIOPort;

    std::string         m_strPublishURL;

    bool                m_bError;
    static              CPortUse m_portsUsed;

    virtual ~CImageWriter();
    void RetrieveThumbnailConfiguration();
    void SetMediaProcessorParams();
    void CreateMediaProcessor();
    void DeleteMediaProcessor();
};

#endif /* CIMAGEWRITER_H_ */
