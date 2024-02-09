/*
 * wineventhandler.cpp
 *
 *  Created on: Dec 13, 2010
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"

#include "common.hh"
#include "wineventhandler.hh"

using namespace V2DLib;

const BYTE CWindowsEventHandler::EXTENDED_KEYS[] =
{ VK_MENU, VK_CONTROL, VK_INSERT, VK_DELETE, VK_HOME, VK_END,
    VK_PRIOR, VK_NEXT, VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN, VK_NUMLOCK,
    VK_CANCEL, VK_RETURN, VK_DIVIDE, VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2,
    VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD7,
    VK_NUMPAD8, VK_NUMPAD9 };

CWindowsEventHandler::CWindowsEventHandler()
: m_prevKeyFlag(0),
  m_controlIsPressed(false),
  m_menuIsPressed(false),
  m_deleteIsPressed(false),
  m_shiftIsPressed(false),
  m_winIsPressed(false),
  m_ctrlAltDelEnabled(false),
  m_bInitializedKeys(false),
  m_nMouseInputCount(0),
  m_nKeyboardInputCount(0),
  m_nInputErrors(0)
{

}

CWindowsEventHandler::~CWindowsEventHandler()
{

}

void CWindowsEventHandler::SetFrameDimensions(Dimension aDim)
{
    m_FrameDim = aDim;
}

void CWindowsEventHandler::SetFrameOffset(Point aOffset)
{
    m_FrameOffset = aOffset;
}


void CWindowsEventHandler::SendMouseEvent(const Point & mousePos, uint8_t keyFlag)
{
    if (m_FrameDim.Area() == 0)
        return;

    DWORD dwFlags = 0;
    dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

    bool prevState;
    bool currState;
    prevState = (m_prevKeyFlag & 1) != 0;
    currState = (keyFlag & 1) != 0;
    if (currState != prevState) {
        if (currState) {
            dwFlags |= MOUSEEVENTF_LEFTDOWN;
        }
        else {
            dwFlags |= MOUSEEVENTF_LEFTUP;
        }
    }
    prevState = (m_prevKeyFlag & 2) != 0;
    currState = (keyFlag & 2) != 0;
    if (currState != prevState) {
        if (currState) {
            dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
        }
        else {
            dwFlags |= MOUSEEVENTF_MIDDLEUP;
        }
    }
    prevState = (m_prevKeyFlag & 4) != 0;
    currState = (keyFlag & 4) != 0;
    if (currState != prevState) {
        if (currState) {
            dwFlags |= MOUSEEVENTF_RIGHTDOWN;
        }
        else {
            dwFlags |= MOUSEEVENTF_RIGHTUP;
        }
    }

    DWORD mouseWheelValue = 0;
    bool prevWheelUp = (m_prevKeyFlag & 8) != 0;
    bool currWheelUp = (keyFlag & 8) != 0;
    bool prevWheelDown = (m_prevKeyFlag & 16) != 0;
    bool currWheelDown = (keyFlag & 16) != 0;

    if (currWheelUp && !prevWheelUp) {
        dwFlags |= MOUSEEVENTF_WHEEL;
        mouseWheelValue = 120;
    }
    else if (currWheelDown && !prevWheelDown) {
        dwFlags |= MOUSEEVENTF_WHEEL;
        mouseWheelValue = -120;
    }

    m_prevKeyFlag = keyFlag;

    Point point = m_FrameDim.Clip(mousePos);

    int x = (INT32)((point.x + m_FrameOffset.x) * 65535
            / (m_FrameDim.width - 1));
    int y = (INT32)((point.y + m_FrameOffset.y) * 65535 / (m_FrameDim.height
            - 1));

    INPUT input;
    memset(&input, 0, sizeof(INPUT));
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = dwFlags;
    input.mi.dx = x;
    input.mi.dy = y;
    input.mi.mouseData = mouseWheelValue;
    //SYSLOG_INFO("Sending Mouse event %d %d", x, y);
    m_nMouseInputCount++;
    if (SendInput(1, &input, sizeof(INPUT)) == 0)
        m_nInputErrors++;
    //DWORD error = GetLastError();
}

void CWindowsEventHandler::SendKeyboardEvent(uint32_t key, bool down)
{
    BYTE vkCode;
    WCHAR ch;

    if (!m_bInitializedKeys) {
        ResetModifiers();
        m_bInitializedKeys = true;
    }

    if (m_keyMap.KeySymToVirtualCode(key, &vkCode)) {
        SYSLOG_INFO("Key Virtual code %x %d", vkCode, down);
        InjectKeyEvent(vkCode, !down);
    } else if (m_keyMap.KeySymToUnicodeChar(key, &ch)) {
        SYSLOG_INFO("Key Unicode Char %x %d", ch, down);
        InjectCharEvent(ch, !down);
    } else {
        SYSLOG_INFO("Unknown %d key", key);
    }

}

void CWindowsEventHandler::InjectKeyEvent(BYTE vkCode, bool release)
{
    if (vkCode == VK_CONTROL || vkCode == VK_RCONTROL || vkCode == VK_LCONTROL) {
        m_controlIsPressed = !release;
    }
    if (vkCode == VK_MENU || vkCode == VK_RMENU || vkCode == VK_LMENU) {
        m_menuIsPressed = !release;
    }
    if (vkCode == VK_DELETE) {
        m_deleteIsPressed = !release;
    }
    if (vkCode == VK_SHIFT || vkCode == VK_RSHIFT || vkCode == VK_LSHIFT) {
        m_shiftIsPressed = !release;
    }
    if (vkCode == VK_LWIN || vkCode == VK_RWIN) {
        m_winIsPressed = !release;
    }

    if (m_controlIsPressed && m_menuIsPressed && m_deleteIsPressed &&
            !m_winIsPressed && !m_shiftIsPressed) {
        if (m_ctrlAltDelEnabled) {
         //   Environment::simulateCtrlAltDel();
        }
    } else {
        INPUT keyEvent = {0};

        keyEvent.type = INPUT_KEYBOARD;
        keyEvent.ki.wVk = vkCode;
        keyEvent.ki.wScan = MapVirtualKey(vkCode, 0);

        if (release) {
            keyEvent.ki.dwFlags = KEYEVENTF_KEYUP;
        }

        if (IsExtendedKey(vkCode)) {
            keyEvent.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
        }

        m_nKeyboardInputCount++;
        if (SendInput(1, &keyEvent, sizeof(keyEvent)) == 0) {
            m_nInputErrors++;
            SYSLOG_ERROR("InjectKeyEvent: SendInput() function failed");
//            DWORD errCode = GetLastError();
//            if (errCode != ERROR_SUCCESS) {
//                throw SystemException(errCode);
//            } else {
//                throw Exception(_T("SendInput() function failed"));
//            }
        }
    }
}

void CWindowsEventHandler::InjectCharEvent(WCHAR ch, bool release)
{
    bool ctrlOrAltPressed = m_controlIsPressed || m_menuIsPressed;
    SHORT vkKeyScanResult = 0;
    HKL hklCurrent = (HKL)0x04090409;
    bool exception = false;
    hklCurrent = GetCurrentKbdLayout();
    if (hklCurrent != NULL) {
        vkKeyScanResult = SearchVirtKey(ch, hklCurrent, exception);
    }
    else {
        exception = true;
    }

    if (exception) {
        if (ctrlOrAltPressed) {
            SYSLOG_ERROR("InjectCharEvent: ctrlOrAltPressed");
            return;
            // throw;
        }
        INPUT keyEvent = {0};

        keyEvent.type = INPUT_KEYBOARD;
        keyEvent.ki.wVk = 0;
        keyEvent.ki.wScan = ch;
        keyEvent.ki.dwFlags = KEYEVENTF_UNICODE;

        if (release) {
            keyEvent.ki.dwFlags |= KEYEVENTF_KEYUP;
        }
        m_nKeyboardInputCount++;
        if (SendInput(1, &keyEvent, sizeof(keyEvent)) == 0) {
            m_nInputErrors++;
            SYSLOG_ERROR("InjectCharEvent: SendInput() function failed");
            //throw SystemException();
        }
        return;
    }
    bool controlSym;
    if ((ch >= L'A' && ch <= L'Z') || (ch >= L'a' && ch <= L'z')) {
        controlSym = m_controlIsPressed && !m_menuIsPressed && !m_shiftIsPressed;
    } else {
        controlSym = false;
    }

    bool invariantToShift = IsInvariantToShift((BYTE)vkKeyScanResult, hklCurrent);

    bool shiftedKey = (vkKeyScanResult >> 8 & 1) != 0 && !CapsToggled();
    bool shiftPressNeeded = shiftedKey && !m_shiftIsPressed && !release &&
            !controlSym;
    bool shiftUpNeeded = !shiftedKey && m_shiftIsPressed && !release &&
            !controlSym && !invariantToShift;
    bool ctrlPressNeeded = (vkKeyScanResult >> 9 & 1) && !m_controlIsPressed &&
            !release;
    bool altPressNeeded = (vkKeyScanResult >> 10 & 1) && !m_menuIsPressed &&
            !release;
    if ((ctrlPressNeeded || altPressNeeded) &&
            (m_controlIsPressed || m_menuIsPressed)) {
        return;
    }

    if (ctrlPressNeeded) {
        InjectKeyEvent(VK_CONTROL, false);
    }
    if (altPressNeeded) {
        InjectKeyEvent(VK_MENU, false);
    }
    if (shiftPressNeeded) {
        InjectKeyEvent(VK_SHIFT, false);
    } else if (shiftUpNeeded) {
        InjectKeyEvent(VK_SHIFT, true);
    }
    InjectKeyEvent(vkKeyScanResult & 255, release);
    if (shiftPressNeeded) {
        InjectKeyEvent(VK_SHIFT, true);
    } else if (shiftUpNeeded) {
        InjectKeyEvent(VK_SHIFT, false);
    }
    if (altPressNeeded) {
        InjectKeyEvent(VK_MENU, true);
    }
    if (ctrlPressNeeded) {
        InjectKeyEvent(VK_CONTROL, true);
    }
}

bool CWindowsEventHandler::IsExtendedKey(BYTE vkCode) {
    int i = sizeof(EXTENDED_KEYS);
    while (i-- > 0) {
        if (EXTENDED_KEYS[i] == vkCode) {
            return true;
        }
    }
    return false;
}

bool CWindowsEventHandler::IsInvariantToShift(BYTE virtKey, HKL keyboardLayout)
{
    unsigned char kbdState[256];
    memset(kbdState, 0, sizeof(kbdState));
    WCHAR outBuff1[20], outBuff2[20];

    int count1 = ToUnicodeEx(virtKey, 0, kbdState, outBuff1,
            sizeof(outBuff1) / sizeof(WCHAR),
            0, keyboardLayout);

    kbdState[VK_SHIFT] = 128;
    int count2 = ToUnicodeEx(virtKey, 0, kbdState, outBuff2,
            sizeof(outBuff2) / sizeof(WCHAR),
            0, keyboardLayout);
    if (count1 != count2) return false;
    if (memcmp(outBuff1, outBuff2, count1 * sizeof(WCHAR)) != 0) {
        return false;
    } else {
        return true;
    }
}

bool CWindowsEventHandler::CapsToggled()
{
  return (GetKeyState(VK_CAPITAL) & 1) != 0;
}

HKL CWindowsEventHandler::GetCurrentKbdLayout()
{
    HWND hwnd = GetForegroundWindow();
    if (hwnd == 0) {
        SYSLOG_ERROR("Can't insert key event because a window is losing activation");
//        throw Exception(_T("Can't insert key event because")
//                _T(" a window is losing activation"));
        return NULL;
    }
    DWORD threadId = GetWindowThreadProcessId(hwnd, 0);
    return GetKeyboardLayout(threadId);
}

SHORT CWindowsEventHandler::SearchVirtKey(WCHAR ch, HKL hklCurrent, bool & exception)
{
//    bool modifiersPressed = m_controlIsPressed || m_menuIsPressed ||
//            m_shiftIsPressed;
    bool onlyCtrlPressed = m_controlIsPressed && !m_menuIsPressed &&
            !m_shiftIsPressed;
    SHORT vkKeyScanResult = VkKeyScanExW(ch, hklCurrent);
    if (vkKeyScanResult == -1) {
        if (onlyCtrlPressed) {
            if (ch >= L'A' && ch <= L'Z') {
                return (SHORT)ch;
            } else if (ch >= L'a' && ch <= L'z') {
                return (SHORT)(ch - L'a' + L'A');
            }
        }
//        StringStorage errMess;
//        errMess.format(_T("Can't translate the %d character to the scan code"),
//                (unsigned int)ch);
//        throw Exception(errMess.getString());
        SYSLOG_ERROR("Can't translate the %d character to the scan code",
                (unsigned int)ch);
        exception = true;
        return 0;
    }
    if (IsDeadKey(vkKeyScanResult, hklCurrent)) {
        SYSLOG_INFO("Special dead symbol must be inserted only as unicode character");
//        throw Exception(_T("Special dead symbol must be inserted")
//                _T(" only as unicode character"));
        exception = true;
        return 0;
    }
    if (!IsOneKeyEventChar(ch, vkKeyScanResult, hklCurrent)) {
        SYSLOG_ERROR("Can't get the %d character by one keyboard event",
                (unsigned int)ch);
//        StringStorage errMess;
//        errMess.format(_T("Can't get the %d character by one keyboard event"),
//                (unsigned int)ch);
//        throw Exception(errMess.getString());
        exception = true;
        return 0;
    }
    return vkKeyScanResult;
}

bool CWindowsEventHandler::IsDeadKey(SHORT scanResult, HKL keyboardLayout)
{
    unsigned char kbdState[256];
    memset(kbdState, 0, sizeof(kbdState));
    WCHAR outBuff[20];
    bool withShift = (scanResult >> 8 & 1) != 0;
    bool withCtrl  = (scanResult >> 9 & 1) != 0;
    bool withAlt   = (scanResult >> 10 & 1) != 0;

    kbdState[VK_SHIFT]   = withShift ? 128 : 0;
    kbdState[VK_CONTROL] = withCtrl  ? 128 : 0;
    kbdState[VK_MENU]    = withAlt   ? 128 : 0;

    unsigned char virtKey = scanResult & 255;

    int count = ToUnicodeEx(virtKey, 0, kbdState, outBuff,
            sizeof(outBuff) / sizeof(WCHAR),
            0, keyboardLayout);
    bool result = count == -1;
    count = ToUnicodeEx(virtKey, 0, kbdState, outBuff,
            sizeof(outBuff) / sizeof(WCHAR),
            0, keyboardLayout);
    result = result || count == -1;
    return result;
}

bool CWindowsEventHandler::IsOneKeyEventChar(WCHAR ch, SHORT scanResult,
        HKL keyboardLayout)
{
    unsigned char kbdState[256];
    memset(kbdState, 0, sizeof(kbdState));
    WCHAR outBuff[20];
    bool withShift = (scanResult >> 8 & 1) != 0;
    bool withCtrl  = (scanResult >> 9 & 1) != 0;
    bool withAlt   = (scanResult >> 10 & 1) != 0;

    kbdState[VK_SHIFT]   = withShift ? 128 : 0;
    kbdState[VK_CONTROL] = withCtrl  ? 128 : 0;
    kbdState[VK_MENU]    = withAlt   ? 128 : 0;

    unsigned char virtKey = scanResult & 255;

    int count = ToUnicodeEx(virtKey, 0, kbdState, outBuff,
            sizeof(outBuff) / sizeof(WCHAR),
            0, keyboardLayout);
    if (count == 1) {
        return outBuff[0] == ch;
    } else {
        return false;
    }
}

void CWindowsEventHandler::ResetModifiers()
{
    InjectKeyEvent(VK_MENU, true);
    InjectKeyEvent(VK_LMENU, true);
    InjectKeyEvent(VK_RMENU, true);
    InjectKeyEvent(VK_SHIFT, true);
    InjectKeyEvent(VK_LSHIFT, true);
    InjectKeyEvent(VK_RSHIFT, true);
    InjectKeyEvent(VK_CONTROL, true);
    InjectKeyEvent(VK_LCONTROL, true);
    InjectKeyEvent(VK_RCONTROL, true);
    InjectKeyEvent(VK_LWIN, true);
    InjectKeyEvent(VK_RWIN, true);
    InjectKeyEvent(VK_DELETE, true);
}
