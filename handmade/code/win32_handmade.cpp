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

    // static in C/C++ means different things depending on where it is placed...so Casey does this neat trick:
    #define internal_function static 
    #define local_persist static
    #define global_variable static// doesn't this still have the same effect as internal_function? as in only acessed from here? We get auto init to zero though

    //TODO: This is a global for now
    global_variable bool Running;
    global_variable BITMAPINFO BitmapInfo; //we want global persistence
    global_variable void * BitmapMemory; 
    global_variable HBITMAP BitmapHandle;
    global_variable HDC BitmapDeviceContext;

    // DIB = device independent bitmap: cf GDI
    // Note Casey prefixes any of his own functions (non windows API) with "Win32" to specify platform specific functions
    // and to avoid collision with any Windows functions. I am not sure if I think this is the clearest though, as it makes it look
    // like Windows API...but the functions will usually only work for Windows anyway, so maybe its fine...
    void
    Win32ResizeDIBSection(int Width , int Height)
    {
        if(BitmapHandle)
        {
            DeleteObject(BitmapHandle);
        }
        if(!BitmapDeviceContext) 
        {
            BitmapDeviceContext = CreateCompatibleDC(0);
        }

        BITMAPINFO BitmapInfo;
        // Specify our BitmapInfo structure attributes
        BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
        BitmapInfo.bmiHeader.biWidth = Width;
        BitmapInfo.bmiHeader.biHeight = Height;
        BitmapInfo.bmiHeader.biPlanes = 1;
        BitmapInfo.bmiHeader.biBitCount = 32;
        BitmapInfo.bmiHeader.biCompression = BI_RGB;
        // Casey :Since BitmapInfo is static, its initialised to zero,
        // so we don't need to set the rest of the fields to 0...
        // *BUT* I do not think this is correct = the bmiHeader field is not necessarily all field zero (usually we will, but this
        // is not guaranteed)
        BitmapInfo.bmiHeader.biSizeImage = 0;
        BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
        BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
        BitmapInfo.bmiHeader.biClrUsed = 0;
        BitmapInfo.bmiHeader.biClrImportant = 0;


        BitmapHandle = CreateDIBSection(BitmapDeviceContext,
                                       &BitmapInfo,
                                       DIB_RGB_COLORS,
                                       &BitmapMemory,
                                       NULL,0);

            
        
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
                // I had width zero, what does this do?
                LONG Width = Paint.rcPaint.right - Paint.rcPaint.left;
                LONG Height = Paint.rcPaint.top - Paint.rcPaint.bottom;
                Win32UpdateWindow(DeviceContext, X, Y, Width, Height);
                // Don't do willy-nilly - Casey only uses static for debugging, where it can be useful
                // Also understand what he is saying with lexical scope and static vs global
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


    int CALLBACK // #define CALLBACK/WINAPI/APIENTRY __stdcall so we could (and I think should in this case) use WINAPI or APIENTRY - CALLBACK may be a
                // a misleading marker
    WinMain(HINSTANCE Instance,
           HINSTANCE PrevInstance,
           LPSTR CmdLine,
           int ShowCmd)
    {
        int * IntPtr = 0;
        
        WNDCLASS WindowClass = {}; //set all to zero - C++; C may need {0}

        // Don't need WindowsClass.style _at all_  apparently Casey (or I can if I want) check
        WindowClass.style = CS_OWNDC;//|CS_HREDRAW|CS_VREDRAW; //MSDN for more detail

        // Casey: We need to know the callback name here, because as we saw around 30min at Day 2 Q&A, windows can ask for it
        // at anytime (before _we_ dispatch it). Like Casey says, would be better if we didn't need to define the WindowProc callback
        // ourselves.
        // 
        // Remove it and see - you will get a runtime error, as windows wants to call it.
        //
        //BUT check this out! I decided to set it to DefWindowProc; And we are back in business! BUT ofcourse we are then not 
        //able to handle messages. We could still enter the default case in our implementation of the callback and print out the message
        //and maybe even map it to the name using a table we can make for eg. the WINE website with windows messages and their values.
        WindowClass.lpfnWndProc = MainWindowCallback;
        //WindowClass.lpfnWndProc = DefWindowProc;
        WindowClass.hInstance = Instance; // could call kernel function GetModuleHandle(0)
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


       
