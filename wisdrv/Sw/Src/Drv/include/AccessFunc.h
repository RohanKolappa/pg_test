/*****************************************************************************
*
*   Copyright WIS Technologies (c) (2003)
*   All Rights Reserved
*
******************************************************************************
*
*   FILE: 
*       AccessFunc.h 
*
*   DESCRIPTION:
*		The driver module provide the board module services through the structure
*		defined as follows
*
*   AUTHOR:
*		Jun Ding<jding@wischip.com>
*
*    <SOURCE CONTROL TAGS TBD>
*
*****************************************************************************/

#ifndef _ACCESS_FUNC_H_
#define _ACCESS_FUNC_H_

#include "typedef.h"

enum I2C_MODE
{
	IM_I2C,
	IM_SCCB
};

#ifndef _GPIO_MODE
#define _GPIO_MODE
#define GM_ONCHIP	0
#define GM_CYPRESS	1
#endif

/*****************************************************************************
*
*   I2C bus read/write access
*
*****************************************************************************/

typedef SINT32 (*I2C_WRITEREGISTER)(void *context, UINT8 dev_addr, SINT32 addr_width, UINT16 reg_addr, UINT8 reg_value, SINT32 i2c_mode);
typedef SINT32 (*I2C_READREGISTER)(void *context, UINT8 dev_addr, SINT32 addr_width, UINT16 reg_addr, UINT8 *reg_value, SINT32 i2c_mode);

typedef SINT32 (*I2C_BURSTWRITEREGISTER)(void *context, UINT8 dev_addr, SINT32 addr_width, UINT16 start_reg_addr, SINT32 reg_num, UINT8 *reg_value, SINT32 i2c_mode);
typedef SINT32 (*I2C_BURSTREADREGISTER)(void *context, UINT8 dev_addr, SINT32 addr_width, UINT16 start_reg_addr, SINT32 reg_num, UINT8 *reg_value, SINT32 i2c_mode);

/*****************************************************************************
*
*   SPI bus read/write access
*
*****************************************************************************/

typedef SINT32 (*SPI_WRITEREGISTER)(void *context, SINT32 op_len, UINT8 op_code, SINT32 addr_len, UINT16 reg_addr, SINT32 data_len, UINT16 reg_data, SINT32 spi_mode);
typedef SINT32 (*SPI_READREGISTER)(void *context, SINT32 op_len, UINT8 op_code, SINT32 addr_len, UINT16 reg_addr, SINT32 data_len, UINT16 *reg_data, SINT32 spi_mode);

/*****************************************************************************
*
*   General purpose input/output read/write access
*
*****************************************************************************/

typedef SINT32 (*GPIO_WRITEPINS)(void *context, SINT32 WriteNum, SINT32 *Index, SINT32 *Value, SINT32 Mode);
typedef SINT32 (*GPIO_READPINS)(void *context, SINT32 ReadNum, SINT32 *Index, SINT32 *Value, SINT32 Mode);

/*****************************************************************************
*
*   Chip Control Bus Register read/write access
*
*****************************************************************************/

typedef SINT32 (*WRITECBUSREGFW)(void *context, UINT16 Addr, UINT16 Data);
typedef SINT32 (*READCBUSREGFW)(void *context, UINT16 Addr, UINT16 *pData);


/*****************************************************************************
*
*   Read dword value from registry
*
*****************************************************************************/
typedef UINT32 (*GETDWORDVALUEFROMREGISTRY)(char *Name, UINT32 DefaultValue);

/*****************************************************************************
*
*   Board Callback
*
*****************************************************************************/

typedef SINT32 (*BOARD_CALLBACK)(void *context);


/*****************************************************************************
*
*   Access Functions represent the service provided by the driver module
*
*****************************************************************************/

typedef struct _ACCESS_FUNCTIONS
{
	I2C_WRITEREGISTER	I2C_WriteRegister;
	I2C_READREGISTER	I2C_ReadRegister;

	I2C_BURSTWRITEREGISTER	I2C_BurstWriteRegister;
	I2C_BURSTREADREGISTER	I2C_BurstReadRegister;
	
	SPI_WRITEREGISTER	SPI_WriteRegister;
	SPI_READREGISTER	SPI_ReadRegister;

	GPIO_WRITEPINS		GPIO_WritePins;
	GPIO_READPINS		GPIO_ReadPins;

	WRITECBUSREGFW		WriteCBusRegFW;
	READCBUSREGFW		ReadCBusRegFW;

	GETDWORDVALUEFROMREGISTRY	GetDWORDValueFromRegistry;

	BOARD_CALLBACK		BCB_OnVSyncLost;
	
} ACCESS_FUNCTIONS;


#endif

/****************************** end of AccessFunc.h **************************/

