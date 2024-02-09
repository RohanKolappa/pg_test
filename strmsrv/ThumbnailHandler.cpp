/*
 * CThumbnailHandler.cpp
 *
 *  Created on: May 10, 2013
 *      Author: durga
 */
#include "strmsrv_compatibility.h"

#include "ThumbnailHandler.h"
#include "ThumbnailGenerator.h"
#include "FFUtils.hh"


CThumbnailHandler::CThumbnailHandler()
: CCodecHandler()
{
    m_pThumbnailGenerator = NULL;

    m_bCreateLiveTN    = false;
    m_bCreateSnapshot  = false;
    m_bCreateLargeTN   = false;
    m_bCreateSmallTN   = false;

    m_bCreatePrimaryTN = false;
    m_bCreateStreamTNs = false;

    m_nPrimaryTNWidth  = -1;
    m_nPrimaryTNHeight = -1;
    m_nStreamTNWidth   = -1;
    m_nStreamTNHeight  = -1;

    m_nThumbnailInterval = 0;
    m_nWaitTimeForPrimaryTN = 0;

    m_strThumbnailDir = "";
    m_bCreatedTNDirectory = false;

    m_bCreatedPrimaryTN = false;
    m_bCreatedFirstStreamTN = false;

    m_nNextThumbnailOffset = 0;

    m_bProcessedFinalThumbnails = false;

    m_nTotalThumbnails = 0;
}

CThumbnailHandler::~CThumbnailHandler()
{
    DestroyThumbnailGenerator();
}

bool CThumbnailHandler::CreateImageFile(unsigned char ** a_pImageData,
                                        int a_nTNSize, char * a_pStrFileName)
{
    FILE *pTNFile = fopen(a_pStrFileName, "w+");
    if (pTNFile == NULL) {
        return false;
    }

    fwrite(*a_pImageData, 1, a_nTNSize, pTNFile);
    fclose(pTNFile);

    return true;
}

void CThumbnailHandler::EnableLiveTN(bool a_bEnableLiveTN,
                                     bool a_bEnableSnapshot,
                                     bool a_bEnableLargeTN,
                                     bool a_bEnableSmallTN)
{
    m_bCreateLiveTN = a_bEnableLiveTN;
    m_bCreateSnapshot = a_bEnableSnapshot;
    m_bCreateLargeTN = a_bEnableLargeTN;
    m_bCreateSmallTN = a_bEnableSmallTN;
}

bool CThumbnailHandler::IsLiveThumbnailEnabled()
{
    return m_bCreateLiveTN;
}

bool CThumbnailHandler::IsSnapshotEnabled()
{
    return m_bCreateSnapshot;
}

bool CThumbnailHandler::IsLargeTNEnabled()
{
    return m_bCreateLargeTN;
}

bool CThumbnailHandler::IsSmallTNEnabled()
{
    return m_bCreateSmallTN;
}

bool CThumbnailHandler::IsCreatingAnyTN()
{
    if (m_bCreateStreamTNs == false &&
            m_bCreatePrimaryTN == false &&
            m_bCreateLiveTN == false)
    {
        return false;
    }

    return true;
}

void CThumbnailHandler::EnablePrimaryTN(bool a_bEnablePrimaryTN)
{
    m_bCreatePrimaryTN = a_bEnablePrimaryTN;
}

bool CThumbnailHandler::IsPrimaryTNEnabled()
{
    return m_bCreatePrimaryTN;
}

void CThumbnailHandler::EnableStreamTN(bool a_bEnableStreamTN)
{
    m_bCreateStreamTNs = a_bEnableStreamTN;
}

bool CThumbnailHandler::IsStreamTNEnabled()
{
    return m_bCreateStreamTNs;
}

void CThumbnailHandler::SetPrimaryTNResolution(int a_nWidth, int a_nHeight)
{
    if (a_nWidth <= 0 || a_nHeight <= 0) {
        return;
    }

    m_nPrimaryTNWidth = a_nWidth;
    m_nPrimaryTNHeight = a_nHeight;
}

int CThumbnailHandler::GetPrimaryTNWidth()
{
    return m_nPrimaryTNWidth;
}

int CThumbnailHandler::GetPrimaryTNHeight()
{
    return m_nPrimaryTNHeight;
}

void CThumbnailHandler::SetStreamTNResolution(int a_nWidth, int a_nHeight)
{
    if (a_nWidth <= 0 || a_nHeight <= 0) {
        return;
    }

    m_nStreamTNWidth = a_nWidth;
    m_nStreamTNHeight = a_nHeight;
}

int CThumbnailHandler::GetStreamTNWidth()
{
    return m_nStreamTNWidth;
}

int CThumbnailHandler::GetStreamTNHeight()
{
    return m_nStreamTNHeight;
}

void CThumbnailHandler::SetPrimaryTNOffset(uint64_t a_nOffset)
{
    m_nWaitTimeForPrimaryTN = a_nOffset;
}

