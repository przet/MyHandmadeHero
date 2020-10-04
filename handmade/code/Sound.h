#ifndef SOUND_H
#define SOUND_H
#include <Windows.h>
#include <dsound.h>
#include "Defines_Typedefs.h"

internal void
Win32InitDSound(HWND WindowHandle, int32 BufferSize = 48000 * sizeof(int16) * 2, int32 SamplesPerSecond = 44100, int32 BitsPerSample = 16)
{
    LPDIRECTSOUND DirectSound;
    // NOTE : Load the library
    HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
    if (DSoundLibrary)
    {
        // NOTE : Get a Direct Sound Object
        //HRESULT (*DirectSoundCreate)(LPCGUID pcGuidDevice, LPDIRECTSOUND * ppDS, LPUNKNOWN pUnkOuter)
        //    = (HRESULT(*)(LPCGUID pcGuidDevice, LPDIRECTSOUND * ppDS, LPUNKNOWN pUnkOuter))
        //    GetProcAddress(DSoundLibrary, "DirectSoundCreate");

        // TODO find out why ^ didn't work (and v did)
        direct_sound_create* DirectSoundCreate = (direct_sound_create*)
            GetProcAddress(DSoundLibrary, "DirectSoundCreate");

        if (DirectSoundCreate && DirectSoundCreate(0, &DirectSound, 0) == DS_OK)
        {
            // Leave unitialised to make sure we initialise everything in correct order
            WAVEFORMATEX WaveFormat;
            WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
            WaveFormat.nChannels = 2;
            WaveFormat.nSamplesPerSec = SamplesPerSecond;
            WaveFormat.wBitsPerSample = 16;
            WaveFormat.nBlockAlign = WaveFormat.nChannels * WaveFormat.wBitsPerSample / 8;
            WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
            WaveFormat.cbSize = 0;

            if (DirectSound->SetCooperativeLevel(WindowHandle, DSSCL_PRIORITY) == DS_OK)
            {
                DSBUFFERDESC BufferDesc = {};
                BufferDesc.dwSize = sizeof(BufferDesc);
                BufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

                // "Create" a primary buffer
                // TODO: DSBCAPS_GLOBALFOUCS?
                LPDIRECTSOUNDBUFFER PrimaryBuffer;
                if (DirectSound->CreateSoundBuffer(&BufferDesc, &PrimaryBuffer, 0) == DS_OK)
                {
                    HRESULT Error = PrimaryBuffer->SetFormat(&WaveFormat);
                    if (Error == DS_OK)
                    {
                        //NOTE:Format set!
                        OutputDebugStringA("Primary buffer format was set. \n");
                    }
                    else
                    {
                        //TODO:Diagnostic
                        OutputDebugStringA("Primary buffer format was NOT set. \n");
                    }
                }
                else
                {
                    //TODO:Diagnostic
                }
            }
            else
            {
                //TODO:Diagnostic
            }

            // "Create" a secondary buffer
            // TODO: DSBCAPS_GETCURRENTPOSITION?
            DSBUFFERDESC BufferDesc = {};
            BufferDesc.dwSize = sizeof(BufferDesc);
            BufferDesc.dwFlags = 0;
            BufferDesc.dwBufferBytes = BufferSize;
            BufferDesc.lpwfxFormat = &WaveFormat;
            HRESULT Error = DirectSound->CreateSoundBuffer(&BufferDesc, &GlobalSecondaryBuffer, 0);
            if (Error == DS_OK)
            {
                OutputDebugStringA("Secondary buffer created successfully. \n");
            }
            else
            {
                OutputDebugStringA("Secondary buffer NOT created successfully. \n");
            }

        }
    }
}

#endif // !SOUND_H
