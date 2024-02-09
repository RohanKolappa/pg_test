#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>


#include "SDRAMTests.h"
#include "test_mem_module.h"
#include "mem_model.h"

static unsigned int lfsr_reg = 1;

unsigned int lfsr(unsigned int set)
{
    if (set)
        lfsr_reg = set;
    else
        lfsr_reg = (lfsr_reg >> 1) ^ (unsigned int)((0 - (lfsr_reg & 1u)) & 0xd0000001u);

    return lfsr_reg;
}

void printDataBits(
		mem_model * model_dut, 
		unsigned int byte, 
		unsigned char data
	)
{
	for (unsigned char i = 0; i <8; i++)
	{
		if (data & (1<<i))
			printf("Data line: %s\n", model_dut->getDataPin(byte*8+i));
	}
}
int data_bus_test(mem_model * model_dut)
{

	if (!model_dut)
	{
		printf("Device model is NULL!");
		exit(0);
	}

    //Attempt to set each bit of the data bus to a high, then a low value
    unsigned int byte = 0;
	unsigned int width = model_dut->data_width_bytes();
    //Set to zero first and check for bits stuck high
	unsigned char pattern = 0;
	unsigned char * bytes = (unsigned char* )memalign(sizeof(uint32_t),sizeof(unsigned char) * width);    

	unsigned char byte_mask = 0xFF;

	//Set all bytes at once
	memset(bytes,pattern,width);
	model_dut->setBytes(0,bytes,width);
	
	pattern = 1;
	int failed = 0;
    while (pattern != 0)
    {
		memset(bytes,pattern,width);
        model_dut->setBytes(0,bytes,width);
		model_dut->getBytes(0,bytes,width);
		for (unsigned int byte = 0; byte < width; byte++)
		{
			byte_mask = model_dut->byte_mask(byte);
			if ((pattern & byte_mask) && ((bytes[byte] & byte_mask) != (pattern & byte_mask)))	//If we need to test this bit, and it doesn't match
			{
				printf("Failed data bus test: bit(s) stuck low: byte %d bit 0x%08x\n",byte,pattern);
				printDataBits(model_dut,byte,pattern);				
				failed++;
			}
			else if ((model_dut->getByte(byte) & (~pattern)) & byte_mask)
			{
				printf("Failed data bus test: bit(s) shorted: byte %d bit 0x%08x\n",byte,model_dut->getByte(byte));
				printDataBits(model_dut,byte,model_dut->getByte(byte));
				failed++;
			}
		}

        pattern <<= 1;

    }
	free(bytes);
    return failed;
}

void printAddrBits(
		mem_model * model_dut, 
		unsigned int addr
	)
{
	unsigned int i = 0;
	while (addr)
	{
		if (addr & 0x1)
			printf("Address line: %s\n",model_dut->getAddrPin(i));
		i++;
		addr >>= 1;
	}
}

int addr_bus_test(mem_model * model_dut)
{

	if (!model_dut)
	{
		printf("Device model is NULL!");
		exit(0);
	}
	
	//Work out actual address range we need to test:
    int addr_start = model_dut->address_lsb();
    int addr_end   = model_dut->address_width();

    unsigned char pattern        = 0xAA;
    unsigned char antipattern    = 0x55;


    //Stuck bits test:
    //Set all power of two addresses to pattern - note we only set one byte from each address
    for (int i = addr_start; i <= addr_end; i++)
    {
		model_dut->setByte(1<<i,pattern);
		//printf("0x%08x ",1<<i);
    }

	//printf("\n");
    //check for address bits stuck highprintf binary

    //Set bytes at zero:
	model_dut->setByte(0,antipattern);

    //If any of the power of two addresses show the anti-pattern then we know there is a bit stuck in the address.
    //I think that because what you are effectively doing is halving the address space it's not possible to tell
    //whether the bit is stuck high or low from the results.
    for (int i = addr_start; i <= addr_end; i++)
    {
		//printf("0x%08x ",1<<i);
        if (model_dut->getByte(1<<i) != pattern)
        {
            printf("Failed address bus test: bit %d stuck!\n",i);
			printAddrBits(model_dut,1<<i);
            return -1;
        }
    }
	//printf("\n");
    //Check for low/shorted bits
    //set byte 0 to pattern - all addresses now have pattern set
	model_dut->setByte(0,pattern);
    for (int i = addr_start; i <= addr_end; i++)
    {
        //set current byte to antipattern
		model_dut->setByte(1<<i,antipattern);
		//printf("0x%08x:  ",1<<i);

		//If the 0 addres gets altered then one of the address bits is stuck low
		if (model_dut->getByte(0) != pattern)    
        {
            printf("Failed address bus test: bit 0x%08x stuck low!\n",i);
			printAddrBits(model_dut,1<<i);
            return -1;
        }

        //Now to test for shorted bits
        for (int j = addr_start; j <= addr_end; j++)
        {
			//printf("0x%08x ",1<<j);
            //If any other bytes have been changed we have a short between two lines
			if ((model_dut->getByte(1 << j) != pattern) && (j != i)) 
            {
                printf("Failed address bus test: bits 0x%08x 0x%08x shorted\n",i,j);
				printAddrBits(model_dut,1<<i);
				printAddrBits(model_dut,1<<j);
                return -1;
            }
        }
		//printf("\n");
		model_dut->setByte(1 << i,pattern);

    }

    return 0;
}

