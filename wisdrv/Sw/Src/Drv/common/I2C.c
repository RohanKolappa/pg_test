/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        I2C.cpp
*
*    DESCRIPTION:
*        
*   
*
*    AUTHOR:
*	
*
*    <SOURCE CONTROL TAGS TBD>
*
******************************************************************************/

#include "platform.h"
#include "I2C.h"
#include "function.h"
#include "usb_base.h"

#define SPI_I2C_ADDR_BASE 0x1400

#define STATUS_REG_ADDR				(SPI_I2C_ADDR_BASE + 0x2)

#define I2C_CTRL_REG_ADDR			(SPI_I2C_ADDR_BASE + 0x6)
#define I2C_DEV_UP_ADDR_REG_ADDR	(SPI_I2C_ADDR_BASE + 0x7)
#define I2C_LO_ADDR_REG_ADDR		(SPI_I2C_ADDR_BASE + 0x8)
#define I2C_DATA_REG_ADDR			(SPI_I2C_ADDR_BASE + 0x9)
#define I2C_CLKFREQ_REG_ADDR		(SPI_I2C_ADDR_BASE + 0xa)

#define I2C_STATE_MASK				0x0007
#define I2C_READ_READY_MASK			0x0008

#define SPI_CTRL_REG_ADDR			(SPI_I2C_ADDR_BASE + 0x0)
#define SPI_OPLEN_REG_ADDR			(SPI_I2C_ADDR_BASE + 0x1)
#define SPI_OPC_REG_ADDR			(SPI_I2C_ADDR_BASE + 0x3)
#define SPI_ADDR_REG_ADDR			(SPI_I2C_ADDR_BASE + 0x4)
#define SPI_DATA_REG_ADDR			(SPI_I2C_ADDR_BASE + 0x5)

#define SPI_STATE_MASK				0x0E00
#define SPI_READ_READY_MASK			0x0010

enum I2C_STATE
{
	IS_IDLE = 0,  
	// there are other states, but driver doesn't care
};
#ifdef __KERNEL__
#define	I2C_LOCK	\
        down (&pdxc->i2c_lock)
#define I2C_UNLOCK	 \
        up (&pdxc->i2c_lock)

#else
#define	I2C_LOCK	\
        KeWaitForMutexObject(&pdxc->I2CLock,Executive,KernelMode,FALSE,0)
#define I2C_UNLOCK	 \
        KeReleaseMutex(&pdxc->I2CLock,FALSE)
#endif

/*
	 i2c_mode:
		bit 15 == 1 -- Cypress mode
		bit 15 == 0 -- On Chip mode
*/

