#include "netviz_mem_module.h"

netviz_mem_module::netviz_mem_module(int address_width, int data_width_bytes) :
	mem_model(address_width, data_width_bytes, 0) 
{

  pInterface = newDMAInterface(0);
  if (pInterface == NULL)
    throw ("Couldn't get DMA Interface!\n");

  pChannel  = pInterface->newDmaChannel(0,false,true);
  if (pChannel == NULL)
    throw ("Couldn't get DMA Channel!\n");


}



netviz_mem_module::~netviz_mem_module()
{
  delete pChannel;
  deleteDMAInterface(pInterface);

}

int netviz_mem_module::getBytes(unsigned int offset, unsigned char *data, unsigned int length)
{
  return pChannel->MemoryXFer(data, offset, length,false);
}

int netviz_mem_module::setBytes(unsigned int offset, unsigned char *data, unsigned int length)
{
	return pChannel->MemoryXFer(data, offset, length,true);
}

unsigned char netviz_mem_module::getByte(unsigned int offset)
{
  unsigned int value;
  pChannel->MemoryXFer((unsigned char *)&value, offset, sizeof(unsigned char),false);
  return value;
}

int netviz_mem_module::setByte(unsigned int offset, unsigned char data)
{
  unsigned int value = data;
  pChannel->MemoryXFer((unsigned char *)&value, offset, sizeof(unsigned char),true);
  return value;
}
