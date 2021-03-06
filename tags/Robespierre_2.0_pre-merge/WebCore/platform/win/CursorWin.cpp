/*
 * Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "Cursor.h"

#include "Image.h"
#include "IntPoint.h"
#include <windows.h>

#define ALPHA_CURSORS

namespace WebCore {

Cursor::Cursor(const Cursor& other)
    : m_impl(other.m_impl)
{
}

static inline bool supportsAlphaCursors() 
{
    OSVERSIONINFO osinfo = {0};
    osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osinfo);
    return osinfo.dwMajorVersion > 5 || (osinfo.dwMajorVersion == 5 && osinfo.dwMinorVersion > 0);
}

Cursor::Cursor(Image* img, const IntPoint& hotspot) 
{ 
    static bool doAlpha = supportsAlphaCursors();
    HBITMAP hCursor;
    BITMAPINFO cursorImage = {0};
    cursorImage.bmiHeader.biSize = sizeof(BITMAPINFO);
    cursorImage.bmiHeader.biWidth = img->width();
    cursorImage.bmiHeader.biHeight = img->height();
    cursorImage.bmiHeader.biPlanes = 1;
    cursorImage.bmiHeader.biBitCount = 32;
    cursorImage.bmiHeader.biCompression = BI_RGB;
    HDC dc = GetDC(0);
    HDC workingDC = CreateCompatibleDC(dc);
    if (doAlpha) {
        hCursor = CreateDIBSection(dc, (BITMAPINFO *)&cursorImage, DIB_RGB_COLORS, 0, 0, 0);
        assert(hCursor);

        img->getHBITMAP(hCursor); 
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(workingDC, hCursor);
        SetBkMode(workingDC,TRANSPARENT);
        SelectObject(workingDC, hOldBitmap);

        HBITMAP hMask = CreateBitmap(img->width(),img->height(),1,1,NULL);

        ICONINFO ii;
        ii.fIcon = FALSE;
        ii.xHotspot = hotspot.x();
        ii.yHotspot = hotspot.y();
        ii.hbmMask = hMask;
        ii.hbmColor = hCursor;

        m_impl = CreateIconIndirect(&ii);
      
        DeleteObject(hMask); 
        DeleteObject(hCursor); 
        DeleteObject(hOldBitmap);    
    } else {
        //Platform doesn't support alpha blended cursors, so we need
        //to create the mask manually
        HDC andMaskDC = CreateCompatibleDC(dc);
        HDC xorMaskDC = CreateCompatibleDC(dc);
        hCursor = CreateDIBSection(dc, &cursorImage, DIB_RGB_COLORS, 0, 0, 0);
        assert(hCursor);
        img->getHBITMAP(hCursor); 
        BITMAP cursor;
        GetObject(hCursor, sizeof(BITMAP), &cursor);
        HBITMAP andMask = CreateBitmap(cursor.bmWidth, cursor.bmHeight, 1, 1, NULL);
        HBITMAP xorMask = CreateCompatibleBitmap(dc, cursor.bmWidth, cursor.bmHeight);
        HBITMAP oldCursor = (HBITMAP)SelectObject(workingDC, hCursor);
        HBITMAP oldAndMask = (HBITMAP)SelectObject(andMaskDC, andMask);
        HBITMAP oldXorMask = (HBITMAP)SelectObject(xorMaskDC, xorMask);

        SetBkColor(workingDC, RGB(0,0,0));  
        BitBlt(andMaskDC, 0, 0, cursor.bmWidth, cursor.bmHeight, workingDC, 0, 0, SRCCOPY);
    
        SetBkColor(xorMaskDC, RGB(255, 255, 255));
        SetTextColor(xorMaskDC, RGB(255, 255, 255));
        BitBlt(xorMaskDC, 0, 0, cursor.bmWidth, cursor.bmHeight, andMaskDC, 0, 0, SRCCOPY);
        BitBlt(xorMaskDC, 0, 0, cursor.bmWidth, cursor.bmHeight, workingDC, 0,0, SRCAND);

        SelectObject(workingDC, oldCursor);
        SelectObject(andMaskDC, oldAndMask);
        SelectObject(xorMaskDC, oldXorMask);


        ICONINFO icon = {0};
        icon.fIcon = FALSE;
        icon.xHotspot = hotspot.x();
        icon.yHotspot = hotspot.y();
        icon.hbmMask = andMask;
        icon.hbmColor = xorMask;
        m_impl = CreateIconIndirect(&icon);

        DeleteObject(andMask);
        DeleteObject(xorMask);
        DeleteObject(hCursor);
        DeleteDC(xorMaskDC);
        DeleteDC(andMaskDC);
    }
    DeleteDC(workingDC);
    ReleaseDC(0, dc);
}

Cursor::~Cursor()
{
    DestroyIcon(m_impl);
}

Cursor& Cursor::operator=(const Cursor& other)
{
    m_impl = other.m_impl;
    return *this;
}

Cursor::Cursor(HCURSOR c)
    : m_impl(c)
{
}

const Cursor& pointerCursor()
{
    static Cursor c = LoadCursor(0, IDC_ARROW);
    return c;
}

const Cursor& crossCursor()
{
    static Cursor c = LoadCursor(0, IDC_CROSS);
    return c;
}

const Cursor& handCursor()
{
    static Cursor c = LoadCursor(0, IDC_HAND);
    return c;
}

const Cursor& iBeamCursor()
{
    static Cursor c = LoadCursor(0, IDC_IBEAM);
    return c;
}

const Cursor& waitCursor()
{
    static Cursor c = LoadCursor(0, IDC_WAIT);
    return c;
}

const Cursor& helpCursor()
{
    static Cursor c = LoadCursor(0, IDC_HELP);
    return c;
}

const Cursor& eastResizeCursor()
{
    static Cursor c = LoadCursor(0, IDC_SIZEWE);
    return c;
}

const Cursor& northResizeCursor()
{
    static Cursor c = LoadCursor(0, IDC_SIZENS);
    return c;
}

const Cursor& northEastResizeCursor()
{
    static Cursor c = LoadCursor(0, IDC_SIZENESW);
    return c;
}

const Cursor& northWestResizeCursor()
{
    static Cursor c = LoadCursor(0, IDC_SIZENWSE);
    return c;
}

const Cursor& southResizeCursor()
{
    static Cursor c = LoadCursor(0, IDC_SIZENS);
    return c;
}

const Cursor& southEastResizeCursor()
{
    static Cursor c = LoadCursor(0, IDC_SIZENWSE);
    return c;
}

const Cursor& southWestResizeCursor()
{
    static Cursor c = LoadCursor(0, IDC_SIZENESW);
    return c;
}

const Cursor& westResizeCursor()
{
    static Cursor c = LoadCursor(0, IDC_SIZEWE);
    return c;
}

const Cursor& northSouthResizeCursor()
{
    static Cursor c = LoadCursor(0, IDC_SIZENS);
    return c;
}

const Cursor& eastWestResizeCursor()
{
    static Cursor c = LoadCursor(0, IDC_SIZEWE);
    return c;
}

const Cursor& northEastSouthWestResizeCursor()
{
    static Cursor c = LoadCursor(0, IDC_SIZENESW);
    return c;
}

const Cursor& northWestSouthEastResizeCursor()
{
    static Cursor c = LoadCursor(0, IDC_SIZENWSE);
    return c;
}

const Cursor& columnResizeCursor()
{
    // FIXME: Windows does not have a standard column resize cursor
    static Cursor c = LoadCursor(0, IDC_SIZEWE);
    return c;
}

const Cursor& rowResizeCursor()
{
    // FIXME: Windows does not have a standard row resize cursor
    static Cursor c = LoadCursor(0, IDC_SIZENS);
    return c;
}

const Cursor& noneCursor()
{
    static Cursor c = LoadCursor(0, IDC_ARROW);
    return c;
}

}
