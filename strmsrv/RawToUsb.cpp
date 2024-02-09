#include "RawToUsb.h"

CRawToUsb::CRawToUsb(CUSBHID *usbHid,SRVR *srvr)
{
	m_pUSBHID = usbHid;
    m_pSrvr = srvr;
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
	m_pEncode00xx = new unsigned int[ARRAY_SIZE];
	memset(m_pEncode00xx,0,ARRAY_SIZE*sizeof(unsigned int));
    m_sButton[0] = m_sButton[1] = m_sButton[2] = m_sButton[3] = 0;
    m_sButton[4] = m_sButton[5] = m_sButton[6] = m_sButton[7] = 0;
}

CRawToUsb::~CRawToUsb()
{
	if(m_pEncode00xx)
	{
		delete [] m_pEncode00xx;
		m_pEncode00xx = NULL;
	}
   
}


int	CRawToUsb::ProcessRawKeyboardData(unsigned int scancode, unsigned char down)
{

	//SYSLOG_INFO("-%u- %s(down=%u key=%04X)\n", __LINE__, __FUNCTION__, down, key);
#if 0
	char nm[64]={0};

	switch(scancode) {
	case 0x76: sprintf(nm, "Esc");break;        case 0x0d: sprintf(nm, "Tab");break;            case 0x14: sprintf(nm, "LCtrl");break;          case 0x21: sprintf(nm, "C"); break;
	case 0x16: sprintf(nm, "1!");break;         case 0x15: sprintf(nm, "Q");break;              case 0x1c: sprintf(nm, "A");break;              case 0x2a: sprintf(nm, "V"); break;
	case 0x1e: sprintf(nm, "2@");break;         case 0x1d: sprintf(nm, "W");break;              case 0x1b: sprintf(nm, "S");break;              case 0x32: sprintf(nm, "B"); break;
	case 0x26: sprintf(nm, "3#");break;         case 0x24: sprintf(nm, "E");break;              case 0x23: sprintf(nm, "D");break;              case 0x31: sprintf(nm, "N"); break;
	case 0x25: sprintf(nm, "4$");break;         case 0x2d: sprintf(nm, "R");break;              case 0x2b: sprintf(nm, "F");break;              case 0x3a: sprintf(nm, "M"); break;
	case 0x2e: sprintf(nm, "5\"%\"");break;     case 0x2c: sprintf(nm, "T");break;              case 0x34: sprintf(nm, "G");break;              case 0x41: sprintf(nm, ",<"); break;
	case 0x36: sprintf(nm, "6^");break;         case 0x35: sprintf(nm, "Y");break;              case 0x33: sprintf(nm, "H");break;              case 0x49: sprintf(nm, ".>"); break; 
	case 0x3d: sprintf(nm, "7&");break;         case 0x3c: sprintf(nm, "U");break;              case 0x3b: sprintf(nm, "J");break;              case 0x4a: sprintf(nm, "/?"); break;
	case 0x3e: sprintf(nm, "8*");break;         case 0x43: sprintf(nm, "I");break;              case 0x42: sprintf(nm, "K");break;              case 0x59: sprintf(nm, "RShift"); break; 
	case 0x46: sprintf(nm, "9(");break;         case 0x44: sprintf(nm, "O");break;              case 0x4b: sprintf(nm, "L");break;              case 0x7c: sprintf(nm, "KP-*"); break; 
	case 0x45: sprintf(nm, "0)");break;         case 0x4d: sprintf(nm, "P");break;              case 0x4c: sprintf(nm, ";:");break;             case 0x11: sprintf(nm, "LAlt"); break;
	case 0x4e: sprintf(nm, "-_");break;         case 0x54: sprintf(nm, "[{");break;             case 0x5d: sprintf(nm, "\\|");break;            case 0x29: sprintf(nm, "space"); break;
	case 0x55: sprintf(nm, "=+");break;         case 0x5b: sprintf(nm, "]}");break;             case 0x1a: sprintf(nm, "Z"); break;             case 0x58: sprintf(nm, "CapsLock"); break;
	case 0x66: sprintf(nm, "Backspace");break;  case 0x5a: sprintf(nm, "Enter");break;          case 0x22: sprintf(nm, "X"); break;             case 0x05: sprintf(nm, "F1"); break;
    case 0x06: sprintf(nm, "F2");break;         case 0x09: sprintf(nm, "F10");break;            case 0x73: sprintf(nm, "KP-5");break;           case 0x78: sprintf(nm, "F11");break;
    case 0x04: sprintf(nm, "F3");break;         case 0x77: sprintf(nm, "NumLock");break;        case 0x74: sprintf(nm, "KP-6 / Right");break;   case 0x07: sprintf(nm, "F12");break;
    case 0x0c: sprintf(nm, "F4");break;         case 0x7e: sprintf(nm, "ScrollLock");break;     case 0x79: sprintf(nm, "KP-+");break;           case 0xe05a: sprintf(nm, "KP-Enter");break;
    case 0x03: sprintf(nm, "F5");break;         case 0x6c: sprintf(nm, "KP-7 / Home");break;    case 0x69: sprintf(nm, "KP-1 / End");break;     case 0xe014: sprintf(nm, "RCtrl");break;
    case 0x0b: sprintf(nm, "F6");break;         case 0x75: sprintf(nm, "KP-8 / Up");break;      case 0x72: sprintf(nm, "KP-2");break;           case 0xe04a: sprintf(nm, "KP-/");break;
    case 0x83: sprintf(nm, "F7");break;         case 0x7d: sprintf(nm, "KP-9 / PgUp");break;    case 0x7a: sprintf(nm, "KP-3 / PgDn");break;    case 0xff15: sprintf(nm, "PrtScr / SysRq");break;
    case 0x0a: sprintf(nm, "F8");break;         case 0x7b: sprintf(nm, "KP--");break;           case 0x70: sprintf(nm, "KP-0 / Ins");break;     case 0xe011: sprintf(nm, "RAlt");break;
    case 0x01: sprintf(nm, "F9");break;         case 0x6b: sprintf(nm, "KP-4 / Left");break;    case 0x71: sprintf(nm, "KP-. / Del");break;     case 0xe06c: sprintf(nm, "Home");break;
    case 0xe075: sprintf(nm, "Up");break;       case 0xe07d: sprintf(nm, "PgUp");break;         case 0xe06b: sprintf(nm, "Left");break;         case 0xe074: sprintf(nm, "Right"); break;
    case 0xe069: sprintf(nm, "End");break;      case 0xe072: sprintf(nm, "Down");break;         case 0xe07a: sprintf(nm, "PgDn");break;         case 0xe070: sprintf(nm, "Insert"); break;
    case 0xe071: sprintf(nm, "Delete");break;   case 0xff13: sprintf(nm, "Pause / Break");break;case 0xe01f: sprintf(nm, "LMeta");break;        case 0xe027: sprintf(nm, "RMeta"); break;
    case 0xe02f: sprintf(nm, "Compose");break;  case 0x52: sprintf(nm, "'\"");break;            case 0x0e: sprintf(nm, "`~");break;             case 0x12: sprintf(nm, "LShift");break;
    default: sprintf(nm, "%c", (char)scancode);break;
	}

	SYSLOG_INFO("-%u- %s( %s (0x%04X), down=%u )\n", __LINE__, __FUNCTION__, nm, scancode,((down)? 1:0));
#endif

    ConvertRawToUsb(scancode, down);

    return 0;

}