SINT32	I2C_WriteRegister(PDEVICE_EXTENSION_COMMON pdxc, UINT8 dev_addr, SINT32 addr_len, UINT16 reg_addr, UINT8 reg_value, SINT32 i2c_mode)
{
	UINT16 data;
	int r,i,ret;

	I2C_LOCK;

	PrintMsg(DEBUG_I2C, ("I2C_WriteRegister : %04x %04x %04x %d",dev_addr,reg_addr,reg_value,i2c_mode));

    SuspendDebugOutput();

	if((i2c_mode & 0x8000) != 0)	// Cypress mode 
	{
		UINT8 I2CData[4];
		UINT32 I2CLen;

		if(addr_len==8)
		{
			I2CData[0] = (UINT8)reg_addr;
			I2CData[1] = (UINT8)reg_value;
			I2CLen = 2;
		}
		else
		{
			I2CData[0] = (UINT8)((reg_addr>>8) & 0xff);
			I2CData[1] = (UINT8)(reg_addr & 0xff);
			I2CData[2] = (UINT8)reg_value;
			I2CLen = 3;
		}

		ret = (pdxc->hw_funcs.DoVendorRequest)(pdxc, 0x20, 0, dev_addr>>1, I2CData, I2CLen, DIR_OUT);
		goto _Exit;
	}

	else							// On Chip mode							
	{
		// please refer to Specification for Module spi_i2c_master

		// to avoid too much debug message, we low the DebugLevel here and restore the DebugLevel when function returns

		// 1) Wait until i2c_state of spi_i2c_status_reg is IDLE;

		for(i=0;i<10;i++)
		{
			r = ReadCBusRegFW(pdxc, STATUS_REG_ADDR, &data);
			if(r != 0)
			{
				ret = 1;
				goto _Exit;
			}

			if( (data &	I2C_STATE_MASK) == IS_IDLE )
				break;

			PISleep(100);
		}
		if(i==10)
		{
			ret = 1;
			goto _Exit;
		}

		// 2) Write i2c_ctrl_reg if needed;

		data = 0x0000; 
		if(addr_len==16)
			data |= 0x0001; 
		r = WriteCBusRegFW(pdxc, I2C_CTRL_REG_ADDR, data);
		if(r != 0)
		{
			ret = 2;
			goto _Exit;
		}

		// 3) Write i2c_loaddr_reg if needed;
		r = WriteCBusRegFW(pdxc, I2C_LO_ADDR_REG_ADDR, reg_addr);
		if(r != 0)
		{
			ret = 3;
			goto _Exit;
		}

		// 4) Write i2c_data_reg
		r = WriteCBusRegFW(pdxc, I2C_DATA_REG_ADDR, reg_value);
		if(r != 0)
		{
			ret = 4;
			goto _Exit;
		}

		// 5) Write i2c_devaddr_upaddr_reg. 
		dev_addr /= 2;
		data = (((UINT16)dev_addr)<<9);
		if(addr_len==16)
			data |= (reg_addr>>8) & 0x00ff;

		r = WriteCBusRegFW(pdxc, I2C_DEV_UP_ADDR_REG_ADDR, data);
		if(r != 0)
		{
			ret = 5;
			goto _Exit;
		}

		ret = 0;
	}

_Exit:
	ResumeDebugOutput();
	I2C_UNLOCK;

	return ret;
}

