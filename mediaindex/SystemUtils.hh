/*
 * SystemUtils.h
 *
 *  Created on: Mar 16, 2010
 *      Author: rkale
 */

#ifndef SYSTEMUTILS_H_
#define SYSTEMUTILS_H_

namespace mediaindex {

// Generates a 128 bit (16 byte) UUID and copies into the provided data pointer
void GenerateUUID(uint8_t *pDesination);
uint64_t GenerateUID();

};

#endif /* SYSTEMUTILS_H_ */
