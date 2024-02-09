/*
 * v2dencoderlistener.hh
 *
 *  Created on: Feb 11, 2011
 *      Author: rkale
 */

#ifndef V2DENCODERLISTENER_HH_
#define V2DENCODERLISTENER_HH_


class IV2DEncoderListener {
public:
    virtual ~IV2DEncoderListener() {};

    virtual void LutChanged(struct LUT_struct * a_pLUT, int a_nSliceSize) = 0;
    virtual void CompressionChanged(int a_nMQuant, int a_nDCQuant) = 0;

    virtual void Stopped() = 0;
};


#endif /* V2DENCODERLISTENER_HH_ */
