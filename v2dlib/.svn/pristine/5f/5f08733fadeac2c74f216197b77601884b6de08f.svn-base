/*
 * layerUtility.hh
 *
 *  Created on: Mar 8, 2012
 *      Author: User
 */

#ifndef LAYERUTILITY_HH_
#define LAYERUTILITY_HH_

class CLayerUtils
{
public:
static int GetLayerSize(uint8_t *a_buff) {

    int lSize = ((*(a_buff) & 0x7F) << 8 | *(a_buff + 1));
    return lSize;
}


static void SetLayersize(uint8_t *a_buff, uint32_t a_nSize) {

    *a_buff |= (a_nSize >> 8) & 0x7F;
    *(a_buff + 1) = a_nSize & 0xFF;
    *a_buff |= 0x80;
}

static void IncLayerSize(unsigned char *slice_buff)
{
    AdjustLayerSize(slice_buff , 1);
}

static void DecLayerSize(unsigned char *slice_buff)
{
   AdjustLayerSize(slice_buff , -1);
}

 static void AdjustLayerSize(unsigned char *slice_buff, int val) {

     short int layerSize = GetLayerSize(slice_buff); //(*(slice_buff + 0) << 8 | *(slice_buff + 1))
        //& 0x7FFF;
        layerSize += val;
        *(slice_buff + 0) |= (layerSize >> 8);
        *(slice_buff + 1) = layerSize & 0x00FF;
 }


};
#endif /* LAYERUTILITY_HH_ */
