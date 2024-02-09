#pragma once


#ifndef TEST_MEM_MODULE_H
#define TEST_MEM_MODULE_H

#include "mem_model.h"

class test_mem_module : public mem_model
{
    public:
        test_mem_module(int address_width, int data_width_bytes,int offset);
		~test_mem_module();

		virtual unsigned char& operator[] (unsigned int index);

		int setBytes(unsigned int offset, unsigned char * data, unsigned int length);
        int getBytes(unsigned int offset, unsigned char * data, unsigned int length);
		int setByte(unsigned int offset, unsigned char data);
		unsigned char getByte(unsigned int offset);

		virtual char * getDevName() 
			{return "test memory module"; };

    protected:
    private:
        unsigned char * buffer;



};

class fault_mem_module_databus : public test_mem_module
{
    public:

        enum databus_fault_type {
            DATABUS_FAULT_NONE,
            DATABUS_FAULT_STUCKLOW,
            DATABUS_FAULT_STUCKHIGH,
            DATABUS_FAULT_SHORT
        };

        fault_mem_module_databus (int address_width, int data_width_bytes,int offset,databus_fault_type fault);

        unsigned char& operator[] (unsigned int index);
		virtual char * getDevName() 
			{return "data bus fault memory module"; };


    protected:
    private:
        unsigned char * buffer;
        databus_fault_type _fault;

};

class fault_mem_module_addrbus : public test_mem_module
{
    public:

        enum addrbus_fault_type {
            ADDRBUS_FAULT_NONE,
            ADDRBUS_FAULT_STUCKLOW,
            ADDRBUS_FAULT_STUCKHIGH,
            ADDRBUS_FAULT_SHORT
        };

        fault_mem_module_addrbus (int address_width, int data_width_bytes,int offset,addrbus_fault_type fault);

        unsigned char& operator[] (unsigned int index);
		virtual char * getDevName() 
			{return "address bus fault memory module"; };


    protected:
    private:
        unsigned char * buffer;
        addrbus_fault_type _fault;

};

class fault_mem_module_cell : public test_mem_module
{
    public:

        enum device_fault_type {
            DEVICE_FAULT_NONE,
            DEVICE_FAULT_BITHIGH,
            DEVICE_FAULT_BITLOW,
            DEVICE_FAULT_ALIAS
        };

        fault_mem_module_cell (int address_width, int data_width_bytes,int offset,device_fault_type fault);

        unsigned char& operator[] (unsigned int index);
		virtual char * getDevName() 
			{return "cell fault memory module"; };

    protected:
    private:
        unsigned char * buffer;
        unsigned int _fault;

};

class fault_mem_module_random : public test_mem_module
{
    public:

        enum random_fault_type {
            RANDOM_FAULT_NONE,
            RANDOM_FAULT_RANDOM,
            RANDOM_FAULT_NOTSIG
        };

        fault_mem_module_random(int address_width, int data_width_bytes,int offset,random_fault_type fault);

        unsigned char& operator[] (unsigned int index);
		virtual char * getDevName() 
			{return "random fault memory module"; };

    protected:
    private:
        unsigned char * buffer;
        unsigned char   fault_value;
        unsigned int _fault;

};


#endif // TEST_MEM_MODULE_H
