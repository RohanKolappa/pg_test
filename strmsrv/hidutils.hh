#ifndef HIDUTILS_H_
#define HIDUTILS_H_

#include "command.hh"

#define NUMBEROFGADGETS  3
#define KBREPORTLEN      8  /* Report Len is configured in Hid driver/kernel */
#define MSRELREPORTLEN   4
#define MSABSREPORTLEN   6 

#define HIDKEYBOARDPATH "/dev/hidg0"     /* Keyboard*/
#define HIDRELMOUSEPATH "/dev/hidg1"     /* relative mouse*/
#define HIDABSMOUSEPATH "/dev/hidg2"     /* absolute mouse*/


#define BIT0 (1<<0)
#define BIT1 (1<<1)
#define BIT2 (1<<2)
#define BIT3 (1<<3)
#define BIT4 (1<<4)
#define BIT5 (1<<5)
#define BIT6 (1<<6)
#define BIT7 (1<<7)
#define BIT8 (1<<8)
#define BIT9 (1<<9)
#define BIT10 (1<<10)
#define BIT11 (1<<11)
#define BIT12 (1<<12)
#define BIT13 (1<<13)
#define BIT14 (1<<14)
#define BIT15 (1<<15)
#define BIT16 (1<<16)
#define BIT17 (1<<17)
#define BIT18 (1<<18)
#define BIT19 (1<<19)
#define BIT20 (1<<20)
#define BIT21 (1<<21)
#define BIT22 (1<<22)
#define BIT23 (1<<23)
#define BIT24 (1<<24)
#define BIT25 (1<<25)
#define BIT26 (1<<26)
#define BIT27 (1<<27)
#define BIT28 (1<<28)
#define BIT29 (1<<29)
#define BIT30 (1<<30)
#define BIT31 (1<<31)


/* from HID USB spec, 8.3 p.56. This is byte 0 in an USB keyb report sent to USB Host, but shifted << 8*/
#define USB_LEFT_CTRL BIT8
#define USB_LEFT_SHIFT BIT9
#define USB_LEFT_ALT BIT10
#define USB_LEFT_GUI BIT11
#define USB_RIGHT_CTRL BIT12
#define USB_RIGHT_SHIFT BIT13
#define USB_RIGHT_ALT BIT14
#define USB_RIGHT_GUI BIT15

#define KEYB_NUM_LOCK BIT0
#define KEYB_CAPS_LOCK BIT1
#define KEYB_SCROLL_LOCK BIT2
#define KEYB_COMPOSE BIT3
#define KEYB_KANA BIT4

#define NO_NUMLOCK_REQ BIT16
#define NUMLOCK_REQ BIT17
#define NO_SHIFT_REQ BIT18
#define NO_ALT_REQ BIT19

// some usb constants, byte 0 of
// K (keyboard)
#define KEYB_TOKEN 0x4B
#define USB_CAPSLOCK_CODE 0x39
#define USB_NUMLOCK_CODE 0x53
/* wait 20msec after sending of NumLock from USB gadget interface*/
#define NUMLOCK_TOGGLE_PAUSE 20

#define ARRAY_SIZE 256
#define KEYBOARD_LAYOUT_ENGLISH_US 1
#define KEYBOARD_LAYOUT_GERMAN_GERMANY 2
#define KEYBOARD_LAYOUT_FRENCH_BELGIUM 3
#define KEYBOARD_LAYOUT_FRENCH_FRANCE 4
#define KEYBOARD_LAYOUT_SPANISH_SPAIN 5

struct _KeySymToUSB
{
	unsigned int	KeySym;// XK_Key
	unsigned int	Button1;// USB key code for dead key if KeySym is a dead key else USB key code for KeySym if KeySym is not a dead key
	unsigned int	Button2;// USB key code for generating the dead key if KeySym is a dead key
	bool			IsDeadKey;// whether KeySym is a dead key or not
};

class CHIDutils {

public:
   CHIDutils();
   ~CHIDutils();

   int		SendMouseEvents(char buttonmask, unsigned short XvncAbsol,unsigned short YvncAbsol );
   int		SendRelMouseEvents( char *pInBuf);
   int		SendKeyboardEvents(unsigned int key, unsigned char down);
   int		ProcessRemoteKeyboard(char *pPayload);
   int		ProcessRemoteMouse(char *pPayload);
   int		SetKeyboardLEDsStatus();
   void		SetScreenWidth(int Width);
   void		SetScreenHeight(int height);
   //void		SendCapsEvent(char *key , int down);
   int      Update(CCommand *a_pCmd);
   int		GetKeyboardFD();
   int		GetAbsoluteMouseFD();
   int		GetRelativeMouseFD();
   int		GetKeyboardLEDStatus();

  
private:

   int				m_nKeyboardfd;
   int				m_nMouseAbsfd;
   int				m_nMouseRelfd;
   int				m_nKeyboardLEDs;
   int				m_nWidth;
   int				m_nHeight;
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
   unsigned char	m_sButton[8];
   unsigned short	m_sVNCpr[16];
   int				m_nKeyBoardLayout;
   unsigned int*	m_pEncode00xx;
   unsigned int		m_nTempKey[2];
   int				SetKeyboardLayout();
   char             m_sKbLayout[60];
   void				InitializeVariables();
   void				OpenGadgets();
   void				CloseGadgets();
   int				WriteAbsoluteMouse(char *pInbuf, int buflen);
   int				WriteRelativeMouse(char *pInbuf, int buflen);
   int				WriteKeyboard(char *pInbuf, int buflen);
   int				ConvertVNCKeysToUSBKeys(unsigned int key, unsigned char down);
   bool				XKKeySymToUSB(unsigned int keysym,_KeySymToUSB &kstu);

};
#endif /* HIDUTILS_H_ */
