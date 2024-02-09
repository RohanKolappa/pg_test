/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        gpio.c
*
*    DESCRIPTION:
*        functions for GPIO pin operation
*   
*
*    AUTHOR:
*	
*
*    <SOURCE CONTROL TAGS TBD>
*
******************************************************************************/
 
#include "gpio.h"
#include "function.h"

#define ADDR_GPIO_OEJ	0x3c80
#define ADDR_GPIO_IN	0x3c81
#define ADDR_GPIO_OUT	0x3c82

#define DIR_IN				0
#define DIR_OUT				1 


/// Mode:
//		0 -- OnChip GPIO
//		1 -- Cypress GPIO

SINT32	GPIO_WritePins(PDEVICE_EXTENSION_COMMON pdxc, SINT32 WriteNum, SINT32 *Index, SINT32 *Value, SINT32 Mode)
{
	UINT32 Mask;
	int i;

	PrintMsg(DEBUG_GPIO, ("GPIO_WritePins : %d %d", WriteNum, Mode));
	for(i=0; i<WriteNum; i++)
	{
		PrintMsg(DEBUG_GPIO, (">>> %d %d", Index[i], Value[i]));
	}

	if(Mode == GM_ONCHIP)
	{
		UINT16 Data;

		PrintMsg(DEBUG_GPIO, ("WriteGPIOPin, WriteNum : %d",WriteNum));
		for(i=0;i<WriteNum;i++)
		{
			PrintMsg(DEBUG_GPIO, ("WriteGPIOPin, Index : %d, Value : %d",Index[i],Value[i]));
		}

		if(ReadCBusRegFW(pdxc, ADDR_GPIO_OUT, &Data)!=0)
			return 1;

		for(i=0;i<WriteNum;i++)
		{
			Mask = 1<<Index[i];
			if(Value[i]==0)
				Data &= ~Mask;
			else
				Data |= Mask;
		}

		if(WriteCBusRegFW(pdxc, ADDR_GPIO_OUT, Data) != 0)
			return 2;

		if(ReadCBusRegFW(pdxc, ADDR_GPIO_OEJ, &Data)!=0)
			return 3;

		for(i=0;i<WriteNum;i++)
		{
			Mask = 1<<Index[i];
			Data &= ~Mask;
		}

		if(WriteCBusRegFW(pdxc, ADDR_GPIO_OEJ, Data) != 0)
			return 4;

		return 0;
	}

	else if(Mode == GM_CYPRESS)
	{
		UINT8 Data[2];

		if(pdxc->hw_funcs.DoVendorRequest(pdxc, 0x41, 0, 0, Data, 2, DIR_IN)!=0)
			return 1;

		Data[1] = 0;

		for(i=0;i<WriteNum;i++)
		{
			Mask = 1<<Index[i];
			if(Value[i]==0)
				Data[0] &= ~Mask;
			else
				Data[0] |= Mask;

			Data[1] |= Mask;
		}

		if(pdxc->hw_funcs.DoVendorRequest(pdxc, 0x40, NULL, ((UINT16)Data[1]<<8)+Data[0], NULL, 0, DIR_OUT)!=0)
			return 2;

		return 0;
	}

	return 0;
}

/// Mode:
//		0 -- OnChip GPIO
//		1 -- Cypress GPIO

SINT32	GPIO_ReadPins(PDEVICE_EXTENSION_COMMON pdxc, SINT32 ReadNum, SINT32 *Index, SINT32 *Value, SINT32 Mode)
{
	int i;

	PrintMsg(DEBUG_GPIO, ("GPIO_ReadPins : %d %d", ReadNum, Mode));
	for(i=0; i<ReadNum; i++)
	{
		PrintMsg(DEBUG_GPIO, (">>> %d", Index[i]));
	}

	if(Mode == GM_ONCHIP)
	{
		UINT16 Data;

		if(ReadCBusRegFW(pdxc, ADDR_GPIO_IN, &Data)!=0)
			return 1;

		for(i=0;i<ReadNum;i++)
		{
			Value[i] = (Data>>Index[i]) & 0x0001;
		}
	}

	else if(Mode == GM_CYPRESS)
	{
		UINT8 Data[2];
	
		if(pdxc->hw_funcs.DoVendorRequest(pdxc, 0x41, 0, 0, Data, 2, DIR_IN)!=0)
			return 1;
		
		for(i=0;i<ReadNum;i++)
		{
			Value[i] = (Data[0]>>Index[i]) & 0x0001;
		}
	}

	for(i=0; i<ReadNum; i++)
	{
		PrintMsg(DEBUG_GPIO, ("<<< %d %d", Index[i], Value[i]));
	}

	return 0;
}

/******************************** end of gpio.c ******************************/
