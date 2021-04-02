#pragma once
#ifndef SOUND__H
#define SOUND__H
#include <dsound.h>
#include <assert.h>
#include <stdint.h>

static WAVEFORMATEX 
win32SetWaveFormat(int32_t SamplesPerSecond)
{
    WAVEFORMATEX WaveFormat = {};
    WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat.nChannels = 2 ;
    WaveFormat.wBitsPerSample = 16;

    assert(WaveFormat.nChannels > 0 && WaveFormat.wBitsPerSample > 0);
    WaveFormat.nBlockAlign = WaveFormat.nChannels * WaveFormat.wBitsPerSample/8 ;

    WaveFormat.nSamplesPerSec = SamplesPerSecond;

    assert(WaveFormat.nBlockAlign > 0);
    WaveFormat.nAvgBytesPerSec = SamplesPerSecond * WaveFormat.nBlockAlign;

    return WaveFormat;
}

static void
win32CreatePrimarySoundBuffer(LPDIRECTSOUND DirectSound, int32_t SamplesPerSecond, WAVEFORMATEX WaveFormat)
{
   DSBUFFERDESC BufferDescription = {};
   BufferDescription.dwSize = sizeof(BufferDescription);
   BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
   LPDIRECTSOUNDBUFFER PrimaryBuffer;
   if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0)))
   {

       HRESULT Error = PrimaryBuffer->SetFormat(&win32SetWaveFormat(SamplesPerSecond));
       if (SUCCEEDED(Error))
       {
           OutputDebugStringA("Primary sound buffer format was set. \n");
       }
       else
       {
           // Primary buffer set format failed - TODO: Diagnostic
           OutputDebugStringA((LPCSTR)Error);
       }

   }
   else
   {
       // Creation of primary sound buffer failed - TODO: Dignostic
   }

}

static void
win32CreateSecondarySoundBuffer(LPDIRECTSOUND DirectSound, int32_t SamplesPerSecond, int32_t BufferSize, WAVEFORMATEX WaveFormat, LPDIRECTSOUNDBUFFER* SecondaryBuffer)
{
   DSBUFFERDESC BufferDescription = {};
   BufferDescription.dwSize = sizeof(BufferDescription);
   BufferDescription.dwBufferBytes = BufferSize;


   BufferDescription.lpwfxFormat = &win32SetWaveFormat(SamplesPerSecond);

   HRESULT Error = DirectSound->CreateSoundBuffer(&BufferDescription, SecondaryBuffer, 0);
   if (SUCCEEDED(Error))
   {
       OutputDebugStringA("Secondary sound buffer was created. \n");
   }
   else
   {
       // Creation of secondary sound buffer failed - TODO: Dignostic
           OutputDebugStringA((LPCSTR)Error);
   }

}

#endif //!SOUND__H
