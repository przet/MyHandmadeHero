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
