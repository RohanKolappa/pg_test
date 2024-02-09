/*
 * CSnapshotHandler.h
 *
 *  Created on: May 21, 2013
 *      Author: durga
 */

#ifndef CSNAPSHOTHANDLER_H_
#define CSNAPSHOTHANDLER_H_

#include "StandardFormatHandler.hh"
#include <iostream>

class CSnapshotHandler: public CStandardFormatHandler {
public:
    CSnapshotHandler();
    virtual ~CSnapshotHandler();

    bool IsNativeOutputEnabled();
    bool ProceedToProcess();

    bool CreateOutput(unsigned char *a_pInBuffer, int a_InBuffSize,
                      unsigned long a_nFlags = 0);
private:
    std::string GetFileExtension(unsigned long a_nFlags);

    FILE *m_pOutputFile;
    std::string m_strFileExt;
    std::string m_strPrevFileExt;

    int m_nImageCount;

};

#endif /* CSNAPSHOTHANDLER_H_ */
