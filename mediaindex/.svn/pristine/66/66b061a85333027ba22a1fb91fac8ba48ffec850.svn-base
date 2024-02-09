/*
 * CMediaProcessorInterface.h
 *
 *  Created on: Feb 7, 2014
 *      Author: durga
 */

#ifndef CMEDIAPROCESSORINTERFACE_H_
#define CMEDIAPROCESSORINTERFACE_H_

#include "MediaServer.hh"

namespace mediaindex {

	class CMediaProcessorInterface {
	public:
		CMediaProcessorInterface() {};
		virtual ~CMediaProcessorInterface() {};

		virtual void StartMedia(uint64_t a_nStartTime) = 0;
		virtual void StopMedia(uint64_t a_nStopTime) = 0;
		virtual void ProcessMediaFrame(CCFrame & a_rFrame) = 0;
	};
};

#endif /* CMEDIAPROCESSORINTERFACE_H_ */
