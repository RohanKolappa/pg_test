/*
 * bitmap.cpp
 *
 *  Created on: Jan 19, 2011
 *      Author: rkale
 */

#include "v2dlib_compatibility.h"
#include "bitmap.hh"
#include "memory_tracker.h"

// Pushkar: Original code had defined BITMAP_SIGNATURE as 'MB' but this
// was causing gcc to throw warnings, so using the below number.
#define BITMAP_SIGNATURE 0x4D42

CBitmap::CBitmap() :
m_BitmapData(0), m_BitmapSize(0), m_ColorTable(0), m_ColorTableSize(0)
{
    Dispose();
}

CBitmap:: CBitmap(const char* Filename) :
        m_BitmapData(0), m_BitmapSize(0), m_ColorTable(0), m_ColorTableSize(0)
{
    Dispose();
    Load(Filename);
}

CBitmap::~CBitmap() {
    Dispose();
}

void CBitmap::Dispose() {
    if (m_BitmapData) delete[] m_BitmapData;
    if (m_ColorTable) delete[] m_ColorTable;
    m_ColorTableSize = 0;
    m_BitmapData = 0;
    m_ColorTable = 0;
    memset(&m_BitmapFileHeader, 0, sizeof(m_BitmapFileHeader));
    memset(&m_BitmapHeader, 0, sizeof(m_BitmapHeader));
}


unsigned int CBitmap::ShiftRightByMask(unsigned int Color, unsigned int Mask, unsigned int DistributeToBits)
{

    if (Mask == 0) return 0;

    unsigned int ShiftCount = 0;
    unsigned int Test = 0x00000001;

    while (ShiftCount < 32) {
        if (Mask & Test) {
            break;
        }
        Test <<= 1;
        ShiftCount++;
    }

    unsigned int BitCount = 32;
    Test = 0x80000000;

    while (BitCount) {
        if ((Mask >> ShiftCount) & Test) {
            break;
        }
        Test >>= 1;
        BitCount--;
    }

    unsigned int BaseColor = (Color & Mask) >> ShiftCount;

    if (DistributeToBits > BitCount) {
        /* We have to fill lower bits */
        unsigned int BitsToFill = DistributeToBits - BitCount;
        while (BitsToFill--) {
            BaseColor <<= 1;
            if (BaseColor & 1) {
                BaseColor |= 1;
            }
        }
    } else if (DistributeToBits < BitCount) {
        BaseColor >>= (BitCount - DistributeToBits);
    }
    return BaseColor;
}

