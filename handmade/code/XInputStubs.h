#ifndef XINPUT_STUBS_H
#define XINPUT_STUBS_H

#include "Defines_Typedefs.h"

X_INPUT_GET_STATE(XInputGetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}

X_INPUT_SET_STATE(XInputSetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}

#endif // !XINPUT_STUBS_H

