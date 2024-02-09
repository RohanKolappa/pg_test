// SdramTest.cpp : Defines the entry point for the console application.
//

#include <stdlib.h>
#include <stdio.h>


#include "mem_model.h"
#include "test_mem_module.h"
#include "netviz_mem_module.h"
#include "SDRAMTests.h"
int conduct_test(mem_model * model_dut, bool random = true);

int main(int argc, char * argv[])
{
  //mem_model * model_dut = new test_mem_module(ADDR_WIDTH,DATA_WIDTH,0);
  //mem_model * model_dut = new fault_mem_module_databus(ADDR_WIDTH,DATA_WIDTH,0,fault_mem_module_databus::DATABUS_FAULT_STUCKHIGH);
  //mem_model * model_dut = new fault_mem_module_addrbus(ADDR_WIDTH,DATA_WIDTH,0,fault_mem_module_addrbus::ADDRBUS_FAULT_SHORT);
  //mem_model * model_dut = new fault_mem_module_cell(ADDR_WIDTH,DATA_WIDTH,0,fault_mem_module_cell::DEVICE_FAULT_ALIAS);
  //mem_model * model_dut = new fault_mem_module_random(ADDR_WIDTH,DATA_WIDTH,0,fault_mem_module_random::RANDOM_FAULT_NOTSIG);
  mem_model * model_dut = new netviz_mem_module(17,8);

  int result = conduct_test(model_dut,false);		

	if (result)
		printf("Test Complete: Failed memory\n");
	else
		printf("Test Complete: Passed\n");

	return 0;
}
	
int conduct_test(mem_model * model_dut, bool random)
{

	printf("\nTesting device: %s\n",model_dut->getDevName());

#if 0 //Check write to RAM bytes
	for (int i = 0 ; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			model_dut->setByte(i*16+j,i*16+j);
		}
		printf("\n");
	}

	printf ("First 256 bytes of RAM is:\n");
	for (int i = 0 ; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			printf("0x%02x ",model_dut->getByte(i*16+j));
		}
		printf("\n");
	}
#endif

    if (data_bus_test(model_dut))
    {
        printf("************* Data bus test failed *************\n");
        return -1;
    }
    printf("Data bus test passed!\n");


    if(addr_bus_test(model_dut))
    {
        printf("************* Address bus test failed *************\n");
        return -1;
    }
    printf("Address bus test passed!\n");

    //if (int result = device_test(model_dut))
	if (int result = device_test_fast(model_dut))
    {
        printf("************* Device test failed for %d locations *************\n",result);
        return -1;
    }
    printf("Device test passed\n");

    if (signal_integrity_test(model_dut))
    {
		printf("************* Signal integrity test failed *************");
		return -1;
    }
    printf("Signal integrity test passed\n");

	if (random == true)
	{
		if (random_test(model_dut))
		{
			printf("************* random test failed *************");
			return -1;
		}
		printf("Random test passed\n");
	}
	return 0;

}