/* Load specified Bitmap and stores it as RGBA in an internal buffer */
int CBitmap::LoadFromBuffer(unsigned char *pBuffer, int nSize) {
    if (pBuffer == NULL) {
        return 4;
    }

    Dispose();

    if ((unsigned int)nSize < (BITMAP_FILEHEADER_SIZE_INT + sizeof(BITMAP_HEADER_INT)))
    {
        return 5;
    }

    memcpy(&m_BitmapFileHeader, pBuffer, BITMAP_FILEHEADER_SIZE_INT);
    if (m_BitmapFileHeader.Signature != BITMAP_SIGNATURE) {
        return 6;
    }

    memcpy(&m_BitmapHeader, pBuffer + BITMAP_FILEHEADER_SIZE_INT, sizeof(BITMAP_HEADER_INT));

    if (m_BitmapHeader.BitCount == 1) {
        m_ColorTableSize = 2;
    } else if (m_BitmapHeader.BitCount == 4) {
        m_ColorTableSize = 16;
    } else if (m_BitmapHeader.BitCount == 8) {
        m_ColorTableSize = 256;
    }

    m_ColorTable = new BGRA_INT[m_ColorTableSize];

    memcpy(m_ColorTable, pBuffer + BITMAP_FILEHEADER_SIZE_INT + m_BitmapHeader.HeaderSize, sizeof(BGRA_INT) * m_ColorTableSize);

    m_BitmapSize = GetWidth() * GetHeight();

    //Commenting out this because later we are taking care about
    //the truncation.
    /*if ((unsigned int)nSize < m_BitmapHeader.SizeImage) {
        return 7;
    }*/

    m_BitmapData = new RGBA_INT[m_BitmapSize];

    unsigned int LineWidth = ((GetWidth() * GetBitCount() / 8) + 3) & ~3;
    unsigned char *Line = new unsigned char[LineWidth + 4];

    unsigned char *pData = pBuffer + m_BitmapFileHeader.BitsOffset;
    if ((pData == NULL) ||
        ((pData - pBuffer) >= nSize )) {
        return 8;
    }

    int Index = 0;

    if (m_BitmapHeader.Compression == 0) {
        for (unsigned int i = 0; i < GetHeight(); i++) {
            if ((pData + LineWidth - pBuffer) > nSize) {
                return 9;
            }
            memcpy(Line, pData, LineWidth);
            pData += LineWidth;

            unsigned char *LinePtr = Line;

            for (unsigned int j = 0; j < GetWidth(); j++) {
                if (m_BitmapHeader.BitCount == 1) {
                    unsigned int Color = *((unsigned char*) LinePtr);
                    for (int k = 0; k < 8; k++) {
                        m_BitmapData[Index].Red = m_ColorTable[Color & 0x80 ? 1 : 0].Red;
                        m_BitmapData[Index].Green = m_ColorTable[Color & 0x80 ? 1 : 0].Green;
                        m_BitmapData[Index].Blue = m_ColorTable[Color & 0x80 ? 1 : 0].Blue;
                        m_BitmapData[Index].Alpha = m_ColorTable[Color & 0x80 ? 1 : 0].Alpha;
                        Index++;
                        Color <<= 1;
                    }
                    LinePtr++;
                    j += 7;
                } else if (m_BitmapHeader.BitCount == 4) {
                    unsigned int Color = *((unsigned char*) LinePtr);
                    m_BitmapData[Index].Red = m_ColorTable[(Color >> 4) & 0x0f].Red;
                    m_BitmapData[Index].Green = m_ColorTable[(Color >> 4) & 0x0f].Green;
                    m_BitmapData[Index].Blue = m_ColorTable[(Color >> 4) & 0x0f].Blue;
                    m_BitmapData[Index].Alpha = m_ColorTable[(Color >> 4) & 0x0f].Alpha;
                    Index++;
                    m_BitmapData[Index].Red = m_ColorTable[Color & 0x0f].Red;
                    m_BitmapData[Index].Green = m_ColorTable[Color & 0x0f].Green;
                    m_BitmapData[Index].Blue = m_ColorTable[Color & 0x0f].Blue;
                    m_BitmapData[Index].Alpha = m_ColorTable[Color & 0x0f].Alpha;
                    Index++;
                    LinePtr++;
                    j++;
                } else if (m_BitmapHeader.BitCount == 8) {
                    unsigned int Color = *((unsigned char*) LinePtr);
                    m_BitmapData[Index].Red = m_ColorTable[Color].Red;
                    m_BitmapData[Index].Green = m_ColorTable[Color].Green;
                    m_BitmapData[Index].Blue = m_ColorTable[Color].Blue;
                    m_BitmapData[Index].Alpha = m_ColorTable[Color].Alpha;
                    Index++;
                    LinePtr++;
                } else if (m_BitmapHeader.BitCount == 16) {
                    unsigned int Color = *((unsigned short int*) LinePtr);
                    m_BitmapData[Index].Red = ((Color >> 10) & 0x1f) << 3;
                    m_BitmapData[Index].Green = ((Color >> 5) & 0x1f) << 3;
                    m_BitmapData[Index].Blue = (Color & 0x1f) << 3;
                    m_BitmapData[Index].Alpha = 255;
                    Index++;
                    LinePtr += 2;
                } else if (m_BitmapHeader.BitCount == 24) {
                    unsigned int Color = *((unsigned int*) LinePtr);
                    m_BitmapData[Index].Blue = Color & 0xff;
                    m_BitmapData[Index].Green = (Color >> 8) & 0xff;
                    m_BitmapData[Index].Red = (Color >> 16) & 0xff;
                    m_BitmapData[Index].Alpha = 0;
                    Index++;
                    LinePtr += 3;
                } else if (m_BitmapHeader.BitCount == 32) {
                    unsigned int Color = *((unsigned int*) LinePtr);
                    m_BitmapData[Index].Blue = Color & 0xff;
                    m_BitmapData[Index].Green = (Color >> 8) & 0xff;
                    m_BitmapData[Index].Red = (Color >> 16) & 0xff;
                    m_BitmapData[Index].Alpha = Color >> 24;
                    Index++;
                    LinePtr += 4;
                }
            }
        }
    } else if (m_BitmapHeader.Compression == 1) { // RLE 8
        unsigned char Count = 0;
        unsigned char ColorIndex = 0;
        int x = 0, y = 0;

        while (pData != NULL) {
            if ((pData + 1 - pBuffer) > nSize) {
                return 9;
            }
            memcpy(&Count, pData, 1);
            pData += 1;

            if ((pData - pBuffer) > nSize) {
                return 9;
            }
        	memcpy(&ColorIndex, pData, 1);
        	pData += 1;

            if (Count > 0) {
                Index = x + y * GetWidth();
                for (int k = 0; k < Count; k++) {
                    m_BitmapData[Index + k].Red = m_ColorTable[ColorIndex].Red;
                    m_BitmapData[Index + k].Green = m_ColorTable[ColorIndex].Green;
                    m_BitmapData[Index + k].Blue = m_ColorTable[ColorIndex].Blue;
                    m_BitmapData[Index + k].Alpha = m_ColorTable[ColorIndex].Alpha;
                }
                x += Count;
            } else if (Count == 0) {
                int Flag = ColorIndex;
                if (Flag == 0) {
                    x = 0;
                    y++;
                } else if (Flag == 1) {
                    break;
                } else if (Flag == 2) {
                    char rx = 0;
                    char ry = 0;
                    if ((pData + 1 - pBuffer) > nSize) {
                        return 9;
                    }
                    memcpy(&rx, pData, 1);
                    pData += 1;
                    if ((pData + 1 - pBuffer) > nSize) {
                        return 9;
                    }
                    memcpy(&ry, pData, 1);
                    pData += 1;
                    x += rx;
                    y += ry;
                } else {
                    Count = Flag;
                    Index = x + y * GetWidth();
                    for (int k = 0; k < Count; k++) {
                        if ((pData + 1 - pBuffer) > nSize) {
                            return 9;
                        }
                        memcpy(&ColorIndex, pData, 1);
                        pData += 1;

                        m_BitmapData[Index + k].Red = m_ColorTable[ColorIndex].Red;
                        m_BitmapData[Index + k].Green = m_ColorTable[ColorIndex].Green;
                        m_BitmapData[Index + k].Blue = m_ColorTable[ColorIndex].Blue;
                        m_BitmapData[Index + k].Alpha = m_ColorTable[ColorIndex].Alpha;
                    }
                    x += Count;
                }
            }
        }
    } else if (m_BitmapHeader.Compression == 2) { // RLE 4
        /* RLE 4 is not supported */
    } else if (m_BitmapHeader.Compression == 3) { // BITFIELDS

        /* We assumes that mask of each color component can be in any order */
        for (unsigned int i = 0; i < GetHeight(); i++) {
            if ((pData + LineWidth - pBuffer) > nSize) {
                return 9;
            }
            memcpy(Line, pData, LineWidth);
            pData += LineWidth;

            unsigned char *LinePtr = Line;

            for (unsigned int j = 0; j < GetWidth(); j++) {

                unsigned int Color = 0;

                if (m_BitmapHeader.BitCount == 16) {
                    Color = *((unsigned short int*) LinePtr);
                    LinePtr += 2;
                } else if (m_BitmapHeader.BitCount == 32) {
                    Color = *((unsigned int*) LinePtr);
                    LinePtr += 4;
                }
                m_BitmapData[Index].Red = ShiftRightByMask(Color, m_BitmapHeader.RedMask);
                m_BitmapData[Index].Green = ShiftRightByMask(Color, m_BitmapHeader.GreenMask);
                m_BitmapData[Index].Blue = ShiftRightByMask(Color, m_BitmapHeader.BlueMask);
                m_BitmapData[Index].Alpha = ShiftRightByMask(Color, m_BitmapHeader.AlphaMask);

                Index++;
            }
        }
    }
    delete[] Line;

    return 0;
}