/*
	 i2c_mode:
		bit 15 == 1 -- Cypress mode
		bit 15 == 0 -- On Chip mode
*/
SINT32	I2C_ReadRegister(PDEVICE_EXTENSION_COMMON pdxc, UINT8 dev_addr, SINT32 addr_len, UINT16 reg_addr, UINT8 *reg_value, SINT32 i2c_mode)
{
	int r = 0, ret;
	UINT16 data;
	int i;

	I2C_LOCK;
	
	PrintMsg(DEBUG_I2C, ("I2C_ReadRegister : %04x %04x %d",dev_addr,reg_addr,i2c_mode));
	SuspendDebugOutput();

	if((i2c_mode & 0x8000) != 0)	// Cypress mode 
	{
		UINT16 Value;
		Value = dev_addr>>1;
		if(addr_len==16)
			Value += 0x0100;
		ret = (pdxc->hw_funcs.DoVendorRequest)(pdxc, 0x22, reg_addr, Value, reg_value, 1, DIR_IN);
		goto _Exit;
	}

	else							// On Chip mode
	{
		// please refer to Specification for Module spi_i2c_master

		// 1) Wait until i2c_state of spi_i2c_status_reg is IDLE;

		for(i=0;i<10;i++)
		{
			r = ReadCBusRegFW(pdxc, STATUS_REG_ADDR, &data);
			if(r != 0)
			{
				ret = 1;
				goto _Exit;
			}

			if( (data &	I2C_STATE_MASK) == IS_IDLE )
				break;
		
			PISleep(100);
		}
		if(i==10)
		{
			ret = 1;
			goto _Exit;
		}

		// 2) Write i2c_ctrl_reg if needed;
		data = 0x0000; 

		if(i2c_mode==IM_SCCB)
			data |= 0x0010; 

		if(addr_len==16)
			data |= 0x0001; 

		r = WriteCBusRegFW(pdxc, I2C_CTRL_REG_ADDR, data);
		if(r != 0)
		{
			ret = 2;
			goto _Exit;
		}

		// 3) Write i2c_loaddr_reg if needed;
		r = WriteCBusRegFW(pdxc, I2C_LO_ADDR_REG_ADDR, reg_addr);
		if(r != 0)
		{
			ret = 3;
			goto _Exit;
		}

		// 4) read i2c_data_reg to clear i2c_rx_data_rdy;
		r = ReadCBusRegFW(pdxc, I2C_DATA_REG_ADDR, &data);
		if(r != 0)
		{
			ret = 4;
			goto _Exit;
		}

		// 5) Write i2c_devaddr_upaddr_reg. 

		dev_addr /= 2;
		data = (((UINT16)dev_addr)<<9) | 0x0100;
		if(addr_len==16)
			data |= (reg_addr>>8) & 0x00ff;

		r = WriteCBusRegFW(pdxc, I2C_DEV_UP_ADDR_REG_ADDR, data);
		if(r != 0)
		{
			ret = 5;
			goto _Exit;
		}

		// 6) check if i2c_rx_data_rdy is set

		for(i=0;i<10;i++)
		{
			r = ReadCBusRegFW(pdxc, STATUS_REG_ADDR, &data);
			if(r != 0)
			{
				ret = 6;
				goto _Exit;
			}

			if( (data &	I2C_READ_READY_MASK) != 0 )
				break;

			PISleep(100);
		}
		if(i==10)
		{
			ret = 6;
			goto _Exit;
		}

		// 7) read i2c_data_reg for result

		r = ReadCBusRegFW(pdxc, I2C_DATA_REG_ADDR, &data);
		if(r != 0)
		{
			ret = 7;
			goto _Exit;
		}

		*reg_value = (UINT8)data;
		ret = 0;
	}

_Exit:
	ResumeDebugOutput();
	PrintMsg(DEBUG_I2C, ("I2C_ReadRegister return --  reg_data : %04x", *reg_value));

	I2C_UNLOCK;

	return ret;
}

/* Write I2C registers in burst mode
	 i2c_mode:
		bit 15 == 1 -- Cypress mode
		bit 15 == 0 -- On Chip mode
			bit 1 -- MA1
			bit 0 -- MA0
*/

SINT32	I2C_BurstWriteRegister(PDEVICE_EXTENSION_COMMON pdxc, UINT8 dev_addr, SINT32 addr_width, UINT16 start_reg_addr, SINT32 reg_num, UINT8 *reg_value, SINT32 i2c_mode)
{
	PrintMsg(DEBUG_I2C, ("I2C_BurstWriteRegister : dev_addr %04x, reg_num %d, i2c_mode %d",dev_addr,reg_num,i2c_mode));
	DumpBuffer(reg_value, reg_num);

	if((i2c_mode & 0x8000) != 0)	// Cypress mode 
	{
		return (pdxc->hw_funcs.DoVendorRequest)(pdxc, 0x20, 0, dev_addr>>1, reg_value, reg_num, DIR_OUT);
	}

	else							// On Chip mode
	{
		int r = 0;
		UINT16 data;
		int i;

		if(reg_num<4)
			return 10;

		/* 1) Wait until i2c_state of spi_i2c_status_reg is IDLE;	*/

		for(i=0;i<10;i++)
		{
			r = ReadCBusRegFW(pdxc, STATUS_REG_ADDR, &data);
			if(r != 0)
				return 1;

			if( (data &	I2C_STATE_MASK) == IS_IDLE )
				break;
		
			PISleep(100);
		}
		if(i==10)
			return 1;

		/* 2) write 16'h003c to i2c_clkfreq_reg (address 15'h140a)	*/

		r = WriteCBusRegFW(pdxc, I2C_CLKFREQ_REG_ADDR, 0x003c);
		if(r != 0)
			return 2;

		/* 3) write 16'h000b to i2c_ctrl_reg (address 15'h1406)		*/
	
		r = WriteCBusRegFW(pdxc, I2C_CTRL_REG_ADDR, 0x000b);
		if(r != 0)
			return 3;

		/* 4) write {8'hxx, w_data3} to i2c_data_reg (address 15'h1409)	*/

		r = WriteCBusRegFW(pdxc, I2C_DATA_REG_ADDR, reg_value[2]);
		if(r != 0)
			return 4;

		/* 5) write {8'hxx, w_data2} to i2c_loaddr_reg (address 15'h1408), */

		r = WriteCBusRegFW(pdxc, I2C_LO_ADDR_REG_ADDR, reg_value[1]);
		if(r != 0)
			return 5;

		CHECK_INT_LOCK(pdxc);

		/* 6) write interrupt 0x0X40 to firmware */

		r = (pdxc->hw_funcs.WriteInterrupt)(pdxc, (UINT16)(0x0040|((i2c_mode&0x3)<<8)), (UINT16)(reg_value[0]*256+reg_value[3]) );
		if(r != 0)
		{
			CHECK_INT_UNLOCK(pdxc);
			return 6;
		}

		/* 7) should get interrupt 0x0040 back */

		r = CheckInterruptPipe(pdxc, 0x0040, -1);

		CHECK_INT_UNLOCK(pdxc);

		if(r < 0)
			return 7;

		return 0;
	}
}

