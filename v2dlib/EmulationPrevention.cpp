/*
 * EmulationPrevention.cpp
 *
 *  Created on: Mar 1, 2012
 *      Author: User
 */

#include "v2dlib_compatibility.h"
#include "EmulationPrevention.hh"
#include "layerUtils.hh"

int CEmulationPrevention::EmulateSlice(unsigned char *slice_buff, int SliceLen)
{
    unsigned int nbytesEmulated = 0;
    unsigned int totalbytesEmulated = 0;
    unsigned char *pdata = slice_buff + 4;
    int remlayerSize = SliceLen - 4;

    int len = 0;
    if ((SliceLen - 6) <= 0)
        return -1;

    while (len < SliceLen - 6) {
        int layersize = CLayerUtils ::GetLayerSize(pdata) ;
        int layerflag = ((*(pdata + 2)) >> 7) & 0x01;
        if (layersize > SliceLen || layersize == 0)
            return -1;

        nbytesEmulated = ParseForEmulation(pdata, layersize, remlayerSize);
        pdata += layersize + nbytesEmulated;
        len += layersize + nbytesEmulated;
        totalbytesEmulated += nbytesEmulated;
        remlayerSize -= layersize;

        if (layerflag == 1)
            break;

    }
    return totalbytesEmulated;
}

int CEmulationPrevention::ParseForEmulation(unsigned char *slice_buff,
        int LayerSize, int remlayerSize)
{
    unsigned int nbytesEmulated = 0;
    unsigned char * pdata = slice_buff + 2;
    int len = 2;

    while (len < LayerSize - 2) {
        if (*(pdata) == 0) {
            if (*(pdata + 1) == 0) {
                if ((*(pdata + 2) == 1) || (*(pdata + 2) == 3)) {

                    PutEmulation(slice_buff, pdata, len, remlayerSize);
                    CLayerUtils::IncLayerSize(slice_buff);

                    nbytesEmulated += 1;
                    LayerSize += 1;
                    remlayerSize += 1;
                }
            }
        }
        pdata += 1;
        len += 1;
    }

    return nbytesEmulated;
}

void CEmulationPrevention::PutEmulation(unsigned char *slice_buff, unsigned char *pdata,
        int len, int remlayerSize)
{
    unsigned char * tempbuff = new unsigned char[remlayerSize + 2];
    *tempbuff = 0x03;
    memcpy(tempbuff + 1, pdata + 2, remlayerSize - (len + 1));
    memcpy(slice_buff + (len + 2), tempbuff, remlayerSize - (len + 1));
    delete[] tempbuff;

}

int CEmulationPrevention::DeemulateSlice(unsigned char *slice_buff , int SliceLen){

    unsigned int nbytesDeEmulated = 0;
    unsigned int totalbytesDeEmulated = 0;
    unsigned char *pdata = slice_buff + 4;
    int len = 0;
    int remlayerSize = SliceLen - 4;

    if ((SliceLen - 6) <= 0) {
        printf("Slice length is too short :%d\n",SliceLen);
        return -1;
    }
    while (len < SliceLen - 6) {
        int layersize = CLayerUtils ::GetLayerSize(pdata) ;
        int layerflag = ((*(pdata + 2)) >> 7) & 0x01;
        if (layersize > SliceLen || layersize == 0) {
            printf("Wrong Handling of DeEmulation Data :%d,%d ,%d\n",layersize,SliceLen, remlayerSize);
            return -1;
        }
        nbytesDeEmulated = ParseForDeEmulation(pdata, layersize, remlayerSize);
        len += layersize - nbytesDeEmulated;
        pdata += layersize - nbytesDeEmulated;
        SliceLen -= nbytesDeEmulated;
        remlayerSize -= layersize;
        totalbytesDeEmulated += nbytesDeEmulated;

        if(layerflag == 1)
            break ;

    }

    return totalbytesDeEmulated;

}

int CEmulationPrevention::ParseForDeEmulation(unsigned char *slice_buff,
        int LayerSize, int remlayerSize) {

    unsigned int nbytesEmulated = 0;
    unsigned char * pdata = slice_buff + 2;
    int len = 2;

    while (len < LayerSize - 2) {
        if (*(pdata) == 0) {
            if (*(pdata + 1) == 0) {
                if (*(pdata + 2) == 0x03) {

                    memcpy(slice_buff + (len + 2), slice_buff + (len + 3),
                            remlayerSize - (len + 3));

                    CLayerUtils::DecLayerSize(slice_buff);
                    nbytesEmulated += 1;
                    LayerSize -= 1;
                    remlayerSize -= 1;
                }
            }
        }
        pdata += 1;
        len += 1;
    }

    return nbytesEmulated;
}




