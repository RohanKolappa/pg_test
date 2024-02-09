/*
 * winscreenscraper.hh
 *
 *  Created on: Nov 28, 2010
 *      Author: rkale
 */

#ifndef WINSCREENSCRAPER_HH_
#define WINSCREENSCRAPER_HH_

#include <windows.h>
#include "geometry.hh"
#include "resolutionchangelistener.hh"
#include "codecdefs.h"

class CWindowsScreenScraper
{
public:
    CWindowsScreenScraper(IResolutionChangeListener * a_listener);
    ~CWindowsScreenScraper();

    bool Grab();
    V2DLib::Dimension FrameDimension() const { return m_FrameDim; };
    void *GetBuffer() { return m_buffer; };
    enum ePixelFormat GetPixelFormat() { return m_ePixelFormat; };

private:

    struct BMI
    {
        BITMAPINFOHEADER bmiHeader;
        UINT32 red;
        UINT32 green;
        UINT32 blue;
    };

    V2DLib::Dimension m_FrameDim;
    V2DLib::Dimension m_FullFrameRect;
    V2DLib::Rect m_FullScreenRect;
    V2DLib::Point m_cursorLocation;
    V2DLib::Dimension m_cursorShape;
    V2DLib::Point m_cursorHotSpot;
    HDC m_screenDC;
    HDC m_destDC;
    HBITMAP m_hbmOld;
    HBITMAP m_hbmDIB;
    void *m_buffer;
    HCURSOR m_currentHCursor;
    bool m_bCursorShowing;
    bool m_bCursorAvailable;
    enum ePixelFormat m_ePixelFormat;
    uint64_t m_nGrabCount;

    IResolutionChangeListener * m_pResolutionChangeListener;

    bool AllocateDIBSection();
    void DestroyDIBSection();
    bool GetScreenProperties(BMI & bmi);
    bool ScreenPropertiesChanged();
    void DumpBMI(BMI & bmi);
    bool OverlayMouse();
    bool HasCursorChanged();
    HCURSOR GetHCursor();
    bool RetrieveCursorInfo();

};

#endif /* WINSCREENSCRAPER_HH_ */
