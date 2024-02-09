/*
 * CSnapshotHandler.cpp
 *
 *  Created on: May 21, 2013
 *      Author: durga
 */
#include <stdio.h>
#include "MediaFlags.hh"
#include "SnapshotHandler.hh"


CSnapshotHandler::CSnapshotHandler()
{
    m_pOutputFile = NULL;
    m_strFileExt = "";
    m_strPrevFileExt = "";

    m_nImageCount = 0;
}

CSnapshotHandler::~CSnapshotHandler()
{
}

bool CSnapshotHandler::IsNativeOutputEnabled()
{
    return true;
}

bool CSnapshotHandler::ProceedToProcess()
{
    return true;
}

bool CSnapshotHandler::CreateOutput(unsigned char *a_pInBuffer, int a_InBuffSize,
                                    unsigned long a_nFlags)
{
    std::string strFileExt = GetFileExtension(a_nFlags);

    char strOutputFile[255];
    sprintf(strOutputFile, "%s", m_strOutputFileName.c_str());

    if (m_nImageCount > 0){
        sprintf(strOutputFile, "%s_%d", strOutputFile, m_nImageCount);
    }

    sprintf(strOutputFile, "%s.%s", strOutputFile, strFileExt.c_str());

    m_pOutputFile = fopen(strOutputFile, "w");
    if (m_pOutputFile == NULL) {
        printf("Failed in creating standard output file. \n");
        return false;
    }

    int nWritten = fwrite(a_pInBuffer, 1, a_InBuffSize, m_pOutputFile);
    if (nWritten != a_InBuffSize) {
        printf("Failed in writing image data. \n");
        fclose(m_pOutputFile);
        return false;
    }

    fclose(m_pOutputFile);

    m_strPrevFileExt = strFileExt;
    m_nImageCount++;

    return true;
}

std::string CSnapshotHandler::GetFileExtension(unsigned long a_nFlags)
{
    switch(a_nFlags & IMAGETYPE_MASK) {
    case IMAGETYPE_JPEG:
        return "jpg";

    case IMAGETYPE_PNG:
        return "png";

    case IMAGETYPE_BMP:
        return "bmp";

    case IMAGETYPE_GIF:
        return "gif";

    case IMAGETYPE_TIFF:
        return "tiff";

    default:
        printf("Un-known file format. \n");
        return "";
    }
}