int	CRawToUsb::ConvertRawToUsb(unsigned int scancode, unsigned char down)
{

    unsigned int  ModifKeys = 0, Found, Value, i;
    unsigned char newbut;
    unsigned char noshift = 0;
    unsigned char noalt = 0;
    unsigned int EmulateModifPress = 0;
    unsigned int IsPrintable=0;
   
    if(!m_pUSBHID || !m_pSrvr)
    {
        SYSLOG_ERROR("-%u- %s( Either m_pUSBHID is NULL or m_pSrvr is NULL.\n", __LINE__, __FUNCTION__);
        return -1;
    }

   
    Found = 0;
    Value = 0;

    if(GetUsbCode(scancode,&Value,&IsPrintable))
    {
        if(Value == 0)
        {
            SYSLOG_INFO("-%u- %s( Undefined Usb code=%04X for ScanCode=%04X)\n", __LINE__, __FUNCTION__,Value,scancode);
            return -1;
        }

        Found=1;

        //SYSLOG_INFO("-%u- %s( CAPSLOCK GetKeyboardLEDStatus() = %d and GetKeyboardLEDStatus() & KEYB_CAPS_LOCK = %d )\n", __LINE__, __FUNCTION__, GetKeyboardLEDStatus(), (GetKeyboardLEDStatus() & KEYB_CAPS_LOCK));

        // Handling of CAPSLOCK begins
        //Send CAPS-LOCK ON.This is intensionally done on keyup
        if (!down && (Value == USB_CAPSLOCK_CODE) && m_pSrvr->capslock) 
        {
            if( !(GetKeyboardLEDStatus()  & KEYB_CAPS_LOCK) )
            {
                repBuf[0] = (KEYB_TOKEN | (USB_CAPSLOCK_CODE << 16) | m_nModifKeysLatched);
                repBuf[1] = 0;
                repBuf[0] = (repBuf[0] & ~0xFFFF) | ((repBuf[0] >> 8) & 0xFF);
                SYSLOG_INFO("-%u- %s( Writing CAPSLOCK ON Rep = %x %x )\n", __LINE__, __FUNCTION__, repBuf[0], repBuf[1]);
                if (!m_pUSBHID->WriteKeyboard((char *)repBuf, 8))
                    return -1;
            }

            return 0;
        }

        //send CAPS-LOCK OFF.This is intensionally done on keyup
        if (!down && (Value == USB_CAPSLOCK_CODE) && !m_pSrvr->capslock) 
        {
            if( (GetKeyboardLEDStatus()  & KEYB_CAPS_LOCK) )
            {
                repBuf[0] = (KEYB_TOKEN | (USB_CAPSLOCK_CODE << 16) | m_nModifKeysLatched);
                repBuf[1] = 0;
                repBuf[0] = (repBuf[0] & ~0xFFFF) | ((repBuf[0] >> 8) & 0xFF);
                SYSLOG_INFO("-%u- %s( Writing CAPSLOCK OFF Rep = %x %x )\n", __LINE__, __FUNCTION__, repBuf[0], repBuf[1]);
                if (!m_pUSBHID->WriteKeyboard((char *)repBuf, 8))
                    return -1;
            }

            return 0;
        }

        //return if CAPS keydown is received, we will not write it on USB.writing of CAPS is already done on CAPS keyup 
        if (down && (Value == USB_CAPSLOCK_CODE)) 
        {
           SYSLOG_INFO("-%u- %s(Returning as CAPSLOCK is already in ON/OFF state.)\n", __LINE__, __FUNCTION__);
           return 0;
        }
        // Handling CAPSLOCK ends

        /*
        // Handling NUMLOCK begins
        //Send NUM-LOCK ON.This is intensionally done on keyup
        if (!down && (Value == USB_NUMLOCK_CODE) && m_pSrvr->numlock) 
        {
            if( !(GetKeyboardLEDStatus()  & KEYB_NUM_LOCK) )
            {
                repBuf[0] = KEYB_TOKEN | (USB_NUMLOCK_CODE << 16);
                repBuf[1] = 0;
                repBuf[0] = (repBuf[0] & ~0xFFFF) | ((repBuf[0] >> 8) & 0xFF);
                SYSLOG_INFO("-%u- %s( Writing NUMLOCK ON Rep = %x %x )\n", __LINE__, __FUNCTION__, repBuf[0], repBuf[1]);
                if (!m_pUSBHID->WriteKeyboard((char *)repBuf, 8))
                    return -1;

                usleep(NUMLOCK_TOGGLE_PAUSE * 1000);
            }
          
            return 0;
        }

        //send NUM-LOCK OFF.This is intensionally done on keyup
        if (!down && (Value == USB_NUMLOCK_CODE) && !m_pSrvr->numlock) 
        {
            if( (GetKeyboardLEDStatus()  & KEYB_NUM_LOCK) )
            {
                repBuf[0] = KEYB_TOKEN | (USB_NUMLOCK_CODE << 16);
                repBuf[1] = 0;
                repBuf[0] = (repBuf[0] & ~0xFFFF) | ((repBuf[0] >> 8) & 0xFF);
                SYSLOG_INFO("-%u- %s( Writing NUMLOCK OFF Rep = %x %x )\n", __LINE__, __FUNCTION__, repBuf[0], repBuf[1]);
                if (!m_pUSBHID->WriteKeyboard((char *)repBuf, 8))
                    return -1;

                usleep(NUMLOCK_TOGGLE_PAUSE * 1000);
            }

            return 0;
        }

        //return if NUMLOCK keydown is received, we will not write it on USB.writing of NUM is already done on NUM keyup 
        if (down && (Value == USB_NUMLOCK_CODE)) 
        {
           SYSLOG_INFO("-%u- %s(Returning as NUMLOCK is already in ON/OFF state.)\n", __LINE__, __FUNCTION__);
           return 0;
        }
        // Handling NUMLOCK ends
        */
     
        if(IsPrintable)// for printable keys 
        {
            // check if modifier keys already pressed
            // next, check whether R_Shift is not already pressed, and in this case ignore the L_Shift from Encode00xx[] array
            if ((m_nModifKeysLatched) && (Value & 0xFF00))
            {
                // Set/Unset right modifier bits to simulate char
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
        }
        else// for non-printable keys like CTRL,ALT,SHIFT,ENTER,F1..F12 etc.
        {
            ModifKeys = Value & ~0xFF;    /* only bits 31..8 are interesting */
            //Value = EncodeFFxx[VNCkey & 0x00FF];    /* all bits */
            //Found = 1;
            if (ModifKeys && !(Value & 0x000F00FF)) {   /* this case is to handle specificly Ctrl, Alt, Shift, GUI keys pressed alone. *//* These keys are implemented as Sticky keys, since VNC protocol does not use separated codes */
                // static unsigned int m_nPrevModifKeysLatched;   /* for Alt+x, Ctrl+s etc. bijv. VNC client sends first CTRL code, next 's' code to server */

                switch (ModifKeys) 
                {    /* The state of NumLock is not relevant here */
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
            } 
            else 
            {    /* handle here only the cases where NumLock is required/banned */
                if ((ModifKeys & 0xF0000) && (Value & 0xFF) && down) 
                {  /* All this fun below was added because NumLock does not exist for VNC protocol. */

                    switch (ModifKeys) 
                    {
                    case (NO_NUMLOCK_REQ):  /* NumLock LED must be off the VNC key to be interpretted correctly by Slave */
                        if (GetKeyboardLEDStatus() & KEYB_NUM_LOCK) 
                        {  /* try to turn off NumLock at Slave */
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
        }

        if (!Found)
        {
            SYSLOG_INFO("-%u- %s( Still not supported code or repeated Special Key (Shift, Ctrl...)=%04X )\n", __LINE__, __FUNCTION__, scancode);
            return -1;   /* sorry, still not supported code, or repeated Special Key (Shift, Ctrl...) */
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

        if( (IsPrintable) && (EmulateModifPress)) {
            if (down == false)
                EmulateModifPress = 0;
        }

        // for non-printable characters like ALT,CTRL,SHIFT etc.
        if( (!IsPrintable) && (EmulateModifPress))
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
                if ((m_sRAWpr[i] == 0) || (m_sRAWpr[i] == scancode)) { // ok, new pressed VNC key or repeated press key
                    m_sRAWpr[i] = scancode;
                    Found = 1;
                    break;
                }
            } else {    // a release code arrived, check for available press code
                if (m_sRAWpr[i] == scancode) {
                    uint16_t j;
                    m_sRAWpr[15] = 0;
                    for (j = i; j < 15; j++) // free the position, move all records left
                        m_sRAWpr[j] = m_sRAWpr[j + 1];
                    Found = 1;
                    break;
                }   
            }

            if (!Found) {
                for ( i = 0; i < 15; i++)
                    m_sRAWpr[i] = 0;
                SYSLOG_INFO("-%u- %s ( Spurious release code %04X, releasing pressed keys.)\n", __LINE__, __FUNCTION__,scancode);
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

            /*
            SYSLOG_INFO("-%u- %s( m_nModifKeysLatched=%04X, EmulateModifPress=%04X, RawScanCode=%04X , Value=%04X, down=%u, repBuf[0]=%04X, repBuf[1]=%04X )\n",
                __LINE__, 
                __FUNCTION__, 
                m_nModifKeysLatched,
                EmulateModifPress,
                scancode,
                Value,
                ((down)? 1:0),
                repBuf[0],
                repBuf[1]);
            */

            if (!m_pUSBHID->WriteKeyboard((char *)repBuf, 8)) 
            {
				return -1;
			}

    }
    else
    {
        SYSLOG_INFO("-%u- %s( Undefined ScanCode=%04X )\n", __LINE__, __FUNCTION__, scancode);
        return -1;
    }

    return 0;
}



int	CRawToUsb::GetKeyboardLEDStatus()
{
	if(m_pUSBHID)
		return m_pUSBHID->GetKeyboardLEDStatus();

	return 0;
}

int	CRawToUsb::GetUsbCode(unsigned int RawCode,unsigned int *UsbCode,unsigned int *IsPrintable)
{

    *UsbCode = 0;

    for (int i = 0; i < RawToUsbKeyCodeSize; i++)
    {
        if(RawToUsbKeyCodes[i].RawCode == RawCode)
        {
            *UsbCode = RawToUsbKeyCodes[i].UsbCode;
            *IsPrintable = RawToUsbKeyCodes[i].IsPrintable;
            return 1;// success
        }
    }

    return 0;// failure 
}


