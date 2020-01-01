/* =======================================================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori - Philippe Rzetelski following along
   ======================================================================================================= */
   // ^ version control system keyword replacement

    #include <windows.h>
    #include <stdint.h> //uint8_t
    #include <xinput.h>
    #include "Defines.h"
    #include "Typedefs.h"
    #include "Structures.h"
    #include "RenderWeirdGradient.h"
    #include "DisplayBufferInWindow.h"
    #include "ResizeDIBSection.h"

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

    X_INPUT_GET_STATE(XInputGetStateStub)
    {
        return 0;
    }

    X_INPUT_SET_STATE(XInputSetStateStub)
    {
        return 0;
    }

    global_variable x_input_get_state* XInputGetState_ = XInputGetStateStub;
    global_variable x_input_set_state* XInputSetState_ = XInputSetStateStub;

    internal void
    Win32LoadXInput(void)
    {
        HMODULE XInputLibrary = LoadLibrary("xinput1_3.dll");
        if (XInputLibrary)
        {
            XInputGetState = (x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
            XInputSetState = (x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
        }

    }

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

        switch(Message)
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
            } break;

            case WM_PAINT:
            {
                PAINTSTRUCT Paint;
                HDC DeviceContext = BeginPaint(WindowHandle, &Paint);
                win32_window_dimension Dimension = win32GetWindowDimension(WindowHandle);
                Win32DisplayBufferInWindow(GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height);

                local_persist DWORD Operation = BLACKNESS;
                PatBlt(DeviceContext, 0, 0, Dimension.Width, Dimension.Height, Operation);
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
        Win32LoadXInput();
        int * IntPtr = 0;
        
        WNDCLASS WindowClass = {}; 

        Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);

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
                GlobalRunning = true;
                while(GlobalRunning)
                {
                    while (PeekMessageA(&Message,0,0,0,PM_REMOVE))
                    {
                        if (Message.message == WM_QUIT)
                        {
                            GlobalRunning = false;
                        }
                            
                        TranslateMessage(&Message);
                        DispatchMessageA(&Message);
                    }

                    // TODO should we poll more frequently? Xinput only gives back the state of the controller if we ask for it.
                    DWORD dwResult;
                    for (DWORD i=0; i < XUSER_MAX_COUNT; ++i)
                    {
                        XINPUT_STATE state;
                        ZeroMemory(&state, sizeof(XINPUT_STATE));

                        dwResult = XInputGetState(i, &state);

                        if (dwResult == ERROR_SUCCESS)
                        {
                            // Controller connected
                            // TODO: See if ControllerState.dwPacketNumber increments too rapidly
                            XINPUT_GAMEPAD* pad = &state.Gamepad;

                            bool dPadUp                     =             pad->wButtons & XINPUT_GAMEPAD_DPAD_UP;
                            bool dPadDown                 =             pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
                            bool dPadRight                 =             pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
                            bool dPadLeft                 =             pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
                            bool gPadStart                 =             pad->wButtons & XINPUT_GAMEPAD_START;
                            bool gPadBack                =             pad->wButtons & XINPUT_GAMEPAD_BACK;
                            bool gPadLeftShoulder       =             pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
                            bool gPadRightShoulder      =             pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
                            bool gPadA                    =             pad->wButtons & XINPUT_GAMEPAD_A;
                            bool gPadB                    =             pad->wButtons & XINPUT_GAMEPAD_B;
                            bool dPadX                    =             pad->wButtons & XINPUT_GAMEPAD_X;
                            bool dPadY                    =             pad->wButtons & XINPUT_GAMEPAD_Y;

                            int16 lStickX = pad->sThumbLX;
                            int16 lStickY = pad->sThumbLY;

                        }
                        else
                        {
                            // Controller not connected
                        }
                    
                    }

                    RenderWeirdGradient(GlobalBackBuffer, XOffset, YOffset);

                    HDC DeviceContext = GetDC(WindowHandle);
                    win32_window_dimension Dimension = win32GetWindowDimension(WindowHandle);
                    Win32DisplayBufferInWindow(GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height);
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

