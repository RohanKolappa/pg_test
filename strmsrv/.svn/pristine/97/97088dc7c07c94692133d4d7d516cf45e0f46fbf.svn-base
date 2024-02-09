/*
 * CMpegtsMediaHandler.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: durga
 */

#include <stdio.h>
#include "MpegtsMediaHandler.hh"

CMpegtsMediaHandler::CMpegtsMediaHandler()
: CStandardFormatHandler()
{
    m_bInitialized = false;
    m_pOutputFile = NULL;
    m_bCompleted = false;
}

CMpegtsMediaHandler::~CMpegtsMediaHandler()
{
    CompleteOutput();
}

bool CMpegtsMediaHandler::ProceedToProcess()
{
    return true;
}

bool CMpegtsMediaHandler::IsThumbnailCreationEnabled()
{
    return false;
}

bool CMpegtsMediaHandler::IsNativeOutputEnabled()
{
    return true;
}

void CMpegtsMediaHandler::Initialize()
{
    std::string strOutputFile = m_strOutputFileName + ".ts";

    m_pOutputFile = fopen(strOutputFile.c_str(), "w");
    if (m_pOutputFile == NULL) {
        printf("Failed in creating MPEGTS output file.\n");
        return;
    }

    m_bInitialized = true;
}

void CMpegtsMediaHandler::WriteFrames(unsigned char *a_pFrameData, int a_nLen)
{
    if (!m_bInitialized) {
        Initialize();
    }

    if (!m_pOutputFile)
        return;

    fwrite(a_pFrameData, sizeof(unsigned char), a_nLen, m_pOutputFile);
}

void CMpegtsMediaHandler::CompleteOutput()
{
    if (m_pOutputFile == NULL)
        return;

    fclose(m_pOutputFile);
    m_pOutputFile = NULL;
}