int testint64(
	  uint64_t l, 
	  uint64_t k, 
	  mem_model * model_dut
	)
{
	for (int i = 0 ; 
		 i<(sizeof(uint64_t)/sizeof(unsigned char))
		 ;i++)
	{
		unsigned char l_c = (l >> (8*i)) & 0xFF;
		unsigned char k_c = (l >> (8*i)) & 0xFF;

		if ((l_c ^ k_c) & model_dut->byte_mask(i))
			return i;
	}
	return 0;
}

//NOTE: This makes the assumption that k and l are aligned correctly (e.g. byte_mask(0) applies to *j)
int testbytes(unsigned char * l, unsigned char * k, unsigned int length, mem_model * model_dut)
{
	for (unsigned int i =0 ; i < length; i++)
	{
		if ((*(l+i) ^ *(k+i)) & model_dut->byte_mask(i))
			return i;
	}
	return 0;
}

int device_test(mem_model * model_dut)
{
    //Write a value to every cell in the device, then write the inverse
    //confirm correct operation of every memory cell

	if (!model_dut)
	{
		printf("Device model is NULL!");
		exit(0);
	}

    unsigned int failed = 0;

    //write a count to each address
    uint64_t k =0,l = 0, kinv = -1;
	for (unsigned int i = 0; i < model_dut->buffer_size(); i+= model_dut->data_width_bytes())
    {
        if (++k == 0) k = 1;	//This is probably a bit unlikely now...
		//model_dut->setByte(i,k);
		model_dut->setBytes(i,(unsigned char *)&k,model_dut->data_width_bytes());
    }

    //Check the value and write it's inverse
    k = 0;
	for (unsigned int i = 0; i < model_dut->buffer_size(); i+= model_dut->data_width_bytes())
    {
        if (++k == 0) k = 1;

		model_dut->getBytes(i,(unsigned char *)&l,model_dut->data_width_bytes());
		//if (l != k)
		if (testint64(l,k,model_dut))
        {
            printf("Failure at address: %d\n",i);
            failed++;
        }



		kinv = ~k;
		model_dut->setBytes(i,(unsigned char *)&kinv,model_dut->data_width_bytes());
    }

    if (failed) return failed;

    //Check each inverted value and then zero
    k = 0;
	for (unsigned int i = 0; i < model_dut->buffer_size(); i+=model_dut->data_width_bytes())
    {
        if (++k == 0) k = 1;
		
		model_dut->getBytes(i,(unsigned char *)&l,model_dut->data_width_bytes());
        //if (l != ~k)
		if (testint64(l,~k,model_dut))
        {
            printf("Failure at address: %d\n",i);
            failed++;
        }

		model_dut->setByte(i,0);
    }
    return failed;
}

void transfer_block
(
	mem_model * model_dut, 
	unsigned int offset,
	unsigned char * buffer,
	unsigned int size,
	bool write,
	bool offsetBuffer = true
	)
{
	//Break up a large transfer into bite size chunks (really big transfers
	//timeout in the DMA
	unsigned int sent =0;
	while (size >= 32*1024*1024)		//32 MB Chunks
	{
		if (size == 0)
		{
			printf("WTF!");
			break;
		}

		if (write)
			model_dut->setBytes(sent,
								buffer+ (offsetBuffer ? sent : 0),
								32*1024*1024);
		else
			model_dut->getBytes(sent,
								buffer+ (offsetBuffer ? sent : 0),
								32*1024*1024);

		size -= 32*1024*1024;
		sent += 32*1024*1024;
	}
	if (size)
	{
		if (write)
		{
			model_dut->setBytes(sent,
								buffer+ (offsetBuffer ? sent : 0),
								size);
		}
		else
		{
			model_dut->getBytes(sent,
								buffer+ (offsetBuffer ? sent : 0),
								size);
		}
	}
}

