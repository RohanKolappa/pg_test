/*
 * wineventhandler.hh
 *
 *  Created on: Dec 13, 2010
 *      Author: rkale
 */

#ifndef WINEVENTHANDLER_HH_
#define WINEVENTHANDLER_HH_

#include "keymap.hh"
#include "geometry.hh"

class CWindowsEventHandler
{
public:
    CWindowsEventHandler();
    ~CWindowsEventHandler();

    void SetFrameDimensions(V2DLib::Dimension aDim);
    void SetFrameOffset(V2DLib::Point aOffset);

    void SendKeyboardEvent(uint32_t key, bool down);
    void SendMouseEvent(const V2DLib::Point & mousePos, uint8_t keyFlag);
    uint64_t GetMouseInputCount() { return m_nMouseInputCount; };
    uint64_t GetKeyboardInputCount() { return m_nKeyboardInputCount; };
    uint32_t GetInputErrorCount() { return m_nInputErrors; };

private:
    V2DLib::Dimension m_FrameDim;
    V2DLib::Point m_FrameOffset;
    uint8_t m_prevKeyFlag;
    CKeymap m_keyMap;

    static const BYTE EXTENDED_KEYS[];
    bool m_controlIsPressed;
    bool m_menuIsPressed;
    bool m_deleteIsPressed;
    bool m_shiftIsPressed;
    bool m_winIsPressed;
    bool m_ctrlAltDelEnabled;
    bool m_bInitializedKeys;
    uint64_t m_nMouseInputCount;
    uint64_t m_nKeyboardInputCount;
    uint32_t m_nInputErrors;

    void InjectKeyEvent(BYTE vkCode, bool release);
    void InjectCharEvent(WCHAR ch, bool release);
    bool IsExtendedKey(BYTE vkCode);
    bool IsInvariantToShift(BYTE virtKey, HKL keyboardLayout);
    bool CapsToggled();
    HKL GetCurrentKbdLayout();
    SHORT SearchVirtKey(WCHAR ch, HKL hklCurrent, bool & exception);
    bool IsDeadKey(SHORT scanResult, HKL keyboardLayout);
    bool IsOneKeyEventChar(WCHAR ch, SHORT scanResult,
            HKL keyboardLayout);
    void ResetModifiers();

};

#endif /* WINEVENTHANDLER_HH_ */
