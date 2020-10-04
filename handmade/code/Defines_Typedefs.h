#ifndef DEFINES_TYPEDEFS_H
#define DEFINES_TYPEDEFS_H

//DEFINES

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

#define X_INPUT_GET_STATE(functionName) DWORD WINAPI functionName(DWORD dwUserIndex, XINPUT_STATE* pState)
#define X_INPUT_SET_STATE(functionName) DWORD WINAPI functionName(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_GET_STATE(x_input_get_state);
typedef X_INPUT_SET_STATE(x_input_set_state);

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_

global_variable LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;
#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter)

//TYPEDEFS

// Platform (machine) independent size
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;
typedef DIRECT_SOUND_CREATE(direct_sound_create);

#endif // !DEFINES_TYPEDEFS_H

