// Device.h

#ifndef __DEVICE_H__
#define __DEVICE_H__

#if defined(WIN32)

#include <windows.h>
#include <winioctl.h>

#endif


#include "typedef.h"
#include "../../Drv/gosbdrv/linux/DeviceIO.h"

typedef enum
{
	CR_7007,
	CR_7007SB,

	CR_UNKNOWN=0xff,
}CHIP_REVISION;

enum
{
	RC_SUCCESS				= 0,
	RC_DEVICEALREADYOPEN	= 0x0C20,
	RC_OPENDEVICEFAIL		= 0x0C21,
	RC_DEVICENOTOPEN		= 0x0C22,
	RC_WRITEDEVICEFAIL		= 0x0C23,
	RC_NOMOREDATA			= 0x0C24,
	RC_READDEVICEFAIL		= 0x0C25,
	RC_NOTINDEBUGMODE		= 0x0C26,
	RC_ACKFAIL				= 0x0C27
};

/*

typedef struct _READ_VIDEO_HEAD
{
	unsigned int FrameSize;
	unsigned int FrameCount;
	unsigned int TotalDelay;
	unsigned int ChipDelay;
	unsigned int FrameInBuf;
	unsigned int VIPFrameNum;

} READ_VIDEO_HEAD;



enum INTERFACEMODE
{
	IM_HPI, 
	IM_USB, 
	IM_STANDBY
};

enum SELFTEST
{
	ST_HWMBIST,
	ST_BASICHPICHECK,
	ST_IOCHECK,
	ST_SWMBIST,
	ST_MEMSCAN,
	ST_SETPOWERSTATE
};
*/

class CDevice
{
public:
	CDevice();
	~CDevice();

	SINT32 IsDeviceOpened();

	SINT32 OpenDevice(SINT8 instance);		
				  
	VOID CloseDevice();

	SINT32 DeviceIO(IO_PACKET *pIOPacket);

	// *BufferLength :	[In]  Buffer length
	//					[Out] Actual frame length
	SINT32 GetOneVideoFrame(UINT8 *pBuffer, SINT32 *BufferLength);

	SINT32 GetAudioSamples(UINT8 *pBuffer, SINT32 *BufferLength);

private:

#if defined(WIN32)
	HANDLE	m_handle;
#elif defined(_LINUX)
	int m_handle;
#endif
	
	SINT8	m_instance;
};

	 
#endif