/* Load specified Bitmap and stores it as RGBA in an internal buffer */
int CBitmap::Load(const char *Filename) {
    FILE *pFile = fopen(Filename, "rb");

    Dispose();

    if (pFile == 0)
    {
        return 1;
    }

    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    rewind(pFile);

    unsigned char *pBuffer = (unsigned char*)malloc (sizeof(unsigned char)*lSize);
    if (pBuffer == NULL)
    {
        fclose(pFile);
        return 2;
	}

    size_t nRead = fread (pBuffer, 1 ,lSize, pFile);
    if ((size_t)lSize != nRead)
    {
        free(pBuffer);
        fclose(pFile);

        return 3;
    }

    fclose(pFile);

    int nRet = LoadFromBuffer(pBuffer, nRead);
    if (nRet > 0)
    {
        free(pBuffer);
        return nRet;
    }

    free(pBuffer);

    return 0;
}

bool CBitmap::Save(const char* Filename, unsigned int BitCount) {
    FILE *file = fopen(Filename, "wb");

    if (file == 0) return false;

    BITMAP_FILEHEADER_INT bfh = {0};
    BITMAP_HEADER_INT bh = {0};

    bfh.Signature = BITMAP_SIGNATURE;
    bfh.BitsOffset = sizeof(BITMAP_HEADER_INT) + sizeof(BITMAP_FILEHEADER_INT);
    bfh.Size = (GetWidth() * GetHeight() * BitCount) / 8 + bfh.BitsOffset;

    bh.HeaderSize = sizeof(BITMAP_HEADER_INT);
    bh.BitCount = BitCount;

    if (BitCount == 32) {
        bh.Compression = 3; // BITFIELD
        bh.AlphaMask = 0xff000000;
        bh.BlueMask = 0x00ff0000;
        bh.GreenMask = 0x0000ff00;
        bh.RedMask = 0x000000ff;
    } else {
        bh.Compression = 0; // RGB
    }

    bh.Planes = 1;
    bh.Height = m_BitmapHeader.Height;
    bh.Width  = m_BitmapHeader.Width;
    bh.SizeImage = (GetWidth() * GetHeight() * BitCount) / 8;
    bh.PelsPerMeterX = 3780;
    bh.PelsPerMeterY = 3780;

    if (BitCount == 32) {
        fwrite(&bfh, sizeof(BITMAP_FILEHEADER_INT), 1, file);
        fwrite(&bh, sizeof(BITMAP_HEADER_INT), 1, file);
        fwrite(m_BitmapData, bh.SizeImage, 1, file);
    } else if (BitCount < 16) {
        unsigned char* Bitmap = new unsigned char[bh.SizeImage];

        BGRA_INT *Palette = 0;
        unsigned int PaletteSize = 0;

        if (GetBitsWithPalette(Bitmap, bh.SizeImage, BitCount, Palette, PaletteSize)) {
            bfh.BitsOffset += PaletteSize * sizeof(BGRA_INT);

            fwrite(&bfh, sizeof(BITMAP_FILEHEADER_INT), 1, file);
            fwrite(&bh, sizeof(BITMAP_HEADER_INT), 1, file);
            fwrite(Palette, PaletteSize, sizeof(BGRA_INT), file);
            fwrite(Bitmap, bh.SizeImage, 1, file);
        }
        delete [] Bitmap;
        delete [] Palette;
    } else {
        unsigned char* Bitmap = new unsigned char[bh.SizeImage];

        if (GetBits(Bitmap, bh.SizeImage, BitCount)) {
            fwrite(&bfh, sizeof(BITMAP_FILEHEADER_INT), 1, file);
            fwrite(&bh, sizeof(BITMAP_HEADER_INT), 1, file);
            fwrite(Bitmap, bh.SizeImage, 1, file);
        }
        delete [] Bitmap;
    }

    fclose(file);
    return true;
}

