
#include <stdlib.h>
#include <stdio.h>

#include "test_mem_module.h"

test_mem_module::test_mem_module(int address_width,int data_width_bytes,int offset) : mem_model(address_width,data_width_bytes,offset), buffer(0)
{
    //ctor
    buffer = (unsigned char *)malloc(sizeof(char)*buffer_size());

}

test_mem_module::~test_mem_module()
{
	if (buffer)
		free(buffer);
	buffer = 0;
}

unsigned char& test_mem_module::operator[] (unsigned int index)
{

    if (index >= buffer_size())
    {
        printf("Error: Buffer index out of range!\n");
        throw "Error: Buffer index out of range!";
    }

    if (buffer)
        return buffer[index];


    printf("Error: Buffer not initialized!\n");
    return buffer[index];

}

//void test_mem_module::getBytes(unsigned int offset, unsigned char * data, unsigned int length )
//{
//        if (offset+length <= buffer_size())
//            memcpy(data,buffer+offset,length);
//
//}
//
//void test_mem_module::setBytes(unsigned int offset, unsigned char * data, unsigned int length )
//{
//        if (offset+length <= buffer_size())
//            memcpy(buffer+offset,data,length);
//}

unsigned char test_mem_module::getByte(unsigned int offset)
{
	return (*this)[offset];
}
int test_mem_module::setByte(unsigned int offset, unsigned char data)
{
	(*this)[offset] = data;
	return 0;
}

int
test_mem_module::setBytes(unsigned int offset, unsigned char * data, unsigned int length)
{
    //Block write to several (sequential) offsets.  This models bus widths that are a multiple of bytes wide.
    int leftOver = 0;
    if ((leftOver = (length % data_width_bytes())))
    {
        printf("SetBytes size does not align to data bus width!\n");
    }

    //For this model we operate by simply setting each offset in turn
    for (unsigned int i = 0; i < length; i++)
		setByte(offset+i,data[i]);

	return 0;
}

int
test_mem_module::getBytes(unsigned int offset, unsigned char * data, unsigned int length)
{
    //Block write to several (sequential) offsets.  This models bus widths that are a multiple of bytes wide.
    int leftOver = 0;
    if ((leftOver = (length % data_width_bytes())))
    {
        printf("SetBytes size does not align to data bus width!\n");
    }

    //For this model we operate by simply setting each offset in turn
    for (unsigned int i = 0; i < length; i++)
        data[i] = getByte(offset+i);

	return 0;
}



//************************************ DATA BUS FAULT MODULE ****************************//

fault_mem_module_databus::fault_mem_module_databus(int address_width,int data_width_bytes,int offset,databus_fault_type fault) : test_mem_module(address_width,data_width_bytes,offset), buffer(0), _fault(fault)
{
    //ctor
    buffer = (unsigned char *)malloc(sizeof(unsigned char)*buffer_size());

}

unsigned char& fault_mem_module_databus::operator[] (unsigned int index)
{

    if (index >= buffer_size())
    {
        printf("Error: Buffer index out of range!\n");
        throw "Error: Buffer index out of range!";
    }

    //Create the 'fault' -
    if (_fault == DATABUS_FAULT_STUCKLOW)
        buffer[index] &= ~4;    //Bit 4 will always be low
    if (_fault == DATABUS_FAULT_STUCKHIGH)
        buffer[index] |= 4;    //Bit 4 will always be high
    if (_fault == DATABUS_FAULT_SHORT)
    {
        buffer[index] = buffer[index] | (buffer[index] & 0x8 ? 0x4 : 0);        //If bit 3 is set, then set bit 4 (short)
        buffer[index] = buffer[index] | (buffer[index] & 0x4 ? 0x8 : 0);        //If bit 3 is set, then set bit 4 (short)

    }

    if (buffer)
        return buffer[index];


    printf("Error: Buffer not initialized!\n");
    return buffer[0];
}

//************************************ ADDRESS BUS FAULT MODULE ****************************//

