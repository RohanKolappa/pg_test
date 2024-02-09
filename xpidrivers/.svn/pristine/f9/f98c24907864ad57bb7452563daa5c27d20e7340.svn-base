#pragma once


#ifndef netviz_mem_MODULE_H
#define netviz_mem_MODULE_H

#include "mem_model.h"

#include "../api/dma_interface.h"

class netviz_mem_module : public mem_model
{
    public:
        netviz_mem_module(int address_width, int data_width_bytes);
		~netviz_mem_module();

		int setBytes(unsigned int offset, unsigned char * data, unsigned int length);
        int getBytes(unsigned int offset, unsigned char * data, unsigned int length);
		int setByte(unsigned int offset, unsigned char data);
		unsigned char getByte(unsigned int offset);

		virtual char * getDevName() 
			{return "NetViz memory module"; };

    protected:
    private:
        unsigned char * buffer;
        IDMAInterface * pInterface;
        IDMAChannel   * pChannel;



};

#endif
