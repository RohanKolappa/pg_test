#include "USB.h"

CUSB::CUSB(CUSBHID *usbHid,SRVR *srvr,CLNT *clnt)
{
	m_pUsbHid = usbHid;
    m_pSrvr = srvr;
    m_pClnt = clnt;
    m_pRfbToUsb = new CRfbToUsb(m_pUsbHid);
    m_pRawToUsb = new CRawToUsb(m_pUsbHid,m_pSrvr);
	m_nScreenWidth = 0;
    m_nScreenHeight = 0;
}

CUSB::~CUSB()
{
	if(m_pRfbToUsb)
        delete m_pRfbToUsb;

    if(m_pRawToUsb)
        delete m_pRawToUsb;
}

int	CUSB::ProcessAbsMouseData(char buttonMask, int XvncAbsol, int YvncAbsol)
{
	unsigned int  repBuf[2];
    unsigned int buttonbyte, x, y;
    char  wheel;


    if ((XvncAbsol <= 0) || ((XvncAbsol >= 0) && (XvncAbsol >= (m_nScreenWidth)))){
         return 0;
    }

    if ((YvncAbsol <= 0) || ((YvncAbsol >= 0) && (YvncAbsol >= (m_nScreenHeight)))){
         return 0;
    }

    buttonbyte = buttonMask & BIT0;
    if (buttonMask & BIT1)
        buttonbyte |= BIT2;

    if (buttonMask & BIT2)
        buttonbyte |= BIT1;
     
    wheel = 0;
    if (buttonMask & BIT4)
        wheel = -1;
    if (buttonMask & BIT3)
        wheel = 1;


    x = (XvncAbsol * (MAX_X_IN_USB_REPORT + 1)) / (m_nScreenWidth );
    y = (YvncAbsol * (MAX_Y_IN_USB_REPORT + 1)) / (m_nScreenHeight);


    repBuf[0] = (buttonbyte & 0xFF) | // Buttons to bits 7..0,  byte 1 HID report
               ((x & 0xFF) << 8)   | // X LSB to bits 15..0,   byte 2
               ((x & 0xFF00) << 8) | // X MSB to bits 23..16,  byte 3
               ((y & 0xFF) << 24)  ; // Y LSB to bits 31..24,  byte 4
    

     repBuf[1] = ((y >> 8) & 0xFF)   |   // Y MSB to bits 7..0,  byte 5
                    ((unsigned int)wheel << 8); // Wheel to bits 15..8  byte 6

     char *buf = (char*)repBuf;

	 if(m_pUsbHid)
		 return m_pUsbHid->WriteAbsoluteMouse(buf, MSABSREPORTLEN);
	 else
		 return 0;

}

int	CUSB::ProcessRelMouseData(char *pInBuf)
{

	if (pInBuf == NULL)
		return 0;

	if(m_pUsbHid)
		return m_pUsbHid->WriteRelativeMouse(pInBuf, MSRELREPORTLEN);
	else
		return 0;

}

int	CUSB::ProcessRfbKeyboardData(unsigned int key, unsigned char down)
{
    if(m_pRfbToUsb)
        return m_pRfbToUsb->ProcessRfbKeyboardData(key,down);

    SYSLOG_ERROR("-%u- %s : Could not Process Rfb KeyboardData..\n",__LINE__, __FUNCTION__);

    return -1;

}

int	CUSB::ProcessRawKeyboardData(unsigned int scancode, unsigned char down)
{
    //SYSLOG_INFO("-%u- %s : ProcessRawKeyboardData scancode[%02x] keydown[%d]\n",__LINE__, __FUNCTION__,scancode,down);

    if(m_pRawToUsb)
        return m_pRawToUsb->ProcessRawKeyboardData(scancode,down);

    SYSLOG_ERROR("-%u- %s : Could not Process Raw KeyboardData..\n",__LINE__, __FUNCTION__);
    return -1;
}

int	CUSB::ProcessRemoteKeyboard(char *pPayload)
{
	unsigned int key = 0;
	unsigned char down = 0;
	unsigned int sessionId = 0;

	if (pPayload == NULL)
		return -1;

	int rc = v2dLibParseAbsoluteKeyboard((uint8_t *)pPayload, &key,
		&down, &sessionId);
	// TO DO Need to check SessionId here
    //SYSLOG_INFO("-%u- %s : ProcessRemoteKeyboard \n",__LINE__, __FUNCTION__);
	if (rc > 0) 
    {
        if(m_pRfbToUsb)
        {
            if (!(m_pRfbToUsb->ProcessRfbKeyboardData(key, down)))
                SYSLOG_ERROR("-%u- %s : Could not send keyboard events\n", __LINE__, __FUNCTION__);
        }
	}

	return 1;
}

int	CUSB::ProcessRemoteMouse(char *pPayload)
{
	uint16_t x = 0, y = 0;
	uint8_t buttonmask = 0;
	uint32_t sessionId = 0;

	if (pPayload == NULL)
		return -1;

	int rc = v2dLibParseAbsoluteMouse((uint8_t *)pPayload, &x, &y, &buttonmask,&sessionId);

	// TO DO Need to check SessionId here 
	if (rc > 0 ) {
		if (!( ProcessAbsMouseData(buttonmask, x, y)))
			SYSLOG_ERROR("-%u- %s : Could not send Abs mouse Events\n",__LINE__, __FUNCTION__);

	}

	return 1;

}

int	CUSB::SetKeyboardLEDsStatus()
{
   
   	if(m_pUsbHid)
		return m_pUsbHid->SetKeyboardLEDsStatus();

	return 0;
	
}

void CUSB::SetScreenResolution(int screenWidth,int screenHeight)
{
	m_nScreenWidth = screenWidth;
	m_nScreenHeight = screenHeight;
}

int	CUSB::UpdateKeyboardLayout(CCommand *a_pCmd)
{
    if(m_pRfbToUsb)
    {
        return m_pRfbToUsb->UpdateKeyboardLayout(a_pCmd);
    }
    else
    {
        SYSLOG_ERROR("-%u- %s : Could not update keyboard layout.\n",__LINE__, __FUNCTION__);
    }

	return -1;
}

int	CUSB::GetKeyboardFD()
{
	if(m_pUsbHid)
		return m_pUsbHid->GetKeyboardFD();

	return -1;
}

int	CUSB::GetAbsoluteMouseFD()
{
	if(m_pUsbHid)
		return m_pUsbHid->GetAbsoluteMouseFD();

	return -1;
}

int	CUSB::GetRelativeMouseFD()
{
	if(m_pUsbHid)
		return m_pUsbHid->GetRelativeMouseFD();

	return -1;

}

