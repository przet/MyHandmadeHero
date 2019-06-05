/* =======================================================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori - Philippe Rzetelski following along
   ======================================================================================================= */
   // ^ version control system keyword replacement

    #include <windows.h>

    LRESULT CALLBACK
    MainWindowCallback(HWND WindowHandle,
                       UINT Message,
                       WPARAM WParam, 
                       LPARAM LParam) 
    {
        LRESULT Result = 0; // Zero for now

        // Note Casey's use of {} in case - so that he can keep an variable declared local to a case block, rather than the whole switch block; break outside no other
        // reason than simpler to read, and maybe not removed by mistake, getting mixed up with other code
        switch(Message)
        {
            case WM_SIZE:
            {
                OutputDebugStringA("WM_SIZE\n");
            } break;

            case WM_DESTROY:
            {
                OutputDebugStringA("WM_DESTROY\n");
            } break;

            case WM_CLOSE:
            {
                OutputDebugStringA("WM_CLOSE\n");
	        if (!DestroyWindow(WindowHandle))
		{
	    	    OutputDebugStringA("Something bad happened - window not destroyed\n");
		}
                else
                {
	    	    OutputDebugStringA("Window destroyed!\n");
                }

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
                OutputDebugStringA("WM_PAINT\n");
                PAINTSTRUCT Paint;
                HDC DeviceContext = BeginPaint(WindowHandle, &Paint);
                LONG X = Paint.rcPaint.left;
                LONG Y = Paint.rcPaint.right;
                // I had width zero, what does this do?
                LONG Width = Paint.rcPaint.right - Paint.rcPaint.left;
                LONG Height = Paint.rcPaint.top - Paint.rcPaint.bottom;
                // Don't do willy-nilly - Casey only uses static for debugging, where it can be useful
                // Also understand what he is saying with lexical scope and static vs global
                static DWORD Operation = BLACKNESS;
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


                EndPaint(WindowHandle, &Paint);
            } break;

            default:
            {
                Result = DefWindowProc(WindowHandle, Message, WParam, LParam);
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
        WNDCLASS WindowClass = {}; //set all to zero - C++; C may need {0}

        WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW; //MSDN for more detail
        WindowClass.lpfnWndProc = MainWindowCallback;
        WindowClass.hInstance = Instance; // could call kernel function GetModuleHandle(0)
        WindowClass.lpszClassName = "HandmadeHeroWindowClass";

        if(RegisterClass(&WindowClass))
        {
            HWND WindowHandle =  CreateWindowEx(
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
                for(;;)
                {
                    BOOL MessageResult = GetMessage(&Message,0, 0, 0);
                    if(MessageResult > 0)
                    {
                        TranslateMessage(&Message);
                        DispatchMessage(&Message);
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


       
