#ifndef MAINWINDOWCALLBACK_H
#define MAINWINDOWCALLBACK_H

#include "Defines_Typedefs.h"
#include "Structures.h"

//TODO: This is a global for now
global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackBuffer;

LRESULT CALLBACK
MainWindowCallback(HWND WindowHandle,
    UINT Message,
    WPARAM WParam,
    LPARAM LParam)
{
    LRESULT Result = 0;

    switch (Message)
    {
    case WM_SIZE:
    {
    } break;

    case WM_DESTROY:
    {
        //TODO: Handle this as an error - recreate window?
        GlobalRunning = false;
    } break;

    case WM_CLOSE:
    {
        //TODO: Handle this with a message to the user?
        GlobalRunning = false;
    } break;

    case WM_CREATE:
    {
        OutputDebugStringA("WM_CREATE - We have fire!\n");
    } break;

    case WM_MOVE:
    {
        OutputDebugStringA("WM_MOVE\n");
    } break;

    case WM_MOVING:
    {
        OutputDebugStringA("WM_MOVING\n");
    } break;

    case WM_ACTIVATEAPP:
    {
        OutputDebugStringA("WM_ACTIVATEAPP\n");
    } break;

    case WM_SYSKEYUP: /*case WM_SYSKEYDOWN:*/
    case WM_KEYDOWN:case WM_KEYUP:
    {
        uint32 VKCode = WParam;
        bool WasDown = ((LParam & (1 << 30)) != 0);
        bool IsDown = ((LParam & (1 << 31)) == 0);

        // Ignore key repeat messages
        if (IsDown != WasDown)
        {
            if (VKCode == VK_ESCAPE)
            {
                if (WasDown)
                {
                    OutputDebugStringA("ESC Was Down\n");
                }
                else
                {
                    OutputDebugStringA("ESC Was Up \n");
                }

                if (IsDown)
                {
                    OutputDebugStringA("ESC Is Down\n");
                }
                else
                {
                    OutputDebugStringA("ESC is Up\n");
                }

                OutputDebugStringA("-----------------\n");
            }
        }

        bool32 AltKeyWasDown = (LParam & (1 << 29));
        if (VKCode == VK_F4 && AltKeyWasDown)
        {
            GlobalRunning = false;
        }
    } break;

    case WM_PAINT:
    {
        PAINTSTRUCT Paint;
        HDC DeviceContext = BeginPaint(WindowHandle, &Paint);
        win32_window_dimension Dimension = win32GetWindowDimension(WindowHandle);
        Win32DisplayBufferInWindow(GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height);

        local_persist DWORD Operation = BLACKNESS;
        PatBlt(DeviceContext, 0, 0, Dimension.Width, Dimension.Height, Operation);
        switch (Operation)
        {
        case WHITENESS:
        {
            Operation = BLACKNESS;
            OutputDebugStringA("case whiteness\n");
        }break;

        case BLACKNESS:
        {
            Operation = WHITENESS;
            OutputDebugStringA("case blackness\n");
        }break;

        default:
        {
            OutputDebugStringA("Something went wrong here with the whiteness/blackness\n");
        }
        }

        //SetPixel(DeviceContext, 100, 100, RGB(255,0,0));
        EndPaint(WindowHandle, &Paint);
    } break;

    default:
    {
        Result = DefWindowProcA(WindowHandle, Message, WParam, LParam);
    } break;
    }

    return(Result);
}
#endif // !MAINWINDOWCALLBACK_H

