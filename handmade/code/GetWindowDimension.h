#ifndef GETWINDOWDIM_H
#define GETWINDOWDIM_H

#include "Structures.h"

win32_window_dimension
win32GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;

    return (Result);
}

#endif // !GETWINDOWDIM_H
