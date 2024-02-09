/*
 * ipputils.h
 *
 *  Created on: Jul 17, 2010
 *      Author: rkale
 */

#ifndef IPPUTILS_H_
#define IPPUTILS_H_

//#define USE_IPP

#ifdef USE_IPP
#include <ipp.h>
void inline InitIPPLib() {
    ippStaticInit();
}


#else

void inline InitIPPLib() {
}
#endif

#ifdef USE_IPP_5DOT3
#include <ippi.h>
#include <ippcc.h>
#endif

#endif /* IPPUTILS_H_ */
