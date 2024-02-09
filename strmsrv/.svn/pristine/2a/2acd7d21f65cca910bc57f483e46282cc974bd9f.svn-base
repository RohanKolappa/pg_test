/*
 * CMpegtsMediaHandler.h
 *
 *  Created on: Jun 12, 2013
 *      Author: durga
 */

#ifndef CMPEGTSMEDIAHANDLER_H_
#define CMPEGTSMEDIAHANDLER_H_

#include "StandardFormatHandler.hh"

class CMpegtsMediaHandler: public CStandardFormatHandler {
public:
    CMpegtsMediaHandler();
    virtual ~CMpegtsMediaHandler();

    virtual bool ProceedToProcess();
    virtual bool IsThumbnailCreationEnabled();
    virtual bool IsNativeOutputEnabled();

    void WriteFrames(unsigned char *a_pFrameData, int a_nLen);
    void CompleteOutput();

private:
    void Initialize();

    bool m_bInitialized;
    FILE *m_pOutputFile;

    bool m_bCompleted;
};

#endif /* CMPEGTSMEDIAHANDLER_H_ */
