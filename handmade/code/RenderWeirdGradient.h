internal void
RenderWeirdGradient(win32_offscreen_buffer* Buffer, int XOffset, int YOffset)
{

    // TODO : See what optimizer does (with pass by value of buffer??)

    // casting void to char type (want per byte arithmetic)
    uint8* Row = (uint8*)Buffer->Memory;

    for (int Y = 0; Y < Buffer->Height; ++Y)
    {
        uint32* Pixel = (uint32*)Row;
        for (int X = 0; X < Buffer->Width; ++X)
        {
            // Explanation:
            // 
            // Memory(hex) : BB GG RR xx (xx is pad)
            // Register (hex. Assuming little endian architecture) xx RR GG BB
            //
            // So now that we are dealing with 32 bits directly (i.e 32-bit pixel) we rewrite

            /*
                *Pixel++ = X + XOffset;

                *Pixel++ = Y + YOffset;

                *Pixel++ = 0;

                *Pixel++ = 0;
            */

            // as
            uint8 Blue = X + XOffset;
            uint8 Green = Y + YOffset;
            uint8 Red = 0;
            uint8 Pad = 0;

            *Pixel++ = (Blue | Green << 8 | Red << 16 | Pad << 24);
            // TODO : operator precedence : remove brackets
        }

        Row += Buffer->Pitch;
        // TODO Casey makes a comment here that the separation of operations (Row adding and Pixel)
        // may seem inefficient, we do ++Pixel because sometimes byte boundaries may differ(??).
        // Its < 33:30
    }

}
