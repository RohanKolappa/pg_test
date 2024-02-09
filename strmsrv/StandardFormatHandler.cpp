/*
 * CStandardFormatHandler.cpp
 *
 *  Created on: May 21, 2013
 *      Author: durga
 */

#include "StandardFormatHandler.hh"

using namespace std;

CStandardFormatHandler::CStandardFormatHandler()
: CCodecHandler()
{

}

CStandardFormatHandler::~CStandardFormatHandler()
{

}

void CStandardFormatHandler::SetOutputFileName(std::string a_strOutputFileName)
{
    m_strOutputFileName = a_strOutputFileName;
}

std::string CStandardFormatHandler::GetOutputFileName()
{
    return m_strOutputFileName;
}
