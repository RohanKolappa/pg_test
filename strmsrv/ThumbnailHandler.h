/*
 * CThumbnailHandler.h
 *
 *  Created on: May 10, 2013
 *      Author: durga
 */

#ifndef CTHUMBNAILHANDLER_H_
#define CTHUMBNAILHANDLER_H_

#include "CodecHandler.h"
#include "codecdefs.h"
#include <iostream>


class CThumbnailGenerator;

class CThumbnailHandler : public CCodecHandler{
public:
    CThumbnailHandler();
    virtual ~CThumbnailHandler();

    bool CreateImageFile(unsigned char ** a_pImageData, int a_nTNSize,
                         char * a_pStrFileName);

    virtual void EnableLiveTN(bool a_bEnableLiveTN,
                              bool a_bEnableSnapshot,
                              bool a_bEnableLargeTN,
                              bool a_bEnableSmallTN);

    bool IsLiveThumbnailEnabled();
    bool IsSnapshotEnabled();
    bool IsLargeTNEnabled();
    bool IsSmallTNEnabled();

    bool IsCreatingAnyTN();

    void EnablePrimaryTN(bool a_bEnablePrimaryTN);
    bool IsPrimaryTNEnabled();

    void EnableStreamTN(bool a_bEnableStreamTNs);
    bool IsStreamTNEnabled();

    void SetPrimaryTNResolution(int a_nWidth, int a_nHeight);
    int GetPrimaryTNWidth();
    int GetPrimaryTNHeight();

    void SetStreamTNResolution(int a_nWidth, int a_nHeight);
    int GetStreamTNWidth();
    int GetStreamTNHeight();

    void SetPrimaryTNOffset(uint64_t a_nOffset);
    uint64_t GetPrimaryTNOffset();

    void SetStreamTNFrequency(uint64_t a_nTimeInterval);
    uint64_t GetStreamTNFrequency();

    bool ReCreateThumbnailGenerator(int a_nInputWidth, int a_nInputHeight,
                                    ePixelFormat a_eSrcFormat);
    void DestroyThumbnailGenerator();

    void SetThumbnailResolution(int a_nWidth, int a_nHeight);
    int CreateImage(unsigned char **aBuffer, unsigned char **aOutputBuffer,
                    const int a_nOutBufferSize);

    bool CreateTNDirectory(std::string a_strMediaDir);
    bool DeleteTNDirectory();

    void SetTNDirectory(std::string a_strThumbnailDir);
    std::string GetTNDirectory();

    bool IsTNDirectoryCreated();

    void IncrementTNCount();
    int GetTotalThumbnailCount();

    void SetPrimaryTNCreated();
    bool IsPrimaryTNCreated();

    void SetFirstStreamTNCreated();
    bool IsFirstStreamTNCreated();

    void SetNextTNOffset(uint64_t a_nNextTNOffset);
    uint64_t GetNextTNOffset();

    void SetFinalThumbnailsStatus(bool a_bProcessed);
    bool IsFinalThumbailsProcessed();

    virtual bool ProceedToProcess();

    virtual bool IsThumbnailCreationEnabled();
    virtual bool IsNativeOutputEnabled();

private:
    CThumbnailGenerator *m_pThumbnailGenerator;

    bool m_bCreateLiveTN;
    bool m_bCreateSnapshot;
    bool m_bCreateLargeTN;
    bool m_bCreateSmallTN;

    bool m_bCreatePrimaryTN;
    bool m_bCreateStreamTNs;

    int  m_nPrimaryTNWidth;
    int  m_nPrimaryTNHeight;

    int  m_nStreamTNWidth;
    int  m_nStreamTNHeight;

    uint64_t  m_nThumbnailInterval;
    uint64_t  m_nWaitTimeForPrimaryTN;

    bool m_bCreatedTNDirectory;

    std::string m_strThumbnailDir;

    bool m_bCreatedPrimaryTN;
    bool m_bCreatedFirstStreamTN;

    uint64_t m_nNextThumbnailOffset;

    bool m_bProcessedFinalThumbnails;

    int m_nTotalThumbnails;

};

#endif /* CTHUMBNAILHANDLER_H_ */