// Read multiple I2C registers
SINT32	I2C_BurstReadRegister(PDEVICE_EXTENSION_COMMON pdxc, UINT8 dev_addr, SINT32 addr_width, UINT16 start_reg_addr, SINT32 reg_num, UINT8 *reg_value, SINT32 i2c_mode)
{
	PrintMsg(DEBUG_I2C, ("I2C_BurstReadRegister : dev_addr %04x, reg_num %d, i2c_mode %d",dev_addr,reg_num,i2c_mode));

	if((i2c_mode & 0x8000) != 0)	// Cypress mode 
		return (pdxc->hw_funcs.DoVendorRequest)(pdxc, 0x21, 0, dev_addr>>1, reg_value, reg_num, DIR_IN);

	return SUCCESS;
}

SINT32  SPI_WriteRegister(PDEVICE_EXTENSION_COMMON pdxc, SINT32 op_len, UINT8 op_code, SINT32 addr_len, UINT16 reg_addr, SINT32 data_len, UINT16 reg_data, SINT32 spi_mode)
{
	UINT16 data;
	int r,i;

	PrintMsg(DEBUG_I2C, ("SPI_WriteRegister, op_len %d, op_code %02x, addr_len %d, reg_addr %04x, data_len %d, reg_data %04x, spi_mode %d",
				op_len, op_code, addr_len, reg_addr, data_len, reg_data, spi_mode) );

	// 1) Wait until spi_state of spi_i2c_status_reg is IDLE;

	for(i=0;i<10;i++)
	{
		r = ReadCBusRegFW(pdxc, STATUS_REG_ADDR, &data);
		if(r != 0)
			return 1;

		if( ((data & SPI_STATE_MASK)>>9) == IS_IDLE )
			break;
	
		PISleep(100);
	}
	if(i==10)
		return 1;

	// 2) Write spi_ctrl_reg if needed;

	r = WriteCBusRegFW(pdxc, SPI_CTRL_REG_ADDR, (UINT16)spi_mode);
	if(r != 0)
		return 2;

	// 3) Write spi_oplen_reg if needed;

	data = ( (data_len & 0x001F) << 7 ) | ( ((addr_len-1) & 0x000F) << 3 ) | ( (op_len-1) & 0x0007 ) ;
	r = WriteCBusRegFW(pdxc, SPI_OPLEN_REG_ADDR, data);
	if(r != 0)
		return 3;

	// 4) Write spi_opc_reg if needed;

	data = op_code << (8-op_len);
	r = WriteCBusRegFW(pdxc, SPI_OPC_REG_ADDR, data);
	if(r != 0)
		return 4;

	// 5) Write spi_data_reg for write operation if the write data is different from spi_data_reg or need to clear spi_clear_to_send; or read spi_data_reg if need to clear spi_rx_data_rdy;

	data = reg_data << (16-data_len);
	r = WriteCBusRegFW(pdxc, SPI_DATA_REG_ADDR, data);
	if(r != 0)
		return 5;

	// 6) Write spi_addr_reg. 

	data = reg_addr << (16-addr_len);
	r = WriteCBusRegFW(pdxc, SPI_ADDR_REG_ADDR, data);
	if(r != 0)
		return 6;

	return 0;
}

