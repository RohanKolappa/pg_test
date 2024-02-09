/*
 * CFFMediaProcessorErrorHandler.h
 *
 *  Created on: Jul 29, 2014
 *      Author: durga
 */

#ifndef CFFMEDIAPROCESSORERRORHANDLER_H_
#define CFFMEDIAPROCESSORERRORHANDLER_H_

class CFFMediaProcessorErrorHandler {
public:
    CFFMediaProcessorErrorHandler();
    virtual ~CFFMediaProcessorErrorHandler();

    static void SetThumbnailCreationErrorForSnapshot(int nError);
    static int GetThumbnailCreationErrorForSnapshot();
    static void ClearThumbnailCreationErrorForSnapshot();

private:
    static int m_nThumbnailCreationErrorForSnapshot;
};

#endif /* CFFMEDIAPROCESSORERRORHANDLER_H_ */
