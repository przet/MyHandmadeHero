#ifndef RESIZEDIBSECTION_H
#define RESIZEDIBSECTION_H

#include "Defines_Typedefs.h"
#include "Structures.h"

internal void
Win32ResizeDIBSection(win32_offscreen_buffer* Buffer, int Width, int Height)
{

    if (Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;

    // Specify our BitmapInfo structure attributes
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;

    // Top down so negative (see MSDN)
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;

    // 32 for 4byte alignment (24  =3 bytes; 1 more byte for this "padding" to give 
    // alignment on 4 byte boundary (x86 architecture efficiency)
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;
    Buffer->Info.bmiHeader.biSizeImage = 0;
    Buffer->Info.bmiHeader.biXPelsPerMeter = 0;
    Buffer->Info.bmiHeader.biYPelsPerMeter = 0;
    Buffer->Info.bmiHeader.biClrUsed = 0;
    Buffer->Info.bmiHeader.biClrImportant = 0;

    int BitmapMemorySize = (Buffer->Width * Buffer->Height) * Buffer->BytesPerPixel;

    // VirtualAlloc returns pages - a bit more "raw" than HeapAlloc
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;

    //TODO : we might want to initialise this to black 
}

#endif // !RESIZEDIBSECTION_H