SINT32  SPI_ReadRegister(PDEVICE_EXTENSION_COMMON pdxc, SINT32 op_len, UINT8 op_code, SINT32 addr_len, UINT16 reg_addr, SINT32 data_len, UINT16 *reg_data, SINT32 spi_mode)
{
	UINT16 data;
	int r,i;

	PrintMsg(DEBUG_I2C, ("SPI_ReadRegister, op_len %d, op_code %02x, addr_len %d, reg_addr %04x, data_len %d, spi_mode %d",
				op_len, op_code, addr_len, reg_addr, data_len, spi_mode) );

	// 1) Wait until spi_state of spi_i2c_status_reg is IDLE;

	for(i=0;i<10;i++)
	{
		r = ReadCBusRegFW(pdxc, STATUS_REG_ADDR, &data);
		if(r != 0)
			return 1;

		if( ((data & SPI_STATE_MASK)>>9) == IS_IDLE )
			break;
	
		PISleep(100);
	}
	if(i==10)
		return 1;

	// 2) Write spi_ctrl_reg if needed;

	r = WriteCBusRegFW(pdxc, SPI_CTRL_REG_ADDR, (UINT16)spi_mode);
	if(r != 0)
		return 2;

	// 3) Write spi_oplen_reg if needed;

	data = ( (data_len & 0x001F) << 7 ) | ( ((addr_len-1) & 0x000F) << 3 ) | ( (op_len-1) & 0x0007 ) ;
	r = WriteCBusRegFW(pdxc, SPI_OPLEN_REG_ADDR, data);
	if(r != 0)
		return 3;

	// 4) Write spi_opc_reg if needed;

	data = op_code << (8-op_len);
	r = WriteCBusRegFW(pdxc, SPI_OPC_REG_ADDR, data);
	if(r != 0)
		return 4;

	// 5) read spi_data_reg to clear spi_rx_data_rdy;

	r = ReadCBusRegFW(pdxc, SPI_DATA_REG_ADDR, &data);
	if(r != 0)
		return 5;

	// 6) Write spi_addr_reg. 

	data = reg_addr << (16-addr_len);
	r = WriteCBusRegFW(pdxc, SPI_ADDR_REG_ADDR, data);
	if(r != 0)
		return 6;

	// 7) check if spi_rx_data_rdy is set

	for(i=0;i<10;i++)
	{
		r = ReadCBusRegFW(pdxc, STATUS_REG_ADDR, &data);
		if(r != 0)
			return 7;

		if( (data &	SPI_READ_READY_MASK) != 0 )
			break;

		PISleep(100);
	}
	if(i==10)
		return 7;

	// 8) read spi_data_reg for result

	r = ReadCBusRegFW(pdxc, SPI_DATA_REG_ADDR, &data);
	if(r != 0)
		return 8;

	*reg_data = data & ((1<<data_len)-1);

	PrintMsg(DEBUG_I2C, ("SPI_ReadRegister return --  reg_data : %04x", *reg_data));
	return 0;
}

/*********************************** I2C.c ***********************************/