/* Copies internal RGBA buffer to user specified buffer */

bool CBitmap::GetBits(void* Buffer, unsigned int &Size) {
    bool Result = false;
    if (Size == 0 || Buffer == 0) {
        Size = m_BitmapSize * sizeof(RGBA_INT);
        Result = m_BitmapSize != 0;
    } else {
        memcpy(Buffer, m_BitmapData, Size);
        Result = true;
    }
    return Result;
}

/* Returns internal RGBA buffer */

void* CBitmap::GetBits() {
    return m_BitmapData;
}

/* Copies internal RGBA buffer to user specified buffer and convertes into destination bit format.
 * Supported Bit depths are: 16 (5-5-5), 24, 32
 */

bool CBitmap::GetBits(void* Buffer, unsigned int &Size, unsigned int BitCount) {
    bool Result = false;

    if (Size == 0 || Buffer == 0) {
        Size = (m_BitmapSize * BitCount) / 8;
        return true;
    }

    if (BitCount > 32) {
        return false;
    }

    unsigned char* BufferPtr = (unsigned char*) Buffer;

    for (unsigned int i = 0; i < m_BitmapSize; i++) {
        if (BitCount == 16) {
            ((BGR16_INT*) BufferPtr)->Blue = ShiftRightByMask(m_BitmapData[i].Blue, 0xff, 5);
            ((BGR16_INT*) BufferPtr)->Green = ShiftRightByMask(m_BitmapData[i].Green, 0xff, 5);
            ((BGR16_INT*) BufferPtr)->Red = ShiftRightByMask(m_BitmapData[i].Red, 0xff, 5);
            BufferPtr += 2;
        } else if (BitCount == 24) {
            ((BGR_INT*) BufferPtr)->Blue = m_BitmapData[i].Blue;
            ((BGR_INT*) BufferPtr)->Green = m_BitmapData[i].Green;
            ((BGR_INT*) BufferPtr)->Red = m_BitmapData[i].Red;
            BufferPtr += 3;
        } else if (BitCount == 32) {
            ((BGRA_INT*) BufferPtr)->Blue = m_BitmapData[i].Blue;
            ((BGRA_INT*) BufferPtr)->Green = m_BitmapData[i].Green;
            ((BGRA_INT*) BufferPtr)->Red = m_BitmapData[i].Red;
            ((BGRA_INT*) BufferPtr)->Alpha = m_BitmapData[i].Alpha;
            BufferPtr += 4;
        }
    }

    Result = true;

    return Result;
}

