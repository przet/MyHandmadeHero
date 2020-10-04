#ifndef DISPLAYBUFFERINWINODW_H
#define DISPLAYBUFFERINWINODW_H

#include "Structures.h"
void
Win32DisplayBufferInWindow(win32_offscreen_buffer Buffer, HDC DeviceContext, int WindowWidth, int WindowHeight)
{
    // TODO: Aspect ratio correction
    StretchDIBits(DeviceContext,

        /* Draw to whole window FIRST, leave this (subsection of window
         * to later...
        X, Y, Width, Height,
        X, Y, Width, Height,
        */
        // 0,0 to start at top left corner I believe
        0, 0, WindowWidth, WindowHeight,
        0, 0, Buffer.Width, Buffer.Height,
        Buffer.Memory,
        &Buffer.Info,
        DIB_RGB_COLORS,
        SRCCOPY);
}

#endif // DISPLAYBUFFERINWINODW_H