uint64_t CThumbnailHandler::GetPrimaryTNOffset()
{
    return m_nWaitTimeForPrimaryTN;
}

void CThumbnailHandler::SetStreamTNFrequency(uint64_t a_nTimeInterval)
{
    m_nThumbnailInterval = a_nTimeInterval;
}

uint64_t CThumbnailHandler::GetStreamTNFrequency()
{
    return m_nThumbnailInterval;
}

bool CThumbnailHandler::ReCreateThumbnailGenerator(int a_nInputWidth, int a_nInputHeight,
                                           ePixelFormat a_eSrcFormat)
{
    if (m_pThumbnailGenerator != NULL)
        DestroyThumbnailGenerator();

    m_pThumbnailGenerator = new CThumbnailGenerator(a_nInputWidth,
                                                    a_nInputHeight,
                                                    a_eSrcFormat);
    if (m_pThumbnailGenerator == NULL)
        return false;

    return true;
}

void CThumbnailHandler::DestroyThumbnailGenerator()
{
    delete m_pThumbnailGenerator;
    m_pThumbnailGenerator = NULL;
}

void CThumbnailHandler::SetThumbnailResolution(int a_nWidth, int a_nHeight)
{
    if (m_pThumbnailGenerator == NULL)
        return;

    m_pThumbnailGenerator->SetThumbnailResolution(a_nWidth, a_nHeight);
}

int CThumbnailHandler::CreateImage(unsigned char **a_Buffer, unsigned char **a_OutputBuffer,
                                   const int a_nOutBufferSize)
{
    if (m_pThumbnailGenerator == NULL)
        return -1;

    return m_pThumbnailGenerator->CreateImage(a_Buffer, a_OutputBuffer, a_nOutBufferSize);
}

bool CThumbnailHandler::CreateTNDirectory(std::string a_strMediaDestDir)
{
    if (CFFUtils::IsDirectory(a_strMediaDestDir)) {
        m_strThumbnailDir = a_strMediaDestDir + "/tn";
        if (!DeleteTNDirectory()) {
            printf("DeleteTNDirectory failed \n");
        }

        if (!CFFUtils::CreateDirectory(m_strThumbnailDir)) {
            printf("CFFV2d::CreateTNDirectory failed in creation tn directory %s.\n", m_strThumbnailDir.c_str());
            return false;
        }
    }

    m_bCreatedTNDirectory = true;

    return true;
}

bool CThumbnailHandler::DeleteTNDirectory()
{
    if (!CFFUtils::IsDirectory(m_strThumbnailDir))
        return true;

    if (!CFFUtils::DeleteDirectoryTree(m_strThumbnailDir)) {
        return false;
    }

    return true;
}

void CThumbnailHandler::SetTNDirectory(std::string a_strThumbnailDir)
{
    m_strThumbnailDir = a_strThumbnailDir;
}

std::string CThumbnailHandler::GetTNDirectory()
{
    return m_strThumbnailDir;
}

bool CThumbnailHandler::IsTNDirectoryCreated()
{
    return m_bCreatedTNDirectory;
}

void CThumbnailHandler::IncrementTNCount()
{
    m_nTotalThumbnails++;
}
int CThumbnailHandler::GetTotalThumbnailCount()
{
    return m_nTotalThumbnails;
}

void CThumbnailHandler::SetPrimaryTNCreated()
{
    m_bCreatedPrimaryTN = true;
}

bool CThumbnailHandler::IsPrimaryTNCreated()
{
    return m_bCreatedPrimaryTN;
}

void CThumbnailHandler::SetFirstStreamTNCreated()
{
    m_bCreatedFirstStreamTN = true;
}

bool CThumbnailHandler::IsFirstStreamTNCreated()
{
    return m_bCreatedFirstStreamTN;
}

void CThumbnailHandler::SetNextTNOffset(uint64_t a_nNextTNOffset)
{
    m_nNextThumbnailOffset += a_nNextTNOffset;
}

uint64_t CThumbnailHandler::GetNextTNOffset()
{
    return m_nNextThumbnailOffset;
}

bool CThumbnailHandler::IsFinalThumbailsProcessed()
{
    return m_bProcessedFinalThumbnails;
}

void CThumbnailHandler::SetFinalThumbnailsStatus(bool a_bProcessed)
{
    m_bProcessedFinalThumbnails = a_bProcessed;
}

bool CThumbnailHandler::ProceedToProcess()
{
    if (m_bCreateStreamTNs == false &&
            m_bCreatePrimaryTN == false &&
            m_bCreateLiveTN == false)
    {
        return false;
    }

    return true;
}

bool CThumbnailHandler::IsThumbnailCreationEnabled()
{
    return (m_bCreatePrimaryTN | m_bCreateStreamTNs | m_bCreateLiveTN);
}

bool CThumbnailHandler::IsNativeOutputEnabled()
{
    return false;
}