/* See GetBits().
 * It creates a corresponding color table (palette) which have to be destroyed by the user after usage.
 * Supported Bit depths are: 4, 8
 * Todo: Optimize, use optimized palette, do ditehring
 */

bool CBitmap::GetBitsWithPalette(void* Buffer, unsigned int &Size, unsigned int BitCount, BGRA_INT* &Palette,
        unsigned int &PaletteSize, bool OptimalPalette) {
    bool Result = false;

    if (BitCount > 16) {
        return false;
    }

    if (Size == 0 || Buffer == 0) {
        Size = (m_BitmapSize * BitCount) / 8;
        return true;
    }

    if (BitCount == 4) {
        PaletteSize = 16;
        Palette = new BGRA_INT[PaletteSize];
        for (int r = 0; r < 4; r++) {
            for (int g = 0; g < 2; g++) {
                for (int b = 0; b < 2; b++) {
                    Palette[r | g << 2 | b << 3].Red = (r << 6) | 0x35;
                    Palette[r | g << 2 | b << 3].Green = (g << 7) | 0x55;
                    Palette[r | g << 2 | b << 3].Blue = (b << 7) | 0x55;
                    Palette[r | g << 2 | b << 3].Alpha = 0xff;
                }
            }
        }
    } else if (BitCount == 8) {
        PaletteSize = 256;
        Palette = new BGRA_INT[PaletteSize];
        for (int r = 0; r < 8; r++) {
            for (int g = 0; g < 8; g++) {
                for (int b = 0; b < 4; b++) {
                    Palette[r | g << 3 | b << 6].Red = (r << 5) | 0x15;
                    Palette[r | g << 3 | b << 6].Green = (g << 5) | 0x15;
                    Palette[r | g << 3 | b << 6].Blue = (b << 6) | 0x35;
                    Palette[r | g << 3 | b << 6].Alpha = 0xff;
                }
            }
        }
    }

    unsigned char* BufferPtr = (unsigned char*) Buffer;

    for (unsigned int i = 0; i < m_BitmapSize; i++) {
        if (BitCount == 4) {
            *BufferPtr = (m_BitmapData[i].Red >> 6) | (m_BitmapData[i].Green >> 7) << 2 | (m_BitmapData[i].Blue >> 7) << 3;
            i++;
            *BufferPtr |= ((m_BitmapData[i].Red >> 6) | (m_BitmapData[i].Green >> 7) << 2 | (m_BitmapData[i].Blue >> 7) << 3) << 4;
            BufferPtr++;
        } else if (BitCount == 8) {
            *BufferPtr = (m_BitmapData[i].Red >> 5) | (m_BitmapData[i].Green >> 5) << 3 | (m_BitmapData[i].Blue >> 5) << 6;
            BufferPtr++;
        }
    }

    Result = true;

    return Result;
}

