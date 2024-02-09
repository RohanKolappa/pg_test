#ifndef _DEVICE_IO_H
#define _DEVICE_IO_H


#define IO_MAX 65536	// we need to reduce this number to save some memory

typedef struct _IO_PACKET
{
	SINT32 Type;
	SINT32 Param1;
	SINT32 Param2;
	SINT32 BufLen;
	UINT8 Buf[IO_MAX];
} IO_PACKET;


enum IO_TYPE
{
	// "-" means "not use"

	// get board revision information
	// Param1:	[In]	-, 
	//			[Out]	struct REVISION_INFO, includes the board revision info	
	// Param2:	- 
	// Buf:		-
	// BufLen:	[In]	0
	//			[Out]	0
	IO_GETCHIPREVISION,

	// set the instance number associated with driver, 
	// Param1:	[In]	instance num, 
	//			[Out]	-	
	// Param2:	- 
	// Buf:		-
	// BufLen:	[In]	0
	//			[Out]	0
	IO_SETINSTANCENUM,

	// set video configuration
	// Param1:	- 
	// Param2:	- 
	// Buf:		[In]	an instance of TCFGVIDEO
	//			[Out]	-
	// BufLen:	[In]	sizeof(TCFGVIDEO)
	//			[Out]	0
	IO_SETVIDEOCONFIG,
	
	// set audio configuration
	// Param1:	- 
	// Param2:	- 
	// Buf:		[In]	an instance of TCFGVIDEOFIX
	//			[Out]	-
	// BufLen:	[In]	sizeof(FIXCONFIG)
	//			[Out]	0
	IO_SETFIXCONFIG,
	
	// set fix configuration
	// Param1:	- 
	// Param2:	- 
	// Buf:		[In]	an instance of AUDIO_CONFIG
	//			[Out]	-
	// BufLen:	[In]	sizeof(AUDIO_CONFIG)
	//			[Out]	0
	IO_SETAUDIOCONFIG,

	// make chip start capturing
	// Param1:	[In]	-
	//			[Out]	Result
	// Param2:	- 
	// Buf:		-
	// BufLen:	[In]	0
	//			[Out]	0
	IO_STARTDEVICE,

/*
	// make chip start capturing, this is for compatible with GO7007, for SB, init packet is created in the driver
	// Param1:	[In]	-
	//			[Out]	Result
	// Param2:	- 
	// Buf:		[In]	Initial Packet
	//			[Out]    -
	// BufLen:	[In]	Initial Packet Size
	//			[Out]	0
	IO_STARTDEVICEWITHPACKET,
*/

	// make chip stop capturing
	// Param1:	[In]	-
	//			[Out]	Result
	// Param2:	- 
	// Buf:		-
	// BufLen:	[In]	0
	//			[Out]	0
	IO_STOPDEVICE,


	// read i2c register(s)
	// Param1:	[In]	- 
	//			[Out]	Result
	// Param2:	-
	// Buf:		[In]    <i2c mode><device address><address length><lower byte of register address>
	//					<higher byte of register address><lower byte of register number><higher byte of register number><0>
	//                  * see enum I2C_MODE for i2c mode
	//					* address length can be 8 or 16
	//                  * if register address is 8bits, then register address higher byte is 0
	//                  * register number is number of register to read continually using burst mode
	//			[Out]	value of I2C register(s) (8bit for each)
	// BufLen:	[In]	8
	//			[Out]	number
	IO_I2CREADREGISTER,

	// write i2c register(s)
	// Param1:	[In]	- 
	//			[Out]	Result
	// Param2:	-
	// Buf:		[In]    <i2c mode><device address><address length><lower byte of register address>
	//					<higher byte of register address><lower byte of register number><higher byte of register number><0>
	//					<data1><data2>...  
	//                  * see enum I2C_MODE for i2c mode
	//					* address length can be 8 or 16
	//                  * if register address is 8bits, then register address higher byte is 0
	//                  * register number is number of register to write continually using burst mode
	//			[Out]	-
	// BufLen:	[In]	number +  8
	//			[Out]	0
	IO_I2CWRITEREGISTER,

	// read spi register(s)
	// Param1:	[In]	-
	//			[Out]	Result
	// Param2:	-
	// Buf:		[In]    <lower byte of spi mode><higher byte of spi mode><op code length><op code>
	//					<address length><lower byte of register address><higher byte of register address><data length>
	//					<lower byte of register number><higher byte of register number><0><0>
	//                  * op code length can be 1 - 8
	//                  * address length can be 1 - 16
	//                  * data length can be    0 - 16
	//                  * register number is number of register to read continually using burst mode
	//			[Out]	<lower byte of SPI register><higher byte of SPI register>...
	// BufLen:	[In]	12
	//			[Out]	number*2
	IO_SPIREADREGISTER,

