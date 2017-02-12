#ifndef INCLUDED_COMMAND_DEF
#define INCLUDED_COMMAND_DEF

enum { 
    TILE_SIZE = 256,  // 纹理分块大小
};


struct SOURCE_BITMAP_DATA
{
    HBITMAP hBitmap;
    byte* pBits;
    long  pitch;
    bool  hasAlphaChannel;
};


#endif // INCLUDED_COMMAND_DEF