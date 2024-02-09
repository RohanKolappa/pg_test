/*
 * MatroskaExtensions.h
 *
 *  Created on: Mar 13, 2010
 *      Author: rkale
 */

#ifndef MATROSKAEXTENSIONS_H_
#define MATROSKAEXTENSIONS_H_


#include <matroska/KaxInfoData.h>

using namespace LIBMATROSKA_NAMESPACE;

class MyKaxDateUTC : public KaxDateUTC
{
public:
    void SetEpochDateInMS(int64 NewDate) {
        bValueIsSet = true;
        myDate = int64(NewDate - (UnixEpochDelay * 1000)) * 1000000;
    };
    int64 GetEpochDateInMS() {
        return int64(myDate/1000000 + UnixEpochDelay * 1000);
    };
};


#endif /* MATROSKAEXTENSIONS_H_ */
