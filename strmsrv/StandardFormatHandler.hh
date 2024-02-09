/*
 * CStandardFormatHandler.h
 *
 *  Created on: May 21, 2013
 *      Author: durga
 */

#ifndef CSTANDARDFORMATHANDLER_H_
#define CSTANDARDFORMATHANDLER_H_

#include <string>
#include "CodecHandler.h"

class CStandardFormatHandler: public CCodecHandler {
public:
    CStandardFormatHandler();
    virtual ~CStandardFormatHandler();

    void SetOutputFileName(std::string a_strOutputFileName);
    std::string GetOutputFileName();

protected:
    std::string m_strOutputFileName;
};

#endif /* CSTANDARDFORMATHANDLER_H_ */
