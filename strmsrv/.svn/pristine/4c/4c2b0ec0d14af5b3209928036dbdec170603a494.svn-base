#include "RFBToUSB.h"

CRfbToUsb::CRfbToUsb(CUSBHID *usbHid)
{
	m_pUSBHID = usbHid;
	m_nModifKeysLatched = 0;
    m_nLeftCtrlHold = 0;
    m_nLeftShftHold = 0;
    m_nLeftAltHold = 0;
    m_nLeftGUIHold = 0;
    m_nRightCtrlHold = 0;
    m_nRightShftHold = 0;
    m_nRightAltHold = 0;
    m_nRightGUIHold = 0;
    m_nPrevModifKeysLatched = 0;
    m_prevrepBuf[0] = m_prevrepBuf[1] = 0;
	m_nKeyBoardLayout = KEYBOARD_LAYOUT_ENGLISH_US;
	m_pEncode00xx = new unsigned int[ARRAY_SIZE];
	memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
    m_sButton[0] = m_sButton[1] = m_sButton[2] = m_sButton[3] = 0;
    m_sButton[4] = m_sButton[5] = m_sButton[6] = m_sButton[7] = 0;
	m_pKeySymToUSBStruct = NULL;
	m_nKeySymToUSBStructSize = 0;
	SetKeyboardLayout();
}

CRfbToUsb::~CRfbToUsb()
{
	if(m_pEncode00xx)
	{
		delete [] m_pEncode00xx;
		m_pEncode00xx = NULL;
	}

	if(m_pKeySymToUSBStruct)
	{
		delete [] m_pKeySymToUSBStruct;
		m_pKeySymToUSBStruct = NULL;				
	}

	m_nKeySymToUSBStructSize = 0;
}


int	CRfbToUsb::ProcessRfbKeyboardData(unsigned int key, unsigned char down)
{

	//SYSLOG_INFO("-%u- %s(down=%u key=%04X)\n", __LINE__, __FUNCTION__, down, key);
#if 0
	char nm[64]={0};

	switch(key) {
	case 0x0020: sprintf(nm, "Space");   break;   case 0xFF08: sprintf(nm, "Backspace"); break;   case 0xFF09: sprintf(nm, "Tab");      break;
	case 0xFF0D: sprintf(nm, "Enter");   break;   case 0xFF13: sprintf(nm, "Pause");     break;   case 0xFF14: sprintf(nm, "ScrollLck");break;
	case 0xFF1B: sprintf(nm, "Esc");     break;   case 0xFF50: sprintf(nm, "Home");      break;   case 0xFF51: sprintf(nm, "LeftArrow");break;
	case 0xFF52: sprintf(nm, "UpArrow"); break;   case 0xFF53: sprintf(nm, "RightArrow");break;   case 0xFF54: sprintf(nm, "DownArrow");break;
	case 0xFF55: sprintf(nm, "PgUp");    break;   case 0xFF56: sprintf(nm, "PgDown");    break;   case 0xFF57: sprintf(nm, "End");      break;
	case 0xFF61: sprintf(nm, "PrintScr");break;   case 0xFF63: sprintf(nm, "Insert");    break;   case 0xFF67: sprintf(nm, "R_Menu");   break;
	case 0xFFBE: sprintf(nm, "F1");      break;   case 0xFFBF: sprintf(nm, "F2");        break;   case 0xFFC0: sprintf(nm, "F3");       break;
	case 0xFFC1: sprintf(nm, "F4");      break;   case 0xFFC2: sprintf(nm, "F5");        break;   case 0xFFC3: sprintf(nm, "F6");       break;
	case 0xFFC4: sprintf(nm, "F7");      break;   case 0xFFC5: sprintf(nm, "F8");        break;   case 0xFFC6: sprintf(nm, "F9");       break;
	case 0xFFC7: sprintf(nm, "F10");     break;   case 0xFFC8: sprintf(nm, "F11");       break;   case 0xFFC9: sprintf(nm, "f12");      break;
	case 0xFFE1: sprintf(nm, "L_Shift"); break;   case 0xFFE2: sprintf(nm, "R_Shift");   break;   case 0xFFE3: sprintf(nm, "L_Ctrl");   break;
	case 0xFFE4: sprintf(nm, "R_Ctrl");  break;   case 0xFFE5: sprintf(nm, "CapsLock");  break;   case 0xFFE6: sprintf(nm, "ShiftLock");break;
	case 0xFFE9: sprintf(nm, "L_Alt");   break;   case 0xFFEA: sprintf(nm, "R_Alt");     break;   case 0xFFEB: sprintf(nm, "L_Winkey"); break;
	case 0xFFEC: sprintf(nm, "R_Winkey");break;   case 0xFFFF: sprintf(nm, "Delete");    break;   default: sprintf(nm, "%c", (char)key);break;
	}
	/*
	if(down)
	{
	//SYSLOG_INFO("%s (0x%04X)", nm, key);
	SYSLOG_INFO("-%u- %s(%s (0x%04X))\n", __LINE__, __FUNCTION__, nm, key);
	}
	else
	SYSLOG_INFO("."); //printf("       --up-- %s (0x%04X)\n",  nm, key);
	*/

	SYSLOG_INFO("-%u- %s( %s Key(0x%04X), down=%u )\n", __LINE__, __FUNCTION__, nm, key,((down)? 1:0));


#endif

	if (key == 0xFFE5) 
		return 8;

	ConvertVNCKeysToUSBKeys(key, down);
	return 8;

}

