/* =======================================================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori - Philippe Rzetelski following along
   ======================================================================================================= */
   // ^ version control system keyword replacement

    #include <windows.h>
    #include <stdint.h> //uint8_t

    // Undefine the following to forcce use of ANSI or UNICODE versions - we will always use ANSI,
    // so doing the below will: 1) make us use suffix A 2) If someone tries to compile in Unicode mode, an error will occur (good)
    #undef CreateWindowEx
    #undef OutputDebugString
    #undef TranslateMessage
    #undef DispatchMessage
    #undef DefWindowProc 
    #undef PeekMessage 

    #define internal static 
    #define local_persist static
    #define global_variable static

    // Platform (machine) independent size
    typedef uint8_t uint8;
    typedef uint16_t uint16;
    typedef uint32_t uint32;
    typedef uint64_t uint64;

    typedef int8_t int8;
    typedef int16_t int16;
    typedef int32_t int32;
    typedef int64_t int64;


    struct win32_offscreen_buffer
    {
        BITMAPINFO Info; 
        void * Memory; 
        int Width; 
        int Height; 
        int Pitch; 
        int BytesPerPixel;
    };

    struct win32_window_dimension
    {
        int Width;
        int Height;
    };

    RECT
    GetWindowDimension(HWND Window, win32_window_dimension *WindowDimensions)
    {
        RECT ClientRect;
        GetClientRect(Window, &ClientRect);
        WindowDimensions->Width = ClientRect.right - ClientRect.left;
        WindowDimensions->Height = ClientRect.bottom - ClientRect.top;
        return ClientRect;
    }




    //TODO: This is a global for now
    global_variable bool Running;
    global_variable win32_offscreen_buffer GlobalBackBuffer;
    global_variable win32_window_dimension WindowDimensions;


    internal void
    RenderWeirdGradient(win32_offscreen_buffer Buffer, int XOffset, int YOffset)
    {

        // TODO : See what optimizer does (with pass by value of buffer??)
        
        // casting void to char type (want per byte arithmetic)
        uint8 *Row = (uint8 *)Buffer.Memory;

        for (int Y = 0; Y < Buffer.Height; ++Y)
        {
            uint32 *Pixel = (uint32 *)Row;
            for (int X = 0; X < Buffer.Width; ++X)
            {
                // Explanation:
                // 
                // Memory(hex) : BB GG RR xx (xx is pad)
                // Register (hex. Assuming little endian architecture) xx RR GG BB
                //
                // So now that we are dealing with 32 bits directly (i.e 32-bit pixel) we rewrite

                /*
                    *Pixel++ = X + XOffset;

                    *Pixel++ = Y + YOffset;

                    *Pixel++ = 0;

                    *Pixel++ = 0;
                */

                // as
                uint8 Blue = X + XOffset;
                uint8 Green = Y + YOffset;
                uint8 Red = 0;
                uint8 Pad = 0;

                *Pixel++ = (Blue | Green << 8 | Red << 16 | Pad << 24);
                // TODO : operator precedence : remove brackets
            }

            Row += Buffer.Pitch;
            // TODO Casey makes a comment here that the separation of operations (Row adding and Pixel)
            // may seem inefficient, we do ++Pixel because sometimes byte boundaries may differ(??).
            // Its < 33:30
        }

    }


    internal void
    Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width , int Height)
    {

        if (Buffer->Memory)
        {
            VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
        }

        Buffer->Width = Width;
        Buffer->Height = Height;
        Buffer->BytesPerPixel = 4;

        // Specify our BitmapInfo structure attributes
        Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
        Buffer->Info.bmiHeader.biWidth = Buffer->Width;
        
        // Top down so negative (see MSDN)
        Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
        Buffer->Info.bmiHeader.biPlanes = 1;
        
        // 32 for 4byte alignment (24  =3 bytes; 1 more byte for this "padding" to give 
        // alignment on 4 byte boundary (x86 architecture efficiency)
        Buffer->Info.bmiHeader.biBitCount = 32;
        Buffer->Info.bmiHeader.biCompression = BI_RGB;
        Buffer->Info.bmiHeader.biSizeImage = 0;
        Buffer->Info.bmiHeader.biXPelsPerMeter = 0;
        Buffer->Info.bmiHeader.biYPelsPerMeter = 0;
        Buffer->Info.bmiHeader.biClrUsed = 0;
        Buffer->Info.bmiHeader.biClrImportant = 0;

        int BitmapMemorySize = (Buffer->Width*Buffer->Height)*Buffer->BytesPerPixel;

        // VirtualAlloc returns pages - a bit more "raw" than HeapAlloc
        Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

        Buffer->Pitch = Buffer->Width*Buffer->BytesPerPixel;

        //TODO : we might want to initialise this to black 


    }

    void
    // Alternate name could be DisplayBufferInWindow
    // Passing by value - not changing Buffer
    // See previous points re why it can be safer to pass by value
    Win32UpdateWindow(win32_offscreen_buffer Buffer, HDC DeviceContext, RECT WindowRect, int X , int Y, int Width, int Height)
    {
        int WindowWidth = WindowRect.right - WindowRect.left;
        int WindowHeight = WindowRect.bottom - WindowRect.top;
        StretchDIBits(DeviceContext,
                      
                      /* Draw to whole window FIRST, leave this (subsection of window
                       * to later...
                      X, Y, Width, Height,
                      X, Y, Width, Height,
                      */
                      // 0,0 to start at top left corner I believe
                      0, 0, Buffer.Width, Buffer.Height,
                      0, 0, WindowWidth, WindowHeight,
                      Buffer.Memory,
                      &Buffer.Info,
                      DIB_RGB_COLORS,
                      SRCCOPY); 
    }

    LRESULT CALLBACK
    MainWindowCallback(HWND WindowHandle,
                       UINT Message,
                       WPARAM WParam, 
                       LPARAM LParam) 
    {
        LRESULT Result = 0;

        switch(Message)
        {
            case WM_SIZE:
            {
                GetWindowDimension(WindowHandle, &WindowDimensions);
                Win32ResizeDIBSection(&GlobalBackBuffer, WindowDimensions.Width, WindowDimensions.Height);
            } break;

            case WM_DESTROY:
            {
                //TODO: Handle this as an error - recreate window?
                Running = false;
            } break;

            case WM_CLOSE:
            {
                //TODO: Handle this with a message to the user?
                Running = false;
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

            case WM_PAINT:
            {
                PAINTSTRUCT Paint;
                HDC DeviceContext = BeginPaint(WindowHandle, &Paint);
                LONG X = Paint.rcPaint.left;
                LONG Y = Paint.rcPaint.right;
                LONG Width = Paint.rcPaint.right - Paint.rcPaint.left;
                LONG Height = Paint.rcPaint.top - Paint.rcPaint.bottom;

                RECT ClientRect = GetWindowDimension(WindowHandle, &WindowDimensions);
                Win32UpdateWindow(GlobalBackBuffer, DeviceContext, ClientRect, X, Y, WindowDimensions.Width, WindowDimensions.Height);

                local_persist DWORD Operation = BLACKNESS;
                PatBlt(DeviceContext, X, Y, WindowDimensions.Width, WindowDimensions.Height, Operation);
                switch(Operation)
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


    int CALLBACK 
    WinMain(HINSTANCE Instance,
           HINSTANCE PrevInstance,
           LPSTR CmdLine,
           int ShowCmd)
    {
        int * IntPtr = 0;
        
        WNDCLASS WindowClass = {}; 

        WindowClass.style = CS_HREDRAW | CS_VREDRAW;

        WindowClass.lpfnWndProc = MainWindowCallback;
        WindowClass.hInstance = Instance; 
        WindowClass.lpszClassName = "HandmadeHeroWindowClass";

        if(RegisterClassA(&WindowClass))
        {
            HWND WindowHandle =  CreateWindowExA(
                                  0,
                                  WindowClass.lpszClassName,
                                  "HandMadeHero",
                                  WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                  CW_USEDEFAULT, 
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT, 
                                  CW_USEDEFAULT,
                                  0,
                                  0,
                                  Instance,
                                  0);
            if(WindowHandle)
            {
                int XOffset = 0;
                int YOffset = 0;
                MSG Message;
                Running = true;
                while(Running)
                {
                    while (PeekMessageA(&Message,0,0,0,PM_REMOVE))
                    {
                        if (Message.message == WM_QUIT)
                        {
                            Running = false;
                        }
                            
                        TranslateMessage(&Message);
                        DispatchMessageA(&Message);
                    }

                    RenderWeirdGradient(GlobalBackBuffer, XOffset, YOffset);

                    HDC DeviceContext = GetDC(WindowHandle);
                    RECT ClientRect = GetWindowDimension(WindowHandle, &WindowDimensions);
                    Win32UpdateWindow(GlobalBackBuffer, DeviceContext, ClientRect, 0, 0, WindowDimensions.Width, WindowDimensions.Height);
                    ReleaseDC(WindowHandle, DeviceContext);

                    ++XOffset;

                }
            }
        }
        else
        {
        }


        return(0);
    }