	// write spi register(s)
	// Param1:	[In]	-
	//			[Out]	Result
	// Param2:	-
	// Buf:		[In]    <lower byte of spi mode><higher byte of spi mode><op code length><op code>
	//					<address length><lower byte of register address><higher byte of register address><data length>
	//					<lower byte of register number><higher byte of register number><0><0>
	//					<lower byte of data1><higher byte of data1>...		
	//                  * op code length can be 1 - 8
	//                  * address length can be 1 - 16
	//                  * data length can be    0 - 16
	//                  * register number is number of register to write continually using burst mode
	//			[Out]	-
	// BufLen:	[In]	number*2 +  12
	//			[Out]	0
	IO_SPIWRITEREGISTER,


// following IO types are only for chip debugging

	// read several chip registers through firmware 
	// Param1:	[In]	-
	//			[Out]	Result
	// Param2:	-
	// Buf:		[In]	Array of Address
	//			[Out]   Array of Data
	// BufLen:	[In]	register number * 2	, Address and Data are all 16 bits
	//			[Out]	register number * 2
	IO_READCBUSREGFW,

	// read several chip registers through firmware 
	// Param1:	[In]	-
	//			[Out]	Result
	// Param2:	-
	// Buf:		[In]	Array of Address and Data pair
	//			[Out]   -
	// BufLen:	[In]	register number * 4	, Address and Data are all 16 bits
	//			[Out]	0
	IO_WRITECBUSREGFW,

	// read hpi register
	// Param1:	[In]	addr
	//			[Out]	Result
	// Param2:	[In]    -
	//			[Out]   data
	// Buf:		- 
	// BufLen:	0
	IO_READHPIREGISTER,

	// write hpi register
	// Param1:	[In]	addr
	//			[Out]	Result
	// Param2:	[In]    data
	//			[Out]	-
	// Buf:		- 
	// BufLen:	0
	IO_WRITEHPIREGISTER,

	// read an interrupt from chip, 
	// Param1:	[In]	-
	//			[Out]	Result
	// Param2:	[In]	-
	//			[Out]	(Interrupt value) << 16 | Interrupt data	
	// Buf:		-
	// BufLen:	[In]	0
	//			[Out]	0
	IO_READINTERRUPT,

	// write an interrupt to chip, 
	// Param1:	[In]	Interrupt value	
	//			[Out]	Result
	// Param2:	[In]	Interrupt data	
	//			[Out]	-
	// Buf:		-
	// BufLen:	[In]	0
	//			[Out]	0
	IO_WRITEINTERRUPT,

	// send packet(s) to chip,
	// Param1:	[In]	-
	//			[Out]	Result
	// Param2:	-
	// Buf:		[In]	packet data
	//			[Out]   -		
	// BufLen:	[In]	packet length (in byte)	
	//			[Out]	0
	IO_SENDPACKET,

	// read stream from chip,
	// Param1:	[In]	-
	//			[Out]	Result
	// Param2:	-
	// Buf:		[In]	read buffer
	//			[Out]   -		
	// BufLen:	[In]	read length (in byte)	
	//			[Out]	0
	IO_READSTREAMPIPE,

	//set chip motion detect option
	// Param1:	[In]	- KsMDinterfaceData
	//			[Out]	Result
	// Param2:	-
	// Buf:		[In]	
	//			[Out]   -		
	// BufLen:	[In]		
	//			[Out]	0
	IO_INIT_MD,
	IO_SET_MD_THRESHOLDS_AND_SENSITIVITIES,
	IO_SET_MD_REGIONS,
	IO_WAIT_MD_EVENT,
	IO_MD_STOP,
	IO_SETAUDIO,
	IO_SETVIDEO_SOURCE,
	IO_GETVIDEO_SOURCE,
	IO_SET_RESOLUTION,
	IO_FORCE_IFRAME,
	IO_CHANGE_PFRAME_RATE,
	IO_CHANGE_IFRAME_QUANTIZER, /* For VBR IFrames */
	IO_CHANGE_PFRAME_QUANTIZER, /* For VBR PFrames */
	IO_INSERT_SEQUENCE_HEADER_AT_NEXT_GOP, 	/* Insert Vol header
						   on next I-Frame */
        IO_SET_FPS, /* Change FPS */
	IO_SET_BITRATE, /* Change Bitrate */
	IO_SET_TV_STANDARD,   /* Set PAL/NTSC mode */
	IO_GET_BOARD_TYPE,    /* Determine board type */
	IO_SIGDETECT,    /* check sig lost */
	IO_SET_BRIGHTNESS,
	IO_SET_CONTRAST,
	IO_SET_HUE,
	IO_SET_SATURATION,
	////
	IO_OSD_DISPLAY,
	IO_AV_SYNC_SETTING,
	IO_GET_AVSTATS,
	IO_GET_SEQHEADER,
	IO_ADVANCED = 0x8000,
};

//	DeviceIoControl IOCTL codes supported by WisGo

#if defined(_WINDOWS)

	#define WIS_GO_IOCTL_BASE	0x0900

	#define IOCTL_WISGO_COMMAND			\
		CTL_CODE(FILE_DEVICE_UNKNOWN, WIS_GO_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)

#elif defined(_LINUX)

	#define IOCTL_WISGO_COMMAND			\
		_IOWR('W', 0x80, IO_PACKET)		

#endif


#endif