int device_test_fast(mem_model * model_dut)
{
    unsigned int failed = 0;

	if (!model_dut)
	{
		printf("Device model is NULL!");
		exit(0);
	}

	printf("Device test (fast)...");

	//Speed up the device test by using the DMA a bit more effectively:
	unsigned char * buffer = (unsigned char *)memalign(sizeof(uint32_t),sizeof(unsigned char) * model_dut->buffer_size());


	if (!buffer)
	{
		printf("Couldn't allocate local buffer for test!\n");
		return -1;
	}



	printf("Filling test buffer...");

	//Fill the buffer instead...
    uint64_t k =0,l = 0, kinv = -1;
	for (unsigned int i = 0; i < model_dut->buffer_size(); i+= model_dut->data_width_bytes())
  {
    if (++k == 0) k = 1;	//This is probably a bit unlikely now...
    //memcpy(buffer+i,&k,sizeof(uint64_t)/sizeof(unsigned char));
    *((uint64_t *)(buffer+i))= k;
  }
	printf("Writing buffer...");
	//Write out
	//model_dut->setBytes(0,buffer,model_dut->buffer_size());
	transfer_block(model_dut,0,buffer,model_dut->buffer_size(),true);

	printf("Reading buffer...");
	//Read back
	//model_dut->getBytes(0,buffer,model_dut->buffer_size());
	transfer_block(model_dut,0,buffer,model_dut->buffer_size(),false);

	printf("Check and invert...");
    //Check the value and write it's inverse
    k = 0;
	for (unsigned int i = 0; i < model_dut->buffer_size(); i+= model_dut->data_width_bytes())
    {
        if (++k == 0) k = 1;

		memcpy(&l,buffer+i,sizeof(uint64_t));
		//if (l != k)
		if (testint64(l,k,model_dut))
        {
            printf("Failure at address: %d\n",i);
            failed++;
        }
		
		kinv = ~k;
		//model_dut->setBytes(i,(unsigned char *)&kinv,model_dut->data_width_bytes());
		memcpy(buffer+i,&kinv,sizeof(uint64_t)/sizeof(unsigned char));
    }
	
	printf("Writing buffer...");
	//Write out
	//model_dut->setBytes(0,buffer,model_dut->buffer_size());
	transfer_block(model_dut,0,buffer,model_dut->buffer_size(),true);

	printf("Reading buffer...");
	//Read back
	//model_dut->getBytes(0,buffer,model_dut->buffer_size());
	transfer_block(model_dut,0,buffer,model_dut->buffer_size(),false);

	printf("Check and zero...");

    //Check each inverted value and then zero
    k = 0; kinv = 0;
	for (unsigned int i = 0; i < model_dut->buffer_size(); i+=model_dut->data_width_bytes())
    {
        if (++k == 0) k = 1;
		
		memcpy(&l,buffer+i,sizeof(uint64_t));
        //if (l != ~k)
		if (testint64(l,~k,model_dut))
        {
            printf("Failure at address: %d\n",i);
            failed++;
        }

		memcpy(buffer+i,&kinv,sizeof(uint64_t)/sizeof(unsigned char));
    }

	printf("Writing buffer...");
	//Write out
	//model_dut->setBytes(0,buffer,model_dut->buffer_size());
	transfer_block(model_dut,0,buffer,model_dut->buffer_size(),true);

	free(buffer);

	return failed;


}

int signal_integrity_test(mem_model * model_dut)
{
    //This tests by writing all 0s and all 1s to the lowest and highest address repeatedly, to maximise power usage
	if (!model_dut)
	{
		printf("Device model is NULL!");
		exit(0);
	}

    //We want to test the entire width of the data bus at once here, so we use the setBytes call (which on a _real_ DMA/SDRAM target
    //should write the entire bus at once...)

    unsigned int addr_start = 1 << model_dut->address_start();
    unsigned int width      = model_dut->data_width_bytes();
    unsigned int addr_end   = model_dut->buffer_size() - width;    //Offset to make sure we don't overlap

    unsigned char * data[2];
    data[0] = (unsigned char *)memalign(sizeof(uint32_t),width * sizeof(unsigned char));
    data[1] = (unsigned char *)memalign(sizeof(uint32_t),width * sizeof(unsigned char));

    unsigned char * data_test = (unsigned char *)memalign(sizeof(uint32_t),width * sizeof(unsigned char));

    memset(data[0],0x00,width);
    memset(data[1],0xFF,width);


    unsigned int k = 0;


    for (int l = 0 ; l < SIGNAL_INTEGRITY_OUTER; l++)
    {
        printf("\r                                                             ");
		printf("\rSignal Integrity Pass: %d / %d",l+1,SIGNAL_INTEGRITY_OUTER);
        for (int j = 0 ; j < SIGNAL_INTEGRITY_MIDDLE; j++)
        {
            printf("."); fflush(stdout);
            for (int i = 0; i < SIGNAL_INTEGRITY_INNNER; i++)
            {

                //Set to alternating ones and zeroes
                model_dut->setBytes(0,data[~j&0x1],width);
                model_dut->setBytes(addr_end,data[j&0x1],width);

                //Check
                model_dut->getBytes(0,data_test,width);
                //if (memcmp(data_test,data[~j&0x1],width))
				if(testbytes(data_test,data[j&0x1],width,model_dut))
                {
                    printf("\nSignal integrity test failed\n");
					free(data[0]);
					free(data[1]);
					free(data_test);
                    return -1;
                }

                model_dut->getBytes(addr_end,data_test,width);
                //if (memcmp(data_test,data[j&0x1],width))
				if(testbytes(data_test,data[j&0x1],width,model_dut))
                {
                    printf("\nSignal integrity test failed\n");
					free(data[0]);
					free(data[1]);
					free(data_test);
                    return -1;
                }

            }
        }
    }

    printf("\n");
	
	free(data[0]);
	free(data[1]);
	free(data_test);
    return 0;
}

