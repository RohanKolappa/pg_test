/*
 * cavlc.hh
 *
 *  Created on: Oct 26, 2010
 *      Author: rkale
 */

#ifndef CAVLC_HH_
#define CAVLC_HH_

class CBitStreamWriter;

class CCAVLC {

public:

    static void Encode(CBitStreamWriter * pBitStream, int i_idx,
            int *l, int i_count, int nonZeroPredict);

};

#endif /* CAVLC_HH_ */
