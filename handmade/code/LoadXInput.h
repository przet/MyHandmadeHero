#ifndef LOADXINPUT_H
#define LOADXINPUT_H

#include "Defines_Typedefs.h" 

global_variable x_input_get_state* XInputGetState_ = XInputGetStateStub;
global_variable x_input_set_state* XInputSetState_ = XInputSetStateStub;

internal void
Win32LoadXInput(void)
{
    HMODULE XInputLibrary = LoadLibrary("xinput1_4.dll");
    if (!XInputLibrary)
    {
        XInputLibrary = LoadLibrary("xinput1_3.dll");
    }

    if (XInputLibrary)
    {
        XInputGetState = (x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState = (x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
    }

}

#endif //! LOADXINPUT_H
