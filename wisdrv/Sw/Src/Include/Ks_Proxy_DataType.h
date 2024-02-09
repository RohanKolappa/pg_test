/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        Ks_Proxy_DataType.h
*
*    DESCRIPTION:
*        Data type defines for Driver and Proxy.
*   
*
*    AUTHOR:Xiaoli Xiong
*	
*
*    <SOURCE CONTROL TAGS TBD>
*
******************************************************************************/
#ifndef __KS_PROXY_DATATYPE_H__
#define __KS_PROXY_DATATYPE_H__

//KsGoChip Method set

typedef enum 
{
    KSMETHOD_CUSTOM_VIDEO_CONFIG_METHODID,
} KSPROPERTY_CUSTOM_METHODID;


typedef struct 
{
    KSMETHOD Method;
	unsigned int Error;
    TCFG_FORMAT_EXTENSION VideoConfig;
} KSMETHOD_CUSTOM_PROP_VIDEO, *PKSMETHOD_CUSTOM_PROP_VIDEO;

//for motion detection
typedef enum 
{
    KSMETHOD_MOTION_DETECTION_SETUP_ID,
} KSMETHOD_MOTION_DETECTION_ID;


typedef struct
{
    KSMETHOD Method;
	MDInterfaceData_T data;
}KsMDInterfaceData;

//for KsInfo
typedef enum 
{
    KSMETHOD_INFO_GET_REVISION_INFO_ID,
    KSMETHOD_INFO_GET_MACROVISION_ID,
} KSMETHOD_INFO_ID;


typedef struct
{
    KSMETHOD Method;
	REVISION_INFO RevInfo;
}KSINFO_REVISIONINFODATA_t, *PKSINFO_REVISIONINFODATA_t;

typedef struct
{
    KSMETHOD Method;
	unsigned int Macrovision;
}KSINFO_MACROVISIONDATA_t, *PKSINFO_MACROVISIONDATA_t;

// for KsAccessFunc

typedef enum 
{
    KSMETHOD_ACCFUNC_I2C_WRITE_REG_ID,
	KSMETHOD_ACCFUNC_I2C_READ_REG_ID,

	KSMETHOD_ACCFUNC_I2C_BURST_WRITE_REG_ID,
	KSMETHOD_ACCFUNC_I2C_BURST_READ_REG_ID,

    KSMETHOD_ACCFUNC_SPI_WRITE_REG_ID,
	KSMETHOD_ACCFUNC_SPI_READ_REG_ID,

	KSMETHOD_ACCFUNC_GPIO_WRITE_PINS_ID,
	KSMETHOD_ACCFUNC_GPIO_READ_PINS_ID,
} KSMETHOD_ACCFUNC_ID;


typedef struct
{
    KSMETHOD Method;
	UINT8    DevAddr;
	SINT32   AddrWidth;
	UINT16   RegAddr;
	UINT8    RegValue;
	SINT32   I2CMode;
}KSACCFUNC_I2CDATA_t, *PKSACCFUNC_I2CDATA_t;

typedef struct
{
    KSMETHOD Method;
	UINT8    DevAddr;
	SINT32   AddrWidth;
	UINT16   StartRegAddr;
	SINT32   RegNum;
	UINT8    RegValue[128];
	SINT32   I2CMode;
}KSACCFUNC_I2CBURSTDATA_t, *PKSACCFUNC_I2CBURSTDATA_t;

typedef struct
{
    KSMETHOD Method;
	SINT32   OpLen;
	UINT8    OpCode;
	SINT32   AddrLen;
	UINT16   RegAddr;
	SINT32   DataLen;
	UINT16   RegData;
	SINT32   SPIMode;
}KSACCFUNC_SPIDATA_t, *PKSACCFUNC_SPIDATA_t;

typedef struct
{
    KSMETHOD Method;
	SINT32 PinNum;
	SINT32 Index[128];
	SINT32 Value[128];
	SINT32 Mode;
}KSACCFUNC_GPIODATA_t, *PKSACCFUNC_GPIODATA_t;

// for KsAdvanced

typedef enum 
{
    KSMETHOD_ADVANCED_READ_CBUS_REG_ID,
    KSMETHOD_ADVANCED_WRITE_CBUS_REG_ID,

	KSMETHOD_ADVANCED_READ_HPI_REG_ID,
	KSMETHOD_ADVANCED_WRITE_HPI_REG_ID,

	KSMETHOD_ADVANCED_READ_INTERRUPT_ID,
	KSMETHOD_ADVANCED_WRITE_INTERRUPT_ID,

	KSMETHOD_ADVANCED_SEND_PACKET_ID,
	KSMETHOD_ADVANCED_READ_STREAM_ID,

} KSMETHOD_ADVANCED_ID;

typedef struct
{
	KSMETHOD Method;
	UINT16 Addr;
	UINT16 Data;
}KSADVANCED_REGISTER_t, *PKSADVANCED_REGISTER_t; 

typedef struct
{
	KSMETHOD Method;
	UINT16 IntVal;
	UINT16 IntData;
}KSADVANCED_INTERRUPT_t, *PKSADVANCED_INTERRUPT_t; 

typedef struct
{
	KSMETHOD Method;
	UINT16 Data[32];
}KSADVANCED_SENDPACKET_t, *PKSADVANCED_SENDPACKET_t; 

typedef struct
{
	KSMETHOD Method;
	SINT32 Len;
	UINT8 Buf[1024];
}KSADVANCED_READSTREAM_t, *PKSADVANCED_READSTREAM_t; 

//For KSOSD
typedef enum 
{
    KSMETHOD_OSD_TEXTOUT_ID,
    KSMETHOD_OSD_SHOW_ID,
} KSMETHOD_OSD_ID;

typedef struct
{
	KSMETHOD Method;
	OSDTextoutInfo Info;
}KSMETHOD_OSD_TEXTOUT_INFO; 

#endif

/********************************* end of Ks_Proxy_DataType.h **************************/
