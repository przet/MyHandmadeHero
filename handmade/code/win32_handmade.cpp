/* =======================================================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori - Philippe Rzetelski following along
   ======================================================================================================= */
   // ^ version control system keyword replacement

    #include <windows.h>

    // Undefine the following to forcce use of ANSI or UNICODE versions - we will always use ANSI,
    // so doing the below will: 1) make us use suffix A 2) If someone tries to compile in Unicode mode, an error will occur (good)
    #undef CreateWindowEx
    #undef OutputDebugString
    #undef TranslateMessage
    #undef DispatchMessage
    #undef DefWindowProc 

    #define internal_function static 
    #define local_persist static
    #define global_variable static

    //TODO: This is a global for now
    global_variable bool Running;
    global_variable BITMAPINFO BitmapInfo; //we want global persistence
    global_variable void * BitmapMemory; 

    void
    Win32ResizeDIBSection(int Width , int Height)
    {

        BITMAPINFO BitmapInfo;
        
        // Specify our BitmapInfo structure attributes
        BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
        BitmapInfo.bmiHeader.biWidth = Width;
        BitmapInfo.bmiHeader.biHeight = Height;
        BitmapInfo.bmiHeader.biPlanes = 1;
        
        // 32 for 4byte alignment (24  =3 bytes; 1 more byte for this "padding" to give 
        // alignment on 4 byte boundary (x86 architecture efficiency)
        BitmapInfo.bmiHeader.biBitCount = 32;
        BitmapInfo.bmiHeader.biCompression = BI_RGB;
        BitmapInfo.bmiHeader.biSizeImage = 0;
        BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
        BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
        BitmapInfo.bmiHeader.biClrUsed = 0;
        BitmapInfo.bmiHeader.biClrImportant = 0;

        int BytesPerPixel = 4;
        BitmapMemory = (Width*Height)*BytesPerPixel;

        // VirtualAlloc returns pages - a bit more "raw" than HeapAlloc
        BitmapMemory = VirtualAlloc(BitmapMemorySize); 

    }

    void
    Win32UpdateWindow(HDC DeviceContext, int X , int Y, int Width, int Height)
    {
        StretchDIBits(DeviceContext,
                      X, Y, Width, Height,
                      X, Y, Width, Height,
                      BitmapMemory,
                      &BitmapInfo,
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
                RECT ClientRect;
                GetClientRect(WindowHandle, &ClientRect);
                int Width = ClientRect.right - ClientRect.left;
                int Height = ClientRect.top - ClientRect.bottom;
                Win32ResizeDIBSection(Width, Height);
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
                Win32UpdateWindow(DeviceContext, X, Y, Width, Height);
                local_persist DWORD Operation = BLACKNESS;
                PatBlt(DeviceContext, X, Y, Width, Height, Operation);
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

                SetPixel(DeviceContext, 100, 100, RGB(255,0,0));
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

        WindowClass.style = CS_OWNDC;

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
                MSG Message;
                Running = true;
                while(Running)
                {
                    BOOL MessageResult = GetMessageA(&Message,0, 0, 0);
                    if(MessageResult > 0)
                    {
                        TranslateMessage(&Message);
                        DispatchMessageA(&Message);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else
        {
        }


        return(0);
    }