/* Returns image in BGRA layout (32 bit) with top row first, bottom row last */
bool CBitmap::GetBitsBGRA(void* Buffer, unsigned int &Size) {
    bool Result = false;

    if (Size == 0 || Buffer == 0) {
        Size = (m_BitmapSize * 32) / 8;
        return true;
    }

    if ( Size < ((m_BitmapSize * 32) / 8) ) {
        return false;
    }

    unsigned char* BufferPtr = (unsigned char*) Buffer;

    for (unsigned int i = 0; i < GetHeight(); i++) {

        if (m_BitmapHeader.Height > 0)
            BufferPtr = (unsigned char *) Buffer +
            ((GetHeight() - (1 + i)) * GetWidth() * 4);

        for (unsigned int j = 0; j < GetWidth(); j++) {
            unsigned int k = i*GetWidth() + j;
            ((BGRA_INT*) BufferPtr)->Blue = m_BitmapData[k].Blue;
            ((BGRA_INT*) BufferPtr)->Green = m_BitmapData[k].Green;
            ((BGRA_INT*) BufferPtr)->Red = m_BitmapData[k].Red;
            ((BGRA_INT*) BufferPtr)->Alpha = m_BitmapData[k].Alpha;
            BufferPtr += 4;
        }
    }

    Result = true;

    return Result;
}

/* Assumes Buffer layout is BGR 24 bit with top row first, bottom row last */
void CBitmap::SetBitsBGR(void* Buffer, unsigned int Width, unsigned int Height)
{
    SetBits(Buffer, Width, -Height, 0x00FF0000, 0x0000FF00, 0x000000FF);
}

/* Assumes Buffer layout is BGRA 32 bit with top row first, bottom row last */
void CBitmap::SetBitsBGRA(void* Buffer, unsigned int Width, unsigned int Height) {
    SetBits(Buffer, Width, -Height, 0x00FF0000, 0x0000FF00, 0x000000FF,
        0xFF000000);
}

/* Assumes Buffer layout is RGBA 32 bit with top row first, bottom row last */
void CBitmap::SetBitsRGBA(void* Buffer, unsigned int Width, unsigned int Height) {
    SetBits(Buffer, Width, -Height, 0x000000FF, 0x0000FF00, 0x00FF0000,
        0xFF000000);
}

/* Set Bitmap Bits. Will be converted to RGBA internally */

void CBitmap::SetBits(void* Buffer, unsigned int Width, unsigned int Height,
        unsigned int RedMask, unsigned int GreenMask, unsigned int BlueMask, unsigned int AlphaMask) {
    unsigned char *BufferPtr = (unsigned char*) Buffer;

    Dispose();

    m_BitmapHeader.Width = Width;
    m_BitmapHeader.Height = Height;
    m_BitmapHeader.BitCount = 32;
    m_BitmapHeader.Compression = 3;

    m_BitmapSize = GetWidth() * GetHeight();
    m_BitmapData = new RGBA_INT[m_BitmapSize];

    /* Find BitCount by Mask, maximum is 32 Bit */

    unsigned int Test = 0x80000000;
    unsigned int BitCount = 32;

    while (BitCount) {
        if ((RedMask | GreenMask | BlueMask | AlphaMask) & Test) {
            break;
        }
        Test >>= 1;
        BitCount--;
    }

    for (unsigned int i = 0; i < m_BitmapSize; i++) {
        unsigned int Color = 0;
        if (BitCount <= 8) {
            Color = *((unsigned char*) BufferPtr);
            BufferPtr += 1;
        } else if (BitCount <= 16) {
            Color = *((unsigned short int*) BufferPtr);
            BufferPtr += 2;
        } else if (BitCount <= 24) {
            Color = *((unsigned int*) BufferPtr);
            BufferPtr += 3;
        } else if (BitCount <= 32) {
            Color = *((unsigned int*) BufferPtr);
            BufferPtr += 4;
        } else {
            /* unsupported */
            BufferPtr += 1;
        }
        m_BitmapData[i].Alpha = ShiftRightByMask(Color, AlphaMask);
        m_BitmapData[i].Red = ShiftRightByMask(Color, RedMask);
        m_BitmapData[i].Green = ShiftRightByMask(Color, GreenMask);
        m_BitmapData[i].Blue = ShiftRightByMask(Color, BlueMask);
    }
}

void CBitmap::SetAlphaBits(unsigned char Alpha) {
    for (unsigned int i = 0; i < m_BitmapSize; i++) {
        m_BitmapData[i].Alpha = Alpha;
    }
}


