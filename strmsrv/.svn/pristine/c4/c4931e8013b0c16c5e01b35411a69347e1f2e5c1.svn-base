#ifndef RFBTOUSB_H_
#define RFBTOUSB_H_

#pragma once 

#include "command.hh"
#include "strmsrv_compatibility.h"
#include "common.hh"
#include "vlog.h"
#include "v2d.h"
#include "USBHid.h"
#include "English_US_Layout.h"
#include "German_Germany_Layout.h"
#include "French_France_Layout.h"
#include "French_Belgium_Layout.h"
#include "Spanish_Spain_Layout.h"
#include "Dutch_Netherlands_Layout.h"
#include "Italian_Italy.h"
#include "Protuguese_Portugal.h"
#include "Russian_Russia.h"


class CRfbToUsb
{

public:
	CRfbToUsb(CUSBHID *usbHid);
	~CRfbToUsb();


	int		ProcessRfbKeyboardData(unsigned int key, unsigned char down);
	int		UpdateKeyboardLayout(CCommand *a_pCmd);
	
private :
	CUSBHID				*m_pUSBHID;
	unsigned int		m_nModifKeysLatched;
	unsigned int		repBuf[2];
	unsigned int		m_prevrepBuf[2];
	unsigned int		m_nLeftCtrlHold;
	unsigned int		m_nLeftShftHold;
	unsigned int		m_nLeftAltHold;
	unsigned int		m_nLeftGUIHold;
	unsigned int		m_nRightCtrlHold;
	unsigned int		m_nRightShftHold;
	unsigned int		m_nRightAltHold;
	unsigned int		m_nRightGUIHold;
	unsigned int		m_nPrevModifKeysLatched;
	unsigned char		m_sButton[8];
	unsigned short		m_sVNCpr[16];
	int					m_nKeyBoardLayout;
	unsigned int*		m_pEncode00xx;
	struct _KeySymToUSBStruct* m_pKeySymToUSBStruct;
	int					m_nKeySymToUSBStructSize;
	unsigned int		m_nTempKey[2];
	char				m_sKbLayout[60];
	int					ConvertVNCKeysToUSBKeys(unsigned int key, unsigned char down);
	bool				XKKeySymToUSB(unsigned int keysym,_KeySymToUSB &kstu);
	int					GetKeyboardLEDStatus();
	int					SetKeyboardLayout();

};
#endif /* RFBTOUSB_H_ */
