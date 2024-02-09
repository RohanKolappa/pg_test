/*
 * CCodecHandler.h
 *
 *  Created on: May 14, 2013
 *      Author: durga
 */

#ifndef CCODECHANDLER_H_
#define CCODECHANDLER_H_

class CCodecHandler {
public:
    CCodecHandler();
    virtual ~CCodecHandler();

    virtual void SetNativeOutput() {};

    virtual bool ProceedToProcess() = 0;

    virtual bool IsThumbnailCreationEnabled() { return false; }
    virtual bool IsNativeOutputEnabled() { return false; };

private:

};

#endif /* CCODECHANDLER_H_ */
