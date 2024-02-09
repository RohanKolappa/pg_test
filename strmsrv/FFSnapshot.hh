/*
 * CFFSnapshot.h
 *
 *  Created on: Oct 8, 2012
 *      Author: durga
 */

#ifndef CFFSNAPSHOT_H_
#define CFFSNAPSHOT_H_

#include "FFCodec.hh"
#include "codecdefs.h"

class CFFSnapshot: public CFFCodec {
public:
    CFFSnapshot(int nId, std::vector<int> & trackSpeeds,
                bool bShowProgress, bool bNativeOutput,
                bool bCreateTracks);
    virtual ~CFFSnapshot();

    virtual void ProcessFrame(CSFrame *sFrame, timestamp_t nMediaOffset = 0);
    virtual void StopStream();

    void PrintStats();

private:
    void CreateOutput(unsigned char* a_pInputData, int a_nInBuffSize,
                      unsigned long a_nFlags);
    void CreateThumbnails(unsigned char* a_pInputData, int a_nInBuffSize,
                          unsigned long a_nFlags);
    void CreatePrimaryTN(unsigned char* a_pInputData, int a_nInBuffSize);
    bool CreateSingleImage(unsigned char * a_pInputBuffer, char * a_pStrFileName,
                           int a_nThumbnailWidth, int a_nThumbnailHeight);
    int DecodeJpegImage(unsigned char * a_pInBuffer, int a_InBuffSize,
                         uint64_t & a_OutBuffSize);

    int HandleJpegInput (unsigned char *a_pInBuffer, int a_InBuffSize,
                         uint64_t & a_OutBuffSize);

    int HandleBmpInput (unsigned char *a_pInBuffer, int a_InBuffSize,
                         uint64_t & a_OutBuffSize);

    void CreateStandardMedia(unsigned char * a_pInBuffer, int a_InBuffSize,
                             unsigned long a_nFlags);

    int m_nSnapshotWidth;
    int m_nSnapshotHeight;

    int m_nBytesPerPixel;
    ePixelFormat m_ePixelFormat;

    unsigned char *m_pDecodedBuffer;
};

#endif /* CFFSNAPSHOT_H_ */