int	CRfbToUsb::UpdateKeyboardLayout(CCommand *a_pCmd)
{
	int nLayout;
	char kblayout[60];

	if (a_pCmd->GetIntVar("kblayout", &nLayout) != -1) {
		if (a_pCmd->GetVar("kblanguage", kblayout, 60) != -1) {
			if (strcmp(m_sKbLayout, kblayout) != 0)
				SetKeyboardLayout();
		}
	}
	return 1;
}


int	CRfbToUsb::ConvertVNCKeysToUSBKeys(unsigned int VNCkey, unsigned char down)
{
	unsigned int  ModifKeys = 0, Found, Value, i;
	unsigned char newbut;
	unsigned char noshift = 0;
	unsigned char noalt = 0;
	unsigned int EmulateModifPress = 0;
	_KeySymToUSB kstu = {0,0,0,0};

	VNCkey &= 0x0000FFFF;

	if(!m_pUSBHID)
		return 1;

	//For XPI virtual keyboard will follow convetion to always keep CAPS-LOCK On 

	//Send CAPS-LOCK USB Code
	if (down && (GetKeyboardLEDStatus()  & KEYB_CAPS_LOCK)) {
		repBuf[0] = (KEYB_TOKEN | (USB_CAPSLOCK_CODE << 16) | m_nModifKeysLatched);
		repBuf[1] = 0;
		repBuf[0] = (repBuf[0] & ~0xFFFF) | ((repBuf[0] >> 8) & 0xFF);
		//SYSLOG_INFO(" Rep = %x %x \n", repBuf[0], repBuf[1]);
		//SYSLOG_INFO("-%u- %s( Writing CAPSLOCK ON Rep = %x %x )\n", __LINE__, __FUNCTION__, repBuf[0], repBuf[1]);
		if (!m_pUSBHID->WriteKeyboard((char *)repBuf, 8))
			return 1;
	}

	Found = 0;
	Value = 0;
	switch (VNCkey & 0xFF00) {
	case (0x0000):  /* The state of NumLock is not relevant here */
		{
			//This is a printable character key.
			//Find the USB code for the char from table.
			Value = m_pEncode00xx[VNCkey];

			if(!Value)// not found in the Encode00xx , now look into KeySymToUSBStruct , may be a dead key
            {

                if(XKKeySymToUSB(VNCkey,kstu))
                {
                    Value = kstu.Button2;
                    Found = 1;

                    //When CAPS ON and SHIFT is pressed then small characters needs to be simulated.
                    if( (m_nModifKeysLatched) && !(Value & 0xFF00) )
                    {
                        if( m_nModifKeysLatched & USB_LEFT_SHIFT )
                        {
                            m_nModifKeysLatched &= ~USB_LEFT_SHIFT;
                        }

                        if( m_nModifKeysLatched & USB_RIGHT_SHIFT )
                        {
                            m_nModifKeysLatched &= ~USB_RIGHT_SHIFT;
                        }
                    }

                }
            }
			else
            {
                Found = 1;
                // check if modifier keys already pressed
                // next, check whether R_Shift is not already pressed, and in this case ignore the L_Shift from Encode00xx[] array
                if ((m_nModifKeysLatched) && (Value & 0xFF00))
                {
                    // Set/Unset right modifier bits to simulate char
                    //TO DO : Analyze. Why to set/unset if already set either left or right?

                    /*
                    // an Alt or Shift key must be pressed by SCN
                    if (m_nLeftCtrlHold && (Value & USB_RIGHT_CTRL))
                        Value &= ~USB_RIGHT_CTRL;
                    else if (m_nRightCtrlHold && (Value & USB_LEFT_CTRL))
                        Value &= ~USB_LEFT_CTRL;
                    if (m_nLeftShftHold && (Value & USB_RIGHT_SHIFT))
                        Value &= ~USB_RIGHT_SHIFT;
                    else if (m_nRightShftHold && (Value & USB_LEFT_SHIFT))
                        Value &= ~USB_LEFT_SHIFT;
                    if (m_nLeftAltHold && (Value & USB_RIGHT_ALT))
                        Value &= ~USB_RIGHT_ALT;
                    else if (m_nRightAltHold && (Value & USB_LEFT_ALT))
                        Value &= ~USB_LEFT_ALT;
                    if (m_nLeftGUIHold && (Value & USB_RIGHT_GUI))
                        Value &= ~USB_RIGHT_GUI;
                    else if (m_nRightGUIHold && (Value & USB_LEFT_GUI))
                        Value &= ~USB_LEFT_GUI;
                    */

                    // when CAPS is ON and CTRL is pressed then ignore the modifiers.
                    if( (m_nModifKeysLatched & USB_LEFT_CTRL) ||  (m_nModifKeysLatched & USB_RIGHT_CTRL) )
                    {
                         EmulateModifPress = Value & 0xFF00;
                         if(EmulateModifPress)
                            Value &= ~EmulateModifPress;
                    }
                   
                }
                // AZERTY shift/alt + (number) key
                if (Value & NO_SHIFT_REQ) //NO_ALT_REQ
                {
                    noshift =1;
                }
                if (Value & NO_ALT_REQ)
                {
                    noalt =1;
                }

                //When CAPS ON and SHIFT is pressed then small characters needs to be simulated.
                if( (m_nModifKeysLatched) && !(Value & 0xFF00) )
                {
                    if( m_nModifKeysLatched & USB_LEFT_SHIFT )
                    {
                        m_nModifKeysLatched &= ~USB_LEFT_SHIFT;
                    }

                    if( m_nModifKeysLatched & USB_RIGHT_SHIFT )
                    {
                        m_nModifKeysLatched &= ~USB_RIGHT_SHIFT;
                    }
                }
            }

			break;
		}
	case (0xFF00):
		{
			//This is a non-printable control key e.g. CTRL, ALT, F1, ENTER etc.
			ModifKeys = EncodeFFxx[VNCkey & 0x00FF] & ~0xFF;    /* only bits 31..8 are interesting */
			Value = EncodeFFxx[VNCkey & 0x00FF];    /* all bits */
			Found = 1;
			if (ModifKeys && !(Value & 0x000F00FF)) {   /* this case is to handle specificly Ctrl, Alt, Shift, GUI keys pressed alone. *//* These keys are implemented as Sticky keys, since VNC protocol does not use separated codes */
				// static unsigned int m_nPrevModifKeysLatched;   /* for Alt+x, Ctrl+s etc. bijv. VNC client sends first CTRL code, next 's' code to server */

				switch (ModifKeys) {    /* The state of NumLock is not relevant here */
				case (USB_LEFT_CTRL):   /* XK_Control_L */
					m_nLeftCtrlHold = (down) ? USB_LEFT_CTRL : 0;
					break;
				case (USB_LEFT_SHIFT):  /* XK_Shift_L arrived from client */
					m_nLeftShftHold = (down) ? USB_LEFT_SHIFT : 0;
					break;
				case (USB_LEFT_ALT):    /* XK_Alt_L */
					m_nLeftAltHold = (down) ? USB_LEFT_ALT : 0;
					break;
				case (USB_LEFT_GUI):    /* XK_Hyper_L or XK_Super_L Windows key */
					m_nLeftGUIHold = (down) ? USB_LEFT_GUI : 0;
					break;
				case (USB_RIGHT_CTRL):  /* XK_Control_R */
					m_nRightCtrlHold = (down) ? USB_RIGHT_CTRL : 0;
					break;
				case (USB_RIGHT_SHIFT): /* XK_Shift_R */
					m_nRightShftHold = (down) ? USB_RIGHT_SHIFT : 0;
					break;
				case (USB_RIGHT_ALT):   /* XK_Alt_R */
					m_nRightAltHold = (down) ? USB_RIGHT_ALT : 0;
					break;
				case (USB_RIGHT_GUI):   /* XK_Hyper_R or XK_Super_R Windows key */
					m_nRightGUIHold = (down) ? USB_RIGHT_GUI : 0;
					break;
				default:
					Found = 0;
					break;
				}
				m_nModifKeysLatched = (m_nLeftCtrlHold | m_nLeftShftHold | m_nLeftAltHold | m_nLeftGUIHold | m_nRightCtrlHold | m_nRightShftHold | m_nRightAltHold | m_nRightGUIHold);
				/* do not repeat pressed Shift/Ctrl/Alt/GUI key, otherwise Slave is going nuts */
				if (m_nModifKeysLatched == m_nPrevModifKeysLatched)
					Found = 0;
				m_nPrevModifKeysLatched = m_nModifKeysLatched;
			} else {    /* handle here only the cases where NumLock is required/banned */
				if ((ModifKeys & 0xF0000) && (Value & 0xFF) && down) {  /* All this fun below was added because NumLock does not exist for VNC protocol. */

					switch (ModifKeys) {
					case (NO_NUMLOCK_REQ):  /* NumLock LED must be off the VNC key to be interpretted correctly by Slave */
						if (GetKeyboardLEDStatus() & KEYB_NUM_LOCK) {  /* try to turn off NumLock at Slave */
							m_nTempKey[0] = KEYB_TOKEN | (USB_NUMLOCK_CODE << 16); /* send NumLock keystroke to USB itf */
							m_nTempKey[1] = 0;
							m_nTempKey[0] = (m_nTempKey[0] & ~0xFFFF) | ((m_nTempKey[0] >> 8) & 0xFF);
							if (!m_pUSBHID->WriteKeyboard(( char *)m_nTempKey, 8)) {
								return 1;
							}
							usleep(NUMLOCK_TOGGLE_PAUSE * 1000);
							/*rfbLog(" NO_NUM_REQ "); */
						}/*else the NUMLOCK is OFF already */
						break;
					case (NUMLOCK_REQ): /* NumLock LED must be on */
						if (!(GetKeyboardLEDStatus()  & KEYB_NUM_LOCK)) {
							m_nTempKey[0] = KEYB_TOKEN | (USB_NUMLOCK_CODE << 16);
							m_nTempKey[1] = 0;
							m_nTempKey[0] = (m_nTempKey[0] & ~0xFFFF) | ((m_nTempKey[0] >> 8) & 0xFF);
							if (!m_pUSBHID->WriteKeyboard(( char *)m_nTempKey, 8)) {
								return 1;
							}
							usleep(NUMLOCK_TOGGLE_PAUSE * 1000);
						}/*else the NUMLOCK is ON already */
						break;
						/*add here next special cases */
					default:
						Found = 0;
						break;
					}   
				}   
			}
			/* the Keycode is still Value = EncodeFFxx[VNCkey & 0x00FF] and Found = 1; */
			break;
		}
	default:
		{
			if(XKKeySymToUSB(VNCkey,kstu))
            {
                if(!kstu.IsDeadKey)
                {
                    Value = kstu.Button1;
                    Found = 1;
                    if ((m_nModifKeysLatched) && (Value & 0xFF00))
                    {
                        // Set/Unset right modifier bits to simulate char
                        //TO DO : Analyze. Why to set/unset if already set either left or right?

                        // an Alt or Shift key must be pressed by SCN
                        if (m_nLeftCtrlHold && (Value & USB_RIGHT_CTRL))
                            Value &= ~USB_RIGHT_CTRL;
                        else if (m_nRightCtrlHold && (Value & USB_LEFT_CTRL))
                            Value &= ~USB_LEFT_CTRL;
                        if (m_nLeftShftHold && (Value & USB_RIGHT_SHIFT))
                            Value &= ~USB_RIGHT_SHIFT;
                        else if (m_nRightShftHold && (Value & USB_LEFT_SHIFT))
                            Value &= ~USB_LEFT_SHIFT;
                        if (m_nLeftAltHold && (Value & USB_RIGHT_ALT))
                            Value &= ~USB_RIGHT_ALT;
                        else if (m_nRightAltHold && (Value & USB_LEFT_ALT))
                            Value &= ~USB_LEFT_ALT;
                        if (m_nLeftGUIHold && (Value & USB_RIGHT_GUI))
                            Value &= ~USB_RIGHT_GUI;
                        else if (m_nRightGUIHold && (Value & USB_LEFT_GUI))
                            Value &= ~USB_LEFT_GUI;


                        // when CAPS is ON and CTRL is pressed then ignore the modifiers.
                        if( (m_nModifKeysLatched & USB_LEFT_CTRL) ||  (m_nModifKeysLatched & USB_RIGHT_CTRL) )
                        {
                            EmulateModifPress = Value & 0xFF00;
                            if(EmulateModifPress)
                                Value &= ~EmulateModifPress;
                        }

                    }
                }

                //When CAPS ON and SHIFT is pressed then small characters needs to be simulated.
                if( (m_nModifKeysLatched) && !(Value & 0xFF00) )
                {
                    if( m_nModifKeysLatched & USB_LEFT_SHIFT )
                    {
                        m_nModifKeysLatched &= ~USB_LEFT_SHIFT;
                    }

                    if( m_nModifKeysLatched & USB_RIGHT_SHIFT )
                    {
                        m_nModifKeysLatched &= ~USB_RIGHT_SHIFT;
                    }
                }

                noshift = 0;
                noalt = 0;
                SYSLOG_INFO("-%u- %s( KeySym received not in the range of printable and non-printable characters. VNCkey=%04X Value=%04X )\n", __LINE__, __FUNCTION__, VNCkey,Value);
            }
			else
			{
				Found = 0;
				Value = 0;
			}
			break;
		}
	}// endof Switch

	if (!Value)
	{
		SYSLOG_INFO("-%u- %s( Undefined VNCkey=0x%04X )\n", __LINE__, __FUNCTION__, VNCkey);
	}

	if (!Found)
	{
		return 0;   /* sorry, still not supported code, or repeated Special Key (Shift, Ctrl...) */
	}

	newbut = Value & 0xFF;


	for (i = 2; i < 8; i++) {
		if ((m_sButton[i] == 0) || (m_sButton[i] == newbut)) {
			if (down) {
				m_sButton[i] = newbut;    /* the place is free, or contains the same butt value, write the new button */
				break;
			} else {    /* a button was released, scan m_sButton array and clean it     */
				while (i < 7){ /* move all records left since a button was released */
					m_sButton[i] = m_sButton[i + 1];
					i++;
				}
				m_sButton[i] = 0;
				break;

			}   
		}   
	}

	// this is to handle shift or AltGr states. 'A' can come as 'SHIFT down', 'A down' (Windows VNC client) or 'A' only (JAVA client)

	/* add alt,shift, ctrl... to ordinary keys, bits 15..8 of values from table Encode00xx */

	EmulateModifPress = Value & 0xFF00;

    // for printable characters
	if( ((VNCkey & 0xFF00) == 0) && (EmulateModifPress)) 
    {
        if (down == false)
            EmulateModifPress = 0;
    }
   
    // for non-printable characters like ALT,CTRL,SHIFT etc.
    if( ((VNCkey & 0xFF00 ) == 0xFF00) && (EmulateModifPress))
    {
         if (down == false)
            EmulateModifPress = 0;
    }

	repBuf[0] = (((unsigned int) m_sButton[3] << 24) | ((unsigned int) m_sButton[2] << 16) | ( m_nModifKeysLatched | EmulateModifPress) | KEYB_TOKEN);
	repBuf[1] = (((unsigned int) m_sButton[7] << 24) | ((unsigned int) m_sButton[6] << 16) | ((unsigned int) m_sButton[5] << 8) | ((unsigned int) m_sButton[4] << 0));


	// a check for spurious release code from RemotePointer applet. When a release code arrives, m_sButton press code
	// was not issued before, send 'release all' command to USB keyb module.
	// more than one press code is acceptable.
	Found = 0;
	for ( i = 0; i < 15; i++)   // keep the last 14 pressed m_sButton not released 14 codes . code 15 is always 0
		if (down){
			if ((m_sVNCpr[i] == 0) || (m_sVNCpr[i] == VNCkey)) { // ok, new pressed VNC key or repeated press key
				m_sVNCpr[i] = VNCkey;
				Found = 1;
				break;
			}
		} else {    // a release code arrived, check for available press code
			if (m_sVNCpr[i] == VNCkey) {
				uint16_t j;
				m_sVNCpr[15] = 0;
				for (j = i; j < 15; j++) // free the position, move all records left
					m_sVNCpr[j] = m_sVNCpr[j + 1];
				Found = 1;
				break;
			}   
		}

		if (!Found) {
			for ( i = 0; i < 15; i++)
				m_sVNCpr[i] = 0;
			SYSLOG_INFO("-%u- %s ( Spurious release code %04X, releasing pressed keys.)\n", __LINE__, __FUNCTION__,VNCkey);
		}

		if (!Found) { // the client has just disappeared, m_sButton the clients have a bad habit to leave keys or mouse buttons pressed ....
			repBuf[0] = KEYB_TOKEN;
			repBuf[1] = 0;
			m_nModifKeysLatched = 0;
			m_nLeftCtrlHold = m_nLeftShftHold = m_nLeftAltHold = m_nLeftGUIHold = 0;
			m_nRightCtrlHold = m_nRightShftHold = m_nRightAltHold = m_nRightGUIHold = 0;
			for (i = 0; i < 8; m_sButton[i++] = 0);
		}

		//  move bits 15..8 to 7..0, and 15..8 are 00 now
		unsigned int cc = repBuf[0] >> 8  & 0xFF;
		repBuf[0] &= ~0xFFFF;
		repBuf[0] |= cc;

		// Remove shift bits from modifiers report byte --- numbers on AZERTY...
		if(noshift)
		{
			repBuf[0] &= ~0x22;
		}
		if(noalt)
		{
			repBuf[0] &= ~0x44;
		}

		if(kstu.KeySym && kstu.IsDeadKey)
		{


			// store the previous buffer state first 
			m_prevrepBuf[0] = repBuf[0];
			m_prevrepBuf[1] = repBuf[1];

			//simulate the dead key first and then the remaining ones
			EmulateModifPress = kstu.Button1 & 0xFF00;

			unsigned char deadKey = kstu.Button1 & 0xFF;
			repBuf[0] = (((unsigned int) 0 << 24) | ((unsigned int) deadKey << 16) | EmulateModifPress | KEYB_TOKEN);
			repBuf[1] = (((unsigned int) 0 << 24) | ((unsigned int) 0 << 16) | ((unsigned int) 0 << 8) | ((unsigned int) 0 << 0));

			//  move bits 15..8 to 7..0, and 15..8 are 00 now
			unsigned int cc = repBuf[0] >> 8  & 0xFF;
			repBuf[0] &= ~0xFFFF;
			repBuf[0] |= cc;

			if(down)
			{
				if (!m_pUSBHID->WriteKeyboard((char *)repBuf, 8)) {
					return 1;
				}
			}

			if (!m_pUSBHID->WriteKeyboard((char *)m_prevrepBuf, 8)) {
				return 1;
			}


		}
		else
		{
			if (!m_pUSBHID->WriteKeyboard((char *)repBuf, 8)) {
				return 1;
			}
		}
		return 8;

}

