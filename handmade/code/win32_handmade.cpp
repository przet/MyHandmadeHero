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
    #include "GetWindowDimension.h"
    #include "MainWindowCallback.h"
    #include "XInputStubs.h"
    #include "LoadXInput.h"
    #include <stdio.h>

    extern x_input_get_state* XInputGetState_;
    extern x_input_set_state* XInputSetState_;

   extern bool GlobalRunning;
   extern win32_offscreen_buffer GlobalBackBuffer;

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
                bool Moving = true;
                bool WasWasDown = true;

                while(GlobalRunning)
                {
                    OutputDebugStringA("GlobalRunning\n");
                    int count = 0;
                    while (PeekMessageA(&Message,0,0,0,PM_REMOVE))
                    {
                        OutputDebugStringA("Peeking at message\n");
                        if (Message.message == WM_QUIT)
                        {
                            GlobalRunning = false;
                        }
                        switch (Message.message)
                        {
                            case WM_KEYDOWN:case WM_KEYUP:
                            {
                                uint32 VKCode = Message.wParam;
                                bool WasDown = ((Message.lParam & (1 << 30)) != 0);
                                bool IsDown = ((Message.lParam & (1 << 31)) == 0);

                                if (VKCode == VK_SPACE || VKCode == 'P')
                                {
                                    if (!WasDown && IsDown)
                                    {
                                        Moving = !Moving;
                                        OutputDebugStringA("Space !WasDown && IsDown\n");
                                    }
                                    if (WasDown && !IsDown)
                                    {
                                        OutputDebugStringA("Space WasDown && !IsDown\n");
                                    }
                                    if (WasDown && IsDown)
                                    {
                                        Moving = false;
                                        OutputDebugStringA("Space WasDown && IsDown\n");
                                    }
                                    if (WasWasDown && WasDown && !IsDown)
                                    {
                                        Moving = true;
                                        OutputDebugStringA("Space WasWasDown && WasDown && !IsDown\n");
                                    }
                                    OutputDebugStringA("-----------------------------------------------\n");
                                }

                            WasWasDown = WasDown;
                            }break;

                            default:
                            {
                                //
                            }break;
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

                    if (Moving)
                    {
                        ++XOffset;
                    }

                }
            }
        }
        else
        {
        }


        return(0);
    }