void set_data_lfsr(unsigned char * output, unsigned int length)
{
    //Fill buffer byte at a time - skip zeroes
    unsigned int rand = lfsr(0);
    while (length)
    {
        if (rand ==0)
            rand = lfsr(0);

        if (rand & 0xFF)
        {
            *output++ = rand & 0xFF;
            --length;
        }
        rand >>= 8;

    }

}

int random_test(mem_model * model_dut)
{
    //This will do a random number test - we need a prng to support this for a recreateable pattern.
	if (!model_dut)
	{
		printf("Device model is NULL!");
		exit(0);
	}

    //We want to test the entire width of the data bus at once here, so we use the setBytes call (which on a _real_ DMA/SDRAM target
    //should write the entire bus at once...)

    unsigned int addr_start = 1 << model_dut->address_lsb();
    unsigned int width      = model_dut->data_width_bytes();
    unsigned int addr_end   = model_dut->buffer_size() - width;

    unsigned char * test_data = (unsigned char*)memalign(sizeof(uint32_t),width * sizeof(unsigned char));
    unsigned char * rand_data = (unsigned char*)memalign(sizeof(uint32_t),width * sizeof(unsigned char));
    unsigned char * zero_data = (unsigned char*)memalign(sizeof(uint32_t),width * sizeof(unsigned char));

	unsigned int failed = 0;

  memset(zero_data,0,width);

	unsigned int j = rand();
	
	printf("Random test:\nRandom seed: %d\n",j);
    lfsr(j);	//Use a random start point each time...

	//how do we prevent ourselves from overlapping the address?
    //zero everything first:
	unsigned char * buffer = (unsigned char *)memalign(sizeof(uint32_t),sizeof(unsigned char) * 32*1024*1024);
	memset(buffer,0,sizeof(unsigned char) * 32*1024*1024);
	
	transfer_block(model_dut,0,buffer,model_dut->buffer_size(),true,false);

    for (unsigned int i = 0; i < RANDOM_TEST_SIZE; )
    {

        printf("\rCompleted: %d / %d",i,RANDOM_TEST_SIZE);
        unsigned int lfsr_start = lfsr(0);   //Get LFSR start value

        lfsr(lfsr_start); //reset the LFSR

        for (j = 0 ; j < 1 << 20; j++)
        {
            unsigned int addr = lfsr(0) & (model_dut->address_mask() - width);  //Knock a bit off the address mask to stop it overflowing

            //Stop on an overlap
            model_dut->getBytes(addr,test_data,width);
            if (memcmp(test_data,zero_data,width))
            {
                break;
            }

            //Fill out random data from lfsr and store it
            set_data_lfsr(rand_data,width);
            model_dut->setBytes(addr,rand_data,width);
        }

        //Reset LFSR
        lfsr(lfsr_start);
        i+=j;
        while(j--)  //Perform the same number of tests
        {
            unsigned int addr = lfsr(0) & (model_dut->address_mask() - width);  //Knock a bit off the address mask to stop it overflowing

            set_data_lfsr(rand_data,width);                  //Get our lfsr data

            //Stop on a mis-match
            model_dut->getBytes(addr,test_data,width);       //Get data from mem
            //if (memcmp(test_data,rand_data,width))
            //{
            //    printf("Random test: failure at %d\n",addr);
            //    return -1;
            //}
			for (unsigned int byte = 0; byte < width; byte++)
			{
				if ((test_data[byte] ^ rand_data[byte]) 
					& model_dut->byte_mask(byte))
				{
					printf("Random test: failure %d at address %d, byte: %d\n",
						++failed,addr,byte);
					
				}
			}


            //Reset to zero
            model_dut->setBytes(addr,zero_data,width);
        }
		
		if (i % (RANDOM_TEST_SIZE >> 10) == 0)
		{
			printf("\rCompleted: %d / %d",i>>8,(RANDOM_TEST_SIZE >> 18));
		}

    }

    printf("\n");

	free(test_data); test_data = 0;
	free(rand_data); rand_data = 0;
	free(zero_data); zero_data = 0;
	free(buffer); buffer =0;


    return failed;
}
