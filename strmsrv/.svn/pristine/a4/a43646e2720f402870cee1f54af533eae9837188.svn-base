/*
 * CFFMediaProcessorErrorHandler.cpp
 *
 *  Created on: Jul 29, 2014
 *      Author: durga
 */

#include "FFMediaProcessorErrorHandler.hh"

int CFFMediaProcessorErrorHandler::m_nThumbnailCreationErrorForSnapshot = 0;

CFFMediaProcessorErrorHandler::CFFMediaProcessorErrorHandler() {
}

CFFMediaProcessorErrorHandler::~CFFMediaProcessorErrorHandler() {
}

void CFFMediaProcessorErrorHandler::SetThumbnailCreationErrorForSnapshot(int a_nError)
{
    m_nThumbnailCreationErrorForSnapshot = a_nError;
}

int CFFMediaProcessorErrorHandler::GetThumbnailCreationErrorForSnapshot()
{
    return m_nThumbnailCreationErrorForSnapshot;
}

void CFFMediaProcessorErrorHandler::ClearThumbnailCreationErrorForSnapshot()
{
    m_nThumbnailCreationErrorForSnapshot = 0;
}