bool CRfbToUsb::XKKeySymToUSB(unsigned int keysym,_KeySymToUSB &kstu)
{
	int min = 0;
	int max = m_nKeySymToUSBStructSize - 1;
	int mid;
	bool found = false;

	if(m_pKeySymToUSBStruct == NULL)
		return found;
	//binary search in table 
	while (max >= min) {
		mid = (min + max) / 2;
		if (m_pKeySymToUSBStruct[mid].KSTU.KeySym < keysym)
			min = mid + 1;
		else if (m_pKeySymToUSBStruct[mid].KSTU.KeySym > keysym)
			max = mid - 1;
		else {
			//found it 
			kstu = m_pKeySymToUSBStruct[mid].KSTU;
			found = true;
			break;
		}
	}
	return found;
}

int	CRfbToUsb::GetKeyboardLEDStatus()
{
	if(m_pUSBHID)
		return m_pUSBHID->GetKeyboardLEDStatus();

	return 0;
}

int	CRfbToUsb::SetKeyboardLayout()
{
	FILE *fp;
	char buf[64];
	char *cp;
	char fpath[64] =  "/etc/config/txkblayout_0.txt";

	fp = fopen(fpath,"r");
	if (fp == NULL)
	{

		strcpy(m_sKbLayout,"US");
		m_nKeyBoardLayout = KEYBOARD_LAYOUT_ENGLISH_US;
		m_nKeySymToUSBStructSize = 0;
		
		if(m_pKeySymToUSBStruct)
		{
			delete [] m_pKeySymToUSBStruct;
			m_pKeySymToUSBStruct = NULL;
		}

		memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
		memcpy(m_pEncode00xx,Encode00xx_English_US_Layout,ARRAY_SIZE*sizeof(unsigned int));

		return 0;    /* return default English layout */
	}

	fgets(buf, 64, fp);
	if ((cp = strchr(buf, '\n')) != 0)
		*cp = '\0';
	fclose(fp);


	if (!strcmp(buf, "US"))
	{
		strcpy(m_sKbLayout,"US");
		m_nKeyBoardLayout = KEYBOARD_LAYOUT_ENGLISH_US;
		m_nKeySymToUSBStructSize = 0;
		
		if(m_pKeySymToUSBStruct)
		{
			delete [] m_pKeySymToUSBStruct;
			m_pKeySymToUSBStruct = NULL;
		}

		memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
		memcpy(m_pEncode00xx,Encode00xx_English_US_Layout,ARRAY_SIZE*sizeof(unsigned int));
	}
	else if (!strcmp(buf, "Belgian French"))
	{
		strcpy(m_sKbLayout,"Belgian French");
		m_nKeyBoardLayout = KEYBOARD_LAYOUT_FRENCH_BELGIUM;
		m_nKeySymToUSBStructSize = sizeof(KeySymToUSBStruct_French_Belgium) / sizeof(_KeySymToUSB);
		memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
		memcpy(m_pEncode00xx,Encode00xx_French_Belgium_Layout,ARRAY_SIZE*sizeof(unsigned int));
		
		if(m_pKeySymToUSBStruct)
		{
			delete [] m_pKeySymToUSBStruct;
			m_pKeySymToUSBStruct = NULL;
		}


		m_pKeySymToUSBStruct = new _KeySymToUSBStruct[m_nKeySymToUSBStructSize];
		
		if(m_pKeySymToUSBStruct)
		{
			memset(m_pKeySymToUSBStruct,0,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
			memcpy(m_pKeySymToUSBStruct,KeySymToUSBStruct_French_Belgium,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
		}
				
				
	}
    else if (!strcmp(buf, "Dutch"))
	{
		strcpy(m_sKbLayout,"Dutch");
		m_nKeyBoardLayout = KEYBOARD_LAYOUT_DUTCH_NETHERLANDS;
		m_nKeySymToUSBStructSize = sizeof(KeySymToUSBStruct_Dutch_Netherlands) / sizeof(_KeySymToUSB);
		memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
		memcpy(m_pEncode00xx,Encode00xx_Dutch_Netherlands_Layout,ARRAY_SIZE*sizeof(unsigned int));
		
		if(m_pKeySymToUSBStruct)
		{
			delete [] m_pKeySymToUSBStruct;
			m_pKeySymToUSBStruct = NULL;
		}


		m_pKeySymToUSBStruct = new _KeySymToUSBStruct[m_nKeySymToUSBStructSize];
		
		if(m_pKeySymToUSBStruct)
		{
			memset(m_pKeySymToUSBStruct,0,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
			memcpy(m_pKeySymToUSBStruct,KeySymToUSBStruct_Dutch_Netherlands,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
		}
				
				
	}
    else if (!strcmp(buf, "Italian"))
	{
		strcpy(m_sKbLayout,"Italian");
		m_nKeyBoardLayout = KEYBOARD_LAYOUT_ITALIAN_ITALY;
		m_nKeySymToUSBStructSize = sizeof(KeySymToUSBStruct_Italian_Italy) / sizeof(_KeySymToUSB);
		memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
		memcpy(m_pEncode00xx,Encode00xx_Italian_Italy_Layout,ARRAY_SIZE*sizeof(unsigned int));
		
		if(m_pKeySymToUSBStruct)
		{
			delete [] m_pKeySymToUSBStruct;
			m_pKeySymToUSBStruct = NULL;
		}


		m_pKeySymToUSBStruct = new _KeySymToUSBStruct[m_nKeySymToUSBStructSize];
		
		if(m_pKeySymToUSBStruct)
		{
			memset(m_pKeySymToUSBStruct,0,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
			memcpy(m_pKeySymToUSBStruct,KeySymToUSBStruct_Italian_Italy,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
		}
				
				
	}
	else if (!strcmp(buf, "French"))
	{
		strcpy(m_sKbLayout,"French");
		m_nKeyBoardLayout = KEYBOARD_LAYOUT_FRENCH_FRANCE;
		m_nKeySymToUSBStructSize = sizeof(KeySymToUSBStruct_French_France) / sizeof(_KeySymToUSB);
		memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
		memcpy(m_pEncode00xx,Encode00xx_French_France_Layout,ARRAY_SIZE*sizeof(unsigned int));
		
		if(m_pKeySymToUSBStruct)
		{
			delete [] m_pKeySymToUSBStruct;
			m_pKeySymToUSBStruct = NULL;
		}


		m_pKeySymToUSBStruct = new _KeySymToUSBStruct[m_nKeySymToUSBStructSize];
		
		if(m_pKeySymToUSBStruct)
		{
			memset(m_pKeySymToUSBStruct,0,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
			memcpy(m_pKeySymToUSBStruct,KeySymToUSBStruct_French_France,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
		}
				
	}
	else if (!strcmp(buf, "Spanish"))
	{
        strcpy(m_sKbLayout,"Spanish");
		m_nKeyBoardLayout = KEYBOARD_LAYOUT_SPANISH_SPAIN;
		m_nKeySymToUSBStructSize = sizeof(KeySymToUSBStruct_Spanish_Spain) / sizeof(_KeySymToUSB);
		memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
		memcpy(m_pEncode00xx,Encode00xx_Spanish_Spain_Layout,ARRAY_SIZE*sizeof(unsigned int));

		if(m_pKeySymToUSBStruct)
		{
			delete [] m_pKeySymToUSBStruct;
			m_pKeySymToUSBStruct = NULL;
		}


		m_pKeySymToUSBStruct = new _KeySymToUSBStruct[m_nKeySymToUSBStructSize];
		if(m_pKeySymToUSBStruct)
		{
			memset(m_pKeySymToUSBStruct,0,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
			memcpy(m_pKeySymToUSBStruct,KeySymToUSBStruct_Spanish_Spain,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
		}

        
	}
	else if (!strcmp(buf, "German"))
	{
		strcpy(m_sKbLayout,"German");
		m_nKeyBoardLayout = KEYBOARD_LAYOUT_GERMAN_GERMANY;
		m_nKeySymToUSBStructSize = sizeof(KeySymToUSBStruct_German_Germany) / sizeof(_KeySymToUSB);
		memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
		memcpy(m_pEncode00xx,Encode00xx_German_Germany_Layout,ARRAY_SIZE*sizeof(unsigned int));

		if(m_pKeySymToUSBStruct)
		{
			delete [] m_pKeySymToUSBStruct;
			m_pKeySymToUSBStruct = NULL;
		}


		m_pKeySymToUSBStruct = new _KeySymToUSBStruct[m_nKeySymToUSBStructSize];
		if(m_pKeySymToUSBStruct)
		{
			memset(m_pKeySymToUSBStruct,0,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
			memcpy(m_pKeySymToUSBStruct,KeySymToUSBStruct_German_Germany,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
		}

	}
    else if (!strcmp(buf, "Portuguese"))
	{
		strcpy(m_sKbLayout,"Portuguese");
		m_nKeyBoardLayout = KEYBOARD_LAYOUT_PORTUGUESE_PORTUGAL;
		m_nKeySymToUSBStructSize = sizeof(KeySymToUSBStruct_Portuguese_Portugal) / sizeof(_KeySymToUSB);
		memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
		memcpy(m_pEncode00xx,Encode00xx_Portuguese_Portugal_Layout,ARRAY_SIZE*sizeof(unsigned int));
		
		if(m_pKeySymToUSBStruct)
		{
			delete [] m_pKeySymToUSBStruct;
			m_pKeySymToUSBStruct = NULL;
		}


		m_pKeySymToUSBStruct = new _KeySymToUSBStruct[m_nKeySymToUSBStructSize];
		
		if(m_pKeySymToUSBStruct)
		{
			memset(m_pKeySymToUSBStruct,0,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
			memcpy(m_pKeySymToUSBStruct,KeySymToUSBStruct_Portuguese_Portugal,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
		}
				
				
	}
    else if (!strcmp(buf, "Russian"))
	{
		strcpy(m_sKbLayout,"Russian");
		m_nKeyBoardLayout = KEYBOARD_LAYOUT_RUSSIAN_RUSSIA;
		m_nKeySymToUSBStructSize = sizeof(KeySymToUSBStruct_Russian_Russia) / sizeof(_KeySymToUSB);
		memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
		memcpy(m_pEncode00xx,Encode00xx_Russian_Russia_Layout,ARRAY_SIZE*sizeof(unsigned int));
		
		if(m_pKeySymToUSBStruct)
		{
			delete [] m_pKeySymToUSBStruct;
			m_pKeySymToUSBStruct = NULL;
		}


		m_pKeySymToUSBStruct = new _KeySymToUSBStruct[m_nKeySymToUSBStructSize];
		
		if(m_pKeySymToUSBStruct)
		{
			memset(m_pKeySymToUSBStruct,0,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
			memcpy(m_pKeySymToUSBStruct,KeySymToUSBStruct_Russian_Russia,m_nKeySymToUSBStructSize*sizeof(_KeySymToUSB));
		}
				
		
		
	}
	else
	{
		strcpy(m_sKbLayout,"US");
		m_nKeyBoardLayout = KEYBOARD_LAYOUT_ENGLISH_US;
		m_nKeySymToUSBStructSize = 0;
		
		if(m_pKeySymToUSBStruct)
		{
			delete [] m_pKeySymToUSBStruct;
			m_pKeySymToUSBStruct = NULL;
		}


		memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
		memcpy(m_pEncode00xx,Encode00xx_English_US_Layout,ARRAY_SIZE*sizeof(unsigned int));
	}

	return 1;

}
