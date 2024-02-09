/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        I2C.h
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

#ifndef _I2C_H_
#define _I2C_H_

#include "typedef.h"
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif


// Write a I2C register 
SINT32	I2C_WriteRegister(PDEVICE_EXTENSION_COMMON pdxc, UINT8 dev_addr, SINT32 addr_width, UINT16 reg_addr, UINT8 reg_value, SINT32 i2c_mode);

// Read a I2C register 
SINT32	I2C_ReadRegister(PDEVICE_EXTENSION_COMMON pdxc, UINT8 dev_addr, SINT32 addr_width, UINT16 reg_addr, UINT8 *reg_value, SINT32 i2c_mode);

// Write multiple I2C registers
SINT32	I2C_BurstWriteRegister(PDEVICE_EXTENSION_COMMON pdxc, UINT8 dev_addr, SINT32 addr_width, UINT16 start_reg_addr, SINT32 reg_num, UINT8 *reg_value, SINT32 i2c_mode);

// Read multiple I2C registers
SINT32	I2C_BurstReadRegister(PDEVICE_EXTENSION_COMMON pdxc, UINT8 dev_addr, SINT32 addr_width, UINT16 start_reg_addr, SINT32 reg_num, UINT8 *reg_value, SINT32 i2c_mode);

SINT32  SPI_WriteRegister(PDEVICE_EXTENSION_COMMON pdxc, SINT32 op_len, UINT8 op_code, SINT32 addr_len, UINT16 reg_addr, SINT32 data_len, UINT16 reg_data, SINT32 spi_mode);

SINT32  SPI_ReadRegister(PDEVICE_EXTENSION_COMMON pdxc, SINT32 op_len, UINT8 op_code, SINT32 addr_len, UINT16 reg_addr, SINT32 data_len, UINT16 *reg_data, SINT32 spi_mode);

#ifdef __cplusplus
}
#endif


#endif

/*********************************** end of I2C.h ****************************/
