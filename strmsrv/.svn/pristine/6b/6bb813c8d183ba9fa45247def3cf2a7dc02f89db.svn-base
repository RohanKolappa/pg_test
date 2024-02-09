/*
 * winscreenscraper.cpp
 *
 *  Created on: Nov 28, 2010
 *      Author: rkale
 */


#include "strmsrv_compatibility.h"
#include "common.hh"

#include "winscreenscraper.hh"

using namespace V2DLib;

CWindowsScreenScraper::CWindowsScreenScraper(IResolutionChangeListener * a_listener)
{
    m_screenDC = NULL;
    m_destDC = NULL;
    m_buffer = NULL;
    m_currentHCursor = NULL;
    m_bCursorShowing = false;
    m_bCursorAvailable = false;
    m_pResolutionChangeListener = a_listener;
    m_ePixelFormat = eBGRX32;
    m_nGrabCount = 0;

    if (!AllocateDIBSection()) {
        SYSLOG_INFO("Could not allocate DIB Section");
    }
}

CWindowsScreenScraper::~CWindowsScreenScraper()
{
    DestroyDIBSection();
}

bool CWindowsScreenScraper::AllocateDIBSection()
{
    DestroyDIBSection();

    // Create primary screen's device context
    m_screenDC = GetDC(0);
    if (m_screenDC == NULL) {
        return false;
    }

    // Get screen properties
    BMI bmi;
    if (!GetScreenProperties(bmi))
        return false;
    //DumpBMI(bmi);
    m_FrameDim.SetDim(bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight);
    Dimension primaryScreen(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    if (primaryScreen != m_FrameDim && !primaryScreen.IsEmpty()) {
        m_FrameDim = primaryScreen;
    }
    m_ePixelFormat = eBGRX32;
    if (bmi.bmiHeader.biBitCount == 16) {
        m_ePixelFormat = eBGR24;
    }

    // Create destination DC
    bmi.bmiHeader.biBitCount = (m_ePixelFormat == eBGR24) ? 24 : 32;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_FrameDim.width;
    bmi.bmiHeader.biHeight = - m_FrameDim.height;
    bmi.bmiHeader.biCompression = (m_ePixelFormat == eBGR24) ? BI_RGB : BI_BITFIELDS;
    bmi.red = 0x00FF0000;
    bmi.green = 0x0000FF00;
    bmi.blue = 0x000000FF;

    m_destDC = CreateCompatibleDC(NULL);
    if (m_destDC == NULL) {
        DeleteDC(m_screenDC);
        m_screenDC = NULL;
        SYSLOG_ERROR("Failed creating compatible DC");
        return false;
    }

    // Create a DIB section on the destination DC
    void *buffer;
    m_hbmDIB = CreateDIBSection(m_destDC, (BITMAPINFO *) &bmi, DIB_RGB_COLORS,
            &buffer, NULL, 0);
    if (m_hbmDIB == 0) {
        DeleteDC(m_screenDC);
        m_screenDC = NULL;
        DeleteDC(m_destDC);
        m_destDC = NULL;
        SYSLOG_ERROR("Failed creating DIB section");
        return false;
    }

    m_buffer = buffer;

    m_hbmOld = (HBITMAP) SelectObject(m_destDC, m_hbmDIB);

    return true;
}

void CWindowsScreenScraper::DestroyDIBSection()
{
    if (m_destDC != NULL)
        SelectObject(m_destDC, m_hbmOld);

    if (m_hbmDIB != NULL) {
        DeleteObject(m_hbmDIB);
        m_hbmDIB = NULL;
    }

    if (m_screenDC != NULL) {
        DeleteDC(m_screenDC);
        m_screenDC = NULL;
    }

    if (m_destDC != NULL) {
        DeleteDC(m_destDC);
        m_destDC = NULL;
    }
    m_buffer = NULL;
}

bool CWindowsScreenScraper::GetScreenProperties(BMI & bmi)
{
    // Get screen properties
    memset(&bmi, 0x00, sizeof(BMI));
    bmi.bmiHeader.biBitCount = 0;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    HBITMAP hbm;
    hbm = (HBITMAP) GetCurrentObject(m_screenDC, OBJ_BITMAP);
    if (GetDIBits(m_screenDC, hbm, 0, 0, NULL, (LPBITMAPINFO) & bmi,
            DIB_RGB_COLORS) == 0) {
        DeleteDC(m_screenDC);
        m_screenDC = NULL;
        DeleteObject(hbm);
        return false;
    }

    if (bmi.bmiHeader.biCompression == BI_BITFIELDS) {
        // Call again to get the color info
        if (GetDIBits(m_screenDC, hbm, 0, 0, NULL, (LPBITMAPINFO) & bmi,
                DIB_RGB_COLORS) == 0) {
            DeleteDC(m_screenDC);
            m_screenDC = NULL;
            DeleteObject(hbm);
            return false;
        }
    }
    DeleteObject(hbm);

    return true;
}

bool CWindowsScreenScraper::ScreenPropertiesChanged()
{
    BMI bmi;
    if (!GetScreenProperties(bmi)) {
        return false;
    }

#if 0
    // Will need this when we need to support non primary screen
    m_FullScreenRect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    m_FullScreenRect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    m_FullScreenRect.SetWidth(GetSystemMetrics(SM_CXVIRTUALSCREEN));
    m_FullScreenRect.SetHeight(GetSystemMetrics(SM_CYVIRTUALSCREEN));
    printf("Full Screen Rect = %d %d %d %d\n",
            m_FullScreenRect.left, m_FullScreenRect.top,
            m_FullScreenRect.GetWidth(), m_FullScreenRect.GetHeight());
#endif

    bool bScreenPropertiesChanged = false;

    enum ePixelFormat ePixelFormat = eBGRX32;
    if (bmi.bmiHeader.biBitCount == 16) {
        ePixelFormat = eBGR24;
    }
    if (m_ePixelFormat != ePixelFormat) {
        m_ePixelFormat = ePixelFormat;
        bScreenPropertiesChanged = true;
    }

    Dimension newDim(bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight);
    Dimension primaryScreen(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    if (primaryScreen != newDim && !primaryScreen.IsEmpty()) {
        if (m_FrameDim != primaryScreen) {
            m_FrameDim = primaryScreen;
            bScreenPropertiesChanged = true;
        }
    }
    else  {
        if (m_FrameDim != newDim) {
            m_FrameDim = newDim;
            bScreenPropertiesChanged = true;
        }
    }

    if (bScreenPropertiesChanged && m_pResolutionChangeListener != NULL)
        m_pResolutionChangeListener->OnResolutionChange();

    return bScreenPropertiesChanged;
}

void CWindowsScreenScraper::DumpBMI(BMI & bmi)
{
    SYSLOG_INFO("Size %lu", bmi.bmiHeader.biSize);
    SYSLOG_INFO("Width %ld", bmi.bmiHeader.biWidth);
    SYSLOG_INFO("Height %ld", bmi.bmiHeader.biHeight);
    SYSLOG_INFO("Planes %d", bmi.bmiHeader.biPlanes);
    SYSLOG_INFO("BitCount %d", bmi.bmiHeader.biBitCount);
    SYSLOG_INFO("Compression %lu", bmi.bmiHeader.biCompression);
    SYSLOG_INFO("SizeImage %lu", bmi.bmiHeader.biSizeImage);
    SYSLOG_INFO("XPelsPM %ld", bmi.bmiHeader.biXPelsPerMeter);
    SYSLOG_INFO("YPelsPM %ld", bmi.bmiHeader.biYPelsPerMeter);
    SYSLOG_INFO("ClrUsed %ld", bmi.bmiHeader.biClrUsed);
    SYSLOG_INFO("ClrImprotant %ld", bmi.bmiHeader.biClrImportant);
    SYSLOG_INFO("Red %08x", bmi.red);
    SYSLOG_INFO("Green %08x", bmi.green);
    SYSLOG_INFO("Blue %08x", bmi.blue);
}

bool CWindowsScreenScraper::Grab()
{
    if (m_buffer == NULL || m_destDC == NULL || m_screenDC == NULL)
        return false;
    if (ScreenPropertiesChanged()) {
        AllocateDIBSection();
    }
    m_nGrabCount++;
    if (BitBlt(m_destDC, 0, 0, m_FrameDim.width, m_FrameDim.height,
            m_screenDC, 0, 0, SRCCOPY) == 0) {
        return false;
    }

    OverlayMouse();

    return true;
}

bool CWindowsScreenScraper::OverlayMouse()
{
    if (HasCursorChanged()) {
        m_bCursorAvailable = true;
        if (!RetrieveCursorInfo())
            m_bCursorAvailable = false;
    }

    if (!m_bCursorAvailable) {
        return false;
    }


    // Draw the cursor into the current frame
    if (DrawIconEx(m_destDC,
            m_cursorLocation.x - m_cursorHotSpot.x,
            m_cursorLocation.y - m_cursorHotSpot.y,
            m_currentHCursor, 0, 0, 0, NULL, DI_NORMAL | DI_COMPAT | DI_DEFAULTSIZE) == 0) {
    }

    return true;
}

bool CWindowsScreenScraper::HasCursorChanged()
{
    HCURSOR hCursor = GetHCursor();
    if (hCursor == m_currentHCursor) {
        return false;
    }
    SYSLOG_DEBUG("Got New Cursor %d", (int) m_nGrabCount);
    m_currentHCursor = hCursor;
    return true;
}

HCURSOR CWindowsScreenScraper::GetHCursor()
{
    CURSORINFO cursorInfo;
    cursorInfo.cbSize = sizeof(CURSORINFO);

    if (GetCursorInfo(&cursorInfo) == 0) {
        return NULL;
    }
    m_bCursorShowing = false;
    if (cursorInfo.flags & CURSOR_SHOWING)
        m_bCursorShowing = true;

    Point newCursorLocation(cursorInfo.ptScreenPos.x, cursorInfo.ptScreenPos.y);
    if (m_cursorLocation != newCursorLocation) {
        m_cursorLocation = newCursorLocation;
//        printf("cursor location %d %d\n",
//            m_cursorLocation.x,
//            m_cursorLocation.y);
    }
    return cursorInfo.hCursor;
}

bool CWindowsScreenScraper::RetrieveCursorInfo()
{
    if (!m_bCursorShowing || m_currentHCursor == NULL) {
        return false;
    }

    ICONINFO iconInfo;
    if (!GetIconInfo(m_currentHCursor, &iconInfo)) {
        return false;
    }

    if (iconInfo.hbmMask == NULL) {
        DeleteObject(iconInfo.hbmColor);
        return false;
    }


    BITMAP bmMask;
    if (!GetObject(iconInfo.hbmMask, sizeof(BITMAP), (LPVOID)&bmMask)) {
        DeleteObject(iconInfo.hbmMask);
        DeleteObject(iconInfo.hbmColor);
        return false;
    }

    if (bmMask.bmPlanes != 1 || bmMask.bmBitsPixel != 1) {
        DeleteObject(iconInfo.hbmMask);
        DeleteObject(iconInfo.hbmColor);
        return false;
    }

    m_cursorHotSpot.SetPoint(iconInfo.xHotspot, iconInfo.yHotspot);

    bool isColorShape = (iconInfo.hbmColor != NULL);
    int width = bmMask.bmWidth;
    int height = isColorShape ? bmMask.bmHeight : bmMask.bmHeight/2;

    //int widthBytes = bmMask.bmWidthBytes;
//    printf("width height widthBytes %d %d %d hotspot %d %d\n",
//            width, height, widthBytes, m_cursorHotSpot.x, m_cursorHotSpot.y);

    m_cursorShape.SetDim(width, height);

    DeleteObject(iconInfo.hbmMask);
    DeleteObject(iconInfo.hbmColor);

    return true;
}
