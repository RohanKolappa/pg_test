// Device.cpp

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>

#include "Device.h"
#include "DebugPrint.h"

#define INVALID_HANDLE_VALUE -1

CDevice::CDevice()
{
    m_handle = INVALID_HANDLE_VALUE;
}

CDevice::~CDevice()
{
    if(m_handle != INVALID_HANDLE_VALUE)
	CloseDevice();
}


SINT32 CDevice::IsDeviceOpened()
{
    return (m_handle != INVALID_HANDLE_VALUE);
}


/////////////////////////////////////////
// return 0  : open success
//		  -1 : open fail

SINT32 CDevice::OpenDevice(SINT8 instance)
{
    char device_name[128];
    
    if(m_handle != INVALID_HANDLE_VALUE)	//already open
    {
	if(instance == m_instance)
	    return RC_SUCCESS;		// same device, do nothing
	else
	    CloseDevice();			// reopen;
    }	

    m_instance = instance;
    
    sprintf(device_name,"/dev/wisgo7007_%d",instance);
    m_handle = open(device_name, O_RDWR);
    
    printf("GO7007SB: Using device %s\n", device_name);
    
    if(m_handle == INVALID_HANDLE_VALUE)
    {
	printf("Open File Fail");	
	return RC_OPENDEVICEFAIL;
    }
	
    return RC_SUCCESS;
}

void CDevice::CloseDevice()
{
    if(m_handle == INVALID_HANDLE_VALUE)
	return;

    close(m_handle);
    m_handle = INVALID_HANDLE_VALUE;
}

SINT32 CDevice::DeviceIO(IO_PACKET *pIOPacket)
{
    int r;
    
    if (m_handle == INVALID_HANDLE_VALUE)
	return RC_DEVICENOTOPEN;

    r = ioctl(m_handle, IOCTL_WISGO_COMMAND, pIOPacket);
    
    if(r==0)							       
	return RC_SUCCESS;
    else
	return RC_WRITEDEVICEFAIL;
}

// *BufferLength :	[In]  Buffer length
//					[Out] Actual frame length

SINT32 CDevice::GetOneVideoFrame(UINT8 *pBuffer, SINT32 *BufferLength)
{	
    if(m_handle == INVALID_HANDLE_VALUE)
	return RC_DEVICENOTOPEN;

    int r = read(m_handle, pBuffer, *BufferLength);

    if(r != 0)
    {
	*BufferLength = r;
        return RC_SUCCESS;
    }
    else
	return RC_NOMOREDATA;
}

SINT32 CDevice::GetAudioSamples(UINT8 *pBuffer, SINT32 *BufferLength)
{
    if(m_handle == INVALID_HANDLE_VALUE)
	return RC_DEVICENOTOPEN;
	
    int r = read(m_handle, pBuffer, *BufferLength);

    if(r != 0)
    {
	*BufferLength = r;
        return RC_SUCCESS;
    }
    else
	return RC_NOMOREDATA;
}