fault_mem_module_addrbus::fault_mem_module_addrbus(int address_width,int data_width_bytes,int offset,addrbus_fault_type fault) : test_mem_module(address_width,data_width_bytes,offset), buffer(0), _fault(fault)
{
    //ctor
    buffer = (unsigned char *)malloc(sizeof(unsigned char)*buffer_size());

}

unsigned char& fault_mem_module_addrbus::operator[] (unsigned int index)
{

    unsigned int localindex = index;

    if (index >= buffer_size())
    {
        printf("Error: Buffer index out of range!\n");
        throw "Error: Buffer index out of range!";
    }

    //Create the 'fault' -
    if (_fault == ADDRBUS_FAULT_STUCKLOW)
        localindex &= ~(1 << (address_start() + 2));

    if (_fault == ADDRBUS_FAULT_STUCKHIGH)
        localindex |= (1 << (address_start() + 2));

    if (_fault == ADDRBUS_FAULT_SHORT)
    {
        if ((localindex & (1 << (address_start() + 2))) || (localindex & (1 << (address_start() + 4))))
            localindex |= (1 << (address_start() + 4)) | (1 << (address_start() + 2));
    }


    if (buffer)
        return buffer[localindex];


    printf("Error: Buffer not initialized!\n");
    return buffer[0];

}

//************************************ CELL FAULT MODULE ****************************//

fault_mem_module_cell::fault_mem_module_cell(int address_width,int data_width_bytes,int offset,device_fault_type fault) : test_mem_module(address_width,data_width_bytes,offset), buffer(0), _fault(fault)
{
    //ctor
    buffer = (unsigned char *)malloc(sizeof(unsigned char)*buffer_size());

}

unsigned char& fault_mem_module_cell::operator[] (unsigned int index) 
{

    unsigned int localindex = index;

    if (index >= buffer_size())
    {
        printf("Error: Buffer index out of range!\n");
        throw "Error: Buffer index out of range!";
    }

    //Create the 'fault' -
    if ((_fault == DEVICE_FAULT_BITHIGH) && ((index % 123) == 0) && (index >= 123))
        buffer[index] |= 0x4;

    if ((_fault == DEVICE_FAULT_BITLOW) && ((index % 321) == 0) && (index >= 321))
        buffer[index] &= ~0x4;

    if ((_fault == DEVICE_FAULT_ALIAS) && ((index % 213) == 0) && (index >= 213))
        buffer[index] = buffer[index + 1];


    if (buffer)
        return buffer[localindex];


    printf("Error: Buffer not initialized!\n");
    return buffer[0];
}

//************************************ RANDOM FAULT MODULE ****************************//

fault_mem_module_random::fault_mem_module_random(int address_width,int data_width_bytes,int offset,random_fault_type fault) : test_mem_module(address_width,data_width_bytes,offset), buffer(0), _fault(fault)
{
    //ctor
    buffer = (unsigned char *)malloc(sizeof(unsigned char)*buffer_size());

}

unsigned char& fault_mem_module_random::operator[] (unsigned int index)
{

    unsigned int localindex = index;

    if (index >= buffer_size())
    {
        printf("Error: Buffer index out of range!\n");
        throw "Error: Buffer index out of range!";
    }

    //Create the 'fault' -
    if (_fault == RANDOM_FAULT_RANDOM)
    {
        unsigned int rand_num = rand();

        if (rand_num < (RAND_MAX >> 26))       //This can be made to take forever - by increasing the size of the shift we decrease the likelihood of a fault
        {
            printf("\nInjected Fault ******************************************************* %d\n",rand_num);
            return fault_value;
        }

    }

    if (_fault ==RANDOM_FAULT_NOTSIG)
    {
        if ((index > data_width_bytes()) && (index <= (buffer_size() - data_width_bytes()))
)       {
            unsigned int rand_num = rand();

            if (rand_num < (RAND_MAX >> 26))       //This can be made to take forever - by increasing the size of the shift we decrease the likelihood of a fault
            {
                printf("\nInjected Fault ******************************************************* %d %d\n",rand_num,index );
                return fault_value;
            }

        }
    }


    if (buffer)
        return buffer[localindex];


    printf("Error: Buffer not initialized!\n");
    return buffer[0];

}

