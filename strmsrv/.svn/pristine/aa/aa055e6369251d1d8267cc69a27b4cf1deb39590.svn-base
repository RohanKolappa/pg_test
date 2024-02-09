#ifndef USB_H_
#define USB_H_

#pragma once 

#include "command.hh"
#include "strmsrv_compatibility.h"
#include "common.hh"
#include "vlog.h"
#include "v2d.h"
#include "v2o_lib.h"
#include "USBHid.h"
#include "RFBToUSB.h"
#include "RawToUsb.h"


class CRfbToUsb;
class CRawToUsb;

class CUSB
{

public:
	CUSB(CUSBHID *usbHid, SRVR *srvr,CLNT *clnt);
	~CUSB();

	int		ProcessAbsMouseData(char buttonMask, int XvncAbsol, int YvncAbsol);
	int		ProcessRelMouseData(char *pInBuf);
	int		ProcessRfbKeyboardData(unsigned int key, unsigned char down);
    int		ProcessRawKeyboardData(unsigned int key, unsigned char down);
	int		ProcessRemoteKeyboard(char *pPayload);
	int		ProcessRemoteMouse(char *pPayload);
	int		SetKeyboardLEDsStatus();
	void	SetScreenResolution(int screenWidth,int screenHeight);
	int		UpdateKeyboardLayout(CCommand *a_pCmd);
	int		GetKeyboardFD();
	int		GetAbsoluteMouseFD();
	int		GetRelativeMouseFD();

	
private :
	CUSBHID				*m_pUsbHid;
    CRfbToUsb           *m_pRfbToUsb;
    CRawToUsb           *m_pRawToUsb;
    SRVR                *m_pSrvr;
    CLNT                *m_pClnt;
	int		            m_nScreenWidth;
	int         		m_nScreenHeight;

};
#endif /* USB_H_ */
