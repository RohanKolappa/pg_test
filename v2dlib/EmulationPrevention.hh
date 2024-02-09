/*
 * EmulationPrevention.hh
 *
 *  Created on: Mar 1, 2012
 *      Author: User
 */

#ifndef EMULATIONPREVENTION_HH_
#define EMULATIONPREVENTION_HH_

class CEmulationPrevention
{
public:

    static int EmulateSlice(unsigned char *slice_buff, int SliceLen);
    static int DeemulateSlice(unsigned char * slice_buff, int SliceLen);

private:
    /* function for Emulation */
    static int ParseForEmulation(unsigned char *slice_buff, int LayerSize,
            int remlayerSize);

    /* function for DeEmulation */
    static int ParseForDeEmulation(unsigned char *slice_buff, int LayerSize,
            int remlayerSize);

    static void PutEmulation(unsigned char *slice_buff, unsigned char *pdata,
            int len, int remlayerSize);


};

#endif /* EMULATIONPREVENTION_HH_ */
